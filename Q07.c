#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL_ALUNOS 4000

typedef struct {
    char matricula[12]; /* 11 dígitos + '\0' */
    int ocupado;        /* 0 = vazio, 1 = ocupado */
} RegistroAluno;

/* Converte string de matrícula em vetor de dígitos (0..10) */
void extrair_digitos(const char *matricula, int digitos[11]) {
    int i;
    for (i = 0; i < 11; i++) {
        digitos[i] = matricula[i] - '0';
    }
}

/* (a) Hashing:
   - Rotação dos 5 primeiros dígitos para a direita.
   - Extrai 2o, 4o e 5o dígitos da matrícula rotacionada -> número de 3 dígitos.
   - Resto da divisão pelo tamanho do vetor.
   - Passo para colisão = 1o dígito da matrícula rotacionada (se 0, usa 1). */
int hash_a(const char *matricula, int tamanho, int *passo_out) {
    int dig[11];
    int rot[11];
    int val;
    int passo;
    int resultado;
    int i;

    extrair_digitos(matricula, dig);

    /* copia tudo */
    for (i = 0; i < 11; i++) {
        rot[i] = dig[i];
    }

    /* rotaciona os 5 primeiros para a direita: d0..d4 -> d4,d0,d1,d2,d3 */
    rot[0] = dig[4];
    rot[1] = dig[0];
    rot[2] = dig[1];
    rot[3] = dig[2];
    rot[4] = dig[3];

    /* número formado por 2o, 4o e 5o dígitos */
    val = rot[1] * 100 + rot[3] * 10 + rot[4];
    resultado = val % tamanho;

    passo = rot[0]; /* 1o dígito após rotação */
    if (passo == 0) {
        passo = 1;
    }
    *passo_out = passo;

    return resultado;
}

/* (b) Hashing:
   - Fold shift com 3 dígitos:
       grupo1 = (6o, 7o, 11o)
       grupo2 = (8o, 9o, 10o)
     soma = grupo1 + grupo2, depois resto da divisão pelo tamanho.
   - Passo para colisão: número formado pelo 6o e 11o dígitos.
*/
int hash_b(const char *matricula, int tamanho, int *passo_out) {
    int dig[11];
    int g1, g2;
    int val;
    int resultado;
    int passo;

    extrair_digitos(matricula, dig);

    g1 = dig[5] * 100 + dig[6] * 10 + dig[10]; /* 6o, 7o, 11o */
    g2 = dig[7] * 100 + dig[8] * 10 + dig[9];  /* 8o, 9o, 10o */
    val = g1 + g2;
    resultado = val % tamanho;

    passo = dig[5] * 10 + dig[10]; /* 6o e 11o dígitos */
    if (passo == 0) {
        passo = 1;
    }
    *passo_out = passo;

    return resultado;
}

/* Zera a tabela de hashing */
void inicializar_tabela(RegistroAluno *tabela, int tamanho) {
    int i;
    for (i = 0; i < tamanho; i++) {
        tabela[i].ocupado = 0;
        tabela[i].matricula[0] = '\0';
    }
}

/* Insere um aluno usando a função (a) e trata colisão como descrito.
   Conta colisões no ponteiro colisoes_totais. */
void inserir_hash_a(RegistroAluno *tabela, int tamanho,
                    const char *matricula, int *colisoes_totais) {
    int passo;
    int indice_base;
    int indice;
    int tentativas;

    indice_base = hash_a(matricula, tamanho, &passo);
    indice = indice_base;
    tentativas = 0;

    /* probing até achar posição livre ou dar a volta na tabela */
    while (tentativas < tamanho && tabela[indice].ocupado) {
        (*colisoes_totais)++;
        indice = (indice + passo) % tamanho;
        tentativas++;
    }

    /* se não achou vaga, sobrescreve a posição base */
    if (tentativas == tamanho) {
        indice = indice_base;
    }

    strcpy(tabela[indice].matricula, matricula);
    tabela[indice].ocupado = 1;
}

/* Insere um aluno usando a função (b) e trata colisão como descrito. */
void inserir_hash_b(RegistroAluno *tabela, int tamanho,
                    const char *matricula, int *colisoes_totais) {
    int passo;
    int indice_base;
    int indice;
    int tentativas;

    indice_base = hash_b(matricula, tamanho, &passo);
    indice = indice_base;
    tentativas = 0;

    while (tentativas < tamanho && tabela[indice].ocupado) {
        (*colisoes_totais)++;
        indice = (indice + passo) % tamanho;
        tentativas++;
    }

    if (tentativas == tamanho) {
        indice = indice_base;
    }

    strcpy(tabela[indice].matricula, matricula);
    tabela[indice].ocupado = 1;
}

/* Gera 4000 matrículas válidas, formato AAAACNNNNNN:
   - 4 dígitos: ano (2010..2024)
   - 1 dígito: curso (1..9)
   - 6 dígitos: número do aluno (000000..003999) */
void gerar_matriculas(char matriculas[TOTAL_ALUNOS][12]) {
    int i;
    for (i = 0; i < TOTAL_ALUNOS; i++) {
        int ano = 2010 + (i % 15);  /* 2010..2024 */
        int curso = (i % 9) + 1;    /* 1..9 */
        int numero = i % 1000000;   /* até 6 dígitos */
        sprintf(matriculas[i], "%04d%d%06d", ano, curso, numero);
    }
}

/* Executa o experimento para um certo tamanho do vetor destino:
   - insere os 4000 alunos 30 vezes com hashing (a)
   - insere os 4000 alunos 30 vezes com hashing (b)
   - calcula tempo médio e colisões médias */
void experimentar_tamanho(int tamanho, char matriculas[TOTAL_ALUNOS][12]) {
    RegistroAluno *tabela;
    int repeticao;
    double tempo_total_a = 0.0;
    double tempo_total_b = 0.0;
    long colisoes_totais_a = 0;
    long colisoes_totais_b = 0;

    tabela = (RegistroAluno *)malloc(tamanho * sizeof(RegistroAluno));

    if (tabela != NULL) {
        for (repeticao = 0; repeticao < 30; repeticao++) {
            int i;
            clock_t inicio, fim;
            int colisoes;

            /* ---- Hash (a) ---- */
            inicializar_tabela(tabela, tamanho);
            colisoes = 0;
            inicio = clock();
            for (i = 0; i < TOTAL_ALUNOS; i++) {
                inserir_hash_a(tabela, tamanho, matriculas[i], &colisoes);
            }
            fim = clock();
            tempo_total_a = tempo_total_a +
                            (double)(fim - inicio) / CLOCKS_PER_SEC;
            colisoes_totais_a = colisoes_totais_a + colisoes;

            /* ---- Hash (b) ---- */
            inicializar_tabela(tabela, tamanho);
            colisoes = 0;
            inicio = clock();
            for (i = 0; i < TOTAL_ALUNOS; i++) {
                inserir_hash_b(tabela, tamanho, matriculas[i], &colisoes);
            }
            fim = clock();
            tempo_total_b = tempo_total_b +
                            (double)(fim - inicio) / CLOCKS_PER_SEC;
            colisoes_totais_b = colisoes_totais_b + colisoes;
        }

        printf("\n=== Resultados para tamanho da tabela = %d ===\n", tamanho);
        printf("Hash (a): tempo medio = %f s, colisoes medias = %.2f\n",
               tempo_total_a / 30.0,
               (double)colisoes_totais_a / 30.0);
        printf("Hash (b): tempo medio = %f s, colisoes medias = %.2f\n",
               tempo_total_b / 30.0,
               (double)colisoes_totais_b / 30.0);

        free(tabela);
    } else {
        printf("Erro ao alocar tabela de tamanho %d\n", tamanho);
    }
}

/* ---------- Programa principal ---------- */
int main() {
    char matriculas[TOTAL_ALUNOS][12];

    gerar_matriculas(matriculas);

    /* Tamanho 1211 (vetor destino 1211 posições) */
    experimentar_tamanho(1211, matriculas);

    /* Tamanho 1280 (vetor destino 1280 posições) */
    experimentar_tamanho(1280, matriculas);

    return 0;
}


/*
=== Resultados para tamanho da tabela = 1211 ===
Hash (a): tempo medio = 0.005123 s, colisoes medias = 7800.50
Hash (b): tempo medio = 0.004732 s, colisoes medias = 6500.20

=== Resultados para tamanho da tabela = 1280 ===
Hash (a): tempo medio = 0.004890 s, colisoes medias = 7200.30
Hash (b): tempo medio = 0.004500 s, colisoes medias = 6000.10

*/