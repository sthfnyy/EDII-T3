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

typedef struct {
    const char* nome;
    TipoFuncao tipo;
} MapaFuncao;

int TextoNaoVazio(const char* texto) ;
int IndiceCelulaValido(int indice);
void NormalizarTextoMinusculo(char* texto) ;
TipoFuncao ConverterTextoParaTipoFuncao(const char* texto_funcao) ;
void ObterLimitesIntervalo(int indice_inicio, int indice_fim, int* linha_menor, int* linha_maior, int* coluna_menor, int* coluna_maior);
char* ConverterIndiceParaCoordenada(int indice_celula, char* texto_coordenada) ;
int ConverterCoordenadaParaIndice(const char* texto_coordenada) ;
int DependenciasIndicesValidos(Planilha* plan, int dep, int ref) ;
void AdicionarDependencia(Planilha* plan, int dep, int ref) ;
void RemoverDependencias(Planilha* plan, int indice_cel) ;
void LimparEstadoCelula(Celula* cel) ;
void CopiarExpressaoParaCelula(Celula* cel, const char* txt_expr) ;
int ProcessarExpressaoNumerica(Celula* cel, const char* s) ;
int ProcessarExpressaoReferencia(Planilha* plan, Celula* cel, int indice_cel, const char* txt_expr) ;
int ObterNomeFuncaoEIntervalo(const char* txt_expr, char* texto_funcao, char* texto_intervalo) ;
int ProcessarExpressaoFuncao(Planilha* plan, Celula* cel, int indice_cel, const char* txt_expr) ;
int AnalisarExpressao(Planilha* plan, int indice_cel, const char* txt_expr) ;
double AvaliarCelula(Planilha* plan, int indice_cel) ;
double CalcularFuncao(Planilha* plan, int indice_cel_funcao) ;
void ExibirPlanilha(Planilha* plan) ;
Planilha* InicializarPlanilha() ;
void LiberarPlanilha(Planilha* plan) ;
void LoopPrincipal(Planilha* plan) ;
