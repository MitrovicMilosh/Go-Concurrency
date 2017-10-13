import sys
import threading
import random

threads = []
n = int(sys.argv[1])
nthreads = int(sys.argv[2])

a = [[random.randint(0,n) for i in range(n)] for i in range(n)]	
b = [[random.randint(0,n) for i in range(n)] for i in range(n)]	
c = [[0 for i in range(n)] for i in range(n)]	

def multiply(row):
	for i in range(n):
		for j in range(n):
			c[row][i] += a[row][j]*b[j][i]
	
for i in range(n):
	if(threading.active_count() < nthreads):
		t = threading.Thread(target=multiply, args=(i,))
		threads.append(t)
		t.start()
	else:
		multiply(i)
	
for t in threads:
	t.join()
	