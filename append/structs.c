#include <stdio.h>

typedef struct {
    char *naipe;
    char *numero;
    char *nome[8][17];
    int valor;
} Carta;

typedef struct {
    Carta cartas[3];
} Jogador;

