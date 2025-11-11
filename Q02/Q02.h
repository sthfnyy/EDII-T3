
#define NUM_DISCOS 4
#define NUM_PINOS 3
#define NUM_ESTADOS 81   /* 3^4 */
#define INFINITO 1000000
#define MAX_VIZINHOS 10  /* número máximo de vizinhos de um estado */

/* Estrutura para lista de adjacência */
typedef struct
{
    int vizinhos[MAX_VIZINHOS];
    int qtd_vizinhos;
} ListaAdj;

/* ================== PROTÓTIPOS ================== */

void decodificar_estado(int indice, int vetor_estado[]);
int  codificar_estado(int vetor_estado[]);

void decodificar_estado_rec(int indice, int posicao, int vetor_estado[]);
int  codificar_estado_rec(int posicao, int vetor_estado[], int multiplicador);

int  encontrar_topo_pino(int vetor_estado[], int pino);

void inicializar_matriz_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int linha, int coluna);
void construir_matriz_adjacencia_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int indice_estado);
void construir_matriz_adjacencia(int matriz[NUM_ESTADOS][NUM_ESTADOS]);

void inicializar_grafo_lista_rec(ListaAdj grafo[], int indice);
void construir_grafo_lista_rec(ListaAdj grafo[], int indice_estado);
void construir_grafo_lista(ListaAdj grafo[]);
void adicionar_vizinho(ListaAdj grafo[], int origem, int destino);

int  encontrar_menor_distancia_rec(int distancia[], int visitado[],
                                   int posicao, int menor_valor, int indice_menor);

void dijkstra_matriz(int matriz[NUM_ESTADOS][NUM_ESTADOS],
                     int inicio,
                     int destino,
                     int antecessor[],
                     int distancia[]);

void dijkstra_lista(ListaAdj grafo[],
                    int inicio,
                    int destino,
                    int antecessor[],
                    int distancia[]);

void imprimir_estado(int vetor_estado[]);
void imprimir_caminho_rec(int caminho[], int tamanho, int indice);
void imprimir_caminho(int antecessor[], int inicio, int destino);
