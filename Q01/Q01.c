#include <stdio.h>
#include <time.h>
#include "Q01.h"


void decodificar_torre_rec(int indice, int posicao, int pinos_dos_discos[])
{
    if (posicao < NUM_DISCOS)
    {
        pinos_dos_discos[posicao] = indice % NUM_PINOS;
        decodificar_torre_rec(indice / NUM_PINOS, posicao + 1, pinos_dos_discos);
    }

}

void decodificar_torre(int indice, int pinos_dos_discos[])
{
    decodificar_torre_rec(indice, 0, pinos_dos_discos);
}


int codificar_torre_rec(int posicao, int pinos_dos_discos[], int multiplicador)
{
    int resultado;

    if (posicao == NUM_DISCOS)
    {
        resultado = 0;
    }
    else
    {
        resultado = pinos_dos_discos[posicao] * multiplicador + codificar_torre_rec(posicao + 1, pinos_dos_discos, multiplicador * NUM_PINOS);
    }

    return resultado;
}


int codificar_torre(int pinos_dos_discos[])
{
    int resultado;
    resultado = codificar_torre_rec(0, pinos_dos_discos, 1);
    return resultado;
}


int encontrar_topo_pino(int pinos_dos_discos[], int pino)
{
    int disco;
    int resultado;

    resultado = -1;

    for (disco = 0; disco < NUM_DISCOS; disco++)
    {
        if (pinos_dos_discos[disco] == pino && resultado == -1)
        {
            resultado = disco;
        }
    }

    return resultado;
}


void zerar_matriz_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int linha, int coluna)
{
    if (linha < NUM_ESTADOS)
    {
        matriz[linha][coluna] = 0;

        if (coluna + 1 < NUM_ESTADOS)
        {
            zerar_matriz_rec(matriz, linha, coluna + 1);
        }
        else
        {
            zerar_matriz_rec(matriz, linha + 1, 0);
        }
    }
}


void preencher_matriz_adjacencia_rec(int matriz[NUM_ESTADOS][NUM_ESTADOS], int indice_torre)
{
    int torre_atual[NUM_DISCOS];
    int torre_nova[NUM_DISCOS];
    int pino_origem, pino_destino;
    int disco_mover;
    int topo_destino;
    int i;
    int indice_novo;

    if (indice_torre < NUM_ESTADOS)
    {

        decodificar_torre(indice_torre, torre_atual);

        for (pino_origem = 0; pino_origem < NUM_PINOS; pino_origem++)
        {
            disco_mover = encontrar_topo_pino(torre_atual, pino_origem);

            if (disco_mover != -1)
            {
                for (pino_destino = 0; pino_destino < NUM_PINOS; pino_destino++)
                {
                    if (pino_destino != pino_origem)
                    {
                        topo_destino = encontrar_topo_pino(torre_atual, pino_destino);

                        if (topo_destino == -1 || disco_mover < topo_destino)
                        {
                            for (i = 0; i < NUM_DISCOS; i++)
                            {
                                torre_nova[i] = torre_atual[i];
                            }

                            torre_nova[disco_mover] = pino_destino;
                            indice_novo = codificar_torre(torre_nova);
                            matriz[indice_torre][indice_novo] = 1;
                        }
                    }
                }
            }
        }

        preencher_matriz_adjacencia_rec(matriz, indice_torre + 1);
    }

}

void montar_matriz_adjacencia(int matriz[NUM_ESTADOS][NUM_ESTADOS])
{
    zerar_matriz_rec(matriz, 0, 0);
    preencher_matriz_adjacencia_rec(matriz, 0);
}


int escolher_proximo_vertice_rec(int distancia[], int visitado[], int posicao, int menor_valor, int indice_menor)
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

        resultado = escolher_proximo_vertice_rec(distancia, visitado, posicao + 1, novo_menor_valor, novo_indice_menor);
    }
    else
    {
        resultado = indice_menor;
    }

    return resultado;
}

/* Algoritmo de Dijkstra  */
void dijkstra(int matriz[NUM_ESTADOS][NUM_ESTADOS], int inicio, int destino, int antecessor[], int distancia[])
{
    int visitado[NUM_ESTADOS];
    int i, j;
    int vertice_atual;
    int indice_menor;
    int terminou; 


    for (i = 0; i < NUM_ESTADOS; i++)
    {
        distancia[i] = INFINITO;
        antecessor[i] = -1;
        visitado[i] = 0;
    }

    distancia[inicio] = 0;
    terminou = 0;

    for (i = 0; i < NUM_ESTADOS && !terminou; i++)
    {
        indice_menor = escolher_proximo_vertice_rec(distancia, visitado, 0, INFINITO, -1);

        if (indice_menor == -1)
        {
            terminou = 1;
        }
        else
        {
            vertice_atual = indice_menor;
            visitado[vertice_atual] = 1;

            for (j = 0; j < NUM_ESTADOS; j++)
            {
                if (matriz[vertice_atual][j] == 1 && !visitado[j])
                {
                    if (distancia[vertice_atual] + 1 < distancia[j])
                    {
                        distancia[j] = distancia[vertice_atual] + 1;
                        antecessor[j] = vertice_atual;
                    }
                }
            }
        }
    }
}


void imprimir_torre(int pinos_dos_discos[])
{
    int i;
    printf("[ ");
    for (i = 0; i < NUM_DISCOS; i++)
    {
        printf("%d ", pinos_dos_discos[i]);
    }
    printf("]\n");
}

/* Imprime o caminho usando um vetor de índices de torres, com recursão */
void imprimir_caminho_rec(int caminho[], int tamanho, int indice)
{
    int torre[NUM_DISCOS];

    if (indice < tamanho)
    {
        decodificar_torre(caminho[indice], torre);
        printf("Passo %d: ", indice);
        imprimir_torre(torre);
        imprimir_caminho_rec(caminho, tamanho, indice + 1);
    }

}

/* Reconstrói o caminho  e imprime usando recursão */
void imprimir_caminho(int antecessor[], int inicio, int destino)
{
    int caminho[NUM_ESTADOS];
    int tamanho;
    int atual;
    int i;

    tamanho = 0;
    atual = destino;

    /* Reconstrói o caminho de trás pra frente */
    while (atual != -1 && tamanho < NUM_ESTADOS)
    {
        caminho[tamanho] = atual;
        tamanho++;

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
        /* Inverte o vetor para ficar do início até o destino */
        for (i = 0; i < tamanho / 2; i++)
        {
            int temp;
            temp = caminho[i];
            caminho[i] = caminho[tamanho - 1 - i];
            caminho[tamanho - 1 - i] = temp;
        }

        printf("\nCaminho (da configuracao inicial ate a configuracao final):\n");
        imprimir_caminho_rec(caminho, tamanho, 0);
    }
    else
    {
        printf("Nao foi possivel reconstruir o caminho.\n");
    }

}
