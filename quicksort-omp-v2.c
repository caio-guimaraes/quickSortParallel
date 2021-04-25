#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define n 100000000
int arr[n];


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


/* This function takes first element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition(int* vet, int low, int high){
	int i, j, temp, key;
	key = vet[low];
	i= low + 1;
	j= high;
	while(1){
		while(i < high && key >= vet[i])
    			i++;
		while(key < vet[j])
    			j--;
		if(i < j){
			temp = vet[i];
			vet[i] = vet[j];
			vet[j] = temp;
		}
		else{
			temp= vet[low];
			vet[low] = vet[j];
			vet[j]= temp;
			return(j);
		}
	}
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quicksort(int* vet, int low, int high)
{
	int j;

	if(low < high){
		j = partition(vet, low, high);

		quicksort(vet, low, j - 1);
		quicksort(vet, j + 1, high);

	}
}

void quickSort_parallel(int *vet, int low, int high, int numThreads){
  int j = partition(vet,low,high);// returns the pivot element
		#pragma omp parallel sections num_threads(numThreads)
		{
			#pragma omp section
			{
        			quicksort(vet,low, j - 1);//Thread 1
    			}
			#pragma omp section
			{
        			quicksort(vet, j + 1, high);//Thread 2
   			}
		}
}

int main(int argc, char **argv)
{

  int nthreads = atoi(argv[1]), rank;
  char *nomeArq = argv[2];
  double ti, tf;
  clock_t t;

  int *vet, tamanho;

  vet = inicializa(nomeArq, &tamanho, vet);

	// int start_s=clock();//start time
  t = clock();
	// for(int i=0;i<n;i++){
	// 	arr[i]=rand()%n;//filling random value
	// }

	quickSort_parallel(vet, 0, tamanho-1, nthreads);
	// int stop_s=clock();//end Time
	// printf("Time taken: %.6fs\n", (double)(stop_s - start_s)/CLOCKS_PER_SEC);

  t = clock() - t;
  printf("\n-------------- Vetor Ordenado --------------\n");
  imprimeVetor(vet, tamanho);
  printf("\nTempo de execucao: %lf\n", ((double)t)/((CLOCKS_PER_SEC))); //conversão para double
}
