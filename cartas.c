#include <stdio.h>

void red() { printf("\033[0;31m"); }
void green() { printf("\033[0;32m"); }
void yellow() { printf("\033[0;33m"); }
void blue() { printf("\033[0;34m"); }
void purple() { printf("\033[0;35m"); }
void cyan() { printf("\033[0;36m"); }
void white() { printf("\033[0;37m"); }

int main(){
    int i, j;
    char *dispay[30][185];
    char *nipes[4] = {'♠', '♣', '♥', '♦'};
    char *numeros[13] = {'Q', 'J', 'K', 'A', '2', '3', '4', '7'};
    char *cartas[8][17] = {
        {"╭", "─", "─", "─", "─", "─", "─", "─", "─", "─", "╮", "─", "─", "╮", "─", "─", "╮" },
        {"│", " ", "╭", "─", "─", "─", "─", "─", "╮", "7", "│", " ", "A", "│", " ", "4", "│" },
        {"│", " ", "│", " ", " ", " ", " ", " ", "│", "♥", "│", " ", "♠", "│", " ", "♣", "│" },
        {"│", " ", "│", " ", " ", "♤", " ", " ", "│", " ", "│", " ", " ", "│", " ", " ", "│" },
        {"│", " ", "│", "T", "R", "U", "C", "O", "│", " ", "│", " ", " ", "│", " ", " ", "│" },
        {"│", "♥", "│", " ", " ", " ", " ", " ", "│", " ", "│", " ", " ", "│", " ", " ", "│" },
        {"│", "7", "╰", "─", "─", "─", "─", "─", "╯", " ", "│", " ", " ", "│", " ", " ", "│" },
        {"╰", "─", "─", "─", "─", "─", "─", "─", "─", "─", "╯", "─", "─", "╯", "─", "─", "╯" }
    };
    char *cartasV[7][14] = {
        {"╭", "─", "─", "─", "─", "─", "─", "─", "─", "╮", "─", "╮", "─", "╮" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│", " ", "│", " ", "│" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│", " ", "│", " ", "│" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│", " ", "│", " ", "│" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│", " ", "│", " ", "│" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│", " ", "│", " ", "│" },
        {"╰", "─", "─", "─", "─", "─", "─", "─", "─", "╯", "─", "╯", "─", "╯" }
    };
    for(i = 0; i < 30; i++){
        for(j = 0; j < 85; j++){
            dispay[i][j] = " ";
        }
    }
    for (i = 0; i < 7; i++){
        for (j = 0; j < 14; j++){
            dispay[i + 1][j + 35] = cartasV[i][j];
            dispay[i + 10][j + 1] = cartasV[i][j];
            dispay[i + 10][j + 70] = cartasV[i][j];
        }
    }
    for (i = 0; i < 8; i++){
        for (j = 0; j < 17; j++){
            dispay[i + 21][j + 35] = cartas[i][j];
        }
    }

    for (i = 0; i < 30; i++){
        for (j = 0; j < 85; j++){
            printf("%s", dispay[i][j]);
        }
        printf("\n");
    }

    printf("♠ ♣ ♥ ♦ \n");
}