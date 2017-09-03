package directorytraversal

import (
"fmt"
"io/ioutil"
"os"
"path/filepath"
"sync"
)

func TraverseConcurrent(dirs []string) {

	roots := dirs
	if len(roots) == 0 {
		roots = []string{"."}
	}

	fileSizes := make(chan int64)
	var wg sync.WaitGroup

	for _, root := range roots {
		wg.Add(1)
		go walkDir(root, &wg, fileSizes)
	}
	go func() {
		wg.Wait()
		close(fileSizes)
	}()


	var nfiles, nbytes int64
	for {
		size, ok := <-fileSizes
			if !ok {
				break
			}
			nfiles++
			nbytes += size
	}

	//fmt.Printf("%d files  %.1f GB\n", nfiles, float64(nbytes)/1e9)
}

func walkDir(dir string, wg *sync.WaitGroup, fileSizes chan<- int64) {
	defer wg.Done()
	for _, entry := range dirents(dir) {
		if entry.IsDir() {
			wg.Add(1)
			subdir := filepath.Join(dir, entry.Name())
			go walkDir(subdir, wg, fileSizes)
		} else {
			fileSizes <- entry.Size()
		}
	}
}

var semaphore = make(chan struct{}, 20)

func dirents(dir string) []os.FileInfo {
	semaphore <- struct{}{}
	defer func() { <-semaphore }()

	entries, err := ioutil.ReadDir(dir)
	if err != nil {
		fmt.Fprintf(os.Stderr, "dt conc: %v\n", err)
		return nil
	}
	return entries
}


//***************************************

func TraverseSequential(dirs []string) {

	roots := dirs
	if len(roots) == 0 {
		roots = []string{"."}
	}

	fileSizes := make(chan int64)
	go func() {
		for _, root := range roots {
			walkDirSeq(root, fileSizes)
		}
		close(fileSizes)
	}()

	var nfiles, nbytes int64
	for size := range fileSizes {
		nfiles++
		nbytes += size
	}

	//fmt.Printf("%d files  %.1f GB\n", nfiles, float64(nbytes)/1e9)
}

func walkDirSeq(dir string, fileSizes chan<- int64) {
	for _, entry := range direntsSeq(dir) {
		if entry.IsDir() {
			subdir := filepath.Join(dir, entry.Name())
			walkDirSeq(subdir, fileSizes)
		} else {
			fileSizes <- entry.Size()
		}
	}
}

func direntsSeq(dir string) []os.FileInfo {
	entries, err := ioutil.ReadDir(dir)
	if err != nil {
		fmt.Fprintf(os.Stderr, "dt seq: %v\n", err)
		return nil
	}
	return entries
}
