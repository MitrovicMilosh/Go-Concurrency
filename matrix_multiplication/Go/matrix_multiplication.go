package main

import (
	"math/rand"
	"time"
	"os"
	"sync"
	"strconv"
)

type matrix [][]int
var n, num_routines int
var m1, m2, res matrix
var semaphore chan struct{}

func multiply_row_column(row,col int) int{
	res:=0
	for i:=0; i<n; i++{
		res += m1[row][i] * m2[i][col]
	}
	return res
}

func multiply_row(row int) {
	for i:=0; i<n; i++{
		res[row][i] = multiply_row_column(row,i)
	}
}

func multiply(is_concurrent bool) {
	if is_concurrent {
		wg := sync.WaitGroup{}
		wg.Add(n)
		for i := 0; i < n; i++ {
			select{
			case semaphore <- struct{}{}:
				go func(row int){
					multiply_row(row)
					<- semaphore
					wg.Done()
				}(i)
			default:
				multiply_row(i)
				wg.Done()
			}
		}
		wg.Wait()
	}else{
		for i := 0; i < n; i++ {
			multiply_row(i)
		}
	}
}

func main(){
	n,_ = strconv.Atoi(os.Args[1])

	m1 =  make(matrix, n)
	m2 =  make(matrix, n)
	res =  make(matrix, n)

	rand.Seed(time.Now().UTC().UnixNano())

	for i:=0; i<n; i++{
		m1[i] = rand.Perm(n)
		m2[i] = rand.Perm(n)
		res[i] = make([]int, n)
	}

	if(len(os.Args)>2) {
		num_routines,_ = strconv.Atoi(os.Args[2])
		semaphore = make(chan struct{}, num_routines)
		multiply(true)
	}else {
		multiply(false)
	}
}
