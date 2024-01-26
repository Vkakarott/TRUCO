#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char carta[20];
    char nipe[10];
    int valor;
} Carta ;

Carta baralho[27];

void embaralhar(){
    srand(time(NULL));
    int i, j;
    Carta temp;

    for (i = 26; i > 0; i--) {
        j = rand() % (i + 1);

        if (i != j) {
            temp = baralho[i];
            baralho[i] = baralho[j];
            baralho[j] = temp;
        }
    }
}

void gerarBaralho(){
    FILE *arq;
    char linha[100];
    int i = 0;

    arq = fopen("baralho.bin", "rb");

    if (arq == NULL){
        printf("ERROR!");
        exit(0);
    }

    while (fgets(linha, sizeof(linha), arq) != NULL){
        sscanf(linha, "%s %s %d", baralho[i].carta, baralho[i].nipe, &baralho[i].valor);
        i++;
    }

    fclose(arq);
}

int main(){
    gerarBaralho();
    for (int i = 0; i < 27; i++){
        printf("%s %s %d\n", baralho[i].carta, baralho[i].nipe, baralho[i].valor);
    }
    printf("\n");
    embaralhar();
    for (int i = 0; i < 27; i++){
        printf("%s %s %d\n", baralho[i].carta, baralho[i].nipe, baralho[i].valor);
    }
}