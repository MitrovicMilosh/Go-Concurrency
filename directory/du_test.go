package directorytraversal

import (
	"testing"
)

func BenchmarkTraverseConcurrent(b *testing.B) {
	for i:=0; i<b.N; i++{
		niz := []string{"D:New Folder"}
		TraverseConcurrent(niz)
	}
}

func BenchmarkTraverseSequential(b *testing.B) {
	for i:=0; i<b.N; i++{
		niz := []string{"D:New Folder"}
		TraverseSequential(niz)
	}
}