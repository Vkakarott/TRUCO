#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define HOST_IP "127.0.0.1"
#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int client_socket;
struct sockaddr_in server_addr;
char buffer[MAX_BUFFER_SIZE] = {0};

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

char* receiveFromServer() {
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    return strdup(buffer);
}

void sendToServer(const char *message) {
    send(client_socket, message, strlen(message), 0);
}

int main() {
    

    // Criação do socket do cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
        error("Erro ao criar o socket do cliente");

    // Configuração do endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Conexão ao servidor
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        error("Erro ao conectar ao servidor");

    printf("Conectado ao servidor\n");

    // Receber o nome do jogador do servidor
    char player_name[MAX_BUFFER_SIZE];
    printf("Digite seu nome: ");
    fgets(player_name, MAX_BUFFER_SIZE, stdin);
    send(client_socket, player_name, strlen(player_name), 0);
    printf("Nome enviado para o servidor.\n");

    printf("Bem-vindo, %s!\n", player_name);

    // Loop principal para a lógica do jogo
    while (1) {
        // Receive message from the server
        char *messageFromServer = receiveFromServer();
        printf("Received message from server: %s\n", messageFromServer);

        // Get user input for the response
        char response[MAX_BUFFER_SIZE];
        printf("Digite sua resposta: ");
        fgets(response, MAX_BUFFER_SIZE, stdin);
        sendToServer(response);

        // Free the allocated memory for the received message
        free(messageFromServer);
    }

    // Fechar o socket do cliente
    close(client_socket);

    return 0;
}