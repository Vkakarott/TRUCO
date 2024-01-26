#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *arq;
    char input[100];

    arq = fopen("baralho.bin", "wb");

    if (arq == NULL){
        printf("Erro ao abrir o arquivo.\n");
        exit(0);
    }

    while (1) {
        fgets(input, sizeof(input), stdin);

        size_t len = strlen(input);
        if (input[len - 1] == '\n') input[len - 1] = '\0';

        if (strcmp(input, "sair") == 0) break;

        fwrite(input, sizeof(char), strlen(input), arq);
        fwrite("\n", sizeof(char), 1, arq);
    }

    fclose(arq);

    printf("Arquivo salvo com sucesso.\n");
    
    return 0;
}