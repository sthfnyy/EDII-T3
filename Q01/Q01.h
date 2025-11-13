
#define NUM_DISCOS 4
#define NUM_PINOS 3
#define NUM_ESTADOS 81   /* 3^4 */
#define INFINITO 1000000

void decodificar_estado_rec(int indice, int posicao, int vetor_estado[]);
void decodificar_estado(int indice, int vetor_estado[]);
int  codificar_estado_rec(int posicao, int vetor_estado[], int multiplicador);
int  codificar_estado(int vetor_estado[]);
int  encontrar_topo_pino(int vetor_estado[], int pino);

int  alocar_matriz(int ***pmatriz);
void liberar_matriz(int **matriz);
void inicializar_matriz_rec(int **matriz, int linha, int coluna);

void construir_grafo_matriz_rec(int **matriz, int indice_estado);
void construir_grafo_matriz(int ***pmatriz); /* aloca, zera e preenche */

int  encontrar_menor_distancia_rec(int distancia[], int visitado[], int posicao, int menor_valor, int indice_menor);

void dijkstra_matriz(int **matriz, int inicio, int destino, int antecessor[], int distancia[]);

void imprimir_estado(int vetor_estado[]);
void imprimir_caminho_rec(int caminho[], int tamanho, int indice);
void imprimir_caminho(int antecessor[], int inicio, int destino);
