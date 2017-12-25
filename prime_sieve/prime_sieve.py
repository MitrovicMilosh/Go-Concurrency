import sys, random, math, time
import pp

def worker(prime,start,end,limit):
	for i in range (2,limit):
		minJ = ((start+i-1)/i)*i;
		if minJ < i*i:
			minJ = i*i;	
		j = minJ		
		while j <= end:
			prime[j] = 0
			j += i
			
	return prime
	
n = int(sys.argv[1])
nthreads = int(sys.argv[2])
limit = int(math.sqrt(n)) + 1
ones = (n+1) * [1]
prime = dict(zip(range(n),ones))  
	
job_server = pp.Server()
job_server.set_ncpus(nthreads)

jobs = []
k = 1;
start_time = time.time()
for i in range(nthreads):
	nq = n / nthreads;
	nr = n % nthreads;

	start = k;
	if i < nr:
		k = k + nq + 1;
	else:
		k = k + nq;
		end = k-1;
	
	jobs.append(job_server.submit(worker, (dict(prime.items()[start:end+1]), start, end, limit)))

job_server.wait()
print time.time() - start_time
