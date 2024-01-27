#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <wchar.h>
#include <locale.h>

// Definindo as cartas do baralho
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

// Tamanho da mão de cada jogador por rodada
#define TAMANHO_MAO 3

// Número de pontos para vencer o jogo
#define PONTOS_PARA_VITORIA 12

// Função para limpar a tela
void limparTela() {
    system("clear || cls");
}

// Função para embaralhar o baralho
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

// Função para distribuir cartas para um jogador
void distribuirCartas(char **mao, int indiceInicial) {
    for (int i = 0; i < TAMANHO_MAO; i++) {
        if (indiceInicial + i < TAMANHO_BARALHO) {
            mao[i] = baralho[indiceInicial + i];
        } else {
            mao[i] = NULL;  // Carta Inválida
        }
    }
}

// Função para mostrar a mão de um jogador
void mostrarMao(char **mao, const char *nomeJogador, int vezDoJogador) {
    wprintf(L"%s: ", nomeJogador);
    if (vezDoJogador) {
        for (int i = 0; i < TAMANHO_MAO; i++) {
            if (mao[i] != NULL) {
                wprintf(L"%s ", mao[i]);
            } else {
                wprintf(L"Carta Inválida ");
            }
        }
    } else {
        wprintf(L"?? ?? ?? ");  // Mão oculta para outros jogadores
    }
    wprintf(L"\n");
}

// Função para determinar o vencedor de uma rodada
int determinarVencedor(char **cartasJogadas) {
    int ordemForca[] = {0, 1, 2, 3};  // Ordem de força das cartas (simplificado)
    int vencedor = -1;

    for (int i = 0; i < 4; i++) {
        if (cartasJogadas[ordemForca[i]] != NULL) {
            vencedor = ordemForca[i];
            break;
        }
    }

    return vencedor;
}

int main() {
    // Definindo a localidade para suportar caracteres especiais
    setlocale(LC_ALL, "");

    char *maoJogador1[3], *maoJogador2[3], *maoJogador3[3], *maoJogador4[3];
    char *cartasJogadas[4];

    int pontosDupla1 = 0, pontosDupla2 = 0;
    int rodadasJogadas = 0;

    // Loop principal do jogo
    while (pontosDupla1 < PONTOS_PARA_VITORIA && pontosDupla2 < PONTOS_PARA_VITORIA) {
        limparTela();

        // Embaralhar o baralho
        embaralharBaralho();

        // Distribuir cartas para os jogadores
        distribuirCartas(maoJogador1, 0);
        distribuirCartas(maoJogador2, TAMANHO_MAO);
        distribuirCartas(maoJogador3, 2 * TAMANHO_MAO);
        distribuirCartas(maoJogador4, 3 * TAMANHO_MAO);

        // Loop de rodadas
        while (rodadasJogadas < TAMANHO_MAO) {
            int vezDoJogador = 1;
            // Loop de jogadas dos jogadores
            for (int i = 0; i < 4; i++) {
                limparTela();

                // Mostrar as mãos dos jogadores de acordo com a vez
                mostrarMao(maoJogador1, "Jogador 1", vezDoJogador == 1);
                mostrarMao(maoJogador2, "Jogador 2", vezDoJogador == 2);
                mostrarMao(maoJogador3, "Jogador 3", vezDoJogador == 3);
                mostrarMao(maoJogador4, "Jogador 4", vezDoJogador == 4);

                // Lógica de escolha e jogada da carta para cada jogador...
                printf("Jogador %d, pressione ENTER para escolher uma carta...\n", vezDoJogador);
                getchar();  // Aguarda o jogador pressionar ENTER
                // Simula a escolha automática da carta pelos jogadores 2, 3 e 4
                if (vezDoJogador != 1) {
                    int cartaAleatoria = rand() % TAMANHO_MAO;
                    cartasJogadas[vezDoJogador - 1] = maoJogador1[cartaAleatoria];
                }

                vezDoJogador = (vezDoJogador % 4) + 1;  // Passa para o próximo jogador
            }

            // Determine o vencedor da rodada
            int vencedorRodada = determinarVencedor(cartasJogadas);

            // Atualize os pontos da dupla vencedora
            if (vencedorRodada == 1 || vencedorRodada == 3) {
                pontosDupla1++;
            } else {
                pontosDupla2++;
            }

            rodadasJogadas++;

            // Exiba o resultado da rodada
            limparTela();
            printf("Rodada %d - Pontos:\n", rodadasJogadas);
            printf("Dupla 1: %d | Dupla 2: %d\n", pontosDupla1, pontosDupla2);

            if (pontosDupla1 == PONTOS_PARA_VITORIA || pontosDupla2 == PONTOS_PARA_VITORIA) {
                break;  // Saia do loop principal se uma dupla atingir os pontos necessários para vencer
            }
        }

        // Adicione lógica para lidar com o início de uma nova rodada, se necessário
    }

    return 0;
}

