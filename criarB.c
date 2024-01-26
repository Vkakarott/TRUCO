#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char carta[20];
    char nipe[10];
    int valor;
} Carta ;

int main(){
    Carta baralho[27];
    char linha[100];

    FILE *arq;
    int i = 0;

    arq = fopen("baralho.bin", "rb");

    if (arq == NULL){
        printf("ERROR!\n");
        exit(0);
    }

    while (fgets(linha, sizeof(linha), arq) != NULL){
        sscanf(linha, "%s %s %d", baralho[i].carta, baralho[i].nipe, &baralho[i].valor);
        i++;
    }

    fclose(arq);

    for (i = 0; i < 4; i++){
        printf("%s %s %d\n",baralho[i].carta, baralho[i].nipe, baralho[i].valor);
    }
    
}
