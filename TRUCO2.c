#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//struct para armazenar as cartas
typedef struct {
    char nome[20];
    char carta[2];
    char nipe[5];
    int valor;
} Carta ;

Carta baralho[27];

// Tamanho do baralho
#define TAMANHO_BARALHO 27

// Tamanho da mão de cada jogador por rodada
#define TAMANHO_MAO 3

// Número de pontos para vencer o jogo
#define PONTOS_PARA_VITORIA 12

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
    char linha[20];
    int i = 0;

    arq = fopen("baralho.bin", "rb");

    if (arq == NULL){
        printf("ERROR!");
        exit(0);
    }

    while (fgets(linha, sizeof(linha), arq) != NULL){
        sscanf(linha, "%s %s %s %d", baralho[i].nome, baralho[i].carta, baralho[i].nipe, &baralho[i].valor);
        i++;
    }

    fclose(arq);
}

int main(){
    gerarBaralho();
    for (int i = 0; i < 27; i++){
        printf("%s %s %s %d\n", baralho[i].nome, baralho[i].carta, baralho[i].nipe, baralho[i].valor);
    }
    printf("\n");
    embaralhar();
    for (int i = 0; i < 27; i++){
        printf("%s %s %s %d\n", baralho[i].nome, baralho[i].carta, baralho[i].nipe, baralho[i].valor);
    }
}