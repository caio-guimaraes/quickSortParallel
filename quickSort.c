#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

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
  int i, tamanho;
  double ti, tf;
  clock_t t;

  printf("Digite o tamanho do vetor: ");
  scanf("%d", &tamanho);

  int *vet = (int*)malloc(tamanho*sizeof(int));

  printf("Preencha o vetor: ");
  for(i=0; i<tamanho; i++){
    scanf("%d", &vet[i]);
  }
  // Inicio marcação tempo
  t = clock();
  quickSort(vet, 0, tamanho);
  // Fim marcação tempo
  t = clock() - t;

  imprimiVetor(vet, tamanho);
  printf("Tempo de execucao: %lf", ((double)t)/((CLOCKS_PER_SEC))); //conversão para double

  return 0;
}
