#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define BLOCO 4
#define LENRAM 1024
#define LENCACHE 32
#define QUANTIDADEPROCESSADORES 4
#define NUMACESSOMIN 32
#define NUMACESSOMAX 50

int contCacheImp = 1, hit = 0, miss = 0, modVet[10] = {1, 0, 0, 0, 1, 0, 0, 1, 1, 0};

typedef struct
{
  int ocupada;
  int elemento[BLOCO];
  int m[BLOCO];
  int indice[BLOCO];
  char marcador;
} ElemCache;

void leituraAcessos(int **acessos, int n);
void writeBack(ElemCache *cache, int *ram, int indice);
int verificaCache(ElemCache *cache);
void imprimeRam(int *ram);
void imprimeCache(ElemCache *cache);
void fifo(int *acessos, int *ram, ElemCache *cache, int *modifica, int i, int *vetfifo, int *l);
void aleatorio(int *acessos, int *ram, ElemCache *cache, int *modifica, int i);
int randomInt(int min, int max);
int *gerarAcessos(int n, int *ram, int *modifica);
void iniciaRam(int *ram);
void iniciaCache(ElemCache *cache);
void imprimeSequencia(int *acessos, int *modifica, int n);

int main()
{
  setlocale(LC_ALL, "Portuguese_Brasil");
  int op, aux, i, n;
  srand(time(NULL));

  int ram[LENRAM];
  ElemCache cache[QUANTIDADEPROCESSADORES][LENCACHE];
  int vet[LENRAM];
  iniciaRam(ram);

  // aux = randomInt(NUMACESSOMIN, NUMACESSOMAX);
  // int modifica[aux];
  // acessos = gerarAcessos(aux, ram, modifica);

  // inicia todos os processadores
  for (i = 0; i < QUANTIDADEPROCESSADORES; i++)
    iniciaCache(cache[i]);

  printf("Digite a quantidade de acessos que deseja fazer: ");
  scanf("%d", &n);
  int acessos[n][2];

  leituraAcessos(acessos, n);

  // imprimeSequencia(acessos, modifica, aux);
  // // imprimeRam(ram);
  // int vetfifo[LENCACHE], l = 0;

  // for (i = 0; i < aux; i++)
  // {
  //   if (verificaCache(cache) != -1)
  //   {
  //     fifo(acessos, ram, cache, modifica, i, vetfifo, &l);
  //   }
  //   else
  //   {
  //     do
  //     {
  //       printf("Qual método de subistituição deseja utilizar?\n1 - aleatorio\n2 - fifo\nDigite a opção: ");
  //       scanf("%d", &op);
  //       if (op == 1)
  //       {
  //         aleatorio(acessos, ram, cache, modifica, i);
  //       }
  //       else if (op == 2)
  //       {
  //         fifo(acessos, ram, cache, modifica, i, vetfifo, &l);
  //       }
  //       else
  //       {
  //         printf("Opção invalida!\n");
  //       }
  //     } while (op != 1 && op != 2);
  //   }
  // }

  // // imprimeRam(ram);

  // printf("\n\nHIT: %d MISS: %d\n", hit, miss);
  // // system("pause");
}

void eituraAcessos(int **acessos, int n)
{
  int i;

  for (i = 0; i < n; i++)
  {
    printf("Digite o %d° acesso a memoria e o processador (%d a %d) que realizar o acesso, nesse formado ex: 456, 1: ", i + 1, 0, LENRAM);
    scanf("%d %d", &acessos[i][0], &acessos[i][1]);
  }
}

// função de write back, recebe a memoria cache, a ram e o indice que deve ser modificado
void writeBack(ElemCache *cache, int *ram, int indice)
{
  // varre o bloco da cache
  for (int i = 0; i < BLOCO; i++)
  {
    // quando o indice a ser modificado é igual a 1
    if (cache[indice].m[i] == 1)
      // a ram recebe o elemento modificado
      ram[cache[indice].indice[i]] = cache[indice].elemento[i];
  }
}

// função que verifica a cache
int verificaCache(ElemCache *cache)
{
  // percorre todos os blocos da cache
  for (int i = 0; i < LENCACHE; i++)
  {
    // se o bloco estiver vazio
    if (cache[i].ocupada == 0)
      // retorna o indice do bloco vazio
      return i;
  }
  // caso nenhum bloco esteja vazio retorna -1
  return -1;
}

// função que varre a cache
int varreCache(ElemCache *cache, int elemento, int *b)
{
  // percorre todos os blocos da cache
  for (int i = 0; i < LENCACHE; i++)
  {
    // percorre todos os elementos do bloco
    for (int j = 0; j < BLOCO; j++)
    {
      // caso o elemento do bloco seja igual ao parametro elemento
      if (cache[i].indice[j] == elemento)
      {
        // o ponteiro de b recebe o indice do bloco que esse elemento se encotra
        *b = j;
        // e retorna o indice da cache que ele está
        return i;
      }
    }
  }
  // caso esse elemento não esteja presente na cache retorna -1
  return -1;
}

// função para imprimir a RAM
void imprimeRam(int *ram)
{
  printf("\n----- RAM -----\n");
  // percorre todos os elementos
  for (int i = 0; i < LENRAM; i++)
    // imprime todos os valores
    printf("%d: %4d ", i, ram[i]);
  printf("\n");
}

// função para imprimir a cache
void imprimeCache(ElemCache *cache)
{
  printf("----- CACHE %d -----\n", contCacheImp++);
  // percorre todos os blocos da cache
  for (int i = 0; i < LENCACHE; i++)
  {
    // só imprime se estiver ocupada
    if (cache[i].ocupada == 1)
    {
      // imprime o valor da linha
      printf("linha %d: ", i + 1);
      // imprime todos os elementos do bloco
      for (int j = 0; j < BLOCO; j++)
      {
        printf("%4d ", cache[i].elemento[j]);
      }
      printf("\n");
    }
  }
}

// função para realizar o FIFO
int removePrimeiro(int *p)
{
  // aux deve receber o primeiro elemento do vetor
  int aux = p[0], i;
  for (i = 0; i < LENCACHE - 1; i++)
    // todos os elementos são "puxados" para a frente
    p[i] = p[i + 1];

  // o ultimo elemento deve receber o valor de aux
  p[i] = aux;
  // retornamos o primeiro elemento para realizar o FIFO
  return aux;
}

// algoritmos de substituição FIFO
void fifo(int *acessos, int *ram, ElemCache *cache, int *modifica, int i, int *vetfifo, int *l)
{
  int mod, j, k, len, indiceCache, aux, z, x, ale;
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
      // modifico o elemento e marca o elemento como modificado
      cache[z].elemento[x] = ale, cache[z].m[x] = 1;

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
      vetfifo[*l] = aux;
      // incrementa o indice do vetor para realizar o fifo
      *l = *l + 1;
      // define o bloco da cache como ocupado
      cache[indiceCache].ocupada = 1;
    }
    else
    {
      // indiceCache vai receber o valor que vamos utilizar na cache
      // aqui esse valor vai ser o primeiro elemento do vetor vetfifo
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

// acesso aleatorio
void aleatorio(int *acessos, int *ram, ElemCache *cache, int *modifica, int i)
{
  int mod, j, k, len, indiceCache, aux, z, x, ale;

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
      // modifico o elemento e marca o elemento como modificado
      cache[z].elemento[x] = ale, cache[z].m[x] = 1;

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
      // define o bloco da cache como ocupado
      cache[indiceCache].ocupada = 1;
    }
    else
    {
      // indiceCache vai receber o valor que vamos utilizar na cache
      indiceCache = randomInt(0, LENCACHE - 1);
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

// bloco de código para gerar acessos na memoria RAM
int randomInt(int min, int max) { return min + rand() % (max + 1 - min); }

// função para gerar acessos aleatorios
int *gerarAcessos(int n, int *ram, int *modifica)
{
  int i, *vet;
  // alocação dinamica de um vetor para gerar os acessos
  vet = malloc(sizeof(int) * n);
  for (i = 0; i < n; i++)
  {
    // atribuição de um valor aleatorio para acesso na RAM, vai de 0 até o tamanho da RAM - 1
    vet[i] = randomInt(0, LENRAM - 1);
    // se o elemento deve ou não ser modificado, um vetor global com os seguinte elementos:
    // {1, 0, 0, 0, 1, 0, 0, 1, 1, 0} onde 1 é para modificar e 0 para não modificar
    modifica[i] = modVet[randomInt(0, 9)];
  }

  // retorna o vetor de acessos
  return vet;
}

// inicia memora RAM
void iniciaRam(int *ram)
{
  for (int i = 0; i < LENRAM; i++)
    // gera um valor aleatorio para cada elemento da ram, que vai de 100 a 1000 + tamanho da RAM
    ram[i] = randomInt(100, 1000 + LENRAM);
}

// inicia memoria cache
void iniciaCache(ElemCache *cache)
{
  for (int i = 0; i < LENCACHE; i++)
  {
    // todos os blocos da cache são definidos como vazio
    cache[i].ocupada = 0;
    for (int j = 0; j < BLOCO; j++)
    {
      // os elementos recebem valor -1 inicialmente
      cache[i].elemento[j] = -1;
      // o vetor que marcar se o elemento foi modificado ou não recebe valor 0
      cache[i].m[j] = 0;
      // e o indice do elemento da RAM recebe o valor -1
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
    // imprime o indice da RAM
    printf("%d", acessos[i]);
    if (modifica[i] == 1)
      // caso o elemento deve ser modificado imprime um * na frente do numero
      printf("*");
    printf(" ");
  }
  printf("\n\n");
}
