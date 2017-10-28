//source: https://github.com/Shafaet/OpenMP-Examples/blob/master/Parallel%20Matrix%20Multiplication.cpp

#include <omp.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>

using namespace std;
int **a, **b, **c;

void generate_matrix(int n)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			a[i][j]=rand()%n;
			b[i][j]=rand()%n;
		}
	}
}

void matrix_mult_parallel(int n)
{
	int i,j,k;
	#pragma omp parallel for schedule(dynamic,50) collapse(1) private(i,j,k) shared(a,b,c)
	for(i=0;i<n;i++) for( j=0;j<n;j++) for(k=0;k<n;k++) c[i][j]+=a[i][k]*b[k][j];	
}

int main(int argc, char* argv[]) {
	int n=atoi(argv[1]);
	a = new int*[n];
	b = new int*[n];
	c = new int*[n];

	for(int i = 0; i < n; i++){
		a[i] = new int[n];
		b[i] = new int[n];
		c[i] = new int[n]{0};
	}
	
	if(argc>2)
		omp_set_num_threads(atoi(argv[2]));
	else
		omp_set_num_threads(1);
		
	generate_matrix(n);
	matrix_mult_parallel(n);	

	return 0;	
}
