#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *display[30][65];

char *cartasNaMesa[4][4] = {
        {"╭", "─", "─", "╮"},
        {"│", " ", " ", "│"},
        {"│", " ", " ", "│"},
        {"╰", "─", "─", "╯"}
    };

char *cartasDoAdversario[7][10] = {
        {"╭", "─", "─", "─", "─", "─", "─", "─", "─", "╮" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│" },
        {"│", " ", " ", " ", " ", " ", " ", " ", " ", "│" },
        {"╰", "─", "─", "─", "─", "─", "─", "─", "─", "╯" }
    };

char *demaisCartas[7][2] = {
        { "─", "╮" },
        { " ", "│" },
        { " ", "│" },
        { " ", "│" },
        { " ", "│" },
        { " ", "│" },
        { "─", "╯" }
    };

char *cartas[8][11] = {
        {"╭", "─", "─", "─", "─", "─", "─", "─", "─", "─", "╮" },
        {"│", " ", "╭", "─", "─", "─", "─", "─", "╮", " ", "│" },
        {"│", " ", "│", " ", " ", " ", " ", " ", "│", " ", "│" },
        {"│", " ", "│", " ", " ", " ", " ", " ", "│", " ", "│" },
        {"│", " ", "│", " ", " ", " ", " ", " ", "│", " ", "│" },
        {"│", " ", "│", " ", " ", " ", " ", " ", "│", " ", "│" },
        {"│", " ", "╰", "─", "─", "─", "─", "─", "╯", " ", "│" },
        {"╰", "─", "─", "─", "─", "─", "─", "─", "─", "─", "╯" }
    };

char *dCartas[8][3] = {
        { "─", "─", "╮" },
        { " ", " ", "│" },
        { " ", " ", "│" },
        { " ", " ", "│" },
        { " ", " ", "│" },
        { " ", " ", "│" },
        { " ", " ", "│" },
        { "─", "─", "╯" }
    };

char *menu[7][15] = {
    {"┏", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "┓"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┃", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", "┃"},
    {"┗", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "━", "┛"}
};

void printDisplay(){
    int i, j;
    system("clear");

    for (i = 0; i < 30; i++){
        for (j = 0; j < 65; j++){
            printf("%s", display[i][j]);
        }
        printf("\n");
    }
    getchar();
}

int main(){
    int i, j;

    // iniciar display
    for(i = 0; i < 30; i++){
        for(j = 0; j < 65; j++){
            display[i][j] = "█";
        }
    }

   //tela de inicio
    for(i = 0; i < 7; i++){
        for(j = 0; j < 15; j++){
            display[i+22][j+49] = menu[i][j];
        }
    }
    printDisplay();

    //distribuir cartas
    for(i = 0; i < 7; i++){
        for(j = 0; j < 10; j++){
            display[i + 1][j + 23] = cartasDoAdversario[i][j];
        }
    }
    printDisplay();
    for(i = 0; i < 7; i++){
        for(j = 0; j < 2; j++){
            display[i + 1][j + 33] = demaisCartas[i][j];
        }
    }
    printDisplay();
    for(i = 0; i < 7; i++){
        for(j = 0; j < 2; j++){
            display[i + 1][j + 35] = demaisCartas[i][j];
        }
    }
    printDisplay();
    for(i = 0; i < 7; i++){
        for(j = 0; j < 10; j++){
            display[i + 10][j + 1] = cartasDoAdversario[i][j];
        }
    }
    printDisplay();
    for(i = 0; i < 7; i++){
        for(j = 0; j < 2; j++){
            display[i + 10][j + 11] = demaisCartas[i][j];
        }
    }
    printDisplay();
    for(i = 0; i < 7; i++){
        for(j = 0; j < 2; j++){
            display[i + 10][j + 13] = demaisCartas[i][j];
        }
    }
    printDisplay();
    for(i = 0; i < 7; i++){
        for(j = 0; j < 10; j++){
            display[i + 11][j + 47] = cartasDoAdversario[i][j];
        }
    }
    printDisplay();
    for(i = 0; i < 7; i++){
        for(j = 0; j < 2; j++){
            display[i + 11][j + 57] = demaisCartas[i][j];
        }
    }
    printDisplay();
    for(i = 0; i < 7; i++){
        for(j = 0; j < 2; j++){
            display[i + 11][j + 59] = demaisCartas[i][j];
        }
    }
    printDisplay();
    for (i = 0; i < 8; i++){
        for (j = 0; j < 11; j++){
            display[i + 20][j + 22] = cartas[i][j];
        }
    }
    printDisplay();
    for (i = 0; i < 8; i++){
        for (j = 0; j < 3; j++){
            display[i + 20][j + 33] = dCartas[i][j];
        }
    }
    printDisplay();
    for (i = 0; i < 8; i++){
        for (j = 0; j < 3; j++){
            display[i + 20][j + 36] = dCartas[i][j];
        }
    }


    printDisplay();
}