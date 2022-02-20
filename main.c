#include <stdio.h>
#include <stdlib.h>
#include "config.h"

typedef struct
{
  int bloco;
  int indiceBloco;
  int elemento;
} ElemRam;

typedef struct
{
  int bloco;
  int elemento[2];
} ElemCache;

int main()
{
  int i, j = 0, n, aux, cont = 0;
  FILE *fp;
  fp = fopen("nums.txt", "r");

  fscanf(fp, "%d", &n);

  ElemRam ram[n];
  int vet[n];
  for (i = 0; i < n; i++)
  {
    fscanf(fp, "%d", &aux);
    vet[i] = aux;
    ram[i].elemento = vet[i];
    if (j <= 1)
    {
      if (i > 1)
        j++;
      ram[i].indiceBloco = j;
      j++;
    }
    else
    {
      cont++;
      j = 0;
      ram[i].indiceBloco = j;
    }
    ram[i].bloco = cont;
  }
  for (i = 0; i < n; i = i + 2)
  {
    printf("BLOCO %d\n", ram[i].bloco);
    for (j = i; j < i + 2; j++)
    {
      printf("ELEMENTO %d: %d\n", ram[j].indiceBloco, ram[j].elemento);
    }
  }
}
