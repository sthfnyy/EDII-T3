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



int converter_texto_para_posicao(const char *texto, int *coluna, int *linha);
int **alocar_matriz_adjacencia(int tamanho);
void liberar_matriz_adjacencia(int **matriz, int tamanho);
void limpar_dependencias(int **matriz, int indice);
double avaliar_celula(Celula *planilha, int **matriz_adj, int indice, int *em_calculo, int *calculado);

void recalcular_planilha(Celula *planilha, int **matriz_adj);
void imprimir_planilha(Celula *planilha);
