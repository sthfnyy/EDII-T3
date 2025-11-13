#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Q05.h"


int converter_texto_para_posicao(const char *texto, int *coluna, int *linha) {
    int resultado;
    int pos;
    int numero;
    char letra;

    resultado = 0;
    letra = texto[0];

    if (letra >= 'A' && letra <= 'H') {
        *coluna = letra - 'A';
    } else if (letra >= 'a' && letra <= 'h') {
        *coluna = letra - 'a';
    } else {
        resultado = 1;
    }

    pos = 1;
    numero = 0;
    while (texto[pos] >= '0' && texto[pos] <= '9') {
        numero = numero * 10 + (texto[pos] - '0');
        pos = pos + 1;
    }

    if (numero >= 1 && numero <= NUM_LINHAS) {
        *linha = numero - 1;
    } else {
        resultado = 1;
    }

    return resultado;
}

void limpar_dependencias(Celula *planilha, int indice_origem) {
    NodoAdjacencia *atual;
    NodoAdjacencia *proximo;

    atual = planilha[indice_origem].lista_dependencias;
    while (atual != NULL) {
        proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    planilha[indice_origem].lista_dependencias = NULL;
}

void adicionar_dependencia(Celula *planilha, int indice_origem, int indice_destino) 
{
    NodoAdjacencia *novo;

    novo = (NodoAdjacencia *)malloc(sizeof(NodoAdjacencia));
    if (novo != NULL) {
        novo->indice_destino = indice_destino;
        novo->proximo = planilha[indice_origem].lista_dependencias;
        planilha[indice_origem].lista_dependencias = novo;
    }

}

double avaliar_celula(Celula *planilha, int indice, int *em_calculo, int *calculado) 
{
    double resultado;
    Celula *cel;
    int tipo;
    NodoAdjacencia *nodo;
    int contador;
    double acumulador;
    int primeiro;

    cel = &planilha[indice];
    resultado = 0.0;

    if (calculado[indice]) {
        resultado = cel->valor_cache;
    } else if (em_calculo[indice]) {
        printf("Aviso: ciclo detectado na celula %d. Usando 0.\n", indice);
        resultado = 0.0;
    } else {
        em_calculo[indice] = 1;
        tipo = cel->tipo;

        if (tipo == TIPO_VAZIO) {
            resultado = 0.0;
        } else if (tipo == TIPO_CONSTANTE) {
            resultado = cel->valor_constante;
        } else if (tipo == TIPO_REFERENCIA) {
            /* Deve haver apenas uma dependência */
            if (cel->lista_dependencias != NULL) {
                int destino;
                destino = cel->lista_dependencias->indice_destino;
                resultado = avaliar_celula(planilha, destino, em_calculo, calculado);
            } else {
                resultado = 0.0;
            }
        } else {
            /* Funções: soma, max, min, media */
            acumulador = 0.0;
            contador = 0;
            primeiro = 1;
            nodo = cel->lista_dependencias;

            while (nodo != NULL) {
                double valor_dep;
                valor_dep = avaliar_celula(planilha, nodo->indice_destino, em_calculo, calculado);

                if (tipo == TIPO_SOMA || tipo == TIPO_MEDIA) {
                    acumulador = acumulador + valor_dep;
                    contador = contador + 1;
                } else if (tipo == TIPO_MAX) {
                    if (primeiro) {
                        acumulador = valor_dep;
                        primeiro = 0;
                    } else if (valor_dep > acumulador) {
                        acumulador = valor_dep;
                    }
                    contador = contador + 1;
                } else if (tipo == TIPO_MIN) {
                    if (primeiro) {
                        acumulador = valor_dep;
                        primeiro = 0;
                    } else if (valor_dep < acumulador) {
                        acumulador = valor_dep;
                    }
                    contador = contador + 1;
                }

                nodo = nodo->proximo;
            }

            if (tipo == TIPO_SOMA) {
                resultado = acumulador;
            } else if (tipo == TIPO_MEDIA) {
                if (contador > 0) {
                    resultado = acumulador / contador;
                } else {
                    resultado = 0.0;
                }
            } else if (tipo == TIPO_MAX || tipo == TIPO_MIN) {
                if (contador > 0) {
                    resultado = acumulador;
                } else {
                    resultado = 0.0;
                }
            }
        }

        em_calculo[indice] = 0;
        calculado[indice] = 1;
        cel->valor_cache = resultado;
    }

    return resultado;
}

void recalcular_planilha(Celula *planilha) {
    int *em_calculo;
    int *calculado;
    int i;

    em_calculo = (int *)malloc(TOTAL_CELULAS * sizeof(int));
    calculado = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (em_calculo != NULL && calculado != NULL) {
        for (i = 0; i < TOTAL_CELULAS; i++) {
            em_calculo[i] = 0;
            calculado[i] = 0;
        }
        for (i = 0; i < TOTAL_CELULAS; i++) {
            avaliar_celula(planilha, i, em_calculo, calculado);
        }
    }

    free(em_calculo);
    free(calculado);
}


void imprimir_planilha(Celula *planilha) {
    int linha;
    int coluna;
    int indice;

    printf("\n    ");
    for (coluna = 0; coluna < NUM_COLUNAS; coluna++) {
        printf("   %c   ", 'A' + coluna);
    }
    printf("\n");

    for (linha = 0; linha < NUM_LINHAS; linha++) {
        printf("%2d |", linha + 1);
        for (coluna = 0; coluna < NUM_COLUNAS; coluna++) {
            indice = linha * NUM_COLUNAS + coluna;
            printf(" %6.1f", planilha[indice].valor_cache);
        }
        printf("\n");
    }

}


void liberar_todas_listas(Celula *planilha) {
    int i;

    for (i = 0; i < TOTAL_CELULAS; i++) {
        limpar_dependencias(planilha, i);
    }

}

//Q05
//Novas funções para DFS e BFS

void imprimir_nome_celula(int indice) {
    int linha;
    int coluna;

    linha = indice / NUM_COLUNAS;   /* 0..19  */
    coluna = indice % NUM_COLUNAS;  /* 0..7   */

    printf("%c%d", 'A' + coluna, linha + 1);

}

void dfs_recursivo_dependencias(Celula *planilha, int indice_atual, int *visitado) 
{
    NodoAdjacencia *nodo;

    visitado[indice_atual] = 1;
    printf("Visitando (DFS): ");
    imprimir_nome_celula(indice_atual);
    printf("\n");

    nodo = planilha[indice_atual].lista_dependencias;
    while (nodo != NULL) {
        if (!visitado[nodo->indice_destino]) {
            dfs_recursivo_dependencias(planilha, nodo->indice_destino, visitado);
        }
        nodo = nodo->proximo;
    }

}

void busca_profundidade_dependencias(Celula *planilha, int indice_inicial) 
{
    int *visitado;
    int i;

    visitado = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (visitado != NULL) {
        for (i = 0; i < TOTAL_CELULAS; i++) {
            visitado[i] = 0;
        }

        printf("\n=== DFS a partir de ");
        imprimir_nome_celula(indice_inicial);
        printf(" ===\n");

        dfs_recursivo_dependencias(planilha, indice_inicial, visitado);
    } else {
        printf("Erro ao alocar memoria para DFS.\n");
    }

    free(visitado);

}


void busca_largura_dependencias(Celula *planilha, int indice_inicial) 
{
    int *visitado;
    int *fila;
    int inicio;
    int fim;
    int i;
    int atual;
    NodoAdjacencia *nodo;

    visitado = (int *)malloc(TOTAL_CELULAS * sizeof(int));
    fila = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (visitado != NULL && fila != NULL) {
        for (i = 0; i < TOTAL_CELULAS; i++) {
            visitado[i] = 0;
        }

        inicio = 0;
        fim = 0;

        fila[fim] = indice_inicial;
        fim = fim + 1;
        visitado[indice_inicial] = 1;

        printf("\n=== BFS a partir de ");
        imprimir_nome_celula(indice_inicial);
        printf(" ===\n");

        while (inicio < fim) {
            atual = fila[inicio];
            inicio = inicio + 1;

            printf("Visitando (BFS): ");
            imprimir_nome_celula(atual);
            printf("\n");

            nodo = planilha[atual].lista_dependencias;
            while (nodo != NULL) {
                if (!visitado[nodo->indice_destino]) 
                {
                    visitado[nodo->indice_destino] = 1;
                    fila[fim] = nodo->indice_destino;
                    fim = fim + 1;
                }
                nodo = nodo->proximo;
            }
        }
    } else {
        printf("Erro ao alocar memoria para BFS.\n");
    }

    if (visitado != NULL) {
        free(visitado);
    }
    if (fila != NULL) {
        free(fila);
    }

}


/*Q06*/
/* Imprime algo como A1, C3, H20 a partir do indice 0..TOTAL_CELULAS-1 */
void imprimir_nome_celula(int indice) {
    int linha;
    int coluna;

    linha = indice / NUM_COLUNAS;   /* 0..19  */
    coluna = indice % NUM_COLUNAS;  /* 0..7   */

    printf("%c%d", 'A' + coluna, linha + 1);

}

void dfs_recursivo_dependencias(Celula *planilha, int indice_atual, int *visitado)
{
    NodoAdjacencia *nodo;

    visitado[indice_atual] = 1;
    printf("Visitando (DFS): ");
    imprimir_nome_celula(indice_atual);
    printf("\n");

    nodo = planilha[indice_atual].lista_dependencias;
    while (nodo != NULL) {
        if (!visitado[nodo->indice_destino]) {
            dfs_recursivo_dependencias(planilha, nodo->indice_destino, visitado);
        }
        nodo = nodo->proximo;
    }

}

void busca_profundidade_dependencias(Celula *planilha, int indice_inicial) 
{
    int *visitado;
    int i;

    visitado = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (visitado != NULL) {
        for (i = 0; i < TOTAL_CELULAS; i++) {
            visitado[i] = 0;
        }

        printf("\n=== DFS a partir de ");
        imprimir_nome_celula(indice_inicial);
        printf(" ===\n");

        dfs_recursivo_dependencias(planilha, indice_inicial, visitado);
    } else {
        printf("Erro ao alocar memoria para DFS.\n");
    }

    free(visitado);
}


void busca_largura_dependencias(Celula *planilha, int indice_inicial) 
{
    int *visitado;
    int *fila;
    int inicio;
    int fim;
    int i;
    int atual;
    NodoAdjacencia *nodo;

    visitado = (int *)malloc(TOTAL_CELULAS * sizeof(int));
    fila = (int *)malloc(TOTAL_CELULAS * sizeof(int));

    if (visitado != NULL && fila != NULL) 
    {
        for (i = 0; i < TOTAL_CELULAS; i++) 
        {
            visitado[i] = 0;
        }

        inicio = 0;
        fim = 0;

        fila[fim] = indice_inicial;
        fim = fim + 1;
        visitado[indice_inicial] = 1;

        printf("\n=== BFS a partir de ");
        imprimir_nome_celula(indice_inicial);
        printf(" ===\n");

        while (inicio < fim) 
        {
            atual = fila[inicio];
            inicio = inicio + 1;

            printf("Visitando (BFS): ");
            imprimir_nome_celula(atual);
            printf("\n");

            nodo = planilha[atual].lista_dependencias;
            while (nodo != NULL) 
            {
                if (!visitado[nodo->indice_destino]) {
                    visitado[nodo->indice_destino] = 1;
                    fila[fim] = nodo->indice_destino;
                    fim = fim + 1;
                }
                nodo = nodo->proximo;
            }
        }
    } else {
        printf("Erro ao alocar memoria para BFS.\n");
    }

    if (visitado != NULL) {
        free(visitado);
    }
    if (fila != NULL) {
        free(fila);
    }

}


void testar_tempos(Celula *planilha) {
    int i, j;
    clock_t inicio, fim;
    double tempo_insercao_total = 0.0;
    double tempo_bfs_total = 0.0;
    double tempo_dfs_total = 0.0;
    double tempo_medio_insercao, tempo_medio_bfs, tempo_medio_dfs;
    int indice_teste;

    /* Vamos usar C1 (que depende de A1..B2) como célula para busca */
    int col = 2; /* C */
    int lin = 0; /* linha 1 */
    indice_teste = lin * NUM_COLUNAS + col;

    printf("\n=== Teste de tempos (30 execucoes) ===\n");

    for (i = 0; i < 30; i++) {
        /* (a) Inserção no grafo */
        inicio = clock();
        for (j = 0; j < TOTAL_CELULAS; j++) {
            limpar_dependencias(planilha, j);
        }

        /* Simula a mesma estrutura de dependências que você usa normalmente */
        /* Exemplo: A1=10, B1=20, A2=30, C1=@soma(A1..B2), D1=C1 */

        adicionar_dependencia(planilha, indice_teste, 0);  /* A1 */
        adicionar_dependencia(planilha, indice_teste, 1);  /* B1 */
        adicionar_dependencia(planilha, indice_teste, 8);  /* A2 */
        adicionar_dependencia(planilha, indice_teste, 9);  /* B2 */

        fim = clock();
        tempo_insercao_total += ((double)(fim - inicio) / CLOCKS_PER_SEC);

        /* (b) BFS */
        inicio = clock();
        busca_largura_dependencias(planilha, indice_teste);
        fim = clock();
        tempo_bfs_total += ((double)(fim - inicio) / CLOCKS_PER_SEC);

        /* (c) DFS */
        inicio = clock();
        busca_profundidade_dependencias(planilha, indice_teste);
        fim = clock();
        tempo_dfs_total += ((double)(fim - inicio) / CLOCKS_PER_SEC);
    }

    tempo_medio_insercao = tempo_insercao_total / 30.0;
    tempo_medio_bfs = tempo_bfs_total / 30.0;
    tempo_medio_dfs = tempo_dfs_total / 30.0;

    printf("\n=== Resultados medios ===\n");
    printf("(a) Insercao no grafo     : %f segundos\n", tempo_medio_insercao);
    printf("(b) Busca em Largura (BFS): %f segundos\n", tempo_medio_bfs);
    printf("(c) Busca em Profundidade : %f segundos\n", tempo_medio_dfs);
}


static void inicializar_planilha(Celula *planilha) {
    int i;
    for (i = 0; i < TOTAL_CELULAS; i++) {
        planilha[i].tipo = TIPO_VAZIO;
        planilha[i].valor_constante = 0.0;
        planilha[i].valor_cache = 0.0;
        planilha[i].lista_dependencias = NULL;
    }

}

static void exibir_instrucoes_basicas(void) {
    printf("Planilha (colunas A..H, linhas 1..20).\n");
    printf("Entrada: CELULA VALOR\n");
    printf("Exemplos:\n");
    printf("  A1 10\n");
    printf("  B1 =A1\n");
    printf("  C1 @soma(A1..B2)\n");
    printf("Funcoes: @soma, @max, @min, @media\n\n");
    
}

static void editar_celulas(Celula *planilha) {
    char linha_entrada[100];
    int continuar = 1;

    printf("\n=== Modo de edicao ===\n");
    printf("Digite linhas no formato: CELULA VALOR  (ex.: A1 10, B1 =A1, C1 @soma(A1..B2))\n");
    printf("Digite 'voltar' para retornar ao menu.\n\n");

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
            if (strncmp(linha_entrada, "voltar", 6) == 0 ||
                strncmp(linha_entrada, "VOLTAR", 6) == 0) {
                continuar = 0;
            } else {
                lidos = sscanf(linha_entrada, "%9s %79[^\n]", texto_celula, texto_valor);
                if (lidos == 2) {
                    if (converter_texto_para_posicao(texto_celula, &coluna, &linha) == 0) {
                        char *v = texto_valor;
                        Celula *cel;

                        while (*v == ' ') { v = v + 1; }

                        indice_celula = linha * NUM_COLUNAS + coluna;
                        cel = &planilha[indice_celula];

                        limpar_dependencias(planilha, indice_celula);
                        cel->tipo = TIPO_VAZIO;
                        cel->valor_constante = 0.0;

                        if (v[0] == '@') {
                            char nome_funcao[10];
                            char intervalo[40];
                            int ok_parse = 0;

                            if (sscanf(v, "@%9[^ (](%39[^)])", nome_funcao, intervalo) == 2) {
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

                                    if (converter_texto_para_posicao(texto_inicio, &col_i, &lin_i) == 0 &&
                                        converter_texto_para_posicao(texto_fim, &col_f, &lin_f) == 0) {
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
                                            lin_ini = inteiro_min(lin_i, lin_f);
                                            lin_fim = inteiro_max(lin_i, lin_f);
                                            col_ini = inteiro_min(col_i, col_f);
                                            col_fim = inteiro_max(col_i, col_f);
                                        for (linha = lin_ini; linha <= lin_fim; linha++) 
                                        {
                                            for (coluna = col_ini; coluna <= col_fim; coluna++) 
                                            {
                                                int indice_dep = linha * NUM_COLUNAS + coluna;
                                                adicionar_dependencia(planilha, indice_celula, indice_dep);
                                            }
                                        }
                                    }
                                }
                            }

                            if (!ok_parse) 
                            {
                                printf("Intervalo ou funcao invalidos.\n");
                            }

                        } else if (v[0] == '=') {
                            char texto_ref[10];
                            if (sscanf(v, "=%9s", texto_ref) == 1) 
                            {
                                int col_ref, lin_ref;
                                if (converter_texto_para_posicao(texto_ref, &col_ref, &lin_ref) == 0) {
                                    int indice_dep;
                                    cel->tipo = TIPO_REFERENCIA;
                                    indice_dep = lin_ref * NUM_COLUNAS + col_ref;
                                    adicionar_dependencia(planilha, indice_celula, indice_dep);
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

    recalcular_planilha(planilha);
    printf("\nEstado atual da planilha:\n");
    imprimir_planilha(planilha);

}

static void executar_buscas(Celula *planilha) {
    char linha_entrada[100];
    char texto_busca[10];
    int col_b, lin_b, indice_busca;

    printf("\nDigite uma celula para DFS/BFS (ex.: C1): ");
    if (fgets(linha_entrada, sizeof(linha_entrada), stdin) != NULL &&
        sscanf(linha_entrada, "%9s", texto_busca) == 1) {

        if (converter_texto_para_posicao(texto_busca, &col_b, &lin_b) == 0) {
            indice_busca = lin_b * NUM_COLUNAS + col_b;
            busca_profundidade_dependencias(planilha, indice_busca);
            busca_largura_dependencias(planilha, indice_busca);
        } else {
            printf("Celula invalida para busca.\n");
        }
    }
    return;
}

static void medir_tempos_menu(Celula *planilha) {
    testar_tempos(planilha);
}

static void recalcular_e_imprimir(Celula *planilha) {
    recalcular_planilha(planilha);
    imprimir_planilha(planilha);
    
}

static void limpar_planilha(Celula *planilha) {
    liberar_todas_listas(planilha);
    inicializar_planilha(planilha);
    printf("\nPlanilha limpa.\n");
}

static int ler_opcao_menu(void) {
    int opcao = -1;
    char buf[32];

    printf("\n================ MENU =================\n");
    printf("1) Editar celulas\n");
    printf("2) DFS/BFS nas dependencias\n");
    printf("3) Medir tempos (30 repeticoes)\n");
    printf("4) Recalcular e imprimir planilha\n");
    printf("5) Limpar planilha\n");
    printf("0) Sair\n");
    printf("Escolha: ");

    if (fgets(buf, sizeof(buf), stdin) != NULL) {
        sscanf(buf, "%d", &opcao);
    }
    return opcao;
}

static void menu_principal(Celula *planilha) {
    int rodando = 1;

    exibir_instrucoes_basicas();

    while (rodando) {
        int opcao = ler_opcao_menu();
        switch (opcao) {
            case 1: editar_celulas(planilha); break;
            case 2: executar_buscas(planilha); break;
            case 3: medir_tempos_menu(planilha); break;
            case 4: recalcular_e_imprimir(planilha); break;
            case 5: limpar_planilha(planilha); break;
            case 0: rodando = 0; break;
            default: printf("Opcao invalida.\n"); break;
        }
    }

}

