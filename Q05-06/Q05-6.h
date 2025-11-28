#ifndef Q04_H
#define Q04_H

#include <stdio.h>

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

/* Nó da lista de dependências: quem EU dependo */
typedef struct NoDependencia {
    int indice_celula_dependida;
    struct NoDependencia* prox;
} NoDependencia;

typedef struct {
    Celula* vetor_celulas;
    NoDependencia** listas_dependencias;  /* lista de dependências por célula */
} Planilha;

typedef struct {
    const char* nome;
    TipoFuncao tipo;
} MapaFuncao;

/* ---------- Protótipos da planilha (Q03/Q04) ---------- */

int  TextoNaoVazio(const char* texto);
int  IndiceCelulaValido(int indice);
void NormalizarTextoMinusculo(char* texto);
TipoFuncao ConverterTextoParaTipoFuncao(const char* texto_funcao);
void ObterLimitesIntervalo(int indice_inicio, int indice_fim,
                           int* linha_menor, int* linha_maior,
                           int* coluna_menor, int* coluna_maior);
char* ConverterIndiceParaCoordenada(int indice_celula, char* texto_coordenada);
int   ConverterCoordenadaParaIndice(const char* texto_coordenada);

int  DependenciasIndicesValidos(Planilha* plan, int dep, int ref);
int  DependenciaJaExiste(NoDependencia* lista, int ref);
void AdicionarDependencia(Planilha* plan, int dep, int ref);
void RemoverDependencias(Planilha* plan, int indice_cel);

void LimparEstadoCelula(Celula* cel);
void CopiarExpressaoParaCelula(Celula* cel, const char* txt_expr);
int  ProcessarExpressaoNumerica(Celula* cel, const char* txt_expr);
int  ProcessarExpressaoReferencia(Planilha* plan, Celula* cel,
                                  int indice_cel, const char* txt_expr);
int  ObterNomeFuncaoEIntervalo(const char* txt_expr,
                               char* texto_funcao,
                               char* texto_intervalo);
int  ProcessarExpressaoFuncao(Planilha* plan, Celula* cel,
                              int indice_cel, const char* txt_expr);

int    AnalisarExpressao(Planilha* plan, int indice_cel, const char* txt_expr);
double AvaliarCelula(Planilha* plan, int indice_cel);
double CalcularFuncao(Planilha* plan, int indice_cel_funcao);

void     ExibirPlanilha(Planilha* plan);
Planilha* InicializarPlanilha();
void     LiberarPlanilha(Planilha* plan);

/* ---------- Protótipos da Q05 / Q06 ---------- */

/* Q05 – buscas em largura e profundidade (com impressão) */
void BFS(Planilha* plan, int indice_inicio);
void DFS(Planilha* plan, int indice_inicio);

/* Versões sem impressão (usadas pra medir tempo na Q06) */
void BFS_sem_print(Planilha* plan, int indice_inicio);
void DFS_sem_print(Planilha* plan, int indice_inicio);

/* Q06 – medir tempo médio de:
   (a) inserção (reconstrução das dependências)
   (b) BFS
   (c) DFS
*/
void medir_tempos_q6(Planilha* plan, int indice_inicio);

/* Loop de interação com o usuário */
void LoopPrincipal(Planilha* plan);

#endif
