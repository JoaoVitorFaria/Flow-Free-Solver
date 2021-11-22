/*
Trabalho 02: Resolvedor para o jogo Flow Free implementado em C++
Disciplina: Analise e Projeto de Algoritmos II
Professor: Pedro Henrique Del Bianco Hokama
Aluno: João Vitor de Faria
Matricula: 2019006030
A implementação do BFS foi baseada no código disponível em: https://www.geeksforgeeks.org/shortest-distance-two-cells-matrix-grid/
*/

/*
Este código tem como objetivo resolver instâncias do jogo Flow Free.
Para testar uma instância insira uma entrada no seguinte formato: 

tamanho
000A000
00B000A
000000B
C00000C
D0E0000
0000000
00D000E

Anotações:
1 - O jogo apresenta uma solução, mas não necessariamente será a única solução;
2 - Todas instâncias devem ser representadas por uma matriz quadrada tamanho x tamanho;
3 - As letras representam os pontos que devem ser ligados. 
4 - As letras devem ser inseridas em ordem alfabética
5 - Espaços vazios devem ser representados por 0.
*/


#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<utility>
#include<queue>

// Cores usadas no output
#define corNormal "\x1B[0m"
#define vermelho "\x1B[31m"
#define verde "\x1B[32m"
#define amarelo "\x1B[33m"
#define azul "\x1B[34m"
#define magenta "\x1B[35m"
#define cyan "\x1B[36m"
#define branco "\x1B[37m"
// As cores abaixo são as mesmas declaradas anteriormente, mas em negrito.
#define vermelho2 "\033[1m\033[31m"
#define verde2 "\033[1m\033[32m"
#define amarelo2 "\033[1m\033[33m"
#define azul2 "\033[1m\033[34m"
#define magenta2 "\033[1m\033[35m"
#define cyan2 "\033[1m\033[36m"
#define branco2 "\033[1m\033[317m"

//=========================================Funções usadas na resolução====================================================
struct vertice;
void preparaResolucao(int tamanho_entrada, char **matriz);
int bfs(int tamanho_entrada, char **matriz, char letra);
void inicializaVisited (int tamanho_entrada, char **matriz, bool **visited, vertice *vertice_origem, char letra);
void defineLetrasInstancia(int tamanho_entrada, char ** matriz, char alfabeto[], int &posicao_alfabeto, int &cont_letras, std::vector<char>& vetor_letras);
void defineCaminho(int tamanho_entrada, char **matriz, std::vector<char> vetor_letras, char **copia_matriz_original, int cont_letras);
bool valida(int atual, int tamanho_entrada);
void atualizaMatriz(char **matriz, std::vector<std::pair<int, int>>pos_x_y,  char letra);
void printaResultado(int tamanho_entrada, char **matriz);
//========================================================================================================================

int main(int argc, char *argv[])
{
  int tamanho_entrada;
  char entrada;
  char **matriz;

  // Descomente a linha abaixo para testar varias instâncias em sequência
  //while(1 == scanf("%d", &tamanho_entrada) && tamanho_entrada){
  scanf("%d", &tamanho_entrada);

  // Declaracao e leitura da matriz de entrada
  matriz = (char**)malloc(sizeof(char*)*tamanho_entrada);
  for(int i = 0; i < tamanho_entrada; i++)
  {
    matriz[i] = (char*)malloc(sizeof(char) * tamanho_entrada);
  }

  for(int i = 0; i < tamanho_entrada; i++)
  {
    for(int j = 0; j < tamanho_entrada; j++)
    {
      scanf(" %c", &entrada);
      matriz[i][j] = entrada;
    }
  }

  preparaResolucao(tamanho_entrada, matriz);
  printaResultado(tamanho_entrada, matriz);
  free(matriz);
  //}
  return 0;
}

//=========================================STRUCT QUE DEFINE UM VERTICE NA MATRIZ=========================================
struct vertice
{
  int row;
  int col;
  std::vector<std::pair<int, int>> pos_x_y;
  int dist;
  vertice(int x, int y,std::vector<std::pair<int, int>> z , int d): row(x), col(y), pos_x_y(z), dist(d){}
};
//========================================================================================================================

//==================================================IMPRIME RESULTADO=====================================================
void printaResultado(int tamanho_entrada, char **matriz)
{
  printf("%d\n", tamanho_entrada);
  for(int i = 0; i < tamanho_entrada; i++)
  {
    for(int j = 0; j < tamanho_entrada; j++)
    {
      // Descomente o switch case abaixo para ter um output colorido. Obs: Funciona em ambiente Linux
      /*switch (matriz[i][j]){
        case 'A':
          printf("%s%c", vermelho, matriz[i][j]);
          break;
        case 'B':
          printf("%s%c", azul, matriz[i][j]);
          break;
        case 'C':
          printf("%s%c", cyan, matriz[i][j]);
          break;
        case 'D':
          printf("%s%c", verde, matriz[i][j]);
          break;
        case 'E':
          printf("%s%c", amarelo, matriz[i][j]);
          break;
        case 'F':
          printf("%s%c", branco, matriz[i][j]);
          break;
        case 'G':
          printf("%s%c", magenta, matriz[i][j]);
          break;
        case 'H':
          printf("%s%c", vermelho2, matriz[i][j]);
          break;
        case 'I':
          printf("%s%c", amarelo2, matriz[i][j]);
          break;
        case 'J':
          printf("%s%c", azul2, matriz[i][j]);
          break;
        case 'K':
          printf("%s%c", magenta2, matriz[i][j]);
          break;
        case 'L':
          printf("%s%c", cyan2, matriz[i][j]);
          break;
        case 'M':
          printf("%s%c", branco2, matriz[i][j]);
          break;
        case 'O':
          printf("%s%c", verde2, matriz[i][j]);
          break;
        default:
          printf("%s%c",corNormal, matriz[i][j]);
      }
      */printf("%c", matriz[i][j]);
    }
    printf("\n");
  }
}
//========================================================================================================================

//====================================SALVA AS LETRAS UTILIZADAS NESTA INSTÂNCIA==========================================
void defineLetrasInstancia(int tamanho_entrada, char ** matriz, char alfabeto[], int &posicao_alfabeto, int &cont_letras, std::vector<char>& vetor_letras)
{
  // Faz uma busca pela instância, procurando as letras em ordem alfabética
  while(true)
  {
    bool letra_encontrada = false;
    for(int i = 0; i < tamanho_entrada; i++)
    {
      for(int j = 0; j < tamanho_entrada; j++)
      {
        if(!letra_encontrada && matriz[i][j] == alfabeto[posicao_alfabeto])
        {
          letra_encontrada = true;
          cont_letras += 1;
        }
      }
    }
    posicao_alfabeto++;
    if(!letra_encontrada)break;
  }
  // Copia as letras presentes na instância para o vector vetor_letras
  for(int i = 0; i < cont_letras; i++)
  {
    vetor_letras.push_back(alfabeto[i]);
  }
}
//========================================================================================================================

//==============================================DEFINE O CAMINHO PARA CADA PAR============================================
void defineCaminho(int tamanho_entrada, char **matriz, std::vector<char> vetor_letras, char **copia_matriz_original, int cont_letras)
{
  int posicao_alfabeto = 0;
  while( posicao_alfabeto < cont_letras)
  {
    bool erro = false;
    // Descobre o caminho mais curto entre os dois pontos
    int distancia = bfs(tamanho_entrada, matriz , vetor_letras[posicao_alfabeto]);

    // Caso não tenha encontrado o caminho
    if(distancia == -1)
    {
      // Restaura-se a matriz utilizando a cópia
      for(int i = 0; i < tamanho_entrada ; i++)
      {
        for(int j = 0; j < tamanho_entrada; j++)
        {
          matriz[i][j] = copia_matriz_original[i][j];
        }
      }
      // Altera-se a ordem de exploração
      erro = true;
      int primeiro = posicao_alfabeto - 1;
      int segundo = posicao_alfabeto;
      while(true)
      {
        if(primeiro < 0)break;
        std::swap(vetor_letras[primeiro], vetor_letras[segundo]);
        primeiro--;
        segundo--;
      }
      posicao_alfabeto = 0;
    }
    // Caso o caminho tenha sido encontrado
    if(!erro)
    {
      posicao_alfabeto++;
    }
  }
}
//========================================================================================================================

//======================================INICIALIZA A MATRIZ DE VERTICES VISITADOS=========================================
void inicializaVisited (int tamanho_entrada, char **matriz, bool **visited, vertice *vertice_origem, char letra)
{
  for(int i = 0; i < tamanho_entrada; i++)
  {
    for(int j = 0; j < tamanho_entrada; j++)
    {
      if(matriz[i][j] == '0')visited[i][j] = false;
      else visited[i][j] = true;
      if(matriz[i][j] == letra)
      {
        vertice_origem->row = i ;
        vertice_origem->col = j ;
        visited[i][j] = false;
      }
    }
  }
}
//========================================================================================================================

//=============================================MARCA O CAMINHO ENCONTRADO=================================================
void atualizaMatriz(char **matriz, std::vector<std::pair<int, int>>caminho_percorrido, char letra)
{
for(const std::pair<int, int>& vertice: caminho_percorrido)
  {
    matriz[vertice.first][vertice.second] = letra;
  }
}
//========================================================================================================================
 
//================================================VER O CAMINHO PERCORRIDO================================================
void verCaminho(std::vector<std::pair<int, int>>caminho_percorrido){
  for(const std::pair<int, int>& vertice: caminho_percorrido)
  {
    printf(" i:%d, j:%d\n", vertice.first, vertice.second);
  }
}
//========================================================================================================================

//=========================================VERIFICA SE UM NOVO VERTICE É VALIDO===========================================
bool valida(int atual, int tamanho_entrada){
  return (atual < tamanho_entrada) && (atual >= 0);
}
//========================================================================================================================

//=======================================USA BFS PARA RETORNAR A MENOR DISTANCIA==========================================
int bfs(int tamanho_entrada, char **matriz, char letra)
{
  // Cria-se um matriz para identificar vértices visitados
  bool **visited;
  visited = (bool**)malloc(sizeof(bool*)*tamanho_entrada);
  for(int i = 0; i < tamanho_entrada; i++)
  {
    visited[i] = (bool*)malloc(sizeof(bool) * tamanho_entrada);
  }

  std::vector<std::pair<int, int>> caminho;
  vertice vertice_origem(0, 0, caminho, 0);

  // Esta função marca todas posições que possuem letras como posições que não podem ser visitadas
  // Além disso, configura o vertice_origem.
  inicializaVisited(tamanho_entrada, matriz, visited, &vertice_origem, letra);

  // Fila usada para exploração dos vértices no BFS
  std::queue<vertice> fila;
  fila.push(vertice_origem);
  visited[vertice_origem.row][vertice_origem.col] = true;
  bool verifica = true;

  // Aplica BFS a partir do vertice de origem ate o outro vertice de mesma letra
  while(!fila.empty()){
    vertice atual = fila.front();
    fila.pop();

    // Condicao de parada
    if(matriz[atual.row][atual.col] == letra && !verifica){
      atualizaMatriz(matriz, atual.pos_x_y, letra );
      // Descomente o bloco de codigo abaixo para ver o caminho definido entre os dois vertices
      /*
      printf("\nAnalisando letra : [%c]\n", letra);
      verCaminho(atual.pos_x_y);
      */
      return atual.dist;

    }
    verifica = false;

    // Explora o vertice de baixo
    if(valida(atual.row +1, tamanho_entrada) && visited[atual.row+1][atual.col] == false){
      atual.pos_x_y.push_back(std::make_pair(atual.row, atual.col));
      visited[atual.row+1][atual.col] = true;
      fila.push(vertice(atual.row+1, atual.col, atual.pos_x_y, atual.dist+1 ));
    }

    // Explora o vertice de cima
    if(valida(atual.row  - 1, tamanho_entrada)&& visited[atual.row-1][atual.col] == false){
      atual.pos_x_y.push_back(std::make_pair(atual.row, atual.col));
      visited[atual.row-1][atual.col] = true;
      fila.push(vertice(atual.row-1, atual.col, atual.pos_x_y, atual.dist+1));
    } 

    // Explora o vertice da direita
    if(valida(atual.col  + 1, tamanho_entrada) && visited[atual.row][atual.col+1] == false){
      atual.pos_x_y.push_back(std::make_pair(atual.row, atual.col));
      visited[atual.row][atual.col+1] = true;
      fila.push(vertice(atual.row, atual.col+1, atual.pos_x_y, atual.dist+1));
    }

    // Explora o vertice da esquerda

    if(valida(atual.col  - 1, tamanho_entrada) && visited[atual.row][atual.col-1] == false){
      atual.pos_x_y.push_back(std::make_pair(atual.row, atual.col));
      visited[atual.row][atual.col-1] = true;
      fila.push(vertice(atual.row, atual.col-1,  atual.pos_x_y, atual.dist+1));  
    }
     // Descomente o bloco de codigo abaixo para visualizar como seriam as resolucoes caso fossem permitidos movimentos nas verticais
    /*
    // Explora o sudeste
     if(atual.row +1 <tamanho_entrada && atual.col+1 < tamanho_entrada && visited[atual.row+1][atual.col+1] == false){
      atual.pos_x_y.push_back(std::make_pair(atual.row, atual.col));
      visited[atual.row+1][atual.col+1] = true;
      fila.push(vertice(atual.row+1, atual.col+1, atual.pos_x_y, atual.dist+1));
    }

    // Explora o Sudoeste
     if(atual.row +1 <tamanho_entrada && atual.col-1 >= 0 && visited[atual.row+1][atual.col-1] == false){
      atual.pos_x_y.push_back(std::make_pair(atual.row, atual.col));
      visited[atual.row+1][atual.col-1] = true;
      fila.push(vertice(atual.row+1, atual.col-1, atual.pos_x_y, atual.dist+1));
    }
    // Explora o Noroeste
      if(atual.row - 1 >= 0 && atual.col-1 >= 0 && visited[atual.row-1][atual.col-1] == false){
      atual.pos_x_y.push_back(std::make_pair(atual.row, atual.col));
      visited[atual.row-1][atual.col-1] = true;
      fila.push(vertice(atual.row-1, atual.col-1, atual.pos_x_y, atual.dist+1));
    }
    // Explora o Nordeste
      if(atual.row - 1 >= 0 && atual.col+1 < tamanho_entrada && visited[atual.row-1][atual.col+1] == false){
      atual.pos_x_y.push_back(std::make_pair(atual.row, atual.col));
      visited[atual.row-1][atual.col+1] = true;
      fila.push(vertice(atual.row-1, atual.col+1, atual.pos_x_y, atual.dist+1));
    }*/
  }
  return -1;  

}
//========================================================================================================================

//==================================================INICIA RESOLUÇÃO=======================================================
void preparaResolucao(int tamanho_entrada, char **matriz)
{
  std::vector<char> vetor_letras;
  int cont_letras = 0, posicao_alfabeto = 0;
  // Prepara um vetor que contem o alfabeto inteiro
  char alfabeto[26];
  for(int i = 65; i < 91; i++)
  {
    alfabeto[i - 65] = (char)i;
  }

  // Esta função percorre a instância e salva as letras presentes no vetor_letras.
  defineLetrasInstancia(tamanho_entrada, matriz, alfabeto, posicao_alfabeto, cont_letras, vetor_letras);
  
  /* Descomente para ver as letras da instancia
  for(int i = 0; i < cont_letras; i++){
    printf("%c ", vetor_letras[i]);
  }
  printf("\n");*/

  // Cria uma copia da matriz original
  char **copia_matriz_original;
  copia_matriz_original = (char**) malloc(sizeof(char*)*tamanho_entrada);
  for(int i = 0; i < tamanho_entrada; i++)
  {
    copia_matriz_original[i] = (char*)malloc(sizeof(char) * tamanho_entrada);
  }
  for(int i = 0; i < tamanho_entrada; i++)
  {
    for(int j = 0; j < tamanho_entrada; j++)
    {
      copia_matriz_original[i][j] = matriz[i][j];
    }
  }

  defineCaminho(tamanho_entrada, matriz, vetor_letras, copia_matriz_original, cont_letras);

}
//========================================================================================================================