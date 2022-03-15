#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define BLOCO 8
#define LENRAM 1024
#define LENCACHE 16
#define NUMACESSOMIN 20
#define NUMACESSOMAX 30

int contCacheImp = 1, hit = 0, miss = 0, modVet[10] = {1, 0, 0, 0, 1, 0, 0, 1, 1, 0};

typedef struct
{
  int elemento;
} ElemRam;

typedef struct
{
  int ocupada;
  int elemento[BLOCO];
  int m[BLOCO];
  int indiceM[BLOCO];
} ElemCache;

void writeBack(ElemCache *cache, ElemRam *ram, int indice);
int verificaCache(ElemCache *cache);
void aleatorio(int *acessos, ElemRam *ram, ElemCache *cache, int *modifica, int qtdAcesso);
void imprimeCache(ElemCache *cache);
int randomInt(int min, int max);
int *gerarAcessos(int n, ElemRam *ram, int *modifica);
void iniciaCache(ElemCache *cache);
int cacheCheia(int n, ElemCache *cache);
void imprimeSequencia(int *acessos, int *modifica, int n);

int main()
{
  int i, j = 0, n, aux, cont = 0, *acessos;
  FILE *fp;
  fp = fopen("nums.txt", "r");

  srand(time(NULL));

  ElemRam ram[LENRAM];
  ElemCache cache[LENCACHE];
  int vet[LENRAM];
  for (i = 0; i < LENRAM; i++)
    fscanf(fp, "%d", &ram[i].elemento);

  aux = randomInt(NUMACESSOMIN, NUMACESSOMAX);
  int modifica[aux];
  acessos = gerarAcessos(aux, ram, modifica);

  iniciaCache(cache);

  imprimeSequencia(acessos, modifica, aux);

  // printf("\nRAM: ");
  // for (i = 0; i < LENRAM; i++)
  //   printf("%d ", ram[i].elemento);
  // printf("\n");

  aleatorio(acessos, ram, cache, modifica, aux);

  // printf("\n\nRAM: ");
  // for (i = 0; i < LENRAM; i++)
  //   printf("%d ", ram[i].elemento);
  // printf("\n");

  printf("\n\nHIT: %d MISS: %d", hit, miss);
}

void writeBack(ElemCache *cache, ElemRam *ram, int indice)
{
  for (int i = 0; i < BLOCO; i++)
  {
    if (cache[indice].m[i] == 1)
    {
      ram[cache[indice].indiceM[i]].elemento = cache[indice].elemento[i];
    }
  }
}

int verificaCache(ElemCache *cache)
{
  for (int i = 0; i < LENCACHE; i++)
  {
    if (cache[i].ocupada == 0)
    {
      return i;
    }
  }
  return -1;
}

int varreCache(ElemCache *cache, int elemento)
{
  for (int i = 0; i < LENCACHE; i++)
  {
    for (int j = 0; j < BLOCO; j++)
    {
      if (cache[i].elemento[j] == elemento)
        return i;
    }
  }
  return -1;
}

void imprimeCache(ElemCache *cache)
{
  char text[20];
  int aux, *vet = malloc(BLOCO * sizeof(int));
  printf("----- CACHE %d -----\n", contCacheImp++);
  for (int i = 0; i < LENCACHE; i++)
  {
    if (!(cache[i].elemento[0] == -1))
    {
      printf("linha %d\n", i);
      for (int j = 0; j < BLOCO; j++)
      {
        printf("%d ", cache[i].elemento[j]);
        sprintf(text, "%d", cache[i].elemento[j]);
        vet[j] = strlen(text);
      }
      printf("\n");
      for (int j = 0; j < BLOCO; j++)
      {
        if (j == 0)
        {
          printf("%d", cache[i].m[j]);
        }
        else
        {
          for (int k = 1; k < vet[j - 1]; k++)
            printf(" ");
          printf(" %d", cache[i].m[j]);
        }
      }
      printf("\n");
    }
  }
}

void aleatorio(int *acessos, ElemRam *ram, ElemCache *cache, int *modifica, int qtdAcesso)
{
  int mod, i, j, k, len, indiceCache, aux, z, x;
  for (i = 0; i < qtdAcesso; i++)
  {
    mod = acessos[i] % BLOCO;
    len = (acessos[i] + BLOCO) - mod;
    aux = verificaCache(cache);
    z = varreCache(cache, ram[acessos[i]].elemento);
    if (z > -1)
    {
      hit++;
    }
    else
    {
      if (aux > -1)
      {
        indiceCache = aux;
        cache[indiceCache].ocupada = 1;
      }
      else
      {
        indiceCache = randomInt(0, LENCACHE - 1);

        writeBack(cache, ram, indiceCache);
      }
      for (j = (acessos[i] - mod), k = 0; j < len; j++, k++)
      {
        if (modifica[i] == 1)
        {
          if (acessos[i] == j)
          {
            cache[indiceCache].elemento[k] = ram[j].elemento + 1;
            cache[indiceCache].m[k] = 1;
            cache[indiceCache].indiceM[k] = j;
          }
          else
          {
            cache[indiceCache].elemento[k] = ram[j].elemento;
          }
        }
        else
        {
          cache[indiceCache].elemento[k] = ram[j].elemento;
        }
      }
      miss++;
    }
    imprimeCache(cache);
  }
}

int randomInt(int min, int max) { return min + rand() % (max + 1 - min); }

int *gerarAcessos(int n, ElemRam *ram, int *modifica)
{
  int i, *vet;
  vet = malloc(sizeof(int) * n);
  for (i = 0; i < n; i++)
  {
    vet[i] = randomInt(0, LENRAM - 1);
    modifica[i] = modVet[randomInt(0, 9)];
  }

  return vet;
}

int cacheCheia(int n, ElemCache *cache)
{
  int cont = 0;
  for (int i = 0; i < n; i++)
  {
    if (cache[i].ocupada == 0)
      cont++;
  }
  return cont;
}

void iniciaCache(ElemCache *cache)
{
  for (int i = 0; i < LENCACHE; i++)
  {
    cache[i].ocupada = 0;
    for (int j = 0; j < BLOCO; j++)
    {
      cache[i].elemento[j] = -1;
      cache[i].m[j] = 0;
      cache[i].indiceM[j] = -1;
    }
  }
}

void imprimeSequencia(int *acessos, int *modifica, int n)
{
  printf("Acessos: %d\n", n);
  for (int i = 0; i < n; i++)
  {
    printf("%d", acessos[i]);
    if (modifica[i] == 1)
      printf("*");
    printf(" ");
  }
  printf("\n\n");
}