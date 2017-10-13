package main

import (
	"sync"
	"time"
	"os"
	"math/rand"
	"strconv"
)

var semaphore chan struct{}

func QuickSortSequential(a *[]int, low, hi int) {
	if hi < low {
		return
	}

	p := partition(a, low, hi)
	QuickSortSequential(a, low, p-1)
	QuickSortSequential(a, p+1, hi)
}

func QuickSortConcurrent(a *[]int, low, hi int) {

	if hi < low {
		return
	}

	p := partition(a, low, hi)

	wg := sync.WaitGroup{}
	wg.Add(2)

	select{
	case semaphore <- struct{}{}:
		go func(){
			QuickSortConcurrent(a, low, p-1)
			<- semaphore
			wg.Done()
		}()

	default:
		QuickSortSequential(a,low, p-1)
		wg.Done()
	}

	select{
	case semaphore <- struct{}{}:
		go func(){
			QuickSortConcurrent(a, p+1, hi)
			<- semaphore
			wg.Done()
		}()

	default:
		QuickSortSequential(a, p+1, hi)
		wg.Done()
	}

	wg.Wait()
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
	n,_ := strconv.Atoi(os.Args[1])
	
	rand.Seed(time.Now().UTC().UnixNano())
	list := rand.Perm(n)
	
	if len(os.Args) > 2 {
		num_routines,_ := strconv.Atoi(os.Args[2])
		semaphore = make(chan struct{}, num_routines)
		QuickSortConcurrent(&list,0, len(list)-1)
	}else {
		QuickSortSequential(&list,0, len(list)-1)
	}
}