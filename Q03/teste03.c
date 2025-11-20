#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_COLUNAS 8
#define TOTAL_LINHAS 20
#define TOTAL_CELULAS (TOTAL_COLUNAS * TOTAL_LINHAS)
#define TAMANHO_MAX_EXPRESSAO 64
#define VALOR_PADRAO 0.0

typedef enum {
    TIPO_CONTEUDO_NUMERO,
    TIPO_CONTEUDO_REFERENCIA,
    TIPO_CONTEUDO_FUNCAO,
    TIPO_CONTEUDO_VAZIA
} TipoConteudo;

typedef enum {
    TIPO_FUNCAO_NENHUMA,
    TIPO_FUNCAO_SOMA,
    TIPO_FUNCAO_MAX,
    TIPO_FUNCAO_MIN,
    TIPO_FUNCAO_MEDIA
} TipoFuncao;

typedef struct {
    TipoConteudo tipo_conteudo;
    double valor_numerico;
    char texto_expressao[TAMANHO_MAX_EXPRESSAO];

    int indice_referencia;

    TipoFuncao tipo_funcao;
    int indice_inicio_intervalo;
    int indice_fim_intervalo;
} Celula;

typedef struct {
    Celula* vetor_celulas;
    int** matriz_dependencias;
} Planilha;

char* ConverterIndiceParaCoordenada(int indice_celula, char* texto_coordenada);
int ConverterCoordenadaParaIndice(const char* texto_coordenada);
void AdicionarDependencia(Planilha* ponteiro_planilha, int indice_dependente, int indice_dependencia);
void RemoverDependencias(Planilha* ponteiro_planilha, int indice_celula);
int AnalisarExpressao(Planilha* ponteiro_planilha, int indice_celula, const char* texto_expressao);
double AvaliarCelula(Planilha* ponteiro_planilha, int indice_celula);
double CalcularFuncao(Planilha* ponteiro_planilha, int indice_celula_funcao);
void ExibirPlanilha(Planilha* ponteiro_planilha);
Planilha* InicializarPlanilha();
void LiberarPlanilha(Planilha* ponteiro_planilha);
void LoopPrincipal(Planilha* ponteiro_planilha);

/* -------------------------------------------------- */
/* Conversão de índice <-> coordenada                 */
/* -------------------------------------------------- */

char* ConverterIndiceParaCoordenada(int indice_celula, char* texto_coordenada) {
    char* ponteiro_resultado;
    int indice_linha;
    int indice_coluna;

    ponteiro_resultado = NULL;

    if (indice_celula >= 0) {
        if (indice_celula < TOTAL_CELULAS) {
            indice_coluna = indice_celula % TOTAL_COLUNAS;
            indice_linha = indice_celula / TOTAL_COLUNAS;
            sprintf(texto_coordenada, "%c%d", (char)('A' + indice_coluna), indice_linha + 1);
            ponteiro_resultado = texto_coordenada;
        }
    }

    return ponteiro_resultado;
}

int ConverterCoordenadaParaIndice(const char* texto_coordenada) {
    int indice_resultado;
    int tamanho_texto;
    char caractere_coluna;
    int indice_coluna;
    int indice_linha;

    indice_resultado = -1;

    if (texto_coordenada != NULL) {
        tamanho_texto = (int)strlen(texto_coordenada);
        if (tamanho_texto >= 2) {
            caractere_coluna = texto_coordenada[0];

            if (caractere_coluna >= 'a') {
                if (caractere_coluna <= 'z') {
                    caractere_coluna = (char)(caractere_coluna - 'a' + 'A');
                }
            }

            indice_coluna = caractere_coluna - 'A';
            indice_linha = atoi(texto_coordenada + 1) - 1;

            if (indice_coluna >= 0) {
                if (indice_coluna < TOTAL_COLUNAS) {
                    if (indice_linha >= 0) {
                        if (indice_linha < TOTAL_LINHAS) {
                            indice_resultado = indice_linha * TOTAL_COLUNAS + indice_coluna;
                        }
                    }
                }
            }
        }
    }

    return indice_resultado;
}

/* -------------------------------------------------- */
/* Grafo (matriz de dependências)                     */
/* -------------------------------------------------- */

void AdicionarDependencia(Planilha* ponteiro_planilha, int indice_dependente, int indice_dependencia) {
    int indices_validos;

    indices_validos = 0;

    if (ponteiro_planilha != NULL) {
        if (ponteiro_planilha->matriz_dependencias != NULL) {
            if (indice_dependente >= 0) {
                if (indice_dependente < TOTAL_CELULAS) {
                    if (indice_dependencia >= 0) {
                        if (indice_dependencia < TOTAL_CELULAS) {
                            indices_validos = 1;
                        }
                    }
                }
            }
        }
    }

    if (indices_validos == 1) {
        ponteiro_planilha->matriz_dependencias[indice_dependente][indice_dependencia] = 1;
    }
}

void RemoverDependencias(Planilha* ponteiro_planilha, int indice_celula) {
    int indice_coluna;
    int indices_validos;

    indices_validos = 0;
    indice_coluna = 0;

    if (ponteiro_planilha != NULL) {
        if (ponteiro_planilha->matriz_dependencias != NULL) {
            if (indice_celula >= 0) {
                if (indice_celula < TOTAL_CELULAS) {
                    indices_validos = 1;
                }
            }
        }
    }

    if (indices_validos == 1) {
        while (indice_coluna < TOTAL_CELULAS) {
            ponteiro_planilha->matriz_dependencias[indice_celula][indice_coluna] = 0;
            indice_coluna = indice_coluna + 1;
        }
    }
}

/* -------------------------------------------------- */
/* Analisar expressão digitada na célula              */
/* -------------------------------------------------- */

int AnalisarExpressao(Planilha* ponteiro_planilha, int indice_celula, const char* texto_expressao) {
    int resultado;
    Celula* ponteiro_celula;
    char primeiro_caractere;
    int inicio_numerico;
    double valor_lido;
    int leitura_ok;
    int indice_referencia;
    int indice_loop;
    int posicao_parentese;
    char texto_funcao[16];
    char texto_intervalo[32];
    int tamanho_intervalo;
    char* ponteiro_separador;
    int indice_inicio_intervalo;
    int indice_fim_intervalo;
    int linha_inicio;
    int linha_fim;
    int coluna_inicio;
    int coluna_fim;
    int linha_menor;
    int linha_maior;
    int coluna_menor;
    int coluna_maior;
    int linha_atual;
    int coluna_atual;
    int indice_atual;

    resultado = 0;
    ponteiro_celula = &(ponteiro_planilha->vetor_celulas[indice_celula]);

    RemoverDependencias(ponteiro_planilha, indice_celula);

    ponteiro_celula->tipo_conteudo = TIPO_CONTEUDO_VAZIA;
    ponteiro_celula->valor_numerico = VALOR_PADRAO;
    ponteiro_celula->texto_expressao[0] = '\0';
    ponteiro_celula->indice_referencia = -1;
    ponteiro_celula->tipo_funcao = TIPO_FUNCAO_NENHUMA;
    ponteiro_celula->indice_inicio_intervalo = 0;
    ponteiro_celula->indice_fim_intervalo = 0;

    if (texto_expressao != NULL) {
        if (strlen(texto_expressao) > 0) {
            strncpy(ponteiro_celula->texto_expressao, texto_expressao, TAMANHO_MAX_EXPRESSAO - 1);
            ponteiro_celula->texto_expressao[TAMANHO_MAX_EXPRESSAO - 1] = '\0';

            primeiro_caractere = texto_expressao[0];
            inicio_numerico = 0;

            if (primeiro_caractere >= '0') {
                if (primeiro_caractere <= '9') {
                    inicio_numerico = 1;
                }
            }

            if (primeiro_caractere == '-') {
                if (texto_expressao[1] != '\0') {
                    if (texto_expressao[1] >= '0') {
                        if (texto_expressao[1] <= '9') {
                            inicio_numerico = 1;
                        }
                    }
                }
            }

            if (inicio_numerico == 1) {
                leitura_ok = sscanf(texto_expressao, "%lf", &valor_lido);
                if (leitura_ok == 1) {
                    ponteiro_celula->tipo_conteudo = TIPO_CONTEUDO_NUMERO;
                    ponteiro_celula->valor_numerico = valor_lido;
                    resultado = 1;
                }
            } else {
                if (primeiro_caractere == '=') {
                    indice_referencia = ConverterCoordenadaParaIndice(texto_expressao + 1);
                    if (indice_referencia != -1) {
                        ponteiro_celula->tipo_conteudo = TIPO_CONTEUDO_REFERENCIA;
                        ponteiro_celula->indice_referencia = indice_referencia;
                        AdicionarDependencia(ponteiro_planilha, indice_celula, indice_referencia);
                        resultado = 1;
                    }
                } else {
                    if (primeiro_caractere == '@') {
                        indice_loop = 1;
                        posicao_parentese = -1;
                        while (texto_expressao[indice_loop] != '\0') {
                            if (texto_expressao[indice_loop] == '(') {
                                posicao_parentese = indice_loop;
                                indice_loop = (int)strlen(texto_expressao);
                            } else {
                                if ((indice_loop - 1) < 15) {
                                    texto_funcao[indice_loop - 1] = texto_expressao[indice_loop];
                                }
                            }
                            indice_loop = indice_loop + 1;
                        }

                        if (posicao_parentese != -1) {
                            texto_funcao[posicao_parentese - 1] = '\0';

                            indice_loop = 0;
                            while (texto_funcao[indice_loop] != '\0') {
                                if (texto_funcao[indice_loop] >= 'A') {
                                    if (texto_funcao[indice_loop] <= 'Z') {
                                        texto_funcao[indice_loop] = (char)(texto_funcao[indice_loop] - 'A' + 'a');
                                    }
                                }
                                indice_loop = indice_loop + 1;
                            }

                            if (texto_expressao[posicao_parentese] == '(') {
                                if (texto_expressao[strlen(texto_expressao) - 1] == ')') {
                                    tamanho_intervalo = (int)strlen(texto_expressao) - posicao_parentese - 2;
                                    if (tamanho_intervalo > 0) {
                                        if (tamanho_intervalo < 32) {
                                            strncpy(texto_intervalo, texto_expressao + posicao_parentese + 1, tamanho_intervalo);
                                            texto_intervalo[tamanho_intervalo] = '\0';

                                            if (strcmp(texto_funcao, "soma") == 0) {
                                                ponteiro_celula->tipo_funcao = TIPO_FUNCAO_SOMA;
                                            } else if (strcmp(texto_funcao, "max") == 0) {
                                                ponteiro_celula->tipo_funcao = TIPO_FUNCAO_MAX;
                                            } else if (strcmp(texto_funcao, "min") == 0) {
                                                ponteiro_celula->tipo_funcao = TIPO_FUNCAO_MIN;
                                            } else if (strcmp(texto_funcao, "media") == 0) {
                                                ponteiro_celula->tipo_funcao = TIPO_FUNCAO_MEDIA;
                                            } else {
                                                ponteiro_celula->tipo_funcao = TIPO_FUNCAO_NENHUMA;
                                            }

                                            if (ponteiro_celula->tipo_funcao != TIPO_FUNCAO_NENHUMA) {
                                                ponteiro_separador = strstr(texto_intervalo, "..");
                                                if (ponteiro_separador != NULL) {
                                                    *ponteiro_separador = '\0';
                                                    indice_inicio_intervalo = ConverterCoordenadaParaIndice(texto_intervalo);
                                                    indice_fim_intervalo = ConverterCoordenadaParaIndice(ponteiro_separador + 2);

                                                    if (indice_inicio_intervalo != -1) {
                                                        if (indice_fim_intervalo != -1) {
                                                            ponteiro_celula->tipo_conteudo = TIPO_CONTEUDO_FUNCAO;
                                                            ponteiro_celula->indice_inicio_intervalo = indice_inicio_intervalo;
                                                            ponteiro_celula->indice_fim_intervalo = indice_fim_intervalo;

                                                            linha_inicio = indice_inicio_intervalo / TOTAL_COLUNAS;
                                                            coluna_inicio = indice_inicio_intervalo % TOTAL_COLUNAS;
                                                            linha_fim = indice_fim_intervalo / TOTAL_COLUNAS;
                                                            coluna_fim = indice_fim_intervalo % TOTAL_COLUNAS;

                                                            if (linha_inicio <= linha_fim) {
                                                                linha_menor = linha_inicio;
                                                                linha_maior = linha_fim;
                                                            } else {
                                                                linha_menor = linha_fim;
                                                                linha_maior = linha_inicio;
                                                            }

                                                            if (coluna_inicio <= coluna_fim) {
                                                                coluna_menor = coluna_inicio;
                                                                coluna_maior = coluna_fim;
                                                            } else {
                                                                coluna_menor = coluna_fim;
                                                                coluna_maior = coluna_inicio;
                                                            }

                                                            linha_atual = linha_menor;
                                                            while (linha_atual <= linha_maior) {
                                                                coluna_atual = coluna_menor;
                                                                while (coluna_atual <= coluna_maior) {
                                                                    indice_atual = linha_atual * TOTAL_COLUNAS + coluna_atual;
                                                                    if (indice_atual != indice_celula) {
                                                                        AdicionarDependencia(ponteiro_planilha, indice_celula, indice_atual);
                                                                    }
                                                                    coluna_atual = coluna_atual + 1;
                                                                }
                                                                linha_atual = linha_atual + 1;
                                                            }

                                                            resultado = 1;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (resultado == 0) {
        ponteiro_celula->tipo_conteudo = TIPO_CONTEUDO_VAZIA;
        ponteiro_celula->valor_numerico = VALOR_PADRAO;
        ponteiro_celula->texto_expressao[0] = '\0';
        ponteiro_celula->tipo_funcao = TIPO_FUNCAO_NENHUMA;
        ponteiro_celula->indice_inicio_intervalo = 0;
        ponteiro_celula->indice_fim_intervalo = 0;
    }

    return resultado;
}

/* -------------------------------------------------- */
/* Avaliar célula (recursivo)                         */
/* -------------------------------------------------- */

double AvaliarCelula(Planilha* ponteiro_planilha, int indice_celula) {
    double valor_resultado;
    Celula* ponteiro_celula;

    valor_resultado = 0.0;
    ponteiro_celula = &(ponteiro_planilha->vetor_celulas[indice_celula]);

    if (ponteiro_celula->tipo_conteudo == TIPO_CONTEUDO_NUMERO) {
        valor_resultado = ponteiro_celula->valor_numerico;
    } else {
        if (ponteiro_celula->tipo_conteudo == TIPO_CONTEUDO_REFERENCIA) {
            valor_resultado = AvaliarCelula(ponteiro_planilha, ponteiro_celula->indice_referencia);
        } else {
            if (ponteiro_celula->tipo_conteudo == TIPO_CONTEUDO_FUNCAO) {
                valor_resultado = CalcularFuncao(ponteiro_planilha, indice_celula);
            } else {
                valor_resultado = 0.0;
            }
        }
    }

    return valor_resultado;
}

/* -------------------------------------------------- */
/* Calcular função (@soma, @min, @max, @media)       */
/* -------------------------------------------------- */

double CalcularFuncao(Planilha* ponteiro_planilha, int indice_celula_funcao) {
    double valor_resultado;
    Celula* celula_funcao;
    int indice_inicio;
    int indice_fim;
    int linha_inicio;
    int linha_fim;
    int coluna_inicio;
    int coluna_fim;
    int linha_menor;
    int linha_maior;
    int coluna_menor;
    int coluna_maior;
    int linha_atual;
    int coluna_atual;
    int indice_atual;
    Celula* celula_atual;
    double valor_atual;
    double soma_valores;
    int quantidade_valores;
    int encontrou_primeiro;

    valor_resultado = 0.0;
    celula_funcao = &(ponteiro_planilha->vetor_celulas[indice_celula_funcao]);

    indice_inicio = celula_funcao->indice_inicio_intervalo;
    indice_fim = celula_funcao->indice_fim_intervalo;

    linha_inicio = indice_inicio / TOTAL_COLUNAS;
    coluna_inicio = indice_inicio % TOTAL_COLUNAS;
    linha_fim = indice_fim / TOTAL_COLUNAS;
    coluna_fim = indice_fim % TOTAL_COLUNAS;

    if (linha_inicio <= linha_fim) {
        linha_menor = linha_inicio;
        linha_maior = linha_fim;
    } else {
        linha_menor = linha_fim;
        linha_maior = linha_inicio;
    }

    if (coluna_inicio <= coluna_fim) {
        coluna_menor = coluna_inicio;
        coluna_maior = coluna_fim;
    } else {
        coluna_menor = coluna_fim;
        coluna_maior = coluna_inicio;
    }

    soma_valores = 0.0;
    quantidade_valores = 0;
    encontrou_primeiro = 0;

    linha_atual = linha_menor;
    while (linha_atual <= linha_maior) {
        coluna_atual = coluna_menor;
        while (coluna_atual <= coluna_maior) {
            indice_atual = linha_atual * TOTAL_COLUNAS + coluna_atual;

            if (indice_atual != indice_celula_funcao) {
                celula_atual = &(ponteiro_planilha->vetor_celulas[indice_atual]);

                /* IGNORA células vazias em min/max/media e ignora funções para evitar ciclos entre funções */
                if (celula_atual->tipo_conteudo == TIPO_CONTEUDO_VAZIA) {
                    /* não faz nada */
                } else {
                    if (celula_atual->tipo_conteudo == TIPO_CONTEUDO_FUNCAO) {
                        /* para simplificar, não usamos o valor de outras funções dentro de funções grandes */
                    } else {
                        valor_atual = AvaliarCelula(ponteiro_planilha, indice_atual);

                        if (celula_funcao->tipo_funcao == TIPO_FUNCAO_SOMA) {
                            soma_valores = soma_valores + valor_atual;
                            quantidade_valores = quantidade_valores + 1;
                        } else {
                            if (celula_funcao->tipo_funcao == TIPO_FUNCAO_MEDIA) {
                                soma_valores = soma_valores + valor_atual;
                                quantidade_valores = quantidade_valores + 1;
                            } else {
                                if (celula_funcao->tipo_funcao == TIPO_FUNCAO_MAX) {
                                    if (encontrou_primeiro == 0) {
                                        valor_resultado = valor_atual;
                                        encontrou_primeiro = 1;
                                    } else {
                                        if (valor_atual > valor_resultado) {
                                            valor_resultado = valor_atual;
                                        }
                                    }
                                } else {
                                    if (celula_funcao->tipo_funcao == TIPO_FUNCAO_MIN) {
                                        if (encontrou_primeiro == 0) {
                                            valor_resultado = valor_atual;
                                            encontrou_primeiro = 1;
                                        } else {
                                            if (valor_atual < valor_resultado) {
                                                valor_resultado = valor_atual;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            coluna_atual = coluna_atual + 1;
        }
        linha_atual = linha_atual + 1;
    }

    if (celula_funcao->tipo_funcao == TIPO_FUNCAO_SOMA) {
        valor_resultado = soma_valores;
        if (quantidade_valores == 0) {
            valor_resultado = 0.0;
        }
    } else {
        if (celula_funcao->tipo_funcao == TIPO_FUNCAO_MEDIA) {
            if (quantidade_valores > 0) {
                valor_resultado = soma_valores / (double)quantidade_valores;
            } else {
                valor_resultado = 0.0;
            }
        } else {
            if (celula_funcao->tipo_funcao == TIPO_FUNCAO_MAX) {
                if (encontrou_primeiro == 0) {
                    valor_resultado = 0.0;
                }
            } else {
                if (celula_funcao->tipo_funcao == TIPO_FUNCAO_MIN) {
                    if (encontrou_primeiro == 0) {
                        valor_resultado = 0.0;
                    }
                }
            }
        }
    }

    return valor_resultado;
}

/* -------------------------------------------------- */
/* Exibir planilha                                    */
/* -------------------------------------------------- */

void ExibirPlanilha(Planilha* ponteiro_planilha) {
    int indice_linha;
    int indice_coluna;
    int indice_celula;
    int indice_separador;
    double valor_celula;

    printf("\n+");
    indice_separador = 0;
    while (indice_separador < TOTAL_COLUNAS) {
        printf("----------+");
        indice_separador = indice_separador + 1;
    }

    printf("\n|   \\ C  | A        | B        | C        | D        | E        | F        | G        | H        |");

    printf("\n+");
    indice_separador = 0;
    while (indice_separador < TOTAL_COLUNAS) {
        printf("----------+");
        indice_separador = indice_separador + 1;
    }

    indice_linha = 0;
    while (indice_linha < TOTAL_LINHAS) {
        printf("\n| R %02d   |", indice_linha + 1);

        indice_coluna = 0;
        while (indice_coluna < TOTAL_COLUNAS) {
            indice_celula = indice_linha * TOTAL_COLUNAS + indice_coluna;
            valor_celula = AvaliarCelula(ponteiro_planilha, indice_celula);
            printf(" %8.2lf |", valor_celula);
            indice_coluna = indice_coluna + 1;
        }

        printf("\n+");
        indice_separador = 0;
        while (indice_separador < TOTAL_COLUNAS) {
            printf("----------+");
            indice_separador = indice_separador + 1;
        }

        indice_linha = indice_linha + 1;
    }

    printf("\n");
}

/* -------------------------------------------------- */
/* Inicializar e liberar planilha                     */
/* -------------------------------------------------- */

Planilha* InicializarPlanilha() {
    Planilha* ponteiro_planilha;
    int memoria_ok;
    int indice;
    int indice_linha;
    int indice_coluna;

    ponteiro_planilha = NULL;
    memoria_ok = 1;

    ponteiro_planilha = (Planilha*)malloc(sizeof(Planilha));
    if (ponteiro_planilha == NULL) {
        memoria_ok = 0;
    }

    if (memoria_ok == 1) {
        ponteiro_planilha->vetor_celulas = (Celula*)malloc(TOTAL_CELULAS * sizeof(Celula));
        ponteiro_planilha->matriz_dependencias = (int**)malloc(TOTAL_CELULAS * sizeof(int*));

        if (ponteiro_planilha->vetor_celulas == NULL) {
            memoria_ok = 0;
        }

        if (ponteiro_planilha->matriz_dependencias == NULL) {
            memoria_ok = 0;
        }

        if (ponteiro_planilha->matriz_dependencias != NULL) {
            indice = 0;
            while (indice < TOTAL_CELULAS) {
                ponteiro_planilha->matriz_dependencias[indice] = NULL;
                indice = indice + 1;
            }
        }
    }

    if (memoria_ok == 1) {
        indice = 0;
        while (indice < TOTAL_CELULAS) {
            ponteiro_planilha->vetor_celulas[indice].tipo_conteudo = TIPO_CONTEUDO_VAZIA;
            ponteiro_planilha->vetor_celulas[indice].valor_numerico = VALOR_PADRAO;
            ponteiro_planilha->vetor_celulas[indice].texto_expressao[0] = '\0';
            ponteiro_planilha->vetor_celulas[indice].indice_referencia = -1;
            ponteiro_planilha->vetor_celulas[indice].tipo_funcao = TIPO_FUNCAO_NENHUMA;
            ponteiro_planilha->vetor_celulas[indice].indice_inicio_intervalo = 0;
            ponteiro_planilha->vetor_celulas[indice].indice_fim_intervalo = 0;
            indice = indice + 1;
        }

        indice_linha = 0;
        while (indice_linha < TOTAL_CELULAS) {
            if (ponteiro_planilha->matriz_dependencias[indice_linha] == NULL) {
                ponteiro_planilha->matriz_dependencias[indice_linha] = (int*)malloc(TOTAL_CELULAS * sizeof(int));
                if (ponteiro_planilha->matriz_dependencias[indice_linha] == NULL) {
                    memoria_ok = 0;
                } else {
                    indice_coluna = 0;
                    while (indice_coluna < TOTAL_CELULAS) {
                        ponteiro_planilha->matriz_dependencias[indice_linha][indice_coluna] = 0;
                        indice_coluna = indice_coluna + 1;
                    }
                }
            }
            indice_linha = indice_linha + 1;
        }
    }

    if (memoria_ok == 0) {
        printf("Erro ao alocar memoria para a planilha.\n");
        LiberarPlanilha(ponteiro_planilha);
        ponteiro_planilha = NULL;
    }

    return ponteiro_planilha;
}

void LiberarPlanilha(Planilha* ponteiro_planilha) {
    int indice_linha;

    if (ponteiro_planilha != NULL) {
        if (ponteiro_planilha->matriz_dependencias != NULL) {
            indice_linha = 0;
            while (indice_linha < TOTAL_CELULAS) {
                if (ponteiro_planilha->matriz_dependencias[indice_linha] != NULL) {
                    free(ponteiro_planilha->matriz_dependencias[indice_linha]);
                    ponteiro_planilha->matriz_dependencias[indice_linha] = NULL;
                }
                indice_linha = indice_linha + 1;
            }
            free(ponteiro_planilha->matriz_dependencias);
            ponteiro_planilha->matriz_dependencias = NULL;
        }

        if (ponteiro_planilha->vetor_celulas != NULL) {
            free(ponteiro_planilha->vetor_celulas);
            ponteiro_planilha->vetor_celulas = NULL;
        }

        free(ponteiro_planilha);
    }
}

/* -------------------------------------------------- */
/* Loop principal                                     */
/* -------------------------------------------------- */

void LoopPrincipal(Planilha* ponteiro_planilha) {
    char linha_entrada[TAMANHO_MAX_EXPRESSAO + 16];
    char texto_coordenada[16];
    char texto_expressao[TAMANHO_MAX_EXPRESSAO];
    int indice_celula;
    int sucesso_analisar;
    int continuar;
    int leitura_valida;

    continuar = 1;

    while (continuar == 1) {
        ExibirPlanilha(ponteiro_planilha);

        printf("\nComando (ex: A1 5 ou A2 =A1 ou A3 @soma(A1..A2)):\n");
        printf("Digite 'SAIR' para encerrar.\n> ");

        if (fgets(linha_entrada, sizeof(linha_entrada), stdin) == NULL) {
            continuar = 0;
        } else {
            linha_entrada[strcspn(linha_entrada, "\n")] = '\0';

            if (strcmp(linha_entrada, "SAIR") == 0) {
                continuar = 0;
            } else {
                if (strcmp(linha_entrada, "sair") == 0) {
                    continuar = 0;
                } else {
                    texto_coordenada[0] = '\0';
                    texto_expressao[0] = '\0';

                    leitura_valida = sscanf(linha_entrada, "%s %s", texto_coordenada, texto_expressao);

                    if (leitura_valida == 2) {
                        indice_celula = ConverterCoordenadaParaIndice(texto_coordenada);
                        if (indice_celula != -1) {
                            sucesso_analisar = AnalisarExpressao(ponteiro_planilha, indice_celula, texto_expressao);
                            if (sucesso_analisar == 0) {
                                printf("\nERRO: Expressao ou coordenada invalida.\n");
                            }
                        } else {
                            printf("\nERRO: Coordenada de celula invalida ('%s').\n", texto_coordenada);
                        }
                    } else {
                        printf("\nERRO: Formato incorreto. Use: CELULA VALOR/EXPRESSAO.\n");
                    }
                }
            }
        }
    }
}

/* -------------------------------------------------- */
/* main                                               */
/* -------------------------------------------------- */

int main() {
    Planilha* ponteiro_planilha_principal;
    int codigo_saida;

    ponteiro_planilha_principal = InicializarPlanilha();
    codigo_saida = 0;

    if (ponteiro_planilha_principal != NULL) {
        LoopPrincipal(ponteiro_planilha_principal);
        LiberarPlanilha(ponteiro_planilha_principal);
        codigo_saida = 0;
    } else {
        codigo_saida = 1;
    }

    return codigo_saida;
}
