#include <stdio.h>
#include <time.h>

#define NUM_DISCOS 4
#define NUM_PINOS 3
#define NUM_ESTADOS 81   /* 3^4 */
#define INFINITO 1000000

/* ================== PROTÓTIPOS ================== */

void decodificar_estado(int indice, int vetor_estado[]);
int  codificar_estado(int vetor_estado[]);

void decodificar_estado_rec(int indice, int posicao, int vetor_estado[]);
int  codificar_estado_rec(int posicao, int vetor_estado[], int multiplicador);

int  encontrar_topo_pino(int vetor_estado[], int pino);

void inicializar_matriz_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int linha, int coluna);
void construir_matriz_adjacencia(int matriz[NUM_ESTADOS][NUM_ESTADOS]);
void construir_matriz_adjacencia_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int indice_estado);

int  encontrar_menor_distancia_rec(int distancia[], int visitado[],
                                   int posicao, int menor_valor, int indice_menor);
void dijkstra(int matriz[NUM_ESTADOS][NUM_ESTADOS],
              int inicio,
              int destino,
              int antecessor[],
              int distancia[]);

void imprimir_estado(int vetor_estado[]);
void imprimir_caminho(int antecessor[], int inicio, int destino);
void imprimir_caminho_rec(int caminho[], int tamanho, int indice);

/* ================== CONVERSÕES ÍNDICE ↔ ESTADO ================== */

/* Versão recursiva da decodificação: quebra o número em base 3 */
void decodificar_estado_rec(int indice, int posicao, int vetor_estado[])
{
    if (posicao < NUM_DISCOS)
    {
        vetor_estado[posicao] = indice % NUM_PINOS;
        decodificar_estado_rec(indice / NUM_PINOS, posicao + 1, vetor_estado);
    }

    return;
}

/* Função “bonita” que o resto do código usa */
void decodificar_estado(int indice, int vetor_estado[])
{
    decodificar_estado_rec(indice, 0, vetor_estado);
    return;
}

/* Versão recursiva da codificação: monta número em base 3 */
int codificar_estado_rec(int posicao, int vetor_estado[], int multiplicador)
{
    int resultado;

    if (posicao == NUM_DISCOS)
    {
        resultado = 0;
    }
    else
    {
        resultado = vetor_estado[posicao] * multiplicador +
                    codificar_estado_rec(posicao + 1,
                                         vetor_estado,
                                         multiplicador * NUM_PINOS);
    }

    return resultado;
}

/* Função usada pelo restante do código */
int codificar_estado(int vetor_estado[])
{
    int resultado;
    resultado = codificar_estado_rec(0, vetor_estado, 1);
    return resultado;
}

/* ================== FUNÇÃO AUXILIAR: TOPO DO PINO ================== */

/* Encontra o menor disco em um pino (disco 0 é o menor).
   Retorna o índice do disco ou -1 se o pino estiver vazio. */
int encontrar_topo_pino(int vetor_estado[], int pino)
{
    int disco;
    int resultado;

    resultado = -1;

    for (disco = 0; disco < NUM_DISCOS; disco++)
    {
        if (vetor_estado[disco] == pino && resultado == -1)
        {
            /* Primeira vez que achamos disco nesse pino -> é o menor dele */
            resultado = disco;
        }
    }

    return resultado;
}

/* ================== MATRIZ DE ADJACÊNCIA (RECURSÃO) ================== */

/* Inicializa a matriz toda com 0 usando recursão em linha/coluna */
void inicializar_matriz_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS],
                            int linha,
                            int coluna)
{
    if (linha < NUM_ESTADOS)
    {
        matriz[linha][coluna] = 0;

        if (coluna + 1 < NUM_ESTADOS)
        {
            inicializar_matriz_rec(matriz, linha, coluna + 1);
        }
        else
        {
            inicializar_matriz_rec(matriz, linha + 1, 0);
        }
    }

    return;
}

/* Constrói as arestas do grafo, chamando uma função recursiva por estado */
void construir_matriz_adjacencia_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS],
                                     int indice_estado)
{
    int estado_atual[NUM_DISCOS];
    int estado_novo[NUM_DISCOS];
    int pino_origem, pino_destino;
    int disco_mover;
    int topo_destino;
    int i;
    int indice_novo;

    if (indice_estado < NUM_ESTADOS)
    {
        /* Converte o índice para a configuração de discos (vetor) */
        decodificar_estado(indice_estado, estado_atual);

        /* Tenta mover o disco do topo de cada pino para os outros pinos */
        for (pino_origem = 0; pino_origem < NUM_PINOS; pino_origem++)
        {
            disco_mover = encontrar_topo_pino(estado_atual, pino_origem);

            if (disco_mover != -1)
            {
                for (pino_destino = 0; pino_destino < NUM_PINOS; pino_destino++)
                {
                    if (pino_destino != pino_origem)
                    {
                        topo_destino = encontrar_topo_pino(estado_atual, pino_destino);

                        /* Movimento é válido se destino vazio ou disco_mover é menor */
                        if (topo_destino == -1 || disco_mover < topo_destino)
                        {
                            for (i = 0; i < NUM_DISCOS; i++)
                            {
                                estado_novo[i] = estado_atual[i];
                            }

                            estado_novo[disco_mover] = pino_destino;
                            indice_novo = codificar_estado(estado_novo);
                            matriz[indice_estado][indice_novo] = 1;
                        }
                    }
                }
            }
        }

        /* Chama recursivamente para o próximo estado */
        construir_matriz_adjacencia_rec(matriz, indice_estado + 1);
    }

    return;
}

/* Função principal para montar a matriz de adjacência */
void construir_matriz_adjacencia(int matriz[NUM_ESTADOS][NUM_ESTADOS])
{
    inicializar_matriz_rec(matriz, 0, 0);
    construir_matriz_adjacencia_rec(matriz, 0);
    return;
}

/* ================== DIJKSTRA (COM AJUDA RECURSIVA) ================== */

/* Busca recursivamente o vértice não visitado com menor distância */
int encontrar_menor_distancia_rec(int distancia[],
                                  int visitado[],
                                  int posicao,
                                  int menor_valor,
                                  int indice_menor)
{
    int resultado;
    int novo_menor_valor;
    int novo_indice_menor;

    novo_menor_valor = menor_valor;
    novo_indice_menor = indice_menor;

    if (posicao < NUM_ESTADOS)
    {
        if (!visitado[posicao] && distancia[posicao] < novo_menor_valor)
        {
            novo_menor_valor = distancia[posicao];
            novo_indice_menor = posicao;
        }

        resultado = encontrar_menor_distancia_rec(distancia,
                                                  visitado,
                                                  posicao + 1,
                                                  novo_menor_valor,
                                                  novo_indice_menor);
    }
    else
    {
        resultado = indice_menor;
    }

    return resultado;
}

/* Algoritmo de Dijkstra (parte principal continua iterativa) */
void dijkstra(int matriz[NUM_ESTADOS][NUM_ESTADOS],
              int inicio,
              int destino,
              int antecessor[],
              int distancia[])
{
    int visitado[NUM_ESTADOS];
    int i, j;
    int u;
    int indice_menor;

    /* Inicializa vetores */
    for (i = 0; i < NUM_ESTADOS; i++)
    {
        distancia[i] = INFINITO;
        antecessor[i] = -1;
        visitado[i] = 0;
    }

    distancia[inicio] = 0;

    for (i = 0; i < NUM_ESTADOS; i++)
    {
        /* Usa função recursiva para achar o vértice com menor distância */
        indice_menor = encontrar_menor_distancia_rec(distancia,
                                                     visitado,
                                                     0,
                                                     INFINITO,
                                                     -1);

        if (indice_menor == -1)
        {
            u = -1;
        }
        else
        {
            u = indice_menor;
        }

        if (u == -1)
        {
            /* Não há mais vértices alcançáveis */
            break;
        }

        visitado[u] = 1;

        /* Relaxa as arestas saindo de u */
        for (j = 0; j < NUM_ESTADOS; j++)
        {
            if (matriz[u][j] == 1 && !visitado[j])
            {
                if (distancia[u] + 1 < distancia[j])
                {
                    distancia[j] = distancia[u] + 1;
                    antecessor[j] = u;
                }
            }
        }
    }

    return;
}

/* ================== IMPRESSÃO DE ESTADOS E CAMINHO ================== */

void imprimir_estado(int vetor_estado[])
{
    int i;
    printf("[ ");
    for (i = 0; i < NUM_DISCOS; i++)
    {
        printf("%d ", vetor_estado[i]);
    }
    printf("]\n");
    return;
}

/* Imprime o caminho usando um vetor de estados, com recursão */
void imprimir_caminho_rec(int caminho[], int tamanho, int indice)
{
    int estado[NUM_DISCOS];

    if (indice < tamanho)
    {
        decodificar_estado(caminho[indice], estado);
        printf("Passo %d: ", indice);
        imprimir_estado(estado);
        imprimir_caminho_rec(caminho, tamanho, indice + 1);
    }

    return;
}

/* Reconstrói o caminho (iterativo) e imprime usando recursão */
void imprimir_caminho(int antecessor[], int inicio, int destino)
{
    int caminho[NUM_ESTADOS];
    int tamanho;
    int atual;
    int i;

    tamanho = 0;
    atual = destino;

    /* Reconstrói o caminho de trás pra frente */
    while (atual != -1 && tamanho < NUM_ESTADOS)
    {
        caminho[tamanho] = atual;
        tamanho++;

        if (atual == inicio)
        {
            atual = -1; /* força a saída do laço */
        }
        else
        {
            atual = antecessor[atual];
        }
    }

    if (tamanho > 0)
    {
        /* Inverte o vetor para ficar do início até o destino */
        for (i = 0; i < tamanho / 2; i++)
        {
            int temp;
            temp = caminho[i];
            caminho[i] = caminho[tamanho - 1 - i];
            caminho[tamanho - 1 - i] = temp;
        }

        printf("\nCaminho (do estado inicial ate o estado final):\n");
        imprimir_caminho_rec(caminho, tamanho, 0);
    }
    else
    {
        printf("Nao foi possivel reconstruir o caminho.\n");
    }

    return;
}

/* ================== FUNÇÃO PRINCIPAL ================== */

int main()
{
    int matriz_adj[NUM_ESTADOS][NUM_ESTADOS];
    int configuracao_inicial[NUM_DISCOS];
    int configuracao_final[NUM_DISCOS];
    int indice_inicio;
    int indice_destino;
    int antecessor[NUM_ESTADOS];
    int distancia[NUM_ESTADOS];
    int i;

    clock_t tempo_inicio;
    clock_t tempo_fim;
    double tempo_segundos;

    printf("=== Torre de Hanoi com Grafo e Dijkstra (4 discos) ===\n\n");
    printf("Cada configuracao sera representada por um vetor de 4 inteiros,\n");
    printf("onde cada posicao indica o pino do disco:\n");
    printf("disco 0 = menor, disco 3 = maior.\n");
    printf("Pinos possiveis: 0, 1, 2.\n\n");

    /* Monta a matriz de adjacencia do grafo (usando recursao) */
    construir_matriz_adjacencia(matriz_adj);

    /* Le a configuracao inicial do usuario */
    printf("Digite a configuracao inicial (pino de cada disco):\n");
    for (i = 0; i < NUM_DISCOS; i++)
    {
        printf("Disco %d (0, 1 ou 2): ", i);
        scanf("%d", &configuracao_inicial[i]);
    }

    /* Define a configuracao final: todos os discos no pino 2 */
    for (i = 0; i < NUM_DISCOS; i++)
    {
        configuracao_final[i] = 2;
    }

    indice_inicio = codificar_estado(configuracao_inicial);
    indice_destino = codificar_estado(configuracao_final);

    printf("\nEstado inicial codificado: %d\n", indice_inicio);
    printf("Estado final codificado  : %d\n\n", indice_destino);

    /* Mede apenas o tempo do Dijkstra */
    tempo_inicio = clock();
    dijkstra(matriz_adj, indice_inicio, indice_destino, antecessor, distancia);
    tempo_fim = clock();

    tempo_segundos = (double)(tempo_fim - tempo_inicio) / CLOCKS_PER_SEC;

    if (distancia[indice_destino] >= INFINITO)
    {
        printf("Nao existe caminho do estado inicial ao estado final.\n");
    }
    else
    {
        printf("Menor numero de movimentos: %d\n", distancia[indice_destino]);
        printf("Tempo gasto pelo Dijkstra: %f segundos.\n", tempo_segundos);
        imprimir_caminho(antecessor, indice_inicio, indice_destino);
    }

    return 0;
}
