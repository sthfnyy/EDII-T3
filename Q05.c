#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_COLUNAS 8       /* A..H */
#define NUM_LINHAS 20       /* 1..20 */
#define TOTAL_CELULAS (NUM_COLUNAS * NUM_LINHAS)

#define TIPO_VAZIO     0
#define TIPO_CONSTANTE 1
#define TIPO_REFERENCIA 2
#define TIPO_SOMA      3
#define TIPO_MAX       4
#define TIPO_MIN       5
#define TIPO_MEDIA     6

/* Nó da lista de adjacência: indica uma dependência da célula */
typedef struct NodoAdjacencia {
    int indice_destino;                 /* índice da célula da qual depende */
    struct NodoAdjacencia *proximo;
} NodoAdjacencia;

/* Cada célula tem o tipo, um valor constante e a lista de dependências */
typedef struct {
    int tipo;
    double valor_constante;
    double valor_cache;                 /* valor já calculado */
    NodoAdjacencia *lista_dependencias; /* lista de adjacência */
} Celula;

/* ---------- Protótipos ---------- */

int converter_texto_para_posicao(const char *texto, int *coluna, int *linha);
void limpar_dependencias(Celula *planilha, int indice_origem);
void adicionar_dependencia(Celula *planilha, int indice_origem, int indice_destino);
double avaliar_celula(Celula *planilha, int indice,
                      int *em_calculo, int *calculado);
void recalcular_planilha(Celula *planilha);
void imprimir_planilha(Celula *planilha);
void liberar_todas_listas(Celula *planilha);

/* ---------- Converte "A1" em (coluna, linha) ---------- */

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

/* ---------- Manipulação da lista de adjacência ---------- */

/* Apaga todas as dependências de uma célula (libera a lista ligada) */
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

    return;
}

/* Adiciona uma aresta: origem -> destino (origem depende de destino) */
void adicionar_dependencia(Celula *planilha, int indice_origem, int indice_destino) {
    NodoAdjacencia *novo;

    novo = (NodoAdjacencia *)malloc(sizeof(NodoAdjacencia));
    if (novo != NULL) {
        novo->indice_destino = indice_destino;
        novo->proximo = planilha[indice_origem].lista_dependencias;
        planilha[indice_origem].lista_dependencias = novo;
    }

    return;
}

/* ---------- Avaliação recursiva das células ---------- */

double avaliar_celula(Celula *planilha, int indice,
                      int *em_calculo, int *calculado) {
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

/* Recalcula a planilha inteira chamando avaliar_celula recursivamente */
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

    return;
}

/* ---------- Impressão ---------- */

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

    return;
}

/* Libera todas as listas de adjacência da planilha */
void liberar_todas_listas(Celula *planilha) {
    int i;

    for (i = 0; i < TOTAL_CELULAS; i++) {
        limpar_dependencias(planilha, i);
    }

    return;
}


/*adicionando aqui*/
/* Imprime algo como A1, C3, H20 a partir do indice 0..TOTAL_CELULAS-1 */
void imprimir_nome_celula(int indice) {
    int linha;
    int coluna;

    linha = indice / NUM_COLUNAS;   /* 0..19  */
    coluna = indice % NUM_COLUNAS;  /* 0..7   */

    printf("%c%d", 'A' + coluna, linha + 1);

    return;
}

void dfs_recursivo_dependencias(Celula *planilha,
                                int indice_atual,
                                int *visitado) {
    NodoAdjacencia *nodo;

    visitado[indice_atual] = 1;
    printf("Visitando (DFS): ");
    imprimir_nome_celula(indice_atual);
    printf("\n");

    nodo = planilha[indice_atual].lista_dependencias;
    while (nodo != NULL) {
        if (!visitado[nodo->indice_destino]) {
            dfs_recursivo_dependencias(planilha,
                                       nodo->indice_destino,
                                       visitado);
        }
        nodo = nodo->proximo;
    }

    return;
}

void busca_profundidade_dependencias(Celula *planilha,
                                     int indice_inicial) {
    int *visitado;
    int i;

    visitado = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (visitado != NULL) {
        for (i = 0; i < TOTAL_CELULAS; i++) {
            visitado[i] = 0;
        }

        printf("\n=== DFS a partir de ");
        imprimir_nome_celula(indice_inicial);
        printf(" ===\n");

        dfs_recursivo_dependencias(planilha, indice_inicial, visitado);
    } else {
        printf("Erro ao alocar memoria para DFS.\n");
    }

    free(visitado);

    return;
}


void busca_largura_dependencias(Celula *planilha,
                                int indice_inicial) {
    int *visitado;
    int *fila;
    int inicio;
    int fim;
    int i;
    int atual;
    NodoAdjacencia *nodo;

    visitado = (int *)malloc(TOTAL_CELULAS * sizeof(int));
    fila = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (visitado != NULL && fila != NULL) {
        for (i = 0; i < TOTAL_CELULAS; i++) {
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

        while (inicio < fim) {
            atual = fila[inicio];
            inicio = inicio + 1;

            printf("Visitando (BFS): ");
            imprimir_nome_celula(atual);
            printf("\n");

            nodo = planilha[atual].lista_dependencias;
            while (nodo != NULL) {
                if (!visitado[nodo->indice_destino]) {
                    visitado[nodo->indice_destino] = 1;
                    fila[fim] = nodo->indice_destino;
                    fim = fim + 1;
                }
                nodo = nodo->proximo;
            }
        }
    } else {
        printf("Erro ao alocar memoria para BFS.\n");
    }

    if (visitado != NULL) {
        free(visitado);
    }
    if (fila != NULL) {
        free(fila);
    }

    return;
}



/* ---------- Programa principal ---------- */
int main() {
    Celula *planilha;
    int i;
    char linha_entrada[100];
    int continuar;

    planilha = (Celula *)malloc(TOTAL_CELULAS * sizeof(Celula));
    continuar = 1;

    if (planilha != NULL) {
        /* Inicializa todas as células */
        for (i = 0; i < TOTAL_CELULAS; i++) {
            planilha[i].tipo = TIPO_VAZIO;
            planilha[i].valor_constante = 0.0;
            planilha[i].valor_cache = 0.0;
            planilha[i].lista_dependencias = NULL;
        }

        printf("Planilha (colunas A..H, linhas 1..20).\n");
        printf("Entrada: CELULA VALOR\n");
        printf("Exemplos:\n");
        printf("  A1 10\n");
        printf("  B1 =A1\n");
        printf("  C1 @soma(A1..B2)\n");
        printf("Funcoes: @soma, @max, @min, @media\n");
        printf("Digite FIM para encerrar.\n\n");

        while (continuar) {
            char texto_celula[10];
            char texto_valor[80];
            int lidos;
            int coluna;
            int linha;
            int indice_celula;

            printf("> ");
            if (fgets(linha_entrada, sizeof(linha_entrada), stdin) == NULL) {
                continuar = 0;
            } else {
                if (strncmp(linha_entrada, "FIM", 3) == 0 ||
                    strncmp(linha_entrada, "fim", 3) == 0) {
                    continuar = 0;
                } else {
                    lidos = sscanf(linha_entrada, "%9s %79[^\n]",
                                   texto_celula, texto_valor);
                    if (lidos == 2) {
                        if (converter_texto_para_posicao(texto_celula,
                                                         &coluna, &linha) == 0) {
                            char *v;
                            Celula *cel;

                            v = texto_valor;
                            while (*v == ' ') {
                                v = v + 1;
                            }

                            indice_celula = linha * NUM_COLUNAS + coluna;
                            cel = &planilha[indice_celula];

                            limpar_dependencias(planilha, indice_celula);
                            cel->tipo = TIPO_VAZIO;
                            cel->valor_constante = 0.0;

                            if (v[0] == '@') {
                                char nome_funcao[10];
                                char intervalo[40];
                                int ok_parse;
                                ok_parse = 0;

                                if (sscanf(v, "@%9[^ (](%39[^)])",
                                           nome_funcao, intervalo) == 2) {
                                    char *pontos;
                                    char texto_inicio[10];
                                    char texto_fim[10];
                                    int tam_inicio;
                                    int k;
                                    int col_i, lin_i, col_f, lin_f;
                                    int col_ini, col_fim, lin_ini, lin_fim;
                                    pontos = strstr(intervalo, "..");
                                    if (pontos != NULL) {
                                        tam_inicio = (int)(pontos - intervalo);
                                        for (k = 0; k < tam_inicio && k < 9; k++) {
                                            texto_inicio[k] = intervalo[k];
                                        }
                                        texto_inicio[tam_inicio] = '\0';
                                        strcpy(texto_fim, pontos + 2);

                                        if (converter_texto_para_posicao(texto_inicio,
                                                                         &col_i, &lin_i) == 0 &&
                                            converter_texto_para_posicao(texto_fim,
                                                                         &col_f, &lin_f) == 0) {
                                            ok_parse = 1;
                                            if (strcmp(nome_funcao, "soma") == 0) {
                                                cel->tipo = TIPO_SOMA;
                                            } else if (strcmp(nome_funcao, "max") == 0) {
                                                cel->tipo = TIPO_MAX;
                                            } else if (strcmp(nome_funcao, "min") == 0) {
                                                cel->tipo = TIPO_MIN;
                                            } else if (strcmp(nome_funcao, "media") == 0) {
                                                cel->tipo = TIPO_MEDIA;
                                            } else {
                                                cel->tipo = TIPO_VAZIO;
                                            }

                                            lin_ini = (lin_i < lin_f) ? lin_i : lin_f;
                                            lin_fim = (lin_i > lin_f) ? lin_i : lin_f;
                                            col_ini = (col_i < col_f) ? col_i : col_f;
                                            col_fim = (col_i > col_f) ? col_i : col_f;

                                            for (linha = lin_ini; linha <= lin_fim; linha++) {
                                                for (coluna = col_ini; coluna <= col_fim; coluna++) {
                                                    int indice_dep;
                                                    indice_dep = linha * NUM_COLUNAS + coluna;
                                                    adicionar_dependencia(planilha,
                                                                          indice_celula,
                                                                          indice_dep);
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!ok_parse) {
                                    printf("Intervalo ou funcao invalidos.\n");
                                }

                            } else if (v[0] == '=') {
                                char texto_ref[10];
                                if (sscanf(v, "=%9s", texto_ref) == 1) {
                                    int col_ref;
                                    int lin_ref;
                                    if (converter_texto_para_posicao(texto_ref,
                                                                     &col_ref, &lin_ref) == 0) {
                                        int indice_dep;
                                        cel->tipo = TIPO_REFERENCIA;
                                        indice_dep = lin_ref * NUM_COLUNAS + col_ref;
                                        adicionar_dependencia(planilha,
                                                              indice_celula,
                                                              indice_dep);
                                    } else {
                                        printf("Referencia invalida.\n");
                                    }
                                }
                            } else {
                                double numero;
                                if (sscanf(v, "%lf", &numero) == 1) {
                                    cel->tipo = TIPO_CONSTANTE;
                                    cel->valor_constante = numero;
                                } else {
                                    printf("Valor numerico invalido.\n");
                                }
                            }
                        } else {
                            printf("Celula invalida.\n");
                        }
                    } else {
                        printf("Entrada invalida. Use: CELULA VALOR\n");
                    }
                }
            }
        }

        /* ======= AQUI ENTRA O BLOCO DAS BUSCAS (DFS / BFS) ======= */
        {
            char texto_busca[10];
            int col_b;
            int lin_b;
            int indice_busca;

            printf("\nDigite uma celula para fazer DFS/BFS nas dependencias (ex: C1): ");
            if (fgets(linha_entrada, sizeof(linha_entrada), stdin) != NULL &&
                sscanf(linha_entrada, "%9s", texto_busca) == 1) {
                if (converter_texto_para_posicao(texto_busca, &col_b, &lin_b) == 0) {
                    indice_busca = lin_b * NUM_COLUNAS + col_b;
                    busca_profundidade_dependencias(planilha, indice_busca);
                    busca_largura_dependencias(planilha, indice_busca);
                } else {
                    printf("Celula invalida para busca.\n");
                }
            }
        }
        /* ========================================================= */

        /* Calcula tudo e mostra a planilha final */
        recalcular_planilha(planilha);
        imprimir_planilha(planilha);

        liberar_todas_listas(planilha);
        free(planilha);
    } else {
        printf("Erro ao alocar memoria.\n");
    }

    return 0;
}

/*
A1 10
B1 20
A2 30
C1 @soma(A1..B2)
D1 =C1
FIM


*/
