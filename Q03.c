#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_COLUNAS 8
#define NUM_LINHAS 20
#define TOTAL_CELULAS (NUM_COLUNAS * NUM_LINHAS)

#define TIPO_VAZIO 0
#define TIPO_CONSTANTE 1
#define TIPO_REFERENCIA 2
#define TIPO_SOMA 3
#define TIPO_MAX 4
#define TIPO_MIN 5
#define TIPO_MEDIA 6

/* Cada célula guarda o tipo e o valor numérico */
typedef struct {
    int tipo;              /* vazio, constante, referencia, soma, max, min, media */
    double valor_constante;/* usado quando for constante */
    double valor_cache;    /* valor já calculado da célula */
} Celula;

/* Protótipos */
int converter_texto_para_posicao(const char *texto, int *coluna, int *linha);
int **alocar_matriz_adjacencia(int tamanho);
void liberar_matriz_adjacencia(int **matriz, int tamanho);
void limpar_dependencias(int **matriz, int indice);
double avaliar_celula(Celula *planilha, int **matriz_adj, int indice,
                      int *em_calculo, int *calculado);
void recalcular_planilha(Celula *planilha, int **matriz_adj);
void imprimir_planilha(Celula *planilha);

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

    return;
}

/* Remove todas as arestas de "indice" para outras células */
void limpar_dependencias(int **matriz, int indice) {
    int j;

    for (j = 0; j < TOTAL_CELULAS; j++) {
        matriz[indice][j] = 0;
    }

    return;
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

    return;
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

    return;
}

int main() {
    Celula *planilha;
    int **matriz_adj;
    int i;
    char linha_entrada[100];
    int continuar;

    continuar = 1;

    planilha = (Celula *)malloc(TOTAL_CELULAS * sizeof(Celula));
    matriz_adj = alocar_matriz_adjacencia(TOTAL_CELULAS);

    if (planilha != NULL && matriz_adj != NULL) {
        /* Inicia todas as células vazias */
        for (i = 0; i < TOTAL_CELULAS; i++) {
            planilha[i].tipo = TIPO_VAZIO;
            planilha[i].valor_constante = 0.0;
            planilha[i].valor_cache = 0.0;
        }

        printf("Planilha rudimentar (colunas A..H, linhas 1..20).\n");
        printf("Comandos: CELULA VALOR\n");
        printf("Exemplos: A1 10\n");
        printf("          B1 =A1\n");
        printf("          C1 @soma(A1..B2)\n");
        printf("Funcoes: @soma, @max, @min, @media\n");
        printf("Digite FIM para encerrar a entrada e mostrar a planilha.\n\n");

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
                            int col_i;
                            int linha_i;
                            int col_f;
                            int linha_f;

                            /* Remove espaços iniciais no valor */
                            v = texto_valor;
                            while (*v == ' ') {
                                v++;
                            }

                            indice_celula = linha * NUM_COLUNAS + coluna;
                            cel = &planilha[indice_celula];
                            cel->tipo = TIPO_VAZIO;
                            cel->valor_constante = 0.0;
                            limpar_dependencias(matriz_adj, indice_celula);

                            if (v[0] == '@') {
                                /* Funções: @soma(A1..B2) etc. */
                                char nome_funcao[10];
                                char intervalo[40];

                                if (sscanf(v, "@%9[^ (](%39[^)])",
                                           nome_funcao, intervalo) == 2) {
                                    char *pontos;
                                    char texto_inicio[10];
                                    char texto_fim[10];
                                    int k;
                                    int tamanho_inicio;

                                    pontos = strstr(intervalo, "..");
                                    if (pontos != NULL) {
                                        /* Separa A1 e B2 (por exemplo) */
                                        tamanho_inicio = (int)(pontos - intervalo);
                                        for (k = 0; k < tamanho_inicio && k < 9; k++) {
                                            texto_inicio[k] = intervalo[k];
                                        }
                                        texto_inicio[tamanho_inicio] = '\0';
                                        strcpy(texto_fim, pontos + 2);

                                        if (converter_texto_para_posicao(texto_inicio,
                                                                         &col_i, &linha_i) == 0 &&
                                            converter_texto_para_posicao(texto_fim,
                                                                         &col_f, &linha_f) == 0) {

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

                                            /* Cria arestas no grafo para todas as
                                               células do intervalo */
                                            for (linha_i = linha_i;
                                                 linha_i <= linha_f;
                                                 linha_i++) {
                                                int c;
                                                for (c = col_i; c <= col_f; c++) {
                                                    int idx_dep;
                                                    idx_dep = linha_i * NUM_COLUNAS + c;
                                                    matriz_adj[indice_celula][idx_dep] = 1;
                                                }
                                            }
                                        }
                                    }
                                }
                            } else if (v[0] == '=') {
                                /* Referência simples: =B3 */
                                char texto_ref[10];

                                if (sscanf(v, "=%9s", texto_ref) == 1) {
                                    if (converter_texto_para_posicao(texto_ref,
                                                                     &col_i, &linha_i) == 0) {
                                        int idx_dep;
                                        cel->tipo = TIPO_REFERENCIA;
                                        idx_dep = linha_i * NUM_COLUNAS + col_i;
                                        matriz_adj[indice_celula][idx_dep] = 1;
                                    }
                                }
                            } else {
                                /* Número simples, por exemplo: 10 ou 3.5 */
                                double numero;

                                if (sscanf(v, "%lf", &numero) == 1) {
                                    cel->tipo = TIPO_CONSTANTE;
                                    cel->valor_constante = numero;
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

        /* Depois que o usuário termina, recalculamos e mostramos a planilha */
        recalcular_planilha(planilha, matriz_adj);
        imprimir_planilha(planilha);
    } else {
        printf("Erro ao alocar memoria.\n");
    }

    liberar_matriz_adjacencia(matriz_adj, TOTAL_CELULAS);
    free(planilha);

    return 0;
}


/*
A1 10
B1 20
A2 30
C1 @soma(A1..B2)
D1 =C1
FIM
Saída esperada:
    A       B       C       D       E       F       G       H
 1 |   10.0   20.0   60.0   60.0    0.0    0.0    0.0    0.0
 2 |   30

*/