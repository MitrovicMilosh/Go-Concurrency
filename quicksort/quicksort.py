#source: http://www.parallelpython.com/component/option,com_smf/Itemid,1/topic,138.0

import sys, random, time
import pp

def quicksort(a, depth=-1, srv=None):
	if len(a) <= 1:
		return a
	if depth:
		return quicksort([x for x in a if x < a[0]], depth-1, srv) \
				+ [a[0]] + quicksort([x for x in a[1:] if x >= a[0]], depth-1, srv)
	else:
		return [srv.submit(quicksort, (a,))]
    
ppservers = ()
n = int(sys.argv[1])
depth = 0

if len(sys.argv) > 2:
	depth = int(sys.argv[2])
	ncpus = 2**depth-1
	job_server = pp.Server(ncpus, ppservers=ppservers)
else:
	job_server = pp.Server(ppservers=ppservers)

input = []
for i in xrange(n):
	input.append(random.randint(0,n))

start_time = time.time()
outputraw = quicksort(input, depth, job_server)

output = []
for x in outputraw:
	if callable(x):
		output.extend(x())
	else:
		output.append(x)
job_server.wait()
print time.time() - start_time
