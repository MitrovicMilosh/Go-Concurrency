import sys, random, math, time
import pp

def multiply(row,a,b,c,n):
	for i in range(n):
		for j in range(n):
			c[row][i] += a[row][j]*b[j][i]
	return c[row]
	

n = int(sys.argv[1])
ncpus = int(sys.argv[2])

a = [[random.randint(0,n) for i in range(n)] for i in range(n)]	
b = [[random.randint(0,n) for i in range(n)] for i in range(n)]	
c = [[0 for i in range(n)] for i in range(n)]	

job_server = pp.Server()
job_server.set_ncpus(ncpus)

start_time = time.time()
jobs = []

for i in range(n):
	jobs.append(job_server.submit(multiply, (i,a,b,c,n)))

job_server.wait()
print time.time() - start_time