import sys
import random
import multiprocessing
from itertools import starmap, izip, repeat, imap
from operator import mul

def calc_row_of_product_matrix(a_row, b, izip=izip):
    return map(lambda col: sum(starmap(mul,izip(a_row,col))), izip(*b))

def eval_func_tuple(f_args):
    return f_args[0](*f_args[1:])

class multimatrix(list):

    def __mul__(self, b, izip=izip, repeat=repeat):
        pool = multiprocessing.Pool(multiprocessing.cpu_count())
        return pool.map(eval_func_tuple, izip(repeat(calc_row_of_product_matrix), self, repeat(b))) 

class itermatrix(list):

    @staticmethod
    def sumprod(row, col, sum=sum, starmap=starmap, mul=mul):
        return sum(starmap(mul,izip(row,col)))

    def __mul__(self, b, imap=imap, izip=izip):
        return imap(lambda row: imap(lambda col: itermatrix.sumprod(row, col), izip(*b)), self)

def iterate_results(result):
    return[[col for col in row] for row in result]

def random_v(K=1000,min=-1000,max=1000):
    return [random.randint(min,max) for k in range(K)]

def random_m(N=1000, K=1000):
    return [random_v(K) for n in range(N)]

if __name__ == '__main__':
	n=300
	a = random_m(n, n)
	if len(sys.argv) > 1:
		p = itermatrix(a) * itermatrix(a)
		iterate_results(p)	
	else:
		p = multimatrix(a) * multimatrix(a)
