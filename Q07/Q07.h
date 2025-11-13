#define TOTAL_ALUNOS 4000

typedef struct {
    char matricula[12]; /* 11 dígitos + '\0' */
    int ocupado;        /* 0 = vazio, 1 = ocupado */
} RegistroAluno;


void extrair_digitos(const char *matricula, int digitos[11]) ;
int hash_a(const char *matricula, int tamanho, int *passo_out) ;
int hash_b(const char *matricula, int tamanho, int *passo_out) ;
void inicializar_tabela(RegistroAluno *tabela, int tamanho) ;
void inserir_hash_a(RegistroAluno *tabela, int tamanho, const char *matricula, int *colisoes_totais);
void inserir_hash_b(RegistroAluno *tabela, int tamanho, const char *matricula, int *colisoes_totais) ;
void gerar_matriculas(char matriculas[TOTAL_ALUNOS][12]) ;
void experimentar_tamanho(int tamanho, char matriculas[TOTAL_ALUNOS][12]);

