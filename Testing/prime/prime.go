package main

import (
	"math"
	"sync"
	"os"
	"strconv"
)

var num_goroutines int

func Prime(list *[]bool, n int, is_concurrent bool){
	sqrt := int(math.Sqrt(float64(n)))
	first := 0
	step := int(n/ num_goroutines)
	last := step
	wg := sync.WaitGroup{}
	wg.Add(num_goroutines)

	if is_concurrent {
		for i:=0; i< num_goroutines-1; i++{
			go mark_prime(list,first,last,sqrt,&wg,true)
			first = last + 1
			last += step
		}
	}else {
		for i:=0; i< num_goroutines-1; i++{
			mark_prime(list,first,last,sqrt,&wg,false)
			first = last + 1
			last += step
		}
	}
	
	mark_prime(list,first,n-1,sqrt,&wg,false)
	wg.Wait()
}

func mark_prime(list *[]bool, first, last, sqrt int, wg *sync.WaitGroup, is_concurrent bool){
	for i:=2; i<=sqrt && i*i<=last; i++{
		if !(*list)[i] {
			var j int
			if (i*i < first) {
				if((first - i*i)%i == 0){
					j = i*i + ((first-i*i)/i)*i
				}else {
					j = i*i + ((first-i*i)/i+1)*i
				}
			}else {
				j = i*i
			}
			if is_concurrent {
				for ; j<=last && !(*list)[i]; j+=i {

					(*list)[j] = true
				}
			} else {
				for ; j<=last; j+=i {

					(*list)[j] = true
				}
			}
		}
	}
	wg.Done()
}

func main() {
	n ,_ := strconv.Atoi(os.Args[1])
	list := make([]bool, n+1, n+1)
	num_goroutines = 1	

	if len(os.Args) > 2 {
		num_goroutines,_ = strconv.Atoi(os.Args[2])
		Prime(&list,n,true)
	}else {
		Prime(&list,n,false)
	}

}
