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