#include <stdio.h>
#include "Q05-6.h"

int main(void)
{
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
