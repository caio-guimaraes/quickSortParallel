#include <stdio.h>
#include <stdlib.h>

void imprimiVetor(int vet[], int tamanho){
    int i;
    for (i = 0; i < tamanho; i++)
        printf("%d ", vet[i]);
    printf("\n");
}

static int particiona (int vet[], int inicio, int fim) {
  int pivo = vet[fim]; // pivô
  int i = inicio;
  int temp;
  for (int j = inicio; j < fim; j++){
    if (vet[j] <= pivo) {
      temp = vet[i];
      vet[i] = vet[j];
      vet[j] = temp;
      i++;
    }
  }
  temp = vet[i];
  vet[i] = vet[fim];
  vet[fim] = temp;
  return i;
}

void quickSort(int vet[], int inicio, int fim){
  if (inicio < fim){
    int p = particiona(vet, inicio, fim);
    quickSort(vet, inicio, p-1);
    quickSort(vet, p+1, fim);
  }
}

//vet = 2 5 8 1 6 9 3 0 4 10 7
int main(int argc, char const *argv[]) {
  int i, tamanho;

  printf("Digite o tamanho do vetor: ");
  scanf("%d", &tamanho);

  int *vet = (int*)malloc(tamanho*sizeof(int));

  printf("Preencha o vetor: ");
  for(i=0; i<tamanho; i++){
    scanf("%d", &vet[i]);
  }

  quickSort(vet, 0, tamanho-1);
  imprimiVetor(vet, tamanho);
  return 0;
}
