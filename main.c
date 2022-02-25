#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int hit = 0, miss = 0, modVet[10] = {1, 0, 0, 0, 1, 1, 0, 0, 1, 0};

typedef struct
{
  int bloco;
  int indiceBloco;
  int elemento;
} ElemRam;

typedef struct
{
  int ocupada;
  int bloco;
  int elemento[2];
  int m[2];
  int indiceM[2];
} ElemCache;

int randomInt(int min, int max);
int *gerarAcessos(int n, ElemRam *ram, int *modifica);
void iniciarCache(int n, ElemCache *cache);
int cacheCheia(int n, ElemCache *cache);
void imprimeSequencia(int *acessos, int *modifica, int n);

int main()
{
  int i, j = 0, n, aux, cont = 0, *acessos;
  FILE *fp;
  fp = fopen("nums.txt", "r");

  srand(time(NULL));

  fscanf(fp, "%d", &n);
  ElemRam ram[n];
  ElemCache cache[n / 64];
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

  aux = randomInt(11, 20);
  int modifica[aux];
  acessos = gerarAcessos(aux, ram, modifica);

  iniciarCache((n / 64), cache);

  imprimeSequencia(acessos, modifica, aux);

  // for (i = 0; i < n; i = i + 2)
  // {
  //   printf("BLOCO %d\n", ram[i].bloco);
  //   for (j = i; j < i + 2; j++)
  //   {
  //     printf("ELEMENTO %d: %d\n", ram[j].indiceBloco, ram[j].elemento);
  //   }
  // }
}
int randomInt(int min, int max) { return min + rand() % (max + 1 - min); }

int *gerarAcessos(int n, ElemRam *ram, int *modifica)
{
  int i, *vet;
  vet = malloc(sizeof(int) * n);
  for (i = 0; i < n; i++)
  {
    vet[i] = ram[randomInt(0, 1024)].elemento;
    modifica[i] = modVet[randomInt(0, 10)];
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

void iniciarCache(int n, ElemCache *cache)
{
  for (int i = 0; i < n; i++)
  {
    cache[i].ocupada = 0;
    cache[i].bloco = -1;
    cache[i].elemento[0] = -1;
    cache[i].elemento[1] = -1;
    cache[i].m[0] = 0;
    cache[i].m[1] = 0;
    cache[i].indiceM[0] = -1;
    cache[i].indiceM[1] = -1;
  }
}

int indexRam(ElemRam *ram, int n, int key)
{
  for (int i = 0; i < n; i++)
  {
    if (ram[i].elemento == key)
      return i;
  }
}

void imprimeSequencia(int *acessos, int *modifica, int n)
{
  for (int i = 0; i < n; i++)
  {
    printf("%d", acessos[i]);
    if (modifica[i] == 1)
      printf("*");
    printf(" ");
  }
}

// faltando muita coisa mas principalmente verificar se já está na cache
void aleatorio(ElemRam *ram, ElemCache *cache, int n, int *acessos, int qtdaces, int *modifica)
{
  int i, j = 0, aux, ale;
  for (i = 0; i < qtdaces; i++)
  {
    aux = indexRam(ram, n, acessos[i]);
    if (j < (n / 64))
    {
      if (modifica[i] == 1)
      {
        if (ram[aux].indiceBloco == 1)
        {
          cache[j].elemento[0] = ram[aux - 1].elemento, cache[j].elemento[1] = ram[aux].elemento + 1;
          cache[j].m[1] = 1, cache[j].indiceM[1] = aux, cache[j].bloco = ram[aux].bloco, cache[j].ocupada = 1, j++;
        }
        else
        {
          cache[j].elemento[0] = ram[aux].elemento + 1, cache[j].elemento[1] = ram[aux + 1].elemento;
          cache[j].m[0] = 1, cache[j].indiceM[0] = aux, cache[j].bloco = ram[aux].bloco, cache[j].ocupada = 1, j++;
        }
      }
      else
      {
        if (ram[aux].indiceBloco == 1)
        {
          cache[j].elemento[0] = ram[aux - 1].elemento, cache[j].elemento[1] = ram[aux].elemento;
          cache[j].bloco = ram[aux].bloco, cache[j].ocupada = 1, j++;
        }
        else
        {
          cache[j].elemento[0] = ram[aux].elemento, cache[j].elemento[1] = ram[aux + 1].elemento;
          cache[j].bloco = ram[aux].bloco, cache[j].ocupada = 1, j++;
        }
      }
    }
    else
    {
      ale = randomInt(0, (n / 64) - 1);
      if (cache[ale].indiceM[0] == 1 || cache[ale].indiceM[1] == 1)
      {
        if (cache[ale].indiceM[0] == 1)
        {
          // fazer função para atualizar ram
        }
        else
        {
        }
      }
      else
      {
        if (modifica[i] == 1)
        {
          if (ram[aux].indiceBloco == 1)
          {
            cache[ale].elemento[0] = ram[aux - 1].elemento, cache[ale].elemento[1] = ram[aux].elemento + 1;
            cache[ale].m[1] = 1, cache[ale].indiceM[1] = aux, cache[ale].bloco = ram[aux].bloco;
          }
          else
          {
            cache[ale].elemento[0] = ram[aux].elemento + 1, cache[ale].elemento[1] = ram[aux + 1].elemento;
            cache[ale].m[0] = 1, cache[ale].indiceM[0] = aux, cache[ale].bloco = ram[aux].bloco;
          }
        }
        else
        {
          if (ram[aux].indiceBloco == 1)
          {
            cache[ale].elemento[0] = ram[aux - 1].elemento, cache[ale].elemento[1] = ram[aux].elemento;
            cache[ale].bloco = ram[aux].bloco;
          }
          else
          {
            cache[ale].elemento[0] = ram[aux].elemento, cache[ale].elemento[1] = ram[aux + 1].elemento;
            cache[ale].bloco = ram[aux].bloco;
          }
        }
      }
    }
  }
}
