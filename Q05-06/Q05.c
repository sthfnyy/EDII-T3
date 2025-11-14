#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include "Q05.h"


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
                valor_dep = avaliar_celula(planilha, nodo->indice_destino, em_calculo, calculado);

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


/*======== Funções da Q05 ==========*/
void imprimir_nome_celula(int indice)
{
    int linha;
    int coluna;

    linha = indice / NUM_COLUNAS;
    coluna = indice % NUM_COLUNAS;

    printf("%c%d", 'A' + coluna, linha + 1);
}

/* --------- DFS (busca em profundidade) --------- */

void dfs_recursivo_dependencias(Celula *planilha,
                                int indice_atual,
                                int *visitado)
{
    NodoAdjacencia *nodo;

    visitado[indice_atual] = 1;
    printf("Visitando (DFS): ");
    imprimir_nome_celula(indice_atual);
    printf("\n");

    nodo = planilha[indice_atual].lista_dependencias;
    while (nodo != NULL)
    {
        if (!visitado[nodo->indice_destino])
        {
            dfs_recursivo_dependencias(planilha,
                                       nodo->indice_destino,
                                       visitado);
        }
        nodo = nodo->proximo;
    }
}

void busca_profundidade_dependencias(Celula *planilha,
                                     int indice_inicial)
{
    int *visitado;
    int i;

    visitado = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (visitado != NULL)
    {
        for (i = 0; i < TOTAL_CELULAS; i++)
        {
            visitado[i] = 0;
        }

        printf("\n=== DFS a partir de ");
        imprimir_nome_celula(indice_inicial);
        printf(" ===\n");

        dfs_recursivo_dependencias(planilha, indice_inicial, visitado);

        free(visitado);
    }
    else
    {
        printf("Erro ao alocar memoria para DFS.\n");
    }
}

/* --------- BFS (busca em largura) --------- */

void busca_largura_dependencias(Celula *planilha,
                                int indice_inicial)
{
    int *visitado;
    int *fila;
    int inicio;
    int fim;
    int atual;
    int i;
    NodoAdjacencia *nodo;

    visitado = (int *)malloc(TOTAL_CELULAS * sizeof(int));
    fila = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (visitado != NULL && fila != NULL)
    {
        for (i = 0; i < TOTAL_CELULAS; i++)
        {
            visitado[i] = 0;
        }

        inicio = 0;
        fim = 0;

        fila[fim] = indice_inicial;
        fim = fim + 1;
        visitado[indice_inicial] = 1;

        printf("\n=== BFS a partir de ");
        imprimir_nome_celula(indice_inicial);
        printf(" ===\n");

        while (inicio < fim)
        {
            atual = fila[inicio];
            inicio = inicio + 1;

            printf("Visitando (BFS): ");
            imprimir_nome_celula(atual);
            printf("\n");

            nodo = planilha[atual].lista_dependencias;
            while (nodo != NULL)
            {
                if (!visitado[nodo->indice_destino])
                {
                    visitado[nodo->indice_destino] = 1;
                    fila[fim] = nodo->indice_destino;
                    fim = fim + 1;
                }
                nodo = nodo->proximo;
            }
        }

        free(visitado);
        free(fila);
    }
    else
    {
        printf("Erro ao alocar memoria para BFS.\n");
        if (visitado != NULL) free(visitado);
        if (fila != NULL) free(fila);
    }
}

/*====== Funções da Q06 ======= */
void medir_tempos_experimento(Celula *planilha, int indice_busca)
{
    int repeticao;
    clock_t inicio;
    clock_t fim;
    double soma_insercao;
    double soma_bfs;
    double soma_dfs;

    int indice_teste_insercao;
    int indice_dep_exemplo;

    soma_insercao = 0.0;
    soma_bfs = 0.0;
    soma_dfs = 0.0;

    /* Vamos usar uma celula de teste para insercao (H20 = ultima) */
    indice_teste_insercao = (NUM_LINHAS - 1) * NUM_COLUNAS + (NUM_COLUNAS - 1); /* H20 */
    /* E uma celula simples como dependencia, por exemplo A1 (indice 0) */
    indice_dep_exemplo = 0;

    printf("\n=== Medindo tempos medios (30 repeticoes) ===\n");

    for (repeticao = 0; repeticao < 30; repeticao++)
    {
        /* (a) Insercao de dados no grafo:
           limpamos as dependencias da celula de teste
           e adicionamos uma dependencia simples (H20 depende de A1)
        */
        inicio = clock();
        limpar_dependencias(planilha, indice_teste_insercao);
        adicionar_dependencia(planilha,
                              indice_teste_insercao,
                              indice_dep_exemplo);
        fim = clock();
        soma_insercao = soma_insercao +
                        (double)(fim - inicio) / CLOCKS_PER_SEC;

        /* (b) Busca em largura (BFS) a partir da celula escolhida */
        inicio = clock();
        busca_largura_dependencias(planilha, indice_busca);
        fim = clock();
        soma_bfs = soma_bfs +
                   (double)(fim - inicio) / CLOCKS_PER_SEC;

        /* (c) Busca em profundidade (DFS) a partir da mesma celula */
        inicio = clock();
        busca_profundidade_dependencias(planilha, indice_busca);
        fim = clock();
        soma_dfs = soma_dfs +
                   (double)(fim - inicio) / CLOCKS_PER_SEC;
    }

    printf("\n--- Resultados medios (30 repeticoes) ---\n");
    printf("(a) Insercao no grafo (H20 -> A1): %f segundos\n",
           soma_insercao / 30.0);
    printf("(b) Busca em Largura (BFS)       : %f segundos\n",
           soma_bfs / 30.0);
    printf("(c) Busca em Profundidade (DFS)  : %f segundos\n",
           soma_dfs / 30.0);
}


/*===== Função de preenchimento automatico para teste ===== */
void preencher_exemplo(Celula *planilha)
{
    int i;

    for (i = 0; i < TOTAL_CELULAS; i++)
    {
        planilha[i].tipo = TIPO_VAZIO;
        planilha[i].valor_constante = 0.0;
        planilha[i].valor_cache = 0.0;
        planilha[i].lista_dependencias = NULL;
    }

    /* A1 = 10, B1 = 20, A2 = 30, C1 = @soma(A1..B2) */
    planilha[0].tipo = TIPO_CONSTANTE;   /* A1 */
    planilha[0].valor_constante = 10.0;

    planilha[1].tipo = TIPO_CONSTANTE;   /* B1 */
    planilha[1].valor_constante = 20.0;

    planilha[8].tipo = TIPO_CONSTANTE;   /* A2 (linha 2, col 0) */
    planilha[8].valor_constante = 30.0;

    /* C1 = soma(A1..B2) */
    {
        int indice_c1;
        int linha, coluna;

        indice_c1 = 2; /* C1 = linha 0, coluna 2 */
        planilha[indice_c1].tipo = TIPO_SOMA;

        for (linha = 0; linha <= 1; linha++)
        {
            for (coluna = 0; coluna <= 1; coluna++)
            {
                int indice_dep;
                indice_dep = linha * NUM_COLUNAS + coluna;
                adicionar_dependencia(planilha, indice_c1, indice_dep);
            }
        }
    }
}
