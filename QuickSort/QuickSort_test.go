package quicksort

import (
	"testing"
	"time"
	"math/rand"
)

func BenchmarkQuickSortConcurrent(b *testing.B) {
	for i := 0; i < b.N; i++ {
		rand.Seed(time.Now().UTC().UnixNano())
		list := rand.Perm(1000000)
		QuickSortConcurrent(&list,0, len(list)-1)
	}
}

func BenchmarkQuickSortSequential(b *testing.B) {
	for i := 0; i < b.N; i++ {
		rand.Seed(time.Now().UTC().UnixNano())
		list := rand.Perm(1000000)
		QuickSortSequential(&list,0, len(list)-1)
	}
}
