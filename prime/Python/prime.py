import sys
import math
import threading

class prmfinder(threading.Thread):
	n = int(sys.argv[1])
	nthreads = int(sys.argv[2])
	thrdlist = [] # list of all instances of this class
	prime = (n+1) * [1] # 1 means assumed prime, until find otherwise
	nextk = 2 # next value to try crossing out with
	nextklock = threading.Lock()
	def __init__(self,id):
		threading.Thread.__init__(self)
		self.myid = id
	def run(self):
		lim = math.sqrt(prmfinder.n)
		nk = 0
		while 1:
			# find next value to cross out with
			prmfinder.nextklock.acquire()
			k = prmfinder.nextk
			prmfinder.nextk += 1
			prmfinder.nextklock.release()
			if k > lim: break
			nk += 1 # increment workload data
			if prmfinder.prime[k]: # now cross out
				r = prmfinder.n / k
				for i in range(2,r+1):
					prmfinder.prime[i*k] = 0

def main():
	for i in range(prmfinder.nthreads):
		pf = prmfinder(i) # create thread i
		prmfinder.thrdlist.append(pf)
		pf.start()
	for thrd in prmfinder.thrdlist: thrd.join()

if __name__ == '__main__':
	main()
