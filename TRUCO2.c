#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Tamanho do baralho
#define TAMANHO_BARALHO 27

// Tamanho da mão de cada jogador por rodada
#define TAMANHO_MAO 3

// Número de pontos para vencer o jogo
#define PONTOS_PARA_VITORIA 12

// Número de jopgadores
#define NUMERO_JOGADORES 4

char *display[30][65];
char *menu[7][15] = {
    {"┏", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "┓"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┗", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "┛"}
};

int i, j;

void Display(){
    system("clear");
    for (i = 0; i < 30; i++){
        for (j = 0; j < 65; j++){
            printf("%s", display[i][j]);
        }
        printf("\n");
    }
}

//struct para armazenar as cartas
typedef struct {
    char nome[20];
    char carta[2];
    char nipe[5];
    int valor;
} Carta ;

Carta baralho[27];

typedef struct {
    char nome[20];
    int pontos;
    Carta mao[TAMANHO_MAO];
} Jogador;

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

Jogador jogadores[NUMERO_JOGADORES];

void distribuir(Jogador *jogadores, int jogadorAtual){
    if (jogadorAtual == NUMERO_JOGADORES) return;
    for (i = 0; i < TAMANHO_MAO; i++){
        jogadores[jogadorAtual].mao[i] = baralho[j];
        j++;
    }
    distribuir(jogadores, jogadorAtual+1);
}

int main(){
    gerarBaralho();
    // iniciar display
    for(i = 0; i < 30; i++){
        for(j = 0; j < 65; j++){
            display[i][j] = "█";
        }
    }
    embaralhar();
    strcpy(jogadores[0].nome, "jogador 1");
    strcpy(jogadores[1].nome, "jogador 2");
    strcpy(jogadores[2].nome, "jogador 3");
    strcpy(jogadores[3].nome, "jogador 4");
    j = 0;
    distribuir(jogadores, 0);
    for (i = 0; i < NUMERO_JOGADORES; i++){
        printf("%s\n", jogadores[i].nome);
        for (j = 0; j < TAMANHO_MAO; j++){
            printf("%s %s %s %d\n", jogadores[i].mao[j].nome, jogadores[i].mao[j].carta, jogadores[i].mao[j].nipe, jogadores[i].mao[j].valor);
        }
        printf("\n");
    }
}