#include "matrixnn.hpp"
#include <thread>

int main(int argc, char *argv[]){
		
	int size = 700;
	
	MatrixNN a(size),b(size),c(size);
	a.FillRandom();
	b.FillRandom();
	
	if(argc>1)
		c = a.SlowMult(b);
	else		
		c = a.QuickMult(b);
			
	return 0;
}
