#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Q03.h"


/* Converte "A1" ou "c10" em (coluna, linha) = (0..7, 0..19) */
int converter_texto_para_posicao(const char *texto, int *coluna, int *linha) {
    int i = 0;
    int numero = 0;
    int resultado = 0;
    char letra;

    letra = texto[0];

    if (letra >= 'A' && letra <= 'H') {
        *coluna = letra - 'A';
    } else if (letra >= 'a' && letra <= 'h') {
        *coluna = letra - 'a';
    } else {
        resultado = 1; /* erro */
    }

    i = 1;
    numero = 0;
    while (texto[i] >= '0' && texto[i] <= '9') {
        numero = numero * 10 + (texto[i] - '0');
        i++;
    }

    if (numero >= 1 && numero <= NUM_LINHAS) {
        *linha = numero - 1;
    } else {
        resultado = 1;
    }

    return resultado;
}

/* Aloca matriz de adjacência tamanho x tamanho, inicializada com 0 */
int **alocar_matriz_adjacencia(int tamanho) {
    int **matriz;
    int i, j;

    matriz = (int **)malloc(tamanho * sizeof(int *));
    if (matriz != NULL) {
        for (i = 0; i < tamanho; i++) {
            matriz[i] = (int *)malloc(tamanho * sizeof(int));
            if (matriz[i] != NULL) {
                for (j = 0; j < tamanho; j++) {
                    matriz[i][j] = 0;
                }
            }
        }
    }

    return matriz;
}

/* Libera a matriz de adjacência */
void liberar_matriz_adjacencia(int **matriz, int tamanho) {
    int i;

    if (matriz != NULL) {
        for (i = 0; i < tamanho; i++) {
            free(matriz[i]);
        }
        free(matriz);
    }

}

/* Remove todas as arestas de "indice" para outras células */
void limpar_dependencias(int **matriz, int indice) {
    int j;

    for (j = 0; j < TOTAL_CELULAS; j++) {
        matriz[indice][j] = 0;
    }

}

/* Avalia recursivamente o valor de uma célula seguindo o grafo */
double avaliar_celula(Celula *planilha, int **matriz_adj, int indice,
                      int *em_calculo, int *calculado) {
    double resultado;
    int tipo;
    int j;
    int contador;
    double acumulador;
    int primeiro;
    Celula *cel;

    cel = &planilha[indice];

    if (calculado[indice]) {
        /* Já calculamos essa célula antes, usamos o valor em cache */
        resultado = cel->valor_cache;
    } else if (em_calculo[indice]) {
        /* Se chegamos novamente na mesma célula, temos um ciclo */
        printf("Aviso: ciclo detectado envolvendo a celula %d. Usando valor 0.\n", indice);
        resultado = 0.0;
    } else {
        em_calculo[indice] = 1;  /* marcamos que estamos calculando essa */
        tipo = cel->tipo;
        resultado = 0.0;

        if (tipo == TIPO_VAZIO) {
            resultado = 0.0;
        } else if (tipo == TIPO_CONSTANTE) {
            resultado = cel->valor_constante;
        } else if (tipo == TIPO_REFERENCIA) {
            int destino;
            destino = -1;
            /* Existe apenas uma célula de onde depende */
            for (j = 0; j < TOTAL_CELULAS; j++) {
                if (matriz_adj[indice][j] == 1) {
                    destino = j;
                }
            }
            if (destino != -1) {
                resultado = avaliar_celula(planilha, matriz_adj, destino,
                                           em_calculo, calculado);
            } else {
                resultado = 0.0;
            }
        } else {
            /* Funções: soma, max, min, media */
            acumulador = 0.0;
            contador = 0;
            primeiro = 1;

            for (j = 0; j < TOTAL_CELULAS; j++) {
                if (matriz_adj[indice][j] == 1) {
                    double valor_dep;
                    valor_dep = avaliar_celula(planilha, matriz_adj, j,
                                               em_calculo, calculado);

                    if (tipo == TIPO_SOMA || tipo == TIPO_MEDIA) {
                        acumulador = acumulador + valor_dep;
                        contador = contador + 1;
                    } else if (tipo == TIPO_MAX) {
                        if (primeiro) {
                            acumulador = valor_dep;
                            primeiro = 0;
                        } else if (valor_dep > acumulador) {
                            acumulador = valor_dep;
                        }
                        contador = contador + 1;
                    } else if (tipo == TIPO_MIN) {
                        if (primeiro) {
                            acumulador = valor_dep;
                            primeiro = 0;
                        } else if (valor_dep < acumulador) {
                            acumulador = valor_dep;
                        }
                        contador = contador + 1;
                    }
                }
            }

            if (tipo == TIPO_SOMA) {
                resultado = acumulador;
            } else if (tipo == TIPO_MEDIA) {
                if (contador > 0) {
                    resultado = acumulador / contador;
                } else {
                    resultado = 0.0;
                }
            } else if (tipo == TIPO_MAX || tipo == TIPO_MIN) {
                if (contador > 0) {
                    resultado = acumulador;
                } else {
                    resultado = 0.0;
                }
            }
        }

        em_calculo[indice] = 0;
        calculado[indice] = 1;
        cel->valor_cache = resultado;
    }

    return resultado;
}

/* Recalcula todas as células usando a função recursiva acima */
void recalcular_planilha(Celula *planilha, int **matriz_adj) {
    int *em_calculo;
    int *calculado;
    int i;

    em_calculo = (int *)malloc(TOTAL_CELULAS * sizeof(int));
    calculado = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (em_calculo != NULL && calculado != NULL) {
        for (i = 0; i < TOTAL_CELULAS; i++) {
            em_calculo[i] = 0;
            calculado[i] = 0;
        }
        for (i = 0; i < TOTAL_CELULAS; i++) {
            avaliar_celula(planilha, matriz_adj, i, em_calculo, calculado);
        }
    }

    free(em_calculo);
    free(calculado);

}

/* Imprime a planilha com os valores já calculados */
void imprimir_planilha(Celula *planilha) {
    int linha, coluna;
    int indice;

    printf("\n    ");
    for (coluna = 0; coluna < NUM_COLUNAS; coluna++) {
        printf("   %c   ", 'A' + coluna);
    }
    printf("\n");

    for (linha = 0; linha < NUM_LINHAS; linha++) {
        printf("%2d |", linha + 1);
        for (coluna = 0; coluna < NUM_COLUNAS; coluna++) {
            indice = linha * NUM_COLUNAS + coluna;
            printf(" %6.1f", planilha[indice].valor_cache);
        }
        printf("\n");
    }

}

