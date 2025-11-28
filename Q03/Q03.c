#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Q03.h"


int TextoNaoVazio(const char* texto) 
{
    int valido = 0;
    if (texto != NULL && texto[0] != '\0') {
        valido = 1;
    }
    return valido;
}

int IndiceCelulaValido(int indice)
{
    int valido = 0;
    if (indice >= 0 && indice < TOTAL_CELULAS) {
        valido = 1;
    }
    return valido;
}

void NormalizarTextoMinusculo(char* texto) 
{
    int i = 0;
    if (texto != NULL) {
        while (texto[i] != '\0') {
            if (texto[i] >= 'A' && texto[i] <= 'Z') {
                texto[i] = (char)(texto[i] - 'A' + 'a');
            }
            i++;
        }
    }
}

TipoFuncao ConverterTextoParaTipoFuncao(const char* texto_funcao) 
{
    MapaFuncao mapa[] = {
        {"soma",  TIPO_FUNCAO_SOMA},
        {"max",   TIPO_FUNCAO_MAX},
        {"min",   TIPO_FUNCAO_MIN},
        {"media", TIPO_FUNCAO_MEDIA}
    };
    TipoFuncao tipo_resultado = TIPO_FUNCAO_NENHUMA;
    int qtd = (int)(sizeof(mapa) / sizeof(mapa[0]));
    int i = 0;
    int texto_valido = TextoNaoVazio(texto_funcao);
    int encontrou = 0;

    if (texto_valido == 1) {
        while (i < qtd && encontrou == 0) {
            if (strcmp(texto_funcao, mapa[i].nome) == 0) {
                tipo_resultado = mapa[i].tipo;
                encontrou = 1;
            }
            i++;
        }
    }
    return tipo_resultado;
}

void ObterLimitesIntervalo(int indice_inicio, int indice_fim, int* linha_menor, int* linha_maior, int* coluna_menor, int* coluna_maior)
{
    int linha_inicio = indice_inicio / TOTAL_COLUNAS;
    int coluna_inicio = indice_inicio % TOTAL_COLUNAS;
    int linha_fim = indice_fim / TOTAL_COLUNAS;
    int coluna_fim = indice_fim % TOTAL_COLUNAS;

    if (linha_inicio <= linha_fim) {
        *linha_menor = linha_inicio;
        *linha_maior = linha_fim;
    } else {
        *linha_menor = linha_fim;
        *linha_maior = linha_inicio;
    }

    if (coluna_inicio <= coluna_fim) {
        *coluna_menor = coluna_inicio;
        *coluna_maior = coluna_fim;
    } else {
        *coluna_menor = coluna_fim;
        *coluna_maior = coluna_inicio;
    }
}

/* ------------------ Conversão índice <-> coordenada ------------------ */

char* ConverterIndiceParaCoordenada(int indice_celula, char* texto_coordenada) 
{
    char* resultado = NULL;
    int linha = 0;
    int coluna = 0;

    if (IndiceCelulaValido(indice_celula) == 1) {
        coluna = indice_celula % TOTAL_COLUNAS;
        linha = indice_celula / TOTAL_COLUNAS;
        sprintf(texto_coordenada, "%c%d", (char)('A' + coluna), linha + 1);
        resultado = texto_coordenada;
    }
    return resultado;
}

int ConverterCoordenadaParaIndice(const char* texto_coordenada) 
{
    int indice_resultado = -1;
    int tamanho = 0;
    char ccol = '\0';
    int col = -1;
    int lin = -1;
    int texto_valido = TextoNaoVazio(texto_coordenada);

    if (texto_valido == 1) {
        tamanho = (int)strlen(texto_coordenada);
        if (tamanho >= 2) {
            ccol = texto_coordenada[0];
            if (ccol >= 'a' && ccol <= 'z') {
                ccol = (char)(ccol - 'a' + 'A');
            }
            col = ccol - 'A';
            lin = atoi(texto_coordenada + 1) - 1;
            if (col >= 0 && col < TOTAL_COLUNAS && lin >= 0 && lin < TOTAL_LINHAS) {
                indice_resultado = lin * TOTAL_COLUNAS + col;
            }
        }
    }
    return indice_resultado;
}

/* ---------------------- Dependências (grafo) ---------------------- */

int DependenciasIndicesValidos(Planilha* plan, int dep, int ref) 
{
    int valido = 0;
    if (plan != NULL &&
        plan->matriz_dependencias != NULL &&
        IndiceCelulaValido(dep) == 1 &&
        IndiceCelulaValido(ref) == 1) {
        valido = 1;
    }
    return valido;
}

void AdicionarDependencia(Planilha* plan, int dep, int ref) 
{
    int ok = DependenciasIndicesValidos(plan, dep, ref);
    if (ok == 1) {
        plan->matriz_dependencias[dep][ref] = 1;
    }
}

void RemoverDependencias(Planilha* plan, int indice_cel) 
{
    int ok = 0;
    int c = 0;

    if (plan != NULL &&
        plan->matriz_dependencias != NULL &&
        IndiceCelulaValido(indice_cel) == 1) {
        ok = 1;
    }

    if (ok == 1) {
        c = 0;
        while (c < TOTAL_CELULAS) {
            plan->matriz_dependencias[indice_cel][c] = 0;
            c++;
        }
    }
}

/* ---------------------- Helpers de expressão ---------------------- */

void LimparEstadoCelula(Celula* cel) 
{
    if (cel != NULL) {
        cel->tipo_conteudo = TIPO_CONTEUDO_VAZIA;
        cel->valor_numerico = VALOR_PADRAO;
        cel->texto_expressao[0] = '\0';
        cel->indice_referencia = -1;
        cel->tipo_funcao = TIPO_FUNCAO_NENHUMA;
        cel->indice_inicio_intervalo = 0;
        cel->indice_fim_intervalo = 0;
    }
}

void CopiarExpressaoParaCelula(Celula* cel, const char* txt_expr) 
{
    if (cel != NULL && TextoNaoVazio(txt_expr) == 1) {
        strncpy(cel->texto_expressao, txt_expr, TAMANHO_MAX_EXPRESSAO - 1);
        cel->texto_expressao[TAMANHO_MAX_EXPRESSAO - 1] = '\0';
    }
}

int ProcessarExpressaoNumerica(Celula* cel, const char* s) 
{
    int resultado = 0;  // assume falha

    if (cel != NULL && TextoNaoVazio(s)) {
        double v;
        if (sscanf(s, " %lf", &v) == 1) {
            cel->tipo_conteudo = TIPO_CONTEUDO_NUMERO;
            cel->valor_numerico = v;
            resultado = 1;  // sucesso
        }
    }

    return resultado;  
}


int ProcessarExpressaoReferencia(Planilha* plan, Celula* cel, int indice_cel, const char* txt_expr) 
{
    int resultado = 0;
    if (plan != NULL && cel != NULL &&
        TextoNaoVazio(txt_expr) == 1 && txt_expr[0] == '=') {

        int ref = ConverterCoordenadaParaIndice(txt_expr + 1);
        if (IndiceCelulaValido(ref) == 1) {
            cel->tipo_conteudo = TIPO_CONTEUDO_REFERENCIA;
            cel->indice_referencia = ref;
            AdicionarDependencia(plan, indice_cel, ref);
            resultado = 1;
        }
    }
    return resultado;
}

int ObterNomeFuncaoEIntervalo(const char* txt_expr, char* texto_funcao, char* texto_intervalo) 
{
    int resultado = 0;
    int valido = TextoNaoVazio(txt_expr);
    int i = 1;
    int pos_par = -1;
    int tam = 0;
    int tam_intervalo = 0;

    if (valido == 1 && txt_expr[0] == '@') {
        tam = (int)strlen(txt_expr);
        while (txt_expr[i] != '\0') {
            char c = txt_expr[i];
            if (c == '(') {
                pos_par = i;
                i = tam;
            } else if ((i - 1) < 15) {
                texto_funcao[i - 1] = c;
            }
            i++;
        }
        if (pos_par != -1 &&
            txt_expr[pos_par] == '(' &&
            txt_expr[tam - 1] == ')') {

            texto_funcao[pos_par - 1] = '\0';
            NormalizarTextoMinusculo(texto_funcao);

            tam_intervalo = tam - pos_par - 2;
            if (tam_intervalo > 0 && tam_intervalo < 32) {
                strncpy(texto_intervalo, txt_expr + pos_par + 1, tam_intervalo);
                texto_intervalo[tam_intervalo] = '\0';
                resultado = 1;
            }
        }
    }
    return resultado;
}

int ProcessarExpressaoFuncao(Planilha* plan, Celula* cel, int indice_cel, const char* txt_expr) 
{
    int resultado = 0;
    char texto_funcao[16];
    char texto_intervalo[32];
    int conseguiu = 0;
    TipoFuncao tipo = TIPO_FUNCAO_NENHUMA;
    int indice_inicio = 0;
    int indice_fim = 0;

    if (plan != NULL && cel != NULL &&
        TextoNaoVazio(txt_expr) == 1 && txt_expr[0] == '@') {

        conseguiu = ObterNomeFuncaoEIntervalo(txt_expr, texto_funcao, texto_intervalo);
        if (conseguiu == 1) {
            tipo = ConverterTextoParaTipoFuncao(texto_funcao);
            if (tipo != TIPO_FUNCAO_NENHUMA) {
                char intervalo_local[32];
                char* sep = NULL;
                int intervalo_ok = 0;

                strncpy(intervalo_local, texto_intervalo, sizeof(intervalo_local) - 1);
                intervalo_local[sizeof(intervalo_local) - 1] = '\0';

                sep = strstr(intervalo_local, "..");
                if (sep != NULL) {
                    *sep = '\0';
                    indice_inicio = ConverterCoordenadaParaIndice(intervalo_local);
                    indice_fim = ConverterCoordenadaParaIndice(sep + 2);
                    if (IndiceCelulaValido(indice_inicio) == 1 &&
                        IndiceCelulaValido(indice_fim) == 1) {
                        intervalo_ok = 1;
                    }
                }

                if (intervalo_ok == 1) {
                    int linha_menor = 0, linha_maior = 0;
                    int coluna_menor = 0, coluna_maior = 0;
                    int l, c, indice_atual;

                    cel->tipo_conteudo = TIPO_CONTEUDO_FUNCAO;
                    cel->tipo_funcao = tipo;
                    cel->indice_inicio_intervalo = indice_inicio;
                    cel->indice_fim_intervalo = indice_fim;

                    ObterLimitesIntervalo(indice_inicio, indice_fim,
                                          &linha_menor, &linha_maior,
                                          &coluna_menor, &coluna_maior);

                    for (l = linha_menor; l <= linha_maior; l++) {
                        for (c = coluna_menor; c <= coluna_maior; c++) {
                            indice_atual = l * TOTAL_COLUNAS + c;
                            if (indice_atual != indice_cel) {
                                AdicionarDependencia(plan, indice_cel, indice_atual);
                            }
                        }
                    }
                    resultado = 1;
                }
            }
        }
    }
    return resultado;
}

/* ---------------- Analisar expressão da célula ---------------- */

int AnalisarExpressao(Planilha* plan, int indice_cel, const char* txt_expr) 
{
    int resultado = 0;
    Celula* cel = NULL;

    if (plan != NULL && IndiceCelulaValido(indice_cel) == 1) {
        cel = &(plan->vetor_celulas[indice_cel]);
        RemoverDependencias(plan, indice_cel);
        LimparEstadoCelula(cel);

        if (TextoNaoVazio(txt_expr) == 1) {
            CopiarExpressaoParaCelula(cel, txt_expr);

            if (ProcessarExpressaoNumerica(cel, txt_expr) == 1) {
                resultado = 1;
            } else if (ProcessarExpressaoReferencia(plan, cel, indice_cel, txt_expr) == 1) {
                resultado = 1;
            } else if (ProcessarExpressaoFuncao(plan, cel, indice_cel, txt_expr) == 1) {
                resultado = 1;
            }

            if (resultado == 0) {
                LimparEstadoCelula(cel);
            }
        }
    }
    return resultado;
}

/* -------------------- Avaliar célula (recursivo) -------------------- */

double AvaliarCelula(Planilha* plan, int indice_cel) 
{
    double valor = 0.0;
    Celula* cel = NULL;

    if (plan != NULL && IndiceCelulaValido(indice_cel) == 1) {
        cel = &(plan->vetor_celulas[indice_cel]);
        if (cel->tipo_conteudo == TIPO_CONTEUDO_NUMERO) {
            valor = cel->valor_numerico;
        } else if (cel->tipo_conteudo == TIPO_CONTEUDO_REFERENCIA) {
            valor = AvaliarCelula(plan, cel->indice_referencia);
        } else if (cel->tipo_conteudo == TIPO_CONTEUDO_FUNCAO) {
            valor = 0.0; /* será calculado em CalcularFuncao quando for exibido */
        } else {
            valor = 0.0;
        }
    }
    return valor;
}

/* ---------------- Calcular função (@soma, @min, ...) ---------------- */

double CalcularFuncao(Planilha* plan, int indice_cel_funcao) 
{
    double valor_resultado = 0.0;
    if (plan != NULL && IndiceCelulaValido(indice_cel_funcao) == 1) {
        Celula* cel_f = &(plan->vetor_celulas[indice_cel_funcao]);
        int indice_inicio = cel_f->indice_inicio_intervalo;
        int indice_fim = cel_f->indice_fim_intervalo;
        int linha_menor = 0, linha_maior = 0;
        int coluna_menor = 0, coluna_maior = 0;
        double soma_valores = 0.0;
        int quantidade = 0;
        int encontrou = 0;
        int l, c;

        ObterLimitesIntervalo(indice_inicio, indice_fim,
                              &linha_menor, &linha_maior,
                              &coluna_menor, &coluna_maior);

        for (l = linha_menor; l <= linha_maior; l++) {
            for (c = coluna_menor; c <= coluna_maior; c++) {
                int indice_atual = l * TOTAL_COLUNAS + c;
                if (indice_atual != indice_cel_funcao) {
                    Celula* cel_atual = &(plan->vetor_celulas[indice_atual]);
                    if (cel_atual->tipo_conteudo != TIPO_CONTEUDO_VAZIA &&
                        cel_atual->tipo_conteudo != TIPO_CONTEUDO_FUNCAO) {

                        double v = AvaliarCelula(plan, indice_atual);
                        if (cel_f->tipo_funcao == TIPO_FUNCAO_SOMA ||
                            cel_f->tipo_funcao == TIPO_FUNCAO_MEDIA) {
                            soma_valores += v;
                            quantidade++;
                        } else if (cel_f->tipo_funcao == TIPO_FUNCAO_MAX) {
                            if (encontrou == 0 || v > valor_resultado) {
                                valor_resultado = v;
                                encontrou = 1;
                            }
                        } else if (cel_f->tipo_funcao == TIPO_FUNCAO_MIN) {
                            if (encontrou == 0 || v < valor_resultado) {
                                valor_resultado = v;
                                encontrou = 1;
                            }
                        }
                    }
                }
            }
        }

        switch (cel_f->tipo_funcao) {
            case TIPO_FUNCAO_SOMA:
                valor_resultado = soma_valores;
                if (quantidade == 0) {
                    valor_resultado = 0.0;
                }
                break;
            case TIPO_FUNCAO_MEDIA:
                if (quantidade > 0) {
                    valor_resultado = soma_valores / (double)quantidade;
                } else {
                    valor_resultado = 0.0;
                }
                break;
            case TIPO_FUNCAO_MAX:
            case TIPO_FUNCAO_MIN:
                if (encontrou == 0) {
                    valor_resultado = 0.0;
                }
                break;
            default:
                valor_resultado = 0.0;
                break;
        }
    }
    return valor_resultado;
}

/* ----------------------- Exibição da planilha ----------------------- */

void ExibirPlanilha(Planilha* plan) 
{
    int l, c;

    printf("\n");
    printf("\n|   \\ C  | A        | B        | C        | D        | E        | F        | G        | H        |");
    printf("\n");

    for (l = 0; l < TOTAL_LINHAS; l++) {
        printf("\n| %02d   |", l + 1);
        for (c = 0; c < TOTAL_COLUNAS; c++) {
            int indice = l * TOTAL_COLUNAS + c;
            double valor = 0.0;
            Celula* cel = &(plan->vetor_celulas[indice]);

            if (cel->tipo_conteudo == TIPO_CONTEUDO_FUNCAO) {
                valor = CalcularFuncao(plan, indice);
            } else {
                valor = AvaliarCelula(plan, indice);
            }

            printf(" %8.2lf |", valor);
        }
    }
    printf("\n");
}

/* ----------------- Inicializar / liberar planilha ----------------- */

Planilha* InicializarPlanilha() 
{
    Planilha* plan = (Planilha*)malloc(sizeof(Planilha));
    int memoria_ok = 1;
    int i, j;

    if (plan == NULL) {
        memoria_ok = 0;
    }

    if (memoria_ok == 1) {
        plan->vetor_celulas = (Celula*)malloc(TOTAL_CELULAS * sizeof(Celula));
        plan->matriz_dependencias = (int**)malloc(TOTAL_CELULAS * sizeof(int*));
        if (plan->vetor_celulas == NULL || plan->matriz_dependencias == NULL) {
            memoria_ok = 0;
        } else {
            for (i = 0; i < TOTAL_CELULAS; i++) {
                plan->matriz_dependencias[i] = NULL;
            }
        }
    }

    if (memoria_ok == 1) {
        for (i = 0; i < TOTAL_CELULAS; i++) {
            plan->vetor_celulas[i].tipo_conteudo = TIPO_CONTEUDO_VAZIA;
            plan->vetor_celulas[i].valor_numerico = VALOR_PADRAO;
            plan->vetor_celulas[i].texto_expressao[0] = '\0';
            plan->vetor_celulas[i].indice_referencia = -1;
            plan->vetor_celulas[i].tipo_funcao = TIPO_FUNCAO_NENHUMA;
            plan->vetor_celulas[i].indice_inicio_intervalo = 0;
            plan->vetor_celulas[i].indice_fim_intervalo = 0;
        }

        for (i = 0; i < TOTAL_CELULAS && memoria_ok == 1; i++) {
            plan->matriz_dependencias[i] = (int*)malloc(TOTAL_CELULAS * sizeof(int));
            if (plan->matriz_dependencias[i] == NULL) {
                memoria_ok = 0;
            } else {
                for (j = 0; j < TOTAL_CELULAS; j++) {
                    plan->matriz_dependencias[i][j] = 0;
                }
            }
        }
    }

    if (memoria_ok == 0) {
        printf("Erro ao alocar memoria para a planilha.\n");
        if (plan != NULL) {
            if (plan->matriz_dependencias != NULL) {
                for (i = 0; i < TOTAL_CELULAS; i++) {
                    if (plan->matriz_dependencias[i] != NULL) {
                        free(plan->matriz_dependencias[i]);
                    }
                }
                free(plan->matriz_dependencias);
            }
            if (plan->vetor_celulas != NULL) {
                free(plan->vetor_celulas);
            }
            free(plan);
            plan = NULL;
        }
    }
    return plan;
}

void LiberarPlanilha(Planilha* plan) 
{
    int i = 0;
    if (plan != NULL) {
        if (plan->matriz_dependencias != NULL) {
            for (i = 0; i < TOTAL_CELULAS; i++) {
                if (plan->matriz_dependencias[i] != NULL) {
                    free(plan->matriz_dependencias[i]);
                }
            }
            free(plan->matriz_dependencias);
        }
        if (plan->vetor_celulas != NULL) {
            free(plan->vetor_celulas);
        }
        free(plan);
    }
}

/* ------------------------- Loop principal ------------------------- */

void LoopPrincipal(Planilha* plan) 
{
    char linha_entrada[TAMANHO_MAX_EXPRESSAO + 16];
    char texto_coordenada[16];
    char texto_expressao[TAMANHO_MAX_EXPRESSAO];
    int indice_cel = -1;
    int sucesso = 0;
    int continuar = 1;
    int leitura_valida = 0;

    texto_coordenada[0] = '\0';
    texto_expressao[0] = '\0';

    while (continuar == 1) {
        ExibirPlanilha(plan);
        printf("\nComando (ex: A1 5 ou A2 =A1 ou A3 @soma(A1..A2)):\n");
        printf("Digite 'SAIR' para encerrar.\n> ");

        if (fgets(linha_entrada, sizeof(linha_entrada), stdin) == NULL) {
            continuar = 0;
        } else {
            linha_entrada[strcspn(linha_entrada, "\n")] = '\0';

            if (strcmp(linha_entrada, "SAIR") == 0 || strcmp(linha_entrada, "sair") == 0) {
                continuar = 0;
            } else {
                texto_coordenada[0] = '\0';
                texto_expressao[0] = '\0';
                leitura_valida = sscanf(linha_entrada, "%s %s", texto_coordenada, texto_expressao);

                if (leitura_valida == 2) {
                    indice_cel = ConverterCoordenadaParaIndice(texto_coordenada);
                    if (IndiceCelulaValido(indice_cel) == 1) {
                        sucesso = AnalisarExpressao(plan, indice_cel, texto_expressao);
                        if (sucesso == 0) {
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
