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
