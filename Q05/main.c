#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Q05.h"

int main(void) {
    Celula *planilha = (Celula *)malloc(TOTAL_CELULAS * sizeof(Celula));

    if (planilha != NULL) {
        inicializar_planilha(planilha);
        menu_principal(planilha);
        liberar_todas_listas(planilha);
        free(planilha);
    } else {
        printf("Erro ao alocar memoria.\n");
    }
    return 0; 
}