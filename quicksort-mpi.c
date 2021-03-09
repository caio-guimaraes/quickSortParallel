#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void inicializaVetor(int *vet, int n) {
	int i;
	for (i = 0; i < n; ++i)
		vet[i] = rand() % n;
}

void inicializaVetor_v2(int *vet, int n){
	int i;
	for (i = 0; i < n; i++)
		scanf("%d", &vet[i]);
}

void imprimeVetor(int *vet, int n) {
	int i;
	for (i = 0; i < n; ++i)
		printf("%d ", vet[i]);
	printf("\n\n");
}

void troca(int *vet, int i, int j) {
	int temp = vet[i];
	vet[i] = vet[j];
	vet[j] = temp;
}
/*
	define o pivo como o primeiro elemento do vetor
	passa todos os valores menores que o pivo para a esquerda,
	e retorna o indice do pivo
*/
int particiona (int *vet, int inicio, int fim) {
	int i = inicio, pivo = vet[inicio], temp;
	for (int j = inicio+1; j < fim; j++){
		if (vet[j] <= pivo) {
			i++;
			troca(vet, i, j);
		}
	}
	troca(vet, inicio, i);
	return i;
}

void quicksort(int *vet, int inicio, int fim) {
	if (inicio < fim) {
		int pivo = particiona(vet, inicio, fim);
		quicksort(vet, inicio, pivo);   						// chamada recursiva para a parte esquerda do vetor
		quicksort(vet, pivo + 1, fim);							// chamada recursiva para a parte esquerda do vetor
	}
	// Quando lower = upper retorna
}

void quicksortMPI(int *vet, int inicio, int fim, int rank, int np, int rank_index) {
		/*
			Escolha do processo destino(Processo a enviar e receber vetor):
		 	destino igual a rank + bit 1 deslocado para a esquerda rank_index vezes: alternando entre potencias de 2
		*/
	int dest = rank + (1 << rank_index);

	// Caso o destino seja maior ou igual o numero de processos executa o quicksort
	if (dest >= np) {
		quicksort(vet,inicio,fim);
	}else if (inicio < fim) {											//  Se o destino for menor que o número de processos faz a partição do vetor e envia para os demais processos
		int pivo = particiona(vet, inicio, fim);

		if (pivo - inicio > fim - pivo - 1) {
			MPI_Send(&vet[pivo + 1], fim - pivo - 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
			quicksortMPI(vet, inicio, pivo, rank, np, rank_index + 1);
			MPI_Recv(&vet[pivo + 1], fim - pivo - 1, MPI_INT, dest, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		} else {
			MPI_Send(&vet[inicio], pivo - inicio, MPI_INT, dest, 1, MPI_COMM_WORLD);
			quicksortMPI(vet, pivo + 1, fim, rank, np, rank_index + 1);
			MPI_Recv(&vet[inicio], pivo - inicio, MPI_INT, dest, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
}

int main(int argc, char** argv) {
	int np, rank;
	int *vet_aux;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status msg;

	double ti, ti_2, tf, tf_2;

	// Rank 0 -> Mestre
	if (rank == 0) {
		// Selecciona tamanho do vetor
		int n;
		scanf("%d", &n);

		// Inicializa vetor
		int vet[n];
		inicializaVetor_v2(vet, n);

		// Inicializa tempo de execũção
		ti = MPI_Wtime();
		quicksortMPI(vet, 0, n, rank, np, 0);

		// Finaliza tempo de execução
		tf = MPI_Wtime();

		printf("Vetor ordenado: \n");
		imprimeVetor(vet, n);
		printf("N: %d\n", n);
		printf("Tempo de Execução: %.4f seconds\n", tf-ti);

		int i, end = -1;
		for(i=0; i<np; i++)
			MPI_Send(&end, 1, MPI_INT, i, 2, MPI_COMM_WORLD);


	} else {
		int tamanho, origem, index_count = 0;
		while(1 << index_count <= rank)
    		index_count++;

  	/*
    	Comando bloqueante, que fica aguardando a chegada de uma mensagem, de onde seram
			extraídas as informação do tamanho do vetor e processo de origem. Para, dessa maneira,
			criar o vetor temporário e preencher a origem no Send e Recive
  	*/

		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &msg);

		/*
			Se a tag for 2 significa que o processo 0 já recebeu o vetor ordenado e
			podemos finalizar os processos que não foram utilizados
		*/

		if(msg.MPI_TAG == 2){
			MPI_Finalize();
			return 0;
		}

		MPI_Get_count(&msg, MPI_INT, &tamanho);

		origem = msg.MPI_SOURCE;
		vet_aux = (int *) malloc(sizeof(int)*tamanho);
		MPI_Recv(vet_aux, tamanho, MPI_INT, origem, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		quicksortMPI(vet_aux, 0, tamanho, rank, np, index_count);
		MPI_Send(vet_aux, tamanho, MPI_INT, origem, 1, MPI_COMM_WORLD);
		free(vet_aux);
	}

	MPI_Finalize();
	return 0;
}
