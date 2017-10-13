import sys
import math
import threading

threads = []
n = int(sys.argv[1])
nthreads = int(sys.argv[2])
prime = (n+1) * [1]
limit = int(math.sqrt(n)) + 1
def worker(start,end,limit):
	for i in range (2,limit):
		if prime[i] == 0:
			continue;
		
		minJ = ((start+i-1)/i)*i;
		if minJ < i*i:
			minJ = i*i;	
		j = minJ
		while j <= end:
			prime[j] = 0
			j += i
	return

k = 1;
for i in range(nthreads):
	nq = n / nthreads;
	nr = n % nthreads;

	start = k;
	if i < nr:
		k = k + nq + 1;
	else:
		k = k + nq;
		end = k-1;

	t = threading.Thread(target=worker, args=(start,end,limit,))
	threads.append(t)
	t.start()

for i in range(nthreads):
	threads[i].join()

