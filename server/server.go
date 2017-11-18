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
)

const timeout = 10 * time.Second
const time_available = 30 * time.Second
const max_connections = 2
const max_file_size = 1025*1024
var semaphore = make(chan struct{}, max_connections)

var filters = map[string]*gift.GIFT{

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
	"Lighten":  	gift.Brightness(10),
	"Darken":  		gift.Brightness(-10),
	"Contrast":  	gift.Contrast(30),
	"Sunset":      	gift.ColorBalance(30, -10, -10),
	"Acid":      	gift.ColorBalance(20, 20, -10),
	"Sapphire":     gift.ColorBalance(-10, -10, 30),
	"Blur":       	gift.GaussianBlur(0.6),
	"Vivid":     	gift.Saturation(50),
	"Grey":			gift.Saturation(-50),
	"Fog":			gift.Contrast(-25),
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

type images struct{
	Filters map[string] string
	Base_filters map[string] string
	Original string
}

func DefaultHandler(w http.ResponseWriter, r *http.Request) {
	// get filter names and images
	m := make(map[string]string)
	for name := range filters {
		image_path := path.Join("data","images","filters",name+".jpeg")
		m[name] = image_path
	}

	m2 := make(map[string]string)
	for name := range base_filters {
		image_path := path.Join("data","images","base_filters",name+".jpeg")
		m2[name] = image_path
	}

	data := images{Filters : m, Base_filters: m2}

	// execute template
	tmpl, _ := template.ParseFiles("index.html")
	tmpl.ExecuteTemplate(w, "index", data)
}

func ImageHandler(w http.ResponseWriter, r *http.Request) {
	start := time.Now()
	done := false

	for ;; {
		select {
		case semaphore <- struct{}{}:

			defer func() { <-semaphore }()

			// opening url or source file
			var filename string
			var file_size int64
			var file io.Reader
			if url := r.FormValue("url"); url != "" {
				response, err := http.Get(url)
				if err != nil {
					ErrorHandler(w,"Unable to locate file from url.")
					return
				}

				defer response.Body.Close()
				filename = url
				file_size = response.ContentLength
				file = response.Body

			} else {
				f, header, err := r.FormFile("file")
				if err != nil {
					ErrorHandler(w,"No file selected.")
					return
				}

				defer f.Close()
				filename = header.Filename
				file_size = r.ContentLength
				file = f
			}

			// check file size
			if file_size > max_file_size {
				ErrorHandler(w,"File size to large. Maximum image size is 1 MB.")
				return
			}

			// creating user directory
			dir := path.Join("data","tmp",random_string())
			os.Mkdir(dir, 0777)

			// creating and copying the file
			name_array := strings.Split(filename,".")
			extension := strings.ToLower(name_array[len(name_array)-1])

			out_name := path.Join(dir,"Original."+extension)
			out, _ := os.Create(out_name)

			io.Copy(out, file)
			defer out.Close()

			// check file type
			out.Seek(0,0)
			fileHeader := make([]byte, 512)
			out.Read(fileHeader)

			mime_type := http.DetectContentType(fileHeader)
			if mime_type != "image/jpeg" && mime_type != "image/png" && mime_type != "image/bmp" {
				ErrorHandler(w,"Unsported format. Supported formats are .jpeg, .png and .bmp.")
				out.Close()
				os.RemoveAll(dir)
				return
			}

			// decoding source image
			out.Seek(0,0)
			img := decode_image(extension,out)

			// rotate image
			if r.FormValue("rotate") != "" {
				value, _ :=  strconv.ParseFloat(r.FormValue("rotate"),32)
				angle := float32(value)
				rotate(&img,angle)
			}

			// get selected filters for custom filter
			custom := r.Form["custom"]
			if len(custom) > 0 {
				filters["Custom"] = gift.New()
				for _, name := range custom {
					filters["Custom"].Add(base_filters[name])
				}
			}

			// applying filters and getting result image paths
			m := make(map[string]string)
			wg := sync.WaitGroup{}
			mutex := sync.Mutex{}
			for name := range filters {
				wg.Add(1)
				go func(name string){
					tmp :=  apply_filter(name,&img,dir,extension)
					mutex.Lock()
					m[name] = tmp
					mutex.Unlock()
					wg.Done()
				}(name)
			}
			wg.Wait()

			delete(filters, "Custom")

			// executing template
			data := images{Filters : m, Original: out_name }
			tmpl, _ := template.ParseFiles("results.html")
			tmpl.ExecuteTemplate(w, "results", data)

			// start timer goroutine for cleaning tmp folders
			go func() {
				timer := time.NewTimer(time_available)
				<-timer.C
				os.RemoveAll(dir)
			}()

			done = true

		default:
			if time.Since(start) > timeout {
				ErrorHandler(w,"Server busy, try again later... ")
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

func rotate(src *image.Image, angle float32){
	filter := gift.New(gift.Rotate(angle, color.Transparent, gift.CubicInterpolation))
	dst := image.NewNRGBA(filter.Bounds((*src).Bounds()))
	filter.Draw(dst, *src)
	*src = dst
}

func apply_filter(name string, src *image.Image, dir string, extension string) string{
	filter := filters[name]
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

func random_string() string {
	const letterBytes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
 	b := make([]byte, 20)
	for i := range b {
		b[i] = letterBytes[rand.Intn(len(letterBytes))]
	}
	return string(b)
}

func main() {
	fmt.Println("Starting server...")
	rand.Seed(time.Now().UTC().UnixNano())
	http.HandleFunc("/", DefaultHandler)
	http.HandleFunc("/results", ImageHandler)
	http.Handle("/data/", http.StripPrefix("/data/", http.FileServer(http.Dir("data"))))
	http.ListenAndServe(":8080", nil)
}
