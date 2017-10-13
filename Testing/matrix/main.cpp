#include "matrixnn.hpp"
#include <thread>

int main(int argc, char *argv[]){
		
	int size = atoi(argv[1]);
	
	MatrixNN a(size),b(size),c(size);
	a.FillRandom();
	b.FillRandom();
		
	if(argc>2)
		c = a.QuickMult(b,atoi(argv[2]));
	else		
		c = a.SlowMult(b);
			
	return 0;
}
