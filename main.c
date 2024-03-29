#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define BLOCO 4
#define LENRAM 1024
#define LENCACHE 4
#define QTDCPU 4
#define NUMACESSOMIN 32
#define NUMACESSOMAX 50

int contCacheImp = 1, modVet[10] = {1, 0, 0, 0, 1, 0, 0, 1, 1, 0};
int read_hit = 0, read_miss = 0, write_miss = 0, write_hit = 0;
int cont = 0;

typedef struct
{
  int ocupada;
  int elemento[BLOCO];
  int m[BLOCO];
  int indice[BLOCO];
  char marcador;
} ElemCache;

void leituraAcessos(int n, int **acessos);
void writeBack(ElemCache *cache, int *ram, int indice);
int verificaCache(ElemCache caches[LENCACHE]);
int varreCaches(ElemCache caches[QTDCPU][LENCACHE], int elemento, int cache, int op, int *marcar_volta);
void imprimeRam(int *ram);
void imprimeCacheHorizontal(ElemCache cache[QTDCPU][LENCACHE]);
void imprimeCache(ElemCache cache[QTDCPU][LENCACHE]);
void fifo(int **acessos, int *ram, ElemCache caches[QTDCPU][LENCACHE], int i, int l[LENCACHE], int matfifo[QTDCPU][LENCACHE]);
void aleatorio(int *acessos, int *ram, ElemCache *cache, int *modifica, int i);
int randomInt(int min, int max);
int gerarAcessos(int n, int **acessos);
void iniciaRam(int *ram);
void iniciaCache(ElemCache cache[QTDCPU][LENCACHE]);
void imprimeSequencia(int *acessos, int *modifica, int n);

int main()
{
  setlocale(LC_ALL, "Portuguese_Brasil");
  int op, aux, i, n;
  srand(time(NULL));

  int ram[LENRAM];
  ElemCache caches[QTDCPU][LENCACHE];
  int vet[LENRAM];
  iniciaRam(ram);

  // inicia todos os processadores
  iniciaCache(caches);

  int **acessos;

  do
  {
    printf("Você deseja gerar acessos aleatorios (1) ou escrever os acessos (2)\nDigite a opção: ");
    scanf("%d", &op);
    if (op == 1)
    {
      printf("Digite a quantidade de acessos a ser gerado: ");
      scanf("%d", &n);
    }
    else if (op == 2)
    {
      printf("Digite a quantidade de acesssos a ser digitado: ");
      scanf("%d", &n);
    }
    else
      printf("Opção invalida\n");
  } while (op != 1 && op != 2);

  acessos = malloc(sizeof(int *) * n);
  for (i = 0; i < n; i++)
    acessos[i] = malloc(3 * sizeof(int));
  if (op == 1)
    gerarAcessos(n, acessos);
  else if (op == 2)
    leituraAcessos(n, acessos);

  int matfifo[QTDCPU][LENCACHE];

  int l[LENCACHE];
  for (i = 0; i < LENCACHE; i++)
    l[i] = 0;

  imprimeRam(ram);
  for (i = 0; i < n; i++)
  {
    fifo(acessos, ram, caches, i, l, matfifo);
  }
  imprimeRam(ram);
  for (i = 0; i < n; i++)
    free(acessos[i]);
  free(acessos);
}

void leituraAcessos(int n, int **acessos)
{
  int i;
  for (i = 0; i < n; i++)
  {
    printf("Digite o %d° acesso a memoria (de %d a %d), o processador (de %d a %d) que deve realizar o acesso e se o dado vai ser modificado (1: sim, 0: não), nesse formado ex: %d %d 0: ", i + 1, 0, LENRAM, 1, QTDCPU, randomInt(0, LENRAM - 1), randomInt(1, QTDCPU));
    scanf("%d %d %d", &acessos[i][0], &acessos[i][1], &acessos[i][2]);
    acessos[i][1] -= 1;
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
int verificaCache(ElemCache caches[LENCACHE])
{
  // percorre todos os blocos da cache
  for (int i = 0; i < LENCACHE; i++)
  {
    // se o bloco estiver vazio
    if (caches[i].ocupada == 0)
      // retorna o indice do bloco vazio
      return i;
  }
  // caso nenhum bloco esteja vazio retorna -1
  return -1;
}

// repensar essa função
// função que varre as cahces procurando o marcados do elemento em questão
int varreCaches(ElemCache caches[QTDCPU][LENCACHE], int elemento, int cache, int op, int *marcar_volta)
{
  int i, j, k;
  // percorre todas as caches
  for (i = 0; i < QTDCPU; i++)
  {
    // caso não seja a cache em que o elemento está
    // deve verificar
    if (i != cache)
    {
      // percorre todos os blocos da cache
      for (j = 0; j < LENCACHE; j++)
      {
        // percorre todos os elementos do bloco
        for (k = 0; k < BLOCO; k++)
        {
          // verifica se o elemento em questão é o elemento buscado
          if (caches[i][j].indice[k] == elemento)
          {
            // caso for
            // verifica se ele não vai ser modificado, no caso op == 1
            // marcador deve ser igual a exclusivo ou compartilhado e cache deve ser diferente do index atual
            if (op == 1 && (caches[i][j].marcador == 'E' || caches[i][j].marcador == 'C') && cache != i)
            {
              // caso entre aqui marca como compartilhado
              caches[i][j].marcador = 'C';
              // e retonar o marcador por referencia
              *marcar_volta = 1;
            }
            // verifica se vai ser modificaldo, no caso op == 0
            // marcador deve ser igual a modificado ou compartilhado ou excelusivo
            else if (op == 0 && (caches[i][j].marcador == 'M' || caches[i][j].marcador == 'C' || caches[i][j].marcador == 'E'))
            {
              // caso entre aqui marca esse bloco todo como invalido
              caches[i][j].marcador = 'I';
              *marcar_volta = 0;
            }
          }
        }
      }
    }
  }
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
      if (cache[i].indice[j] == elemento && cache[i].marcador != 'I')
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

void imprimeCacheHorizontal(ElemCache cache[QTDCPU][LENCACHE])
{
  int i, j, k;
  for (i = 0; i < QTDCPU; i++)
  {
    if (i == QTDCPU - 1)
      printf("----- PROCESSADOR %d -------", i + 1);
    else if (i > 0)
      printf("----- PROCESSADOR %d -------+", i + 1);
    else
      printf("----- PROCESSADOR %d -------+", i + 1);
  }
  printf("\n");
  for (i = 0; i < LENCACHE; i++)
  {
    for (j = 0; j < QTDCPU; j++)
    {
      if (cache[j][i].ocupada == 1)
      {
        if (j != 0)
          printf("|");
        printf("l %2d: ", i + 1);
        for (k = 0; k < BLOCO; k++)
        {
          printf("%4d ", cache[j][i].elemento[k]);
          if (k == BLOCO - 1)
            printf("%c", cache[j][i].marcador);
        }
      }
      else
      {
        if (j != 0)
          printf("|");
        printf("l %2d: ", i + 1);
        for (k = 0; k < BLOCO; k++)
        {
          printf("     ");
          if (k == BLOCO - 1)
            printf("%c", cache[j][i].marcador);
        }
      }
    }
    printf("\n");
  }
  printf("\n");
}

// função para imprimir a cache
void imprimeCache(ElemCache cache[QTDCPU][LENCACHE])
{
  int i, j, k;
  for (i = 0; i < QTDCPU; i++)
  {
    printf("----- PROCESSADOR %d -----\n", i + 1);
    // printf("----- CACHE %d -----\n", contCacheImp++);
    // percorre todos os blocos da cache
    for (j = 0; j < LENCACHE; j++)
    {
      // só imprime se estiver ocupada
      if (cache[i][j].ocupada == 1)
      {
        // imprime o valor da linha
        printf("linha %2d: ", j + 1);
        // imprime todos os elementos do bloco
        for (k = 0; k < BLOCO; k++)
        {
          printf("%4d ", cache[i][j].elemento[k]);
        }
        printf("%c\n", cache[i][j].marcador);
      }
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
void fifo(int **acessos, int *ram, ElemCache caches[QTDCPU][LENCACHE], int i, int l[LENCACHE], int matfifo[QTDCPU][LENCACHE])
{
  int mod, j, k, len, indiceCache, aux, z, x, ale, marcar_volta = 0;
  // verifico se o valor deve ser modificado
  if (acessos[i][2] == 0)
  {
    // não é modificado, imprimo um guia para validar a execução
    printf("Acesso ao valor, na RAM, %d no indice %d, pelo processador %d:\n", ram[acessos[i][0]], acessos[i][0], acessos[i][1] + 1);
  }
  else
  {
    // é modificado, gera um numero aleatorio e imprimo um guia para validar a execução
    ale = randomInt(100, 1000 + LENRAM);
    printf("Acesso ao valor, na RAM, %d no indice %d, pelo processador %d, valor modificado é %d:\n", ram[acessos[i][0]], acessos[i][0], acessos[i][1] + 1, ale);
  }
  // resto da divisão do indice do acesso pelo tamanho do bloco
  mod = acessos[i][0] % BLOCO;
  // variavel que vai delimitar o for para preencher o bloco da cache
  len = (acessos[i][0] + BLOCO) - mod;
  // verifica o primeiro bloco não ocupado da cache, caso não exista retorna -1
  aux = verificaCache(caches[acessos[i][1]]);
  // varre a cache procurando o elemento que está na fila, caso ele esteja na cache
  // retorna o inidice que ele está que fica armazenado em z e em x o indice do bloco
  z = varreCache(caches[acessos[i][1]], acessos[i][0], &x);

  //  o elemento está na cache
  if (z > -1)
  {
    // verifico se esse elemento deve ser modificado
    if (acessos[i][2] == 1)
    {
      // modifico o elemento e marca o elemento como modificado
      caches[acessos[i][1]][z].elemento[x] = ale;
      caches[acessos[i][1]][z].m[x] = 1;
      // marco como modificado
      caches[acessos[i][1]][z].marcador = 'M';
      // varre a cache, caso o elemento esteja em algum outro bloco deve ser marcado como indisponivel
      varreCaches(caches, acessos[i][0], acessos[i][1], 1, &marcar_volta);
      write_hit++;
    }
    else
      read_hit++;
  }
  // o elemento não está na cache
  else
  {
    // se aux é maior que -1 significa que existe ao menos um bloco disponivel na cache
    if (aux > -1)
    {
      // caso o elemento deve ser modificado
      if (acessos[i][2] == 1)
        // marca como uma leitura falha
        write_miss++;
      else
        // caso não deve ser modificado, marca como leitura falha
        read_miss++;
      // indiceCache vai receber o valor que vamos utilizar na cache
      indiceCache = aux;
      // vet fifo recebe o valor da posição 0
      matfifo[acessos[i][1]][l[acessos[i][1]]] = aux;
      // incrementa o indice do vetor para realizar o fifo
      l[acessos[i][1]]++;
      // define o bloco da cache como ocupado
      caches[acessos[i][1]][indiceCache].ocupada = 1;
    }
    else
    {
      if (acessos[i][2] == 1)
        // marca como uma leitura falha
        write_miss++;
      else
        // caso não deve ser modificado, marca como leitura falha
        read_miss++;
      // indiceCache vai receber o valor que vamos utilizar na cache
      // aqui esse valor vai ser o primeiro elemento do vetor vetfifo
      indiceCache = removePrimeiro(matfifo[acessos[i][1]]);
      // chama a função writeBack para atualizar os valores na RAM
      writeBack(caches[acessos[i][1]], ram, indiceCache);
    }

    // j vai começar do primeiro indice do bloco da RAM e vai até o ultimo indice
    for (j = (acessos[i][0] - mod), k = 0; j < len; j++, k++)
    {
      // o bloco da cache vai receber os indices do bloco da RAM
      caches[acessos[i][1]][indiceCache].indice[k] = j;
      // verifica se o elemento que deve ser modificado e se está no elemento do bloco
      if (acessos[i][2] == 1 && acessos[i][0] == j)
      {
        // o elemento na cache vai receber um valor aleatorio
        caches[acessos[i][1]][indiceCache].elemento[k] = ale;
        // marco que aquele elemento foi modificado
        caches[acessos[i][1]][indiceCache].m[k] = 1;
        // marco como modificado para o FIFO
        caches[acessos[i][1]][indiceCache].marcador = 'M';
        // varre a cache para invalidar elementos em outras caches
        varreCaches(caches, acessos[i][0], acessos[i][1], 0, &marcar_volta);
      }
      else
      {
        // caso o elemento não precisa de modificação carrego o indice do bloco da RAM
        // no indice equivalente da cache
        caches[acessos[i][1]][indiceCache].elemento[k] = ram[j];
        if (acessos[i][2] != 1)
        {
          // marca ele como exclusivo
          caches[acessos[i][1]][indiceCache].marcador = 'E';
          // varre a cache verificando se ele já está marcado como exclusivo em outro elementos e marca como compartilhado
          varreCaches(caches, acessos[i][0], acessos[i][1], 1, &marcar_volta);
          if (marcar_volta)
            // caso ele exista marca como compartilhado
            caches[acessos[i][1]][indiceCache].marcador = 'C';
        }
      }
    }
    // deve ter mais código aqui
  }
  // imprimo o estado atual da cache
  if (BLOCO > 4 || QTDCPU > 4)
    imprimeCache(caches);
  else
    imprimeCacheHorizontal(caches);
  // imprimo a quantidade de hit e miss
  printf("\nREAD-HIT: %d READ-MISS: %d WRITE-HIT %d WRITE-MISS %d\n", read_hit, read_miss, write_hit, write_miss);
}

// bloco de código para gerar acessos na memoria RAM
int randomInt(int min, int max) { return min + rand() % (max + 1 - min); }

// função para gerar acessos aleatorios
int gerarAcessos(int n, int **acessos)
{
  int i;
  for (i = 0; i < n; i++)
  {
    // atribuição de um valor aleatorio para acesso na RAM, vai de 0 até o tamanho da RAM - 1
    acessos[i][0] = randomInt(0, LENRAM - 1);
    // atribui o valor da cache que deve fazer o processamento
    acessos[i][1] = randomInt(0, QTDCPU - 1);
    // se o elemento deve ou não ser modificado, um vetor global com os seguinte elementos:
    // {1, 0, 0, 0, 1, 0, 0, 1, 1, 0} onde 1 é para modificar e 0 para não modificar
    acessos[i][2] = modVet[randomInt(0, 9)];
  }
}

// inicia memora RAM
void iniciaRam(int *ram)
{
  for (int i = 0; i < LENRAM; i++)
    // gera um valor aleatorio para cada elemento da ram, que vai de 100 a 1000 + tamanho da RAM
    ram[i] = randomInt(100, 1000 + LENRAM);
}

// inicia memoria cache
void iniciaCache(ElemCache cache[QTDCPU][LENCACHE])
{
  int i, j, k;
  for (i = 0; i < QTDCPU; i++)
  {
    for (j = 0; j < LENCACHE; j++)
    {
      // todos os blocos da cache são definidos como vazio
      cache[i][j].ocupada = 0;
      cache[i][j].marcador = ' ';
      for (k = 0; k < BLOCO; k++)
      {
        // os elementos recebem valor -1 inicialmente
        cache[i][j].elemento[k] = -1;
        // o vetor que marcar se o elemento foi modificado ou não recebe valor 0
        cache[i][j].m[k] = 0;
        // e o indice do elemento da RAM recebe o valor -1
        cache[i][j].indice[k] = -1;
      }
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
