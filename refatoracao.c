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

//structs
typedef struct {
    char carta[2];
    char nipe[5];
    int valor;
} Carta ;

typedef struct {
    char nome[20];
    int pontos;
    Carta mao[TAMANHO_MAO];
} Jogador;

typedef struct {
    Carta rodada[NUMERO_JOGADORES];
    char jogadorVencedor[20];
    int maiorCarta;
} Mesa ;

typedef struct {
    char jogador1[20];
    char jogador2[20];
    int pontos;
} Dupla ;

Carta baralho[27];

Dupla dupla1, dupla2;

Jogador jogadores[NUMERO_JOGADORES];

Mesa rodadas[3];

int i, j;

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
        sscanf(linha, "%s %s %d", baralho[i].carta, baralho[i].nipe, &baralho[i].valor);
        i++;
    }

    fclose(arq);
}

void embaralhar(){
    srand(time(NULL));
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

void distribuir(Jogador *jogadores, int jogadorAtual){
    if (jogadorAtual == NUMERO_JOGADORES) return;
    for (int i = 0; i < TAMANHO_MAO; i++){
        jogadores[jogadorAtual].mao[i] = baralho[j];
        j++;
    }
    distribuir(jogadores, jogadorAtual+1);
}

void definirDuplas(Jogador *jogadores){
    strcpy(jogadores[1].nome, "jogador 2");
    strcpy(jogadores[2].nome, "jogador 3");
    strcpy(jogadores[3].nome, "jogador 4");
    strcpy(dupla1.jogador1, jogadores[0].nome);
    strcpy(dupla1.jogador2, jogadores[2].nome);
    strcpy(dupla2.jogador1, jogadores[1].nome);
    strcpy(dupla2.jogador2, jogadores[3].nome);
    printf("Dupla 1: %s e %s\n", dupla1.jogador1, dupla1.jogador2);
    printf("Dupla 2: %s e %s\n", dupla2.jogador1, dupla2.jogador2);
    dupla1.pontos = 0;
    dupla2.pontos = 0;
}

void reorganizarMao(Jogador jogador, int indiceSelecionado, int jogadas) {
    Carta temp = jogador.mao[indiceSelecionado];

    for (int i = indiceSelecionado; i < TAMANHO_MAO - 1 - jogadas; i++) {
        jogador.mao[i] = jogador.mao[i + 1];
    }

    strcpy(jogador.mao[TAMANHO_MAO - 1 - jogadas].carta, "");
    strcpy(jogador.mao[TAMANHO_MAO - 1 - jogadas].nipe, "");
    jogador.mao[TAMANHO_MAO - 1 - jogadas].valor = 0;
}

void jogada(Jogador *jogadores, int jogadorAtual, int *pontosMesa, Mesa mesa, int jogadas){
    if (jogadorAtual == 0){
        for (i = 0; i < TAMANHO_MAO; i++){
            printf("%d - %s%s\n", i, jogadores[0].mao[i].carta, jogadores[0].mao[i].nipe);
        }
        printf("3 - TRUCO!");
        scanf("%d", &i);
        if (i == 3){
            pontosMesa += 3;
            truco();
            return;
        } else {
            mesa.rodada[jogadorAtual] = jogadores[0].mao[i];
            reorganizarMao(jogadores[0], i, jogadas)
            return;
        }
    }
    // boots
}

void truco(){
    
}

void partida(ultimoJogador){
    if (dupla1.pontos >= PONTOS_PARA_VITORIA || dupla2.pontos >= PONTOS_PARA_VITORIA){
        printf("Fim de jogo!\n");
        if (dupla1.pontos >= PONTOS_PARA_VITORIA){
            printf("Dupla 1 venceu!\n");
        } else {
            printf("Dupla 2 venceu!\n");
        }
        exit(0);
    }
    int pontosMesa = 0;
    embaralhar();
    distribuir(jogadores, 0);
    for (i = 0; i < NUMERO_JOGADORES; i++){
        printf("Jogador %d: %s\n", i+1, jogadores[i].nome);
        for (j = 0; j < TAMANHO_MAO; j++){
            printf("%s de %s\n", jogadores[i].mao[j].carta, jogadores[i].mao[j].nipe);
        }
        printf("\n");
    }
    rodada(0, &rodadas[0], 0, ultimoJogador, pontosMesa);
}

void rodada(int jogadas, Mesa *rodadas, int rodadaAtual, int jogadorAtual, int pontosMesa){
    if (jogadas == NUMERO_JOGADORES){
        verificarVencedor(rodadas[rodadaAtual], jogadores, pontosMesa);
        return;
    }
    printf("Rodada %d\n", jogadas+1);
    jogada(jogadores, jogadorAtual, &pontosMesa, rodadas[rodadaAtual], jogadas);
    printf("Mesa: ");
    for (i = 0; i < jogadas; i++){
        printf("%s%s ", rodadas[rodadaAtual].rodada[i].carta, rodadas[rodadaAtual].rodada[i].nipe);
    }
    printf("\n");

    rodada(jogadas+1, rodadas, rodadaAtual, (jogadorAtual < NUMERO_JOGADORES -1) ? jogadorAtual+1 : jogadorAtual - 3, pontosMesa);
}

void verificarVencedor(Mesa mesa, Jogador *jogadores, int pontosMesa){
    int D1 = jogadores[0].pontos + jogadores[2].pontos;
    int D2 = jogadores[1].pontos + jogadores[3].pontos;
    if (D1 == 2){
        dupla1.pontos += pontosMesa;
        printf("Dupla 1 venceu a rodada!\n");
    } else if (D2 == 2){
        dupla2.pontos += pontosMesa;
        printf("Dupla 2 venceu a rodada!\n");
    }
    int maiorCarta = 0;
    int jogadorVencedor = 0;
    for (i = 0; i < NUMERO_JOGADORES; i++){
        if (mesa.rodada[i].valor > maiorCarta){
            maiorCarta = mesa.rodada[i].valor;
            jogadorVencedor = i;
        }
    }
    mesa.maiorCarta = maiorCarta;
    strcpy(mesa.jogadorVencedor, jogadores[jogadorVencedor].nome);
    printf("%s venceu a rodada com %s%s\n", mesa.jogadorVencedor, mesa.rodada[jogadorVencedor].carta, mesa.rodada[jogadorVencedor].nipe);
    jogadores[jogadorVencedor].pontos += 1;
    for (i = 0; i < NUMERO_JOGADORES; i++){
        strcpy(mesa.rodada[i].carta, "");
        strcpy(mesa.rodada[i].nipe, "");
        mesa.rodada->valor = 0;
    }

    partida(jogadorVencedor);
}

int main(){
    gerarBaralho();
    printf("Digite seu nome: ");
    scanf("%s", jogadores[0].nome);
    definirDuplas(jogadores);
    partida(0);
}