package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"time"
)

const timeout = 10
const max_connections = 2
var semaphore = make(chan struct{}, max_connections)

func uploadHandler(w http.ResponseWriter, r *http.Request) {
	start := time.Now()
	done := false

	for ;; {
		select {
		case semaphore <- struct{}{}:
			
			defer func() { <-semaphore }()

			file, header, err := r.FormFile("file")

			if err != nil {
				fmt.Fprintln(w, err)
				return
			}

			defer file.Close()

			name := r.FormValue("name")
			path := "uploads/" + name
			err = os.Mkdir(path, 0777)
			if err != nil {
				fmt.Fprintln(w, err)
			}

			info, err := os.Create(path+"/info.txt")
			if err != nil {
				fmt.Fprintf(w, "Unable to create the info file. ")
				fmt.Fprintln(w, err)
				return
			}
			defer info.Close()

			index := r.FormValue("index")
			email := r.FormValue("email")

			fmt.Fprintln(info,name)
			fmt.Fprintln(info,index)
			fmt.Fprintln(info,email)

			out, err := os.Create(path+"/"+header.Filename)
			if err != nil {
				fmt.Fprintf(w, "Unable to create the file for writing. ")
				fmt.Fprintln(w, err)
				return
			}
			defer out.Close()

			_, err = io.Copy(out, file)
			if err != nil {
				fmt.Fprintln(w, err)
			}

			fmt.Fprintf(w, "File uploaded successfully : ")
			fmt.Fprintf(w, header.Filename)

			done = true

		default:
			if time.Since(start) > time.Second * timeout {
				fmt.Fprintf(w, "Server busy, try again later... ")
				done = true
			}
		}

		if done { break }
	}
}

func main() {
	http.HandleFunc("/", uploadHandler)
	http.ListenAndServe(":8080", nil)
}
