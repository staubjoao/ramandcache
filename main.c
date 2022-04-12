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
  int ocupada;
  int elemento[BLOCO];
  int m[BLOCO];
  int indice[BLOCO];
} ElemCache;

void writeBack(ElemCache *cache, int *ram, int indice);
int verificaCache(ElemCache *cache);
void fifo(int *acessos, int *ram, ElemCache *cache, int *modifica, int qtdAcesso);
void aleatorio(int *acessos, int *ram, ElemCache *cache, int *modifica, int qtdAcesso);
void imprimeRam(int *ram);
void imprimeCache(ElemCache *cache);
int randomInt(int min, int max);
int *gerarAcessos(int n, int *ram, int *modifica);
void iniciaRam(int *ram);
void iniciaCache(ElemCache *cache);
void imprimeSequencia(int *acessos, int *modifica, int n);

int main()
{
  int aux, *acessos;
  srand(time(NULL));

  int ram[LENRAM];
  ElemCache cache[LENCACHE];
  int vet[LENRAM];
  iniciaRam(ram);

  aux = randomInt(NUMACESSOMIN, NUMACESSOMAX);
  int modifica[aux];
  acessos = gerarAcessos(aux, ram, modifica);

  iniciaCache(cache);

  imprimeSequencia(acessos, modifica, aux);
  imprimeRam(ram);

  // //  aleatorio(acessos, ram, cache, modifica, aux);
  fifo(acessos, ram, cache, modifica, aux);
  imprimeRam(ram);

  printf("\n\nHIT: %d MISS: %d", hit, miss);
}

void writeBack(ElemCache *cache, int *ram, int indice)
{
  for (int i = 0; i < BLOCO; i++)
  {
    if (cache[indice].m[i] == 1)
    {
      ram[cache[indice].indice[i]] = cache[indice].elemento[i];
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

void imprimeRam(int *ram)
{
  printf("\n----- RAM -----\n");
  for (int i = 0; i < LENRAM; i++)
    printf("%4d ", i);
  printf("\n");
  for (int i = 0; i < LENRAM; i++)
    printf("%4d ", ram[i]);
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
        printf("%4d ", cache[i].elemento[j]);
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

// algoritmos de substituição FIFO
void fifo(int *acessos, int *ram, ElemCache *cache, int *modifica, int qtdAcesso)
{
  int mod, i, j, k, l, len, indiceCache, aux, z, x, ale;

  int vetfifo[LENCACHE];
  l = 0;
  for (i = 0; i < qtdAcesso; i++)
  {
    // verifico se o valor deve ser modificado
    if (modifica[i] == 0)
    {
      // não é modificado, imprimo um guia para validar a execução
      printf("Acesso ao valor, na RAM, %d no indice %d:\n", ram[acessos[i]], acessos[i]);
    }
    else
    {
      // é modificado, gera um numero aleatorio e imprimo um guia para validar a execução
      ale = randomInt(100, 1000 + LENRAM);
      printf("Acesso ao valor, na RAM, %d no indice %d, valor modificado é %d:\n", ram[acessos[i]], acessos[i], ale);
    }
    // resto da divisão do indice do acesso pelo tamanho do bloco
    mod = acessos[i] % BLOCO;
    // variavel que vai delimitar o for para preencher o bloco da cache
    len = (acessos[i] + BLOCO) - mod;
    // verifica o primeiro bloco não ocupado da cache, caso não exista retorna -1
    aux = verificaCache(cache);
    // varre a cache procurando o elemento que está na fila, caso ele esteja na cache
    // retorna o inidice que ele está que fica armazenado em z e em x o indice do bloco
    z = varreCache(cache, acessos[i], &x);

    // o elemento está na cache
    if (z > -1)
    {
      // verifico se esse elemento deve ser modificado
      if (modifica[i] == 1)
        // modifico o elemento
        cache[z].elemento[x] = ale;

      printf("Valor na cache %d:\n", cache[z].elemento[x]);
      // conto 1 hit
      hit++;
    }
    // o elemento ñ está na cache
    else
    {
      // se aux é maior que -1 significa que existe um bloco(s) disponivel na cache
      if (aux > -1)
      {
        // indiceCache vai receber o valor que vamos utilizar na cache
        indiceCache = aux;
        // vet fifo recebe o valor da posição 0
        vetfifo[l] = aux;
        // incrementa o indice do vetor para realizar o fifo
        l++;
        // define o bloco da cache como ocupado
        cache[indiceCache].ocupada = 1;
      }
      else
      {
        // indiceCache vai receber o valor que vamos utilizar na cache
        indiceCache = removePrimeiro(vetfifo);
        // chama a função writeBack para atualizar os valores na RAM
        writeBack(cache, ram, indiceCache);
      }
      // j vai começar do primeiro indice do bloco da RAM e vai até o ultimo indice
      for (j = (acessos[i] - mod), k = 0; j < len; j++, k++)
      {
        // o bloco da cache vai receber os indices do bloco da RAM
        cache[indiceCache].indice[k] = j;
        // verifica se o elemento vai ser modificado e se está no elemento do bloco
        if (modifica[i] == 1 && acessos[i] == j)
        {
          // o elemento na cache vai receber um valor aleatorio
          cache[indiceCache].elemento[k] = ale;
          // marco que aquele elemento foi modificado
          cache[indiceCache].m[k] = 1;
        }
        else
        {
          // caso o elemento ñ precisa de modificação carrego o indice do bloco da RAM
          // no indice equivalente da cache
          cache[indiceCache].elemento[k] = ram[j];
        }
      }
      // conto um miss
      miss++;
    }
    // imprimo o estado atual da cache
    imprimeCache(cache);
    // imprimo a quantidade de hit e miss
    printf("\nHIT: %d MISS: %d\n", hit, miss);
  }
}

// acesso aleatorio
void aleatorio(int *acessos, int *ram, ElemCache *cache, int *modifica, int qtdAcesso)
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
      if (modifica[i] == 1)
        cache[z].elemento[x] = randomInt(100, 1000 + LENRAM);
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
          cache[indiceCache].elemento[k] = randomInt(100, 1000 + LENRAM);
          cache[indiceCache].m[k] = 1;
        }
        else
        {
          cache[indiceCache].elemento[k] = ram[j];
        }
      }
      miss++;
    }
    imprimeCache(cache);
  }
}
// bloco de código para gerar acessos na memoria RAM
int randomInt(int min, int max) { return min + rand() % (max + 1 - min); }

int *gerarAcessos(int n, int *ram, int *modifica)
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

// inicia memora RAM
void iniciaRam(int *ram)
{
  for (int i = 0; i < LENRAM; i++)
    ram[i] = randomInt(100, 1000 + LENRAM);
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

// imprime a sequencia gerada aleatoriamente
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