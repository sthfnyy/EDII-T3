#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Q07.h"

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
RESULTADO ESPERADO
=== Resultados para tamanho da tabela = 1211 ===
Hash (a): tempo medio = 0.005123 s, colisoes medias = 7800.50
Hash (b): tempo medio = 0.004732 s, colisoes medias = 6500.20

=== Resultados para tamanho da tabela = 1280 ===
Hash (a): tempo medio = 0.004890 s, colisoes medias = 7200.30
Hash (b): tempo medio = 0.004500 s, colisoes medias = 6000.10

*/

/*
RESULTADO NA MINHA MAQUINA

=== Resultados para tamanho da tabela = 1211 ===
Hash (a): tempo medio = 0.022801 s, colisoes medias = 3605733.00
Hash (b): tempo medio = 0.022126 s, colisoes medias = 3628591.00

=== Resultados para tamanho da tabela = 1280 ===
Hash (a): tempo medio = 0.022640 s, colisoes medias = 3736131.00
Hash (b): tempo medio = 0.022651 s, colisoes medias = 3762109.00
*/