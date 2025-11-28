#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Q03.h"


int main() {
    Planilha* plan_principal = InicializarPlanilha();
    int codigo_saida = 0;

    if (plan_principal != NULL) {
        LoopPrincipal(plan_principal);
        LiberarPlanilha(plan_principal);
        codigo_saida = 0;
    } else {
        codigo_saida = 1;
    }
    return codigo_saida;
}