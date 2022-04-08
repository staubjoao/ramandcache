#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define BLOCO 2
#define LENRAM 16
#define LENCACHE 2
#define NUMACESSOMIN 5
#define NUMACESSOMAX 10

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
  int indice[BLOCO];
} ElemCache;

void writeBack(ElemCache *cache, ElemRam *ram, int indice);
int verificaCache(ElemCache *cache);
void fifo(int *acessos, ElemRam *ram, ElemCache *cache, int *modifica, int qtdAcesso);
void aleatorio(int *acessos, ElemRam *ram, ElemCache *cache, int *modifica, int qtdAcesso);
void imprimeRam(ElemRam *ram);
void imprimeCache(ElemCache *cache);
int randomInt(int min, int max);
int *gerarAcessos(int n, ElemRam *ram, int *modifica);
void iniciaCache(ElemCache *cache);
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
  imprimeRam(ram);

  // printf("\nRAM: ");
  // for (i = 0; i < LENRAM; i++)
  //   printf("%d ", ram[i].elemento);
  // printf("\n");

  // testar depois : 7* 4 4* 2* 15 2 1
  //  aleatorio(acessos, ram, cache, modifica, aux);
  fifo(acessos, ram, cache, modifica, aux);
  imprimeRam(ram);

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
      ram[cache[indice].indice[i]].elemento = cache[indice].elemento[i];
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

int varreCache(ElemCache *cache, int elemento, int *b)
{
  for (int i = 0; i < LENCACHE; i++)
  {
    for (int j = 0; j < BLOCO; j++)
    {
      if (cache[i].indice[j] == elemento)
      {
        *b = j;
        return i;
      }
    }
  }
  return -1;
}

void imprimeRam(ElemRam *ram)
{
  printf("\n----- RAM -----\n");
  for (int i = 0; i < LENRAM; i++)
    printf("%4d ", i);
  printf("\n");
  for (int i = 0; i < LENRAM; i++)
    printf("%4d ", ram[i].elemento);
  printf("\n");
}

void imprimeCache(ElemCache *cache)
{
  printf("----- CACHE %d -----\n", contCacheImp++);
  for (int i = 0; i < LENCACHE; i++)
  {
    if (!(cache[i].elemento[0] == -1))
    {
      printf("linha %d: ", i + 1);
      for (int j = 0; j < BLOCO; j++)
      {
        printf("%4d ", cache[i].indice[j]);
      }
      printf("\n");
    }
  }
}

int removePrimeiro(int *p)
{
  int aux = p[0], i;
  for (i = 0; i < LENCACHE - 1; i++)
  {
    p[i] = p[i + 1];
  }
  p[i] = aux;
  return aux;
}

void imprimePila(int *vetfifo, int n)
{
  printf("\nVET: ");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", vetfifo[i]);
  }
  printf("\n");
}

void fifo(int *acessos, ElemRam *ram, ElemCache *cache, int *modifica, int qtdAcesso)
{
  int mod, i, j, k, l, len, indiceCache, aux, z, x;

  int vetfifo[LENCACHE];
  l = 0;
  for (i = 0; i < qtdAcesso; i++)
  {
    mod = acessos[i] % BLOCO;
    len = (acessos[i] + BLOCO) - mod;
    aux = verificaCache(cache);
    z = varreCache(cache, acessos[i], &x);

    if (z > -1)
    {
      if (modifica[i] = 1)
        cache[z].elemento[x] = cache[z].elemento[x] + 1;
      hit++;
    }
    else
    {
      if (aux > -1 && l < LENCACHE)
      {
        indiceCache = aux;
        vetfifo[l] = aux;
        l++;
        cache[indiceCache].ocupada = 1;
      }
      else
      {
        indiceCache = removePrimeiro(vetfifo);
        writeBack(cache, ram, indiceCache);
      }
      for (j = (acessos[i] - mod), k = 0; j < len; j++, k++)
      {
        // verifica se o elemento vai ser modificado
        cache[indiceCache].indice[k] = j;
        if (modifica[i] == 1 && acessos[i] == j)
        {
          cache[indiceCache].elemento[k] = ram[j].elemento + 1;
          cache[indiceCache].m[k] = 1;
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

// acesso aleatorio
void aleatorio(int *acessos, ElemRam *ram, ElemCache *cache, int *modifica, int qtdAcesso)
{
  int mod, i, j, k, len, indiceCache, aux, z, x;

  // for para todos acessos gerados
  for (i = 0; i < qtdAcesso; i++)
  {
    // resto da divisão do acesso pelo tamanho do bloco
    mod = acessos[i] % BLOCO;
    // variavel auxiliar para carregar os blocos na cache
    len = (acessos[i] + BLOCO) - mod;
    // retorna o primeiro incice não ocupado ou -1 caso a cache estiver cheia
    aux = verificaCache(cache);
    // verifica se o elemento já está na cache
    z = varreCache(cache, acessos[i], &x);

    // se estiver na cache conta um acerto
    if (z > -1)
    {
      if (modifica[i] = 1)
        cache[z].elemento[x] = cache[z].elemento[x] + 1;
      hit++;
    }
    // caso contrario armazena na cache ou realiza a subistituição
    else
    {
      // ainda tem elementos sobrando
      if (aux > -1)
      {
        // adiciona sequencialmente
        indiceCache = aux;
        cache[indiceCache].ocupada = 1;
      }
      else
      {
        // gera um valor aleatorio para adicionar na cache
        indiceCache = randomInt(0, LENCACHE - 1);
        // e escreve a modificação na RAM
        writeBack(cache, ram, indiceCache);
      }
      for (j = (acessos[i] - mod), k = 0; j < len; j++, k++)
      {
        // verifica se o elemento vai ser modificado
        cache[indiceCache].indice[k] = j;
        if (modifica[i] == 1 && acessos[i] == j)
        {
          cache[indiceCache].elemento[k] = ram[j].elemento + 1;
          cache[indiceCache].m[k] = 1;
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
// bloco de código para gerar acessos na memoria RAM
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

// inicia memoria cache
void iniciaCache(ElemCache *cache)
{
  for (int i = 0; i < LENCACHE; i++)
  {
    cache[i].ocupada = 0;
    for (int j = 0; j < BLOCO; j++)
    {
      cache[i].elemento[j] = -1;
      cache[i].m[j] = 0;
      cache[i].indice[j] = -1;
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