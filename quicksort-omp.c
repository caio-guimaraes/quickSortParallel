#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>

int *inicializa(char nomeArq[], int *tam, int *vet){
  FILE *arq;
	arq = fopen(nomeArq,"r");
	int i = 0;
	if (arq != NULL) {
    fscanf(arq, "%d", tam);
    vet = (int *)malloc(sizeof(int)*(*tam));
    while(!feof(arq) && i < *tam){
      fscanf(arq,"%d", &vet[i]);
      i++;
    }
    fclose(arq);
  }
  return vet;
}

void imprimeVetor(int *vet, int tam) {
	int i;
	for (i = 0; i < tam; ++i)
		printf("%d ", vet[i]);
	printf("\n\n");
}


void quickSort_parallel_internal(int* vet, int inicio, int fim){
	int i = inicio, j = fim;
	int tmp;
	int pivo = vet[inicio];

		while (i <= j) {
			while (vet[i] < pivo)
				i++;
			while (vet[j] > pivo)
				j--;
      // printf("vet[i]: %d, i: %d\n", vet[i], i);
      // printf("vet[j]: %d, j: %d\n", vet[j], j);
      //printf("%d %d\n", i, j);
      if (i <= j) {
				tmp = vet[i];
				vet[i] = vet[j];
				vet[j] = tmp;
				i++;
				j--;
			}
      //imprimeVetor(vet, 10);

		}

	if (inicio < fim){
		if (inicio < j)
      quickSort_parallel_internal(vet, inicio, j);
		if (i < fim)
      quickSort_parallel_internal(vet, i, fim);
	}else{
		#pragma omp task
		{
      quickSort_parallel_internal(vet, inicio, j);
    }
		#pragma omp task
		{
      quickSort_parallel_internal(vet, i, fim);
    }
	}
}


void quickSort_parallel(int* vet, int lenArray, int numThreads){
	#pragma omp parallel num_threads(numThreads)
	{
		#pragma omp single nowait
		{
			quickSort_parallel_internal(vet, 0, lenArray-1);
		}
	}

}


int main(int argc, char **argv) {
  int nthreads = atoi(argv[1]), rank;
  char *nomeArq = argv[2];
  double ti, tf;
  clock_t t;

  int *vet, tamanho;

  vet = inicializa(nomeArq, &tamanho, vet);
  
  // Inicio de marcação de tempo
  t = clock();

  quickSort_parallel(vet, tamanho, nthreads);
  
  // Fim de marcação de tempo
  t = clock() - t;

  imprimeVetor(vet, tamanho);
  printf("\nTempo de execucao: %lf\n", ((double)t)/((CLOCKS_PER_SEC))); //conversão para double
  // #pragma omp parallel num_threads(nthreads)
  // {
  //   rank = omp_get_thread_num();
  //   // printf("thread atual: %d \n", rank);
  //   if(rank == 0){
  //     int *vet, tamanho;
  //     vet = inicializa(nomeArq, &tamanho, vet);
  //     // printf("arq: %s, tam: %d \n\n", nomeArq, tamanho);
  //     imprimeVetor(vet, tamanho);
  //   }
  // }
}
