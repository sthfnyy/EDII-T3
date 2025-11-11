#include <stdio.h>
#include <time.h>
#include "Q01.h"


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
    montar_matriz_adjacencia(matriz_adj);

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

    indice_inicio = codificar_torre(configuracao_inicial);
    indice_destino = codificar_torre(configuracao_final);

    printf("\nIndice da configuracao inicial: %d\n", indice_inicio);
    printf("Indice da configuracao final  : %d\n\n", indice_destino);

    /* Mede apenas o tempo do Dijkstra */
    tempo_inicio = clock();
    dijkstra(matriz_adj, indice_inicio, indice_destino, antecessor, distancia);
    tempo_fim = clock();

    tempo_segundos = (double)(tempo_fim - tempo_inicio) / CLOCKS_PER_SEC;

    if (distancia[indice_destino] >= INFINITO)
    {
        printf("Nao existe caminho da configuracao inicial ate a final.\n");
    }
    else
    {
        printf("Menor numero de movimentos: %d\n", distancia[indice_destino]);
        printf("Tempo gasto pelo Dijkstra: %f segundos.\n", tempo_segundos);
        imprimir_caminho(antecessor, indice_inicio, indice_destino);
    }

    return 0;
}
