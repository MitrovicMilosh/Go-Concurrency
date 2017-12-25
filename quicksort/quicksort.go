package main

import (
	"sync"
	"time"
	"os"
	"math/rand"
	"strconv"
	"fmt"
)

func QuickSortSequential(a *[]int, low, hi int) {
	if hi < low {
		return
	}

	p := partition(a, low, hi)
	QuickSortSequential(a, low, p-1)
	QuickSortSequential(a, p+1, hi)
}

func QuickSortConcurrent(a *[]int, low, hi, depth int) {
	if hi < low {
		return
	}

	p := partition(a, low, hi)

	if depth > 0 {
		wg := sync.WaitGroup{}
		wg.Add(2)
		go func(){
			QuickSortConcurrent(a, low, p-1, depth-1)
			wg.Done()
		}()
		go func(){
			QuickSortConcurrent(a, p+1, hi, depth-1)
			wg.Done()
		}()
		wg.Wait()
	}else{
		QuickSortSequential(a,low, p-1)
		QuickSortSequential(a, p+1, hi)
	}	
}

func partition(a *[]int, low, hi int) int {
	pivot := (*a)[hi]
	i := low - 1

	for j := low; j < hi; j++{
		if (*a)[j] < pivot {
			i++
			swap(a,i,j)
		}
	}

	if (*a)[hi] < (*a)[i + 1] {
		swap(a,hi,i+1)
	}

	return i + 1
}

func swap(a *[]int, i , j int) {
	(*a)[i], (*a)[j] = (*a)[j], (*a)[i]
}

func main(){
	start := time.Now()
	n,_ := strconv.Atoi(os.Args[1])
	
	rand.Seed(time.Now().UTC().UnixNano())
	list := rand.Perm(n)
	fmt.Println(time.Since(start).Seconds())
	start = time.Now()
	if len(os.Args) > 2 {
		depth,_ := strconv.Atoi(os.Args[2])
		QuickSortConcurrent(&list,0, len(list)-1, depth)
	}else {
		QuickSortSequential(&list,0, len(list)-1)
	}
	fmt.Println(time.Since(start).Seconds())
}
