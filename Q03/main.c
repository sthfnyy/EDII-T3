#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Q03.h"


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