#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define HOST_IP "127.0.0.1"
#define PORT 8080
#define MAX_BUFFER_SIZE 5024

int client_socket;
struct sockaddr_in server_addr;

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Funcao para receber mensagem do servidor
char* receiveFromServer() {
    char buffer[MAX_BUFFER_SIZE] = {0};
    read(client_socket, buffer, MAX_BUFFER_SIZE);
    return strdup(buffer);
}

// Funcao para enviar mensagem para o servidor
void sendToServer(const char *message) {
    send(client_socket, message, strlen(message), 0);
}

int main() {
    // Obter o IP do servidor
    printf("Digite o IP do servidor: ");
    char SERVER_IP[20];
    scanf("%s", SERVER_IP);
    if(strcmp(SERVER_IP, "localhost") == 0)
        strcpy(SERVER_IP, HOST_IP);
    

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
    scanf("%s", player_name);
    send(client_socket, player_name, strlen(player_name), 0);
    printf("Nome enviado para o servidor.\n");
    system("clear");
    printf("Bem-vindo, %s!\n", player_name);
    printf("\033[3;90mAguarde o início do jogo...\n");
    sleep(2);
    
    int userResponse;
    int index = 0;
    // Loop principal para a lógica do jogo
    while (1) {
        char mensage[MAX_BUFFER_SIZE];
        char server_message[MAX_BUFFER_SIZE];
        char response[MAX_BUFFER_SIZE];
        strcpy(mensage, receiveFromServer());
        sscanf(mensage, "%d %[^\n]", &index, server_message);
        printf("%s\n", server_message);
        if (index == 1) {
            scanf("%d", &userResponse);
            snprintf(response, sizeof(response), "%d", userResponse);
            sendToServer(response);
        } else if (index == 2) {
            printf("Obrigado por jogar!\n");
        }
    }
    close(client_socket);

    return 0;
}
