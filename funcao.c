#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *baralho[] = {
    "4♣", "7♥", "A♠", "7♦",
    "3♣", "3♥", "3♠", "3♦",
    "2♣", "2♥", "2♠", "2♦",
    "A♣", "A♥", "A♦",
    "K♣", "K♥", "K♠", "K♦",
    "J♣", "J♥", "J♠", "J♦",
    "Q♣", "Q♥", "Q♠", "Q♦"
};

// Tamanho do baralho
#define TAMANHO_BARALHO sizeof(baralho) / sizeof(baralho[0])

void embaralharBaralho() {
    srand(time(NULL));
    int i, j;
    char *temp;

    for (i = TAMANHO_BARALHO - 1; i > 0; i--) {
        j = rand() % (i + 1);
        if (i != j) {
            temp = baralho[i];
            baralho[i] = baralho[j];
            baralho[j] = temp;
        }
    }
}

int main(){

    embaralharBaralho();
    
    int i;

    for(i=0;i<TAMANHO_BARALHO;i++){
        printf("%s ",baralho[i]);
    }

    return 0;
}