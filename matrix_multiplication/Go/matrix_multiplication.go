package main

import (
	"math/rand"
	"time"
	"os"
	"sync"
)

type matrix [][]int
var n = 700
var m1 =  make(matrix, n)
var m2 =  make(matrix, n)
var res =  make(matrix, n)

var semaphore = make(chan struct{}, 20)

func multiply_conc(row int) {
	for i:=0; i<n; i++{
		res[row][i] = multiply_row_column(row,i)
	}
}

func multiply_row_column(row,col int) int{
	res:=0
	for i:=0; i<n; i++{
		res += m1[row][i] * m2[i][col]
	}
	return res
}

func multiply_seq(row int) {
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
					multiply_conc(row)
					<- semaphore
					wg.Done()
				}(i)
			default:
				multiply_seq(i)
				wg.Done()
			}
		}
		wg.Wait()
	}else{
		for i := 0; i < n; i++ {
			multiply_conc(i)
		}
	}
}


func main(){
	rand.Seed(time.Now().UTC().UnixNano())

	for i:=0; i<n; i++{
		m1[i] = rand.Perm(n)
		m2[i] = rand.Perm(n)
		res[i] = make([]int, n)
	}

	if(len(os.Args)>1) {
		multiply(false)
	}else {
		multiply(true)
	}
}
