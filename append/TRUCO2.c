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

int i, j;

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

Dupla dupla1, dupla2;

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

Mesa mesa;

void distribuir(Jogador *jogadores, int jogadorAtual){
    if (jogadorAtual == NUMERO_JOGADORES) return;
    for (i = 0; i < TAMANHO_MAO; i++){
        jogadores[jogadorAtual].mao[i] = baralho[j];
        j++;
    }
    distribuir(jogadores, jogadorAtual+1);
}

void verificarVencedor(Mesa *mesa, Jogador *jogadores){
    int maiorCarta = 0;
    int jogadorVencedor = 0;
    for (i = 0; i < NUMERO_JOGADORES; i++){
        if (mesa->rodada[i].valor > maiorCarta){
            maiorCarta = mesa->rodada[i].valor;
            jogadorVencedor = i;
        }
    }
    mesa->maiorCarta = maiorCarta;
    strcpy(mesa->jogadorVencedor, jogadores[jogadorVencedor].nome);
    printf("%s venceu a rodada com %s%s\n", mesa->jogadorVencedor, mesa->rodada[jogadorVencedor].carta, mesa->rodada[jogadorVencedor].nipe);
    for (i = 0; i < NUMERO_JOGADORES; i++){
        strcpy(mesa->rodada[i].carta, "");
        strcpy(mesa->rodada[i].nipe, "");
        mesa->rodada[i].valor = 0;
    }
    if ((dupla1.jogador1 == jogadores[jogadorVencedor].nome) || (dupla1.jogador2 == jogadores[jogadorVencedor].nome)){
        dupla1.pontos++;
    } else {
        dupla2.pontos++;
    }
    if (dupla1.pontos == 2){
        printf("%s venceu a partida!\n", jogadores[jogadorVencedor].nome);
        dupla1.pontos = 0;
        return;
    }
    if (dupla2.pontos == 2){
        printf("%s venceu a partida!\n", jogadores[jogadorVencedor].nome);
        dupla2.pontos = 0;
        return;
    }
    sleep(1);
    rodada(mesa, jogadores, jogadorVencedor, 0);
}

void rodada(Mesa *mesa, Jogador *jogadores, int jogadorAtual, int jogadas){
    if (jogadas == NUMERO_JOGADORES) {
        verificarVencedor(mesa, jogadores);
        return;
    }
    printf("Rodada %d\n", jogadas);
    printf("Mesa: ");
    for (i = 0; i < jogadas; i++){
        printf("%s%s ", mesa->rodada[i].carta, mesa->rodada[i].nipe);
    }
    printf("\n");
    if (jogadores[jogadorAtual].nome == jogadores[0].nome){
        printf("Sua vez de jogar.\n");
        printf("Escolha uma carta: ");
        scanf("%d", &i);
        mesa->rodada[jogadas] = jogadores[0].mao[i];
    } else {
        mesa->rodada[jogadas] = jogadores[jogadorAtual].mao[0];
        printf("%s jogou %s%s\n", jogadores[jogadorAtual].nome, jogadores[jogadorAtual].mao[0].carta, jogadores[jogadorAtual].mao[0].nipe);
        sleep(1);
    }
    printf("DEBUG: jogadorAtual=%d, i=%d\n", jogadorAtual, i);
    rodada(mesa, jogadores, (jogadorAtual < NUMERO_JOGADORES -1) ? jogadorAtual+1 : jogadorAtual - 3, jogadas+1);
}

void definirDuplas(Jogador *jogadores){
    printf("Dupla 1: ");
    strcpy(dupla1.jogador1, jogadores[0].nome);
    strcpy(dupla1.jogador2, jogadores[2].nome);
    dupla1.pontos = 0;
    printf("%s e %s\n", dupla1.jogador1, dupla1.jogador2);
    printf("Dupla 2: ");
    strcpy(dupla2.jogador1, jogadores[1].nome);
    strcpy(dupla2.jogador2, jogadores[3].nome);
    dupla2.pontos = 0;
    printf("%s e %s\n", dupla2.jogador1, dupla2.jogador2);
}

int main(){
    gerarBaralho();
    embaralhar();
    strcpy(jogadores[0].nome, "jogador 1");
    strcpy(jogadores[1].nome, "jogador 2");
    strcpy(jogadores[2].nome, "jogador 3");
    strcpy(jogadores[3].nome, "jogador 4");
    j = 0;
    distribuir(jogadores, 0);
    for (i = 0; i < NUMERO_JOGADORES; i++){
        printf("%s: ", jogadores[i].nome);
        for (j = 0; j < TAMANHO_MAO; j++){
            printf("%s%s ", jogadores[i].mao[j].carta, jogadores[i].mao[j].nipe);
        }
        printf("\n");
    }
    definirDuplas(jogadores);
    rodada(&mesa, jogadores, 0, 0);
    printf("Fim de jogo.\n");
}