#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Q04.h"



int converter_texto_para_posicao(const char *texto, int *coluna, int *linha) {
    int resultado;
    int pos;
    int numero;
    char letra;

    resultado = 0;
    letra = texto[0];

    if (letra >= 'A' && letra <= 'H') {
        *coluna = letra - 'A';
    } else if (letra >= 'a' && letra <= 'h') {
        *coluna = letra - 'a';
    } else {
        resultado = 1;
    }

    pos = 1;
    numero = 0;
    while (texto[pos] >= '0' && texto[pos] <= '9') {
        numero = numero * 10 + (texto[pos] - '0');
        pos = pos + 1;
    }

    if (numero >= 1 && numero <= NUM_LINHAS) {
        *linha = numero - 1;
    } else {
        resultado = 1;
    }

    return resultado;
}


void limpar_dependencias(Celula *planilha, int indice_origem) {
    NodoAdjacencia *atual;
    NodoAdjacencia *proximo;

    atual = planilha[indice_origem].lista_dependencias;
    while (atual != NULL) {
        proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    planilha[indice_origem].lista_dependencias = NULL;
}


void adicionar_dependencia(Celula *planilha, int indice_origem, int indice_destino) {
    NodoAdjacencia *novo;

    novo = (NodoAdjacencia *)malloc(sizeof(NodoAdjacencia));
    if (novo != NULL) {
        novo->indice_destino = indice_destino;
        novo->proximo = planilha[indice_origem].lista_dependencias;
        planilha[indice_origem].lista_dependencias = novo;
    }

}

double avaliar_celula(Celula *planilha, int indice, int *em_calculo, int *calculado) 
{
    double resultado;
    Celula *cel;
    int tipo;
    NodoAdjacencia *nodo;
    int contador;
    double acumulador;
    int primeiro;

    cel = &planilha[indice];
    resultado = 0.0;

    if (calculado[indice]) {
        resultado = cel->valor_cache;
    } else if (em_calculo[indice]) {
        printf("Aviso: ciclo detectado na celula %d. Usando 0.\n", indice);
        resultado = 0.0;
    } else {
        em_calculo[indice] = 1;
        tipo = cel->tipo;

        if (tipo == TIPO_VAZIO) {
            resultado = 0.0;
        } else if (tipo == TIPO_CONSTANTE) {
            resultado = cel->valor_constante;
        } else if (tipo == TIPO_REFERENCIA) {
            /* Deve haver apenas uma dependência */
            if (cel->lista_dependencias != NULL) {
                int destino;
                destino = cel->lista_dependencias->indice_destino;
                resultado = avaliar_celula(planilha, destino,
                                           em_calculo, calculado);
            } else {
                resultado = 0.0;
            }
        } else {
            /* Funções: soma, max, min, media */
            acumulador = 0.0;
            contador = 0;
            primeiro = 1;
            nodo = cel->lista_dependencias;

            while (nodo != NULL) {
                double valor_dep;
                valor_dep = avaliar_celula(planilha, nodo->indice_destino,
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

                nodo = nodo->proximo;
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

void recalcular_planilha(Celula *planilha) {
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
            avaliar_celula(planilha, i, em_calculo, calculado);
        }
    }

    free(em_calculo);
    free(calculado);

}


void imprimir_planilha(Celula *planilha) {
    int linha;
    int coluna;
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


void liberar_todas_listas(Celula *planilha) {
    int i;

    for (i = 0; i < TOTAL_CELULAS; i++) {
        limpar_dependencias(planilha, i);
    }

}
