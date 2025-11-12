#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Q02.h"

int main()
{
    int flag = 0; 

    /* grafo e vetores grandes agora no heap */
    ListaAdj *grafo_lista   = (ListaAdj*)malloc(sizeof(ListaAdj) * NUM_ESTADOS);
    int      *antecessor_lista = (int*)malloc(sizeof(int) * NUM_ESTADOS);
    int      *distancia_lista  = (int*)malloc(sizeof(int) * NUM_ESTADOS);

    int aloc_ok = (grafo_lista != NULL) && (antecessor_lista != NULL) && (distancia_lista != NULL);
    if (!aloc_ok)
    {
        printf("Falha ao alocar memoria para estruturas principais.\n");
        flag = 1; 
    }

    if (aloc_ok)
    {
        int configuracao_inicial[NUM_DISCOS];
        int configuracao_final[NUM_DISCOS];

        int indice_inicio;
        int indice_destino;

        int i;

        clock_t tempo_inicio;
        clock_t tempo_fim;
        double tempo_lista;

        printf("=== Torre de Hanoi com Grafo (Lista) e Dijkstra ===\n\n");
        printf("Cada configuracao e um vetor de 4 inteiros, onde cada posicao indica o pino do disco.\n");
        printf("Disco 0 = menor, disco 3 = maior. Pinos possiveis: 0, 1, 2.\n\n");

        /* Constroi grafo em lista de adjacencia */
        construir_grafo_lista(grafo_lista);

        printf("Digite a configuracao inicial (pino de cada disco):\n");
        for (i = 0; i < NUM_DISCOS; i++)
        {
            printf("Disco %d (0, 1 ou 2): ", i);
            scanf("%d", &configuracao_inicial[i]);
        }

        /* Configuracao final: todos os discos no pino 2 */
        for (i = 0; i < NUM_DISCOS; i++)
        {
            configuracao_final[i] = 2;
        }

        indice_inicio  = codificar_estado(configuracao_inicial);
        indice_destino = codificar_estado(configuracao_final);

        printf("\nEstado inicial codificado: %d\n", indice_inicio);
        printf("Estado final codificado  : %d\n\n", indice_destino);

        /* Dijkstra com lista de adjacencia */
        tempo_inicio = clock();
        dijkstra_lista(grafo_lista, indice_inicio, indice_destino,
                       antecessor_lista, distancia_lista);
        tempo_fim = clock();
        tempo_lista = (double)(tempo_fim - tempo_inicio) / CLOCKS_PER_SEC;

        if (distancia_lista[indice_destino] >= INFINITO)
        {
            printf("Nao existe caminho do estado inicial ao estado final.\n");
        }
        else
        {
            printf("Menor numero de movimentos (lista):  %d\n", distancia_lista[indice_destino]);
            printf("Tempo Dijkstra (lista) : %f segundos\n", tempo_lista);

            imprimir_caminho(antecessor_lista, indice_inicio, indice_destino);
        }
    }

    free(distancia_lista);
    free(antecessor_lista);
    free(grafo_lista);

    return 0; 
}
