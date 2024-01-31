#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

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

    // Implemente a lógica do jogo do cliente aqui

    // Exemplo: Receber uma mensagem do servidor
    char buffer[MAX_BUFFER_SIZE];
    if (recv(client_socket, buffer, sizeof(buffer), 0) == -1)
        error("Erro ao receber dados do servidor");

    printf("Mensagem do servidor: %s\n", buffer);

    // Fechar o socket do cliente
    close(client_socket);

    return 0;
}