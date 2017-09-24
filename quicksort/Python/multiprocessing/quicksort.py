# Quicksort and test code, based on Python multiprocessing class and Queue
# code is incomplete, as some special cases such as empty subarrays
# need to be accounted for
# usage: python QSort.py n nthreads
# where we wish to test the sort on a random list of n items,
# using nthreads to do the work

import sys
import random
from multiprocessing import Process, Array, Queue

class glbls: # globals, other than shared
	nthreads = int(sys.argv[2])
	thrdlist = [] # list of all instances of this class
	r = random.Random(9876543)

def sortworker(id,x,q):
	chunkinfo = q.get()
	i = chunkinfo[0]
	j = chunkinfo[1]
	k = chunkinfo[2]
	if k < glbls.nthreads - 1: # need more splitting?
		splitpt = separate(x,i,j)
		q.put((splitpt+1,j,k+1))
		# now, what do I sort?
		rightend = splitpt + 1
	else: rightend = j
	tmp = x[i:(rightend+1)] # need copy, as Array type has no sort() method
	tmp.sort()
	x[i:(rightend+1)] = tmp

def separate(xc, low, high): # common algorithm; see Wikipedia
	pivot = xc[low] # would be better to take, e.g., median of 1st 3 elts
	(xc[low],xc[high]) = (xc[high],xc[low])
	last = low
	for i in range(low,high):
		if xc[i] <= pivot:
			(xc[last],xc[i]) = (xc[i],xc[last])
			last += 1
	(xc[last],xc[high]) = (xc[high],xc[last])
	return last

def main():
	tmp = []
	n = int(sys.argv[1])
	for i in range(n): tmp.append(glbls.r.uniform(0,1))
	x = Array('d',tmp)
	# work items have form (i,j,k), meaning that the given array chunk
	# corresponds to indices i through j of x, and that this is the kth
	# chunk that has been created, x being the 0th
	q = Queue() # work queue
	q.put((0,n-1,0))
	for i in range(glbls.nthreads):
		p = Process(target=sortworker, args=(i,x,q))
		glbls.thrdlist.append(p)
		p.start()
	for thrd in glbls.thrdlist: thrd.join()
	if n < 25: print x[:]

if __name__ == '__main__':
	main()
