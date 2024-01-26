#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *arq;
    char linha[100];

    arq = fopen("baralho.bin", "rb");

    if (arq == NULL){
        printf("Erro ao abrir o arquivo.\n");
        exit(0);
    }

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        printf("%s", linha);
    }

    fclose(arq);

    return 0;
}