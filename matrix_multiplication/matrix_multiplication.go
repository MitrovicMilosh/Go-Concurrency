package main

import (
	"math/rand"
	"time"
	"os"
	"sync"
	"strconv"
	"fmt"
)

type matrix [][]int
var n, num_routines int
var m1, m2, res matrix

func multiply_row(row, num_routines int) {
	for ; row<n; row+=num_routines {
		for i := 0; i < n; i++ {
			for j := 0; j < n; j++ {
				res[row][i] += m1[row][j] * m2[j][i]
			}
		}
	}
}

func multiply_conc(num_routines int) {
	wg := sync.WaitGroup{}
	wg.Add(num_routines)
	for i := 0; i < num_routines; i++ {
		go func(row int){
			multiply_row(row, num_routines)
			wg.Done()
		}(i)
	}
	wg.Wait()
}

func multiply_seq() {
	for i:=0; i<n; i++ {
		for j:=0; j<n; j++{
			for k:=0; k<n; k++{
				res[i][j] += m1[i][k] * m2[k][j]
			}
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

	start := time.Now()
	if(len(os.Args)>2) {
		num_routines,_ = strconv.Atoi(os.Args[2])
		multiply_conc(num_routines)
	}else {
		multiply_seq()
	}
	fmt.Println(time.Since(start).Seconds())
}
