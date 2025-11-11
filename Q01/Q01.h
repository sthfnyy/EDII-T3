
#define NUM_DISCOS 4
#define NUM_PINOS 3
#define NUM_ESTADOS 81   /* 3^4 */
#define INFINITO 1000000


void decodificar_torre(int indice, int pinos_dos_discos[]);
int  codificar_torre(int pinos_dos_discos[]);
void decodificar_torre_rec(int indice, int posicao, int pinos_dos_discos[]);
int  codificar_torre_rec(int posicao, int pinos_dos_discos[], int multiplicador);

/* torre / pinos */
int  encontrar_topo_pino(int pinos_dos_discos[], int pino);

/* matriz de adjacência */
void zerar_matriz_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int linha, int coluna);
void montar_matriz_adjacencia(int matriz[NUM_ESTADOS][NUM_ESTADOS]);
void preencher_matriz_adjacencia_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int indice_torre);

/* dijkstra */
int  escolher_proximo_vertice_rec(int distancia[], int visitado[],
                                   int posicao, int menor_valor, int indice_menor);
void dijkstra(int matriz[NUM_ESTADOS][NUM_ESTADOS], int inicio, int destino, int antecessor[], int distancia[]);

void imprimir_torre(int pinos_dos_discos[]);
void imprimir_caminho(int antecessor[], int inicio, int destino);
void imprimir_caminho_rec(int caminho[], int tamanho, int indice);