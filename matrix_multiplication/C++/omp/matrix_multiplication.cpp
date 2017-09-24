#include <omp.h>
#include <cstdlib>
#include <cstring>

using namespace std;
#define READ(f) freopen(f, "r", stdin)
#define WRITE(f) freopen(f, "w", stdout)
#define pks printf("Case %d: ",++ks);

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
	#pragma omp parallel for schedule(dynamic,50) collapse(2) private(i,j,k) shared(a,b,c)
	for(i=0;i<n;i++)for( j=0;j<n;j++)for(k=0;k<n;k++)d[i][j]+=a[i][k]*b[k][j];
}

int main(int argc, char* argv[]) {
	int n=300;
	generate_matrix(n);
	matrix_mult_parallel(n);	

	return 0;	
}
