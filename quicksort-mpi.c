#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initArray(int *arr, int n) {
	int i;
	for (i = 0; i < n; ++i)
	{
		arr[i] = rand() % n;
	}
}

void printArray(int *arr, int n) {
	int i;
	for (i = 0; i < n; ++i)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void swap(int *arr, int i, int j) {
	int temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;
}

void quicksortNormal(int *arr, int lo, int hi) {
	/*Se lower continuar menor que upper
		Define x: valor do pivo
		   pivot: index do pivo

	  Executado um for do pivot +1 (retira pivô) até o fim
	  percorrendo todo o array e faz swap quando algum valor é
	  menor que pivo, e soma o index para não levar mais em conta
	  essa posição.

	  Chama novamente o quicksortNormal para repetir do inicio até o pivo
	  e do (pivo +1) até upper
	*/
	if (lo < hi) {
		int x = arr[lo];
		int pivo = lo;
		for (int i = lo + 1; i < hi; ++i)
		{
			if (arr[i] <= x) {
				pivo++;
				swap(arr, pivo, i);
			}
		}
		swap(arr, lo, pivo);

		quicksortNormal(arr, lo, pivo);
		quicksortNormal(arr, pivo + 1, hi);
	}
	// Quando lower = upper retorna 
}

void quicksort(int *arr, int lo, int hi, int rank, int np, int rank_index) {
	// Inicializa destino(Processo a enviar e receber array):
		// destino igual rank + potencia de 1 sob rank_index
		/*No processo 0 dest = 1*/
	int dest = rank + (1 << rank_index);
	printf("Processo: %d - Variável dest:%d\n", rank, dest);

	// Caso o destino seja maior ou igual o numero de processos...
	if (dest >= np) {
		// Chamamos o intercala passando a array, lower e upper
		/*Processo o 0 manda 0 à n*/
		quicksortNormal(arr,lo,hi);
	} else if (lo < hi) {
		//  Se o destino for menor que o número de processos
		int x = arr[lo];
		int pivot = lo;

		for (int i = lo + 1; i < hi; ++i)
		{
			if (arr[i] <= x) {
				pivot++;
				swap(arr, pivot, i);
			}
		}
		swap(arr, lo, pivot);

		if (pivot - lo > hi - pivot - 1) {
			MPI_Send(&arr[pivot + 1], hi - pivot - 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
			quicksort(arr, lo, pivot, rank, np, rank_index + 1);
			MPI_Recv(&arr[pivot + 1], hi - pivot - 1, MPI_INT, dest, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		} else {
			MPI_Send(&arr[lo], pivot - lo, MPI_INT, dest, 1, MPI_COMM_WORLD);
			quicksort(arr, pivot + 1, hi, rank, np, rank_index + 1);
			MPI_Recv(&arr[lo], pivot - lo, MPI_INT, dest, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
		}
	}
}

int main(int argc, char** argv) {
	srand(1707);
	int np, rank;
	int *tempArr;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status msg;

	// Executado pelo rank = 0 mestre
	if (rank == 0) {
		// Selecciona array
		int n;
		scanf("%d",&n);
		// Inicializa Array
		int arr[n];
		initArray(arr, n);

		printArray(arr, n);

		// Inicializa tempo de execũção 
		double start = MPI_Wtime();

		 /*Chama o quicksort passando o vetor, inicio 
		lower half, inicio upper half, o rank do processo, 
		o número de processos, e o rank index*/
		/*Inicialmente o passando o vetor completo com
		lower e upper half de 0 a n, posteriormente, a int
		lower half será o pivô e o rank index igual ao rank(inicialmente) */
		
		quicksort(arr, 0, n, rank, np, 0);

		// Finaliza tempo de execução
		double finish = MPI_Wtime();

		printArray(arr, n);
		printf("N: %d\n", n);
		printf("Time: %.4f seconds\n", finish-start);
	} else {
		int arrSize;
		int source;
		int index_count = 0;
		while(1 << index_count <= rank)
    		index_count ++; 


    	/*
    	Espera para receber a informações do recebimento da mensagem com as informação
		do tamanho do array e processo de origem. Para, dessa maneira, criar o array temporário e 
		preencher o source no Send e Recive
    	*/

		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &msg);
		MPI_Get_count(&msg, MPI_INT, &arrSize);


		source = msg.MPI_SOURCE;
		tempArr = (int *) malloc(sizeof(int)*arrSize);
		MPI_Recv(tempArr, arrSize, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("RC: %d ", rc);
		quicksort(tempArr, 0, arrSize, rank, np, index_count);
		MPI_Send(tempArr, arrSize, MPI_INT, source, 1, MPI_COMM_WORLD);
		free(tempArr);
	}
	// printf("LOL %d\n", rank);
	MPI_Finalize();
	return 0;
}
