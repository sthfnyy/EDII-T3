#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Q03.h"



/* -------------------------------------------------- */
/* main                                               */
/* -------------------------------------------------- */

int main() {
    Planilha* ponteiro_planilha_principal;
    int codigo_saida;

    ponteiro_planilha_principal = InicializarPlanilha();
    codigo_saida = 0;

    if (ponteiro_planilha_principal != NULL) {
        LoopPrincipal(ponteiro_planilha_principal);
        LiberarPlanilha(ponteiro_planilha_principal);
        codigo_saida = 0;
    } else {
        codigo_saida = 1;
    }

    return codigo_saida;
}


/*
A1 10
B1 20
A2 30
C1 @soma(A1..B2)
D1 =C1
FIM
Saída esperada:
    A       B       C       D       E       F       G       H
 1 |   10.0   20.0   60.0   60.0    0.0    0.0    0.0    0.0
 2 |   30

*/