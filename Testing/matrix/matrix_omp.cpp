#include <omp.h>
#include <cstdlib>
#include <cstring>

using namespace std;
#define mx 1002

int a[mx][mx];
int b[mx][mx];
int c[mx][mx];
int d[mx][mx];

void generate_matrix(int n)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			a[i][j]=rand()%100;
			b[i][j]=rand()%100;
		}
	}
}


void matrix_mult_parallel(int n)
{
	memset(d,0,sizeof d);
	int i,j,k;
	#pragma omp parallel for schedule(dynamic,50) collapse(1) private(i,j,k) shared(a,b,c)
	for(i=0;i<n;i++)for( j=0;j<n;j++)for(k=0;k<n;k++)d[i][j]+=a[i][k]*b[k][j];
}

int main(int argc, char* argv[]) {
	int n=atoi(argv[1]);
	if(argc>2)
		omp_set_num_threads(atoi(argv[2]));
	else
		omp_set_num_threads(1);
	generate_matrix(n);
	matrix_mult_parallel(n);	

	return 0;	
}
