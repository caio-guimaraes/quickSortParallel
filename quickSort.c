#include <stdio.h>
#include <stdlib.h>

// PSEUDO CODIGO

void swap(double a1, double a2){
  double aux = a1;
  a1 = a2;
  a2 = aux;
}


// Algorithm 10.5
// The sequential Algorithm of Quick Sorting
void QuickSort(double A[], int i1, int i2) {
  if ( i1 < i2 ){
    double pivot = A[i1];
    int is = i1;
    double aux;
    for ( int i = i1+1; i<i2; i++ )
      if ( A[i] <= pivot ) {
          is = is + 1;
          printf("A[is]: %lf A[i]:%lf\n", A[is], A[i]);
          // swap(A[is], A[i]);
          aux = A[is];
          A[is] = A[i];
          A[i] = aux;
          printf("Pos Troca\n");
          printf("A[is]: %lf A[i]:%lf\n\n", A[is], A[i]);
      }
    // swap(A[i1], A[is]);
    aux = A[i1];
    A[i1] = A[is];
    A[is] = aux;
    QuickSort (A, i1, is);
    QuickSort (A, is+1, i2);
  }
}

int main(int argc, char const *argv[]) {
  // double[] vetor = [5, 7, 9, 4, 8, 1];

  int tamanho, i;
	printf("Digite o tamanho do vetor: ");
	scanf("%d", &tamanho);
	double *vetor = (double*) malloc(tamanho*sizeof(double));
	printf("Preencha o vetor: ");
	for(i=0; i<tamanho; i++)
		scanf("%lf", &vetor[i]);

  // printf("Vetor original: \n");
  // imprimiVetor(vetor, tamanho);

  QuickSort(vetor, 0, tamanho - 1);

  // printf("\nVetor ordenado: \n");
  // imprimiVetor(vetor, tamanho);

  return 0;
}
