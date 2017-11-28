package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"time"
	"html/template"
	"path"
	"github.com/disintegration/gift"
	"image"
	"image/color"
	"image/jpeg"
	"image/png"
	"golang.org/x/image/bmp"
	"sync"
	"math/rand"
	"strconv"
	"strings"
	"mime/multipart"
)

const address  = "localhost"
const timeout = 10 * time.Second
const time_available = 15 * time.Minute
const max_connections = 100
const max_file_size = 1025*1024
const safety_max_file_size = 10*1024*1024

var filters = map[string] *gift.GIFT{
	"Twilight": gift.New(
		gift.Brightness(5),
		gift.Gamma(0.55),
		gift.Colorize(240, 50, 15),
	),
	"Sepia": gift.New(
		gift.Sepia(100),
		gift.Contrast(10),
	),
	"Greyscale": gift.New(
		gift.Grayscale(),
		gift.Contrast(10),
	),
	"Alaska": gift.New(
		gift.Contrast(10),
		gift.Colorize(240, 50, 15),
		gift.GaussianBlur(0.55),
		gift.ColorFunc(
			func(r0, g0, b0, a0 float32) (r, g, b, a float32) {
				r = r0 - 0.1
				g = g0 + 0.1
				b = b0 + 0.3
				a = a0
				return
			},
		),
	),
	"Gemini": gift.New(
		gift.Contrast(20),
		gift.Colorize(200, 70, 22),
		gift.Sigmoid(0.5, 3),
		gift.ColorFunc(
			func(r0, g0, b0, a0 float32) (r, g, b, a float32) {
				r = r0 + 0.25
				g = g0 + 0.1
				b = b0 - 0.1
				a = a0
				return
			},
		),
	),
	"Velour": gift.New(
		gift.ColorBalance(15, -10, -10),
		gift.GaussianBlur(0.35),
	),
}

var base_filters = map[string] gift.Filter{
	"Sunset":      	gift.ColorBalance(30, -10, -10),
	"Acid":      	gift.ColorBalance(20, 20, -10),
	"Sapphire":     gift.ColorBalance(-10, -10, 30),
	"Violet":		gift.ColorFunc(
						func(r0, g0, b0, a0 float32) (r, g, b, a float32) {
							r = r0 + 0.15
							g = g0 - 0.1
							b = b0 + 0.1
							a = a0
							return
						},),
	"Postcard":		gift.ColorFunc(
						func(r0, g0, b0, a0 float32) (r, g, b, a float32) {
							r = r0 + 0.15
							g = g0 + 0.15
							b = b0 + 0.5
							a = a0
							return
						},),
	"Washed":		gift.ColorFunc(
						func(r0, g0, b0, a0 float32) (r, g, b, a float32) {
							r = r0 + 0.25
							g = g0 + 0.25
							b = b0 + 0.15
							a = a0 - 0.1
							return
						},),
	"Apocalypse":	gift.Sepia(50),
}

var input_filters = map[string] func(float32) gift.Filter{
	"Brightness":	func(val float32) gift.Filter { return gift.Brightness(val)},
	"Contrast":		func(val float32) gift.Filter { return gift.Contrast(val)},
	"Saturation":	func(val float32) gift.Filter { return gift.Saturation(val)},
	"Hue":			func(val float32) gift.Filter { return gift.Hue(val)},
	"Blur":			func(val float32) gift.Filter { return gift.GaussianBlur(val)},
	"Gamma":		func(val float32) gift.Filter { return gift.Gamma(val)},
}

var input_filter_descriptions = map[string] string {
	"Brightness": 	"(-100 , 100)",
	"Contrast":		"(-100 , 100)",
	"Saturation":	"(-100 , 500)",
	"Hue":			"(-180 , 180)",
	"Blur":			"> 0",
	"Gamma":		"> 0",
}

var semaphore = make(chan struct{}, max_connections)

type tmpl_data struct{
	Filters map[string] 		string
	Base_filters map[string] 	string
	Input_filters map[string] 	string
}

type processor struct{
	w http.ResponseWriter
	r *http.Request
}

type file_info struct{
	Filename 	string
	File_size 	int64
	File		io.Reader
	Url_file	io.ReadCloser
	Source_file	multipart.File
}

func main() {
	fmt.Println("Starting server...")
	rand.Seed(time.Now().UTC().UnixNano())
	http.HandleFunc("/", DefaultHandler)
	http.HandleFunc("/results", ImageHandler)
	http.Handle("/data/", http.HandlerFunc(file_server))
	http.ListenAndServe(address + ":http", nil)
}

func file_server(w http.ResponseWriter, r *http.Request) {
	parts := strings.Split(r.URL.Path, "/")
	last := parts[len(parts)-1]

	if last == "" {
		http.NotFound(w, r)
		return
	}

	fileServer := http.StripPrefix("/data/", http.FileServer(http.Dir("data")))
	fileServer.ServeHTTP(w, r)
}

func DefaultHandler(w http.ResponseWriter, r *http.Request) {
	f := make(map[string]string)
	for name := range filters {
		image_path := path.Join("data","images","filters",name+".jpeg")
		f[name] = image_path
	}

	bf := make(map[string]string)
	for name := range base_filters {
		image_path := path.Join("data", "images", "base_filters", name+".jpeg")
		bf[name] = image_path
	}

	data := tmpl_data{Filters : f, Base_filters: bf, Input_filters: input_filter_descriptions}
	tmpl, _ := template.ParseFiles("index.html")
	tmpl.ExecuteTemplate(w, "index", data)
}

func ImageHandler(w http.ResponseWriter, r *http.Request) {
	start := time.Now()
	done := false
	//for loop if the server is busy
	for ;; {
		select {
		case semaphore <- struct{}{}:

			defer func() { <-semaphore }()

			var f_info file_info
			p := processor{w,r}

			r.Body = http.MaxBytesReader(w, r.Body, safety_max_file_size)
			if !p.open_file(&f_info) {return}

			if f_info.Url_file != nil {
				defer f_info.Url_file.Close()
			}else if f_info.Source_file != nil {
				defer f_info.Source_file.Close()
			}

			if !p.check_file_size(f_info.File_size){return}

			dir_path := create_user_directory()
			extension, original := create_and_copy_file(dir_path,f_info)
			defer original.Close()

			if !p.check_file_type(dir_path, original) {return}

			img := decode_image(extension, original)
			p.rotate(&img)

			custom := p.create_custom_filter()
			img_paths := apply_filters(&img, custom, dir_path, extension)

			data := tmpl_data{Filters : img_paths}
			tmpl, _ := template.ParseFiles("results.html")
			tmpl.ExecuteTemplate(w, "results", data)

			start_cleaner_goroutine(dir_path)
			done = true

		default:
			if time.Since(start) > timeout {
				ErrorHandler(w,"Server too busy, try again later... ")
				done = true
			}
		}

		if done { break }
	}
}

func ErrorHandler(w http.ResponseWriter, message string) {
	tmpl, _ := template.ParseFiles("error.html")
	tmpl.ExecuteTemplate(w, "error", message)
}

func (p processor) open_file(f_info *file_info) bool{
	if url := p.r.FormValue("url"); url != "" {
		response, err := http.Get(url)
		if err != nil {
			ErrorHandler(p.w,"Unable to locate file from url.")
			return false
		}

		f_info.Filename = url
		f_info.File_size = response.ContentLength
		f_info.File = response.Body
		f_info.Url_file = response.Body

	} else {
		f, header, err := p.r.FormFile("file")
		if err != nil {
			ErrorHandler(p.w,"No file selected.")
			return false
		}

		f_info.Filename = header.Filename
		f_info.File_size = p.r.ContentLength
		f_info.File = f
		f_info.Source_file = f
	}

	return true
}

func (p processor) check_file_size(file_size int64) bool{
	if file_size > max_file_size {
		ErrorHandler(p.w,"File size to large. Maximum image size is 1 MB.")
		return false
	}

	return true
}

func create_user_directory() string {
	dir_path := path.Join("data","tmp", random_string())
	os.Mkdir(dir_path, 0700)

	return dir_path
}

func create_and_copy_file(dir_path string, f_info file_info) (extension string, original *os.File){
	name_array := strings.Split(f_info.Filename,".")
	extension = strings.ToLower(name_array[len(name_array)-1])

	original_name := path.Join(dir_path,"Original."+extension)
	original, _ = os.Create(original_name)

	io.Copy(original, f_info.File)

	return extension, original
}

func (p processor)check_file_type(dir_path string, original *os.File) bool {
	original.Seek(0,0)
	fileHeader := make([]byte, 512)
	original.Read(fileHeader)
	original.Seek(0,0)

	mime_type := http.DetectContentType(fileHeader)
	if mime_type != "image/jpeg" && mime_type != "image/png" && mime_type != "image/bmp" {
		ErrorHandler(p.w,"Unsported format. Supported formats are .jpeg, .png and .bmp.")
		original.Close()
		os.RemoveAll(dir_path)
		return false
	}

	return true
}

func decode_image(extension string, file io.Reader) (image.Image){
	var img image.Image

	if extension == "png"{
		img, _ = png.Decode(file)
	}else if extension == "bmp"{
		img, _ = bmp.Decode(file)
	}else{
		img, _ = jpeg.Decode(file)
	}

	return img
}

func (p processor) rotate(src *image.Image) {
	if p.r.FormValue("rotate") == "" {
		return
	}
	value, _ :=  strconv.ParseFloat(p.r.FormValue("rotate"),32)
	angle := float32(value)

	filter := gift.New(gift.Rotate(angle, color.Transparent, gift.CubicInterpolation))
	dst := image.NewNRGBA(filter.Bounds((*src).Bounds()))
	filter.Draw(dst, *src)
	*src = dst
}

func (p processor) create_custom_filter() *gift.GIFT{
	selected_custom := p.r.Form["custom"]
	custom := gift.New()
	for _, name := range selected_custom {
		custom.Add(base_filters[name])
	}

	for name := range input_filter_descriptions {
		if val := p.r.FormValue(name); val != "" {
			x, _ := strconv.ParseFloat(val,32)
			f := input_filters[name](float32(x))
			custom.Add(f)
		}
	}

	return custom
}

func apply_filters(img *image.Image, custom *gift.GIFT, dir_path string, extension string) map[string]string {
	img_paths := make(map[string]string)
	wg := sync.WaitGroup{}
	mutex := &sync.Mutex{}

	for name := range filters {
		wg.Add(1)
		go func(name string){
			tmp := apply_filter(name,nil,img,dir_path,extension)
			mutex.Lock()
			img_paths[name] = tmp
			mutex.Unlock()
			wg.Done()
		}(name)
	}

	tmp := apply_filter("Custom",custom,img,dir_path,extension)

	wg.Wait()
	img_paths["Custom"] = tmp

	return img_paths
}

func apply_filter(name string, custom *gift.GIFT, src *image.Image, dir string, extension string) string{
	var filter *gift.GIFT

	if custom != nil {
		filter = custom
	}else {
		filter = filters[name]
	}

	dst := image.NewNRGBA(filter.Bounds((*src).Bounds()))
	filter.Draw(dst, *src)

	full_path := path.Join(dir,name) + "." + extension
	file, _ := os.Create(full_path)

	if extension == "png"{
		png.Encode(file, dst)
	}else if extension == "bmp"{
		bmp.Encode(file, dst)
	}else{
		jpeg.Encode(file, dst, &jpeg.Options{Quality:100})
	}

	file.Close()

	return full_path
}

func start_cleaner_goroutine(dir_path string) {
	go func() {
		timer := time.NewTimer(time_available)
		<-timer.C
		os.RemoveAll(dir_path)
	}()
}

func random_string() string {
	const letterBytes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
 	b := make([]byte, 20)
	for i := range b {
		b[i] = letterBytes[rand.Intn(len(letterBytes))]
	}
	return string(b)
}
