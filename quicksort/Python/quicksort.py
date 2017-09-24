# Author: Vitalii Vanovschi
# Desc: This program demonstrates parallel version of quicksort algorithm
# implemented using pp module
# Parallel Python Software: http://www.parallelpython.com

import sys, random
import pp

def quicksort(a, depth=-1, srv=None):
    if len(a) <= 1:
        return a
    if depth:
        return quicksort([x for x in a if x < a[0]], depth-1, srv) \
                + [a[0]] + quicksort([x for x in a[1:] if x >= a[0]], depth-1, srv)
    else:
        return [srv.submit(quicksort, (a,))]
    

# tuple of all parallel python servers to connect with
#ppservers = ("*",)
#ppservers = ("10.0.0.1",)
ppservers = ()

# set depth to a positive integer to create 2^n PP jobs 
# or to -1 to avoid using PP
if len(sys.argv) > 1:
	depth = int(sys.argv[1])
else:
	depth = -1

if len(sys.argv) > 2:
    ncpus = int(sys.argv[2])
    # Creates jobserver with ncpus workers
    job_server = pp.Server(ncpus, ppservers=ppservers)
else:
    # Creates jobserver with automatically detected number of workers
    job_server = pp.Server(ppservers=ppservers)

n = 1000000
input = []
for i in xrange(n):
    input.append(random.randint(0,n))

outputraw = quicksort(input, depth, job_server)

output = []
for x in outputraw:
    if callable(x):
        output.extend(x())
    else:
        output.append(x)
