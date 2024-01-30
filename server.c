#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_PLAYERS 4

typedef struct {
    char carta[2];
    char nipe[5];
    int valor;
} Carta;

typedef struct {
    char nome[20];
    int pontos;
    int nCartas;
    Carta mao[3];
    int socket;
} Jogador;

typedef struct {
    Carta rodada[MAX_PLAYERS];
    char jogadorVencedor[20];
    int maiorCarta;
} Mesa;

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void broadcast(Jogador *jogadores, int num_players, void *data, size_t size) {
    for (int i = 0; i < num_players; i++) {
        if (send(jogadores[i].socket, data, size, 0) == -1) {
            perror("Erro ao enviar dados para um jogador");
            // Lida com o erro, como desconectar o jogador ou encerrar o jogo
        }
    }
}

int main() {
    int server_socket, client_sockets[MAX_PLAYERS], client_count = 0;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    // Criação do socket do servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        error("Erro ao criar o socket do servidor");

    // Configuração do endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Associação do socket ao endereço
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        error("Erro ao associar o socket ao endereço");

    // Aguarda conexões
    if (listen(server_socket, MAX_PLAYERS) == -1)
        error("Erro ao aguardar conexões");

    printf("Aguardando jogadores...\n");

    Jogador jogadores[MAX_PLAYERS];

    // Aceita conexões dos jogadores
    while (client_count < MAX_PLAYERS) {
        client_sockets[client_count] = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sockets[client_count] == -1)
            error("Erro ao aceitar conexão");

        printf("Jogador %d conectado\n", client_count + 1);

        // Configuração do jogador
        jogadores[client_count].socket = client_sockets[client_count];
        snprintf(jogadores[client_count].nome, sizeof(jogadores[client_count].nome), "Jogador %d", client_count + 1);
        jogadores[client_count].pontos = 0;
        jogadores[client_count].nCartas = 0;

        // Incremento do contador de jogadores
        client_count++;
    }

    // Loop principal do jogo
    while (1) {
        // Implemente a lógica do jogo aqui

        // Exemplo: Enviar uma mensagem de broadcast para todos os jogadores
        char mensagem[] = "É a sua vez!";
        broadcast(jogadores, MAX_PLAYERS, mensagem, sizeof(mensagem));
    }

    // Fechar o socket do servidor
    close(server_socket);

    return 0;
}