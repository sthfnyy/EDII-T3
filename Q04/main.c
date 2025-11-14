#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Q04.h"

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
