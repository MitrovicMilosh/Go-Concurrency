import sys
import threading
import random

threads = []
n = int(sys.argv[1])
nthreads = int(sys.argv[2])

a = [random.randint(0,n) for i in range(n)]

def separate(low, high):
	pivot = a[low] 
	(a[low],a[high]) = (a[high],a[low])
	last = low
	for i in range(low,high):
		if a[i] <= pivot:
			(a[last],a[i]) = (a[i],a[last])
			last += 1
	(a[last],a[high]) = (a[high],a[last])
	return last

def quicksort(low,hi):
	if hi < low:
		return

	p = separate(low, hi)
	
	if(threading.active_count() < nthreads):
		t1 = threading.Thread(target=quicksort, args=(low,p-1,))
		t2 = threading.Thread(target=quicksort, args=(p+1,hi,))
		threads.append(t1)
		threads.append(t2)
		t1.start()
		t2.start()
	else:
		quicksort(low, p-1)
		quicksort(p+1, hi)

quicksort(0,n-1)		
		
for t in threads:
	t.join()
	