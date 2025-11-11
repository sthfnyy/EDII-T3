#include <stdio.h>
#include <time.h>
#include "Q02.h"


void decodificar_estado_rec(int indice, int posicao, int vetor_estado[])
{
    if (posicao < NUM_DISCOS)
    {
        vetor_estado[posicao] = indice % NUM_PINOS;
        decodificar_estado_rec(indice / NUM_PINOS, posicao + 1, vetor_estado);
    }

}

/* Função de uso geral para decodificar */
void decodificar_estado(int indice, int vetor_estado[])
{
    decodificar_estado_rec(indice, 0, vetor_estado);
}

/* Versão recursiva da codificação: vetor → número em base 3 */
int codificar_estado_rec(int posicao, int vetor_estado[], int multiplicador)
{
    int resultado;

    if (posicao == NUM_DISCOS)
    {
        resultado = 0;
    }
    else
    {
        resultado = vetor_estado[posicao] * multiplicador +
                    codificar_estado_rec(posicao + 1,
                                         vetor_estado,
                                         multiplicador * NUM_PINOS);
    }

    return resultado;
}

/* Função de uso geral para codificar */
int codificar_estado(int vetor_estado[])
{
    int resultado;
    resultado = codificar_estado_rec(0, vetor_estado, 1);
    return resultado;
}

/* ================== TOPO DO PINO ================== */

/* Encontra o disco no topo de um pino (menor índice de disco naquele pino).
   Retorna o índice do disco ou -1 se estiver vazio. */
int encontrar_topo_pino(int vetor_estado[], int pino)
{
    int disco;
    int resultado;

    resultado = -1;

    for (disco = 0; disco < NUM_DISCOS; disco++)
    {
        if (vetor_estado[disco] == pino && resultado == -1)
        {
            resultado = disco;
        }
    }

    return resultado;
}

/* ================== MATRIZ DE ADJACÊNCIA ================== */

void inicializar_matriz_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int linha, int coluna)
{
    if (linha < NUM_ESTADOS)
    {
        matriz[linha][coluna] = 0;

        if (coluna + 1 < NUM_ESTADOS)
        {
            inicializar_matriz_rec(matriz, linha, coluna + 1);
        }
        else
        {
            inicializar_matriz_rec(matriz, linha + 1, 0);
        }
    }

}

/* Constrói a matriz de adjacência percorrendo todos os estados */
void construir_matriz_adjacencia_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS],
                                     int indice_estado)
{
    int estado_atual[NUM_DISCOS];
    int estado_novo[NUM_DISCOS];
    int pino_origem, pino_destino;
    int disco_mover;
    int topo_destino;
    int i;
    int indice_novo;

    if (indice_estado < NUM_ESTADOS)
    {
        decodificar_estado(indice_estado, estado_atual);

        for (pino_origem = 0; pino_origem < NUM_PINOS; pino_origem++)
        {
            disco_mover = encontrar_topo_pino(estado_atual, pino_origem);

            if (disco_mover != -1)
            {
                for (pino_destino = 0; pino_destino < NUM_PINOS; pino_destino++)
                {
                    if (pino_destino != pino_origem)
                    {
                        topo_destino = encontrar_topo_pino(estado_atual, pino_destino);

                        if (topo_destino == -1 || disco_mover < topo_destino)
                        {
                            for (i = 0; i < NUM_DISCOS; i++)
                            {
                                estado_novo[i] = estado_atual[i];
                            }

                            estado_novo[disco_mover] = pino_destino;
                            indice_novo = codificar_estado(estado_novo);
                            matriz[indice_estado][indice_novo] = 1;
                        }
                    }
                }
            }
        }

        construir_matriz_adjacencia_rec(matriz, indice_estado + 1);
    }

}

void construir_matriz_adjacencia(int matriz[NUM_ESTADOS][NUM_ESTADOS])
{
    inicializar_matriz_rec(matriz, 0, 0);
    construir_matriz_adjacencia_rec(matriz, 0);
}

/* ================== LISTA DE ADJACÊNCIA ================== */

void inicializar_grafo_lista_rec(ListaAdj grafo[], int indice)
{
    if (indice < NUM_ESTADOS)
    {
        grafo[indice].qtd_vizinhos = 0;
        inicializar_grafo_lista_rec(grafo, indice + 1);
    }

}

void adicionar_vizinho(ListaAdj grafo[], int origem, int destino)
{
    int pos;

    pos = grafo[origem].qtd_vizinhos;

    if (pos < MAX_VIZINHOS)
    {
        grafo[origem].vizinhos[pos] = destino;
        grafo[origem].qtd_vizinhos = grafo[origem].qtd_vizinhos + 1;
    }

}

/* Constrói a lista de adjacência da mesma forma lógica da matriz */
void construir_grafo_lista_rec(ListaAdj grafo[], int indice_estado)
{
    int estado_atual[NUM_DISCOS];
    int estado_novo[NUM_DISCOS];
    int pino_origem, pino_destino;
    int disco_mover;
    int topo_destino;
    int i;
    int indice_novo;

    if (indice_estado < NUM_ESTADOS)
    {
        decodificar_estado(indice_estado, estado_atual);

        for (pino_origem = 0; pino_origem < NUM_PINOS; pino_origem++)
        {
            disco_mover = encontrar_topo_pino(estado_atual, pino_origem);

            if (disco_mover != -1)
            {
                for (pino_destino = 0; pino_destino < NUM_PINOS; pino_destino++)
                {
                    if (pino_destino != pino_origem)
                    {
                        topo_destino = encontrar_topo_pino(estado_atual, pino_destino);

                        if (topo_destino == -1 || disco_mover < topo_destino)
                        {
                            for (i = 0; i < NUM_DISCOS; i++)
                            {
                                estado_novo[i] = estado_atual[i];
                            }

                            estado_novo[disco_mover] = pino_destino;
                            indice_novo = codificar_estado(estado_novo);
                            adicionar_vizinho(grafo, indice_estado, indice_novo);
                        }
                    }
                }
            }
        }

        construir_grafo_lista_rec(grafo, indice_estado + 1);
    }

}

void construir_grafo_lista(ListaAdj grafo[])
{
    inicializar_grafo_lista_rec(grafo, 0);
    construir_grafo_lista_rec(grafo, 0);
}

/* ================== FUNÇÕES AUXILIARES PARA DIJKSTRA ================== */

/* Encontra recursivamente o vértice não visitado com menor distância */
int encontrar_menor_distancia_rec(int distancia[], int visitado[], int posicao, int menor_valor, int indice_menor)
{
    int resultado;
    int novo_menor_valor;
    int novo_indice_menor;

    novo_menor_valor = menor_valor;
    novo_indice_menor = indice_menor;

    if (posicao < NUM_ESTADOS)
    {
        if (!visitado[posicao] && distancia[posicao] < novo_menor_valor)
        {
            novo_menor_valor = distancia[posicao];
            novo_indice_menor = posicao;
        }

        resultado = encontrar_menor_distancia_rec(distancia,
                                                  visitado,
                                                  posicao + 1,
                                                  novo_menor_valor,
                                                  novo_indice_menor);
    }
    else
    {
        resultado = indice_menor;
    }

    return resultado;
}

/* ================== DIJKSTRA COM MATRIZ ================== */

void dijkstra_matriz(int matriz[NUM_ESTADOS][NUM_ESTADOS], int inicio, int destino, int antecessor[], int distancia[])
{
    int visitado[NUM_ESTADOS];
    int i, j;
    int u;
    int indice_menor;

    for (i = 0; i < NUM_ESTADOS; i++)
    {
        distancia[i] = INFINITO;
        antecessor[i] = -1;
        visitado[i] = 0;
    }

    distancia[inicio] = 0;

    for (i = 0; i < NUM_ESTADOS; i++)
    {
        indice_menor = encontrar_menor_distancia_rec(distancia, visitado, 0, INFINITO, -1);

        if (indice_menor == -1)
        {
            u = -1;
        }
        else
        {
            u = indice_menor;
        }

        if (u == -1)
        {
            break;
        }

        visitado[u] = 1;

        for (j = 0; j < NUM_ESTADOS; j++)
        {
            if (matriz[u][j] == 1 && !visitado[j])
            {
                if (distancia[u] + 1 < distancia[j])
                {
                    distancia[j] = distancia[u] + 1;
                    antecessor[j] = u;
                }
            }
        }
    }

}

/* ================== DIJKSTRA COM LISTA DE ADJACÊNCIA ================== */

void dijkstra_lista(ListaAdj grafo[], int inicio, int destino, int antecessor[], int distancia[])
{
    int visitado[NUM_ESTADOS];
    int i, j;
    int u;
    int v;
    int indice_menor;

    for (i = 0; i < NUM_ESTADOS; i++)
    {
        distancia[i] = INFINITO;
        antecessor[i] = -1;
        visitado[i] = 0;
    }

    distancia[inicio] = 0;

    for (i = 0; i < NUM_ESTADOS; i++)
    {
        indice_menor = encontrar_menor_distancia_rec(distancia, visitado, 0, INFINITO, -1);

        if (indice_menor == -1)
        {
            u = -1;
        }
        else
        {
            u = indice_menor;
        }

        if (u == -1)
        {
            break;
        }

        visitado[u] = 1;

        for (j = 0; j < grafo[u].qtd_vizinhos; j++)
        {
            v = grafo[u].vizinhos[j];

            if (!visitado[v])
            {
                if (distancia[u] + 1 < distancia[v])
                {
                    distancia[v] = distancia[u] + 1;
                    antecessor[v] = u;
                }
            }
        }
    }
}

/* ================== IMPRESSÃO DE ESTADOS E CAMINHO ================== */

void imprimir_estado(int vetor_estado[])
{
    int i;
    printf("[ ");
    for (i = 0; i < NUM_DISCOS; i++)
    {
        printf("%d ", vetor_estado[i]);
    }
    printf("]\n");
}

void imprimir_caminho_rec(int caminho[], int tamanho, int indice)
{
    int estado[NUM_DISCOS];

    if (indice < tamanho)
    {
        decodificar_estado(caminho[indice], estado);
        printf("Passo %d: ", indice);
        imprimir_estado(estado);
        imprimir_caminho_rec(caminho, tamanho, indice + 1);
    }

}

void imprimir_caminho(int antecessor[], int inicio, int destino)
{
    int caminho[NUM_ESTADOS];
    int tamanho;
    int atual;
    int i;

    tamanho = 0;
    atual = destino;

    while (atual != -1 && tamanho < NUM_ESTADOS)
    {
        caminho[tamanho] = atual;
        tamanho = tamanho + 1;

        if (atual == inicio)
        {
            atual = -1;
        }
        else
        {
            atual = antecessor[atual];
        }
    }

    if (tamanho > 0)
    {
        for (i = 0; i < tamanho / 2; i++)
        {
            int temp;
            temp = caminho[i];
            caminho[i] = caminho[tamanho - 1 - i];
            caminho[tamanho - 1 - i] = temp;
        }

        printf("\nCaminho (do estado inicial ate o estado final):\n");
        imprimir_caminho_rec(caminho, tamanho, 0);
    }
    else
    {
        printf("Nao foi possivel reconstruir o caminho.\n");
    }

}
