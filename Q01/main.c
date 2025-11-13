#include <stdio.h>
#include <stdlib.h>   
#include <time.h>
#include "Q01.h"

int main(void)
{
    int ret = 0;
    int i;
    
    int (**pmatriz_adj) = NULL;          
    int (*matriz_adj)[NUM_ESTADOS] = NULL; 

    int *antecessor = (int*)malloc(sizeof(int) * NUM_ESTADOS);
    int *distancia  = (int*)malloc(sizeof(int) * NUM_ESTADOS);

    int configuracao_inicial[NUM_DISCOS];
    int configuracao_final[NUM_DISCOS];
    int indice_inicio;
    int indice_destino;

    clock_t tempo_inicio;
    clock_t tempo_fim;
    double tempo_segundos;

    int aloc_ok = (antecessor != NULL) && (distancia != NULL);

    if (aloc_ok)
    {
        printf("=== Torre de Hanoi com Grafo (Matriz) e Dijkstra (%d discos) ===\n\n", NUM_DISCOS);
        printf("Cada configuracao e um vetor de %d inteiros (0..%d).\n", NUM_DISCOS, NUM_PINOS-1);
        printf("Disco 0 = menor, disco %d = maior.\n\n", NUM_DISCOS - 1);

        construir_grafo_matriz(&pmatriz_adj);

        /* Le a configuracao inicial do usuario */
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

        tempo_inicio = clock();

        dijkstra_matriz(pmatriz_adj, indice_inicio, indice_destino, antecessor, distancia);

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
    }
    else
    {
        printf("Falha ao alocar memoria para as estruturas principais.\n");
        ret = 1;
    }

    if (distancia  != NULL) free(distancia);
    if (antecessor != NULL) free(antecessor);

    if (pmatriz_adj != NULL) liberar_matriz(pmatriz_adj);


    return 0;
}
