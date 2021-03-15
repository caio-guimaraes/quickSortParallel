#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

// void inicializa_v2(int vet[], int tam){
//   int i;
// 	for (i = 0; i < tam; ++i)
// 		vet[i] = rand() % tam;
// }

void troca(int *vet, int i, int j) {
  int temp = vet[i];
  vet[i] = vet[j];
  vet[j] = temp;
}

void imprimiVetor(int vet[], int tamanho){
    int i;
    for (i = 0; i < tamanho; i++)
        printf("%d ", vet[i]);
    printf("\n");
}

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

void quickSort(int vet[], int inicio, int fim){
  if (inicio < fim){
    int p = particiona(vet, inicio, fim);
    quickSort(vet, inicio, p);

    quickSort(vet, p+1, fim);
  }
}

int main(int argc, char const *argv[]) {
  int *vet, tamanho;
  // int tamanho;
  char nomeArq[100];
  double ti, tf;
  clock_t t;

  printf("Digite o nome do arquivo: ");
  scanf("%s", nomeArq);
  vet = inicializa(nomeArq, &tamanho, vet);
  // scanf("%d", &tamanho);
  // int vet[tamanho];
  // inicializa_v2(vet, tamanho);

  // printf("\n-------------- Vetor original --------------\n");
  // imprimiVetor(vet, tamanho);

  // Inicio marcação tempo
  t = clock();
  quickSort(vet, 0, tamanho);
  // Fim marcação tempo
  t = clock() - t;

  printf("\n-------------- Vetor Ordenado --------------\n");
  imprimiVetor(vet, tamanho);
  printf("\nTempo de execucao: %lf\n", ((double)t)/((CLOCKS_PER_SEC))); //conversão para double

  return 0;
}
