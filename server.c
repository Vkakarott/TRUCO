#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#define TAMANHO_BARALHO 27
#define TAMANHO_MAO 3
#define PONTOS_PARA_VITORIA 12
#define NUMERO_CLIENTS 1
#define PORT 8080

int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[5024] = {0};

//structs
typedef struct {
    char carta[10];
    int valor;
} Carta ;

typedef struct {
    char nome[20];
    int pontos;
    int nCartas;
    int clt_skt;
    Carta mao[TAMANHO_MAO];
} Jogador;

typedef struct {
    Carta rodada[NUMERO_CLIENTS];
    char jogadorVencedor[20];
    int maiorCarta;
} Mesa ;

typedef struct {
    char jogador1[20];
    char jogador2[20];
    int pontos;
} Dupla ;

Carta baralho[27];

Dupla dupla1, dupla2;

Jogador jogadores[NUMERO_CLIENTS];

Mesa rodadas[3];

int i, j;

void sendToClient(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
}

char* receiveFromClient(int client_socket) {
    valread = read(client_socket, buffer, sizeof(buffer));
    return strdup(buffer);
}

void gerarBaralho(){
    printf("\033[0;31mGerando baralho...\033[0;37m\n");
    
    FILE *arq;
    char linha[20];
    int i = 0;

    arq = fopen("baralho.bin", "rb");

    if (arq == NULL){
        printf("ERROR!");
        exit(0);
    }

    while (fgets(linha, sizeof(linha), arq) != NULL){
        sscanf(linha, "%s %d", baralho[i].carta, &baralho[i].valor);
        i++;
    }

    fclose(arq);
}

void definirDuplas(){
    printf("\033[0;31mDefinindo duplas...\033[0;37m\n");   
    strcpy(dupla1.jogador1, jogadores[0].nome);
    strcpy(dupla1.jogador2, jogadores[2].nome);
    strcpy(dupla2.jogador1, jogadores[1].nome);
    strcpy(dupla2.jogador2, jogadores[3].nome);
    printf("Dupla 1: %s e %s\n", dupla1.jogador1, dupla1.jogador2);
    printf("Dupla 2: %s e %s\n", dupla2.jogador1, dupla2.jogador2);
    sendToClient(jogadores[0].clt_skt, "0 Você faz parte da dupla 1\n");
    sendToClient(jogadores[1].clt_skt, "0 Você faz parte da dupla 2\n");
    sendToClient(jogadores[2].clt_skt, "0 Você faz parte da dupla 1\n");
    sendToClient(jogadores[3].clt_skt, "0 Você faz parte da dupla 2\n");
    dupla1.pontos = 0;
    dupla2.pontos = 0;
}

void embaralhar(){
    printf("\033[0;31mEmbaralhando...\033[0;37m\n");
    
    srand(time(NULL));
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

void distribuir(int jogadorAtual){
    printf("\033[0;31mDistribuindo...\033[0;37m\n");
    
    if (jogadorAtual == NUMERO_CLIENTS) return;
    for (int i = 0; i < TAMANHO_MAO; i++){
        jogadores[jogadorAtual].mao[i] = baralho[j];
        j++;
    }
    jogadores[jogadorAtual].nCartas = TAMANHO_MAO;
    distribuir(jogadorAtual+1);
}

void reorganizarMao(Jogador jogador, int indiceSelecionado) {
    printf("\033[0;31mReorganizando mão...\033[0;37m\n");
    
    for (int i = indiceSelecionado; i < jogador.nCartas; i++) {
        jogador.mao[i] = jogador.mao[i + 1];
    }

    strcpy(jogador.mao[jogador.nCartas - 1].carta, "");
    jogador.mao[jogador.nCartas - 1].valor = 0;
    jogador.nCartas--;
}

void truco(int client_socket,int rodadaAtual, int *pontosMesa, int valorTruco){
    printf("\033[0;31mTruco...\033[0;37m\n");
    int adversario;
    int player_choice_int;
    
    char message[500];
    char player_choice[5];
    switch (valorTruco){
    case 1:
        printf("TRUCO!\nO jogador %s pediu truco!\n1 - Aceitar\n2 - Pedir seis\n3 - Correr\n", jogadores[client_socket].nome);
        adversario = (client_socket < NUMERO_CLIENTS - 1) ? client_socket+1 : client_socket - 3;
        strcpy(message, "0 TRUCO!");
        for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
        strcpy(message, "0 O jogador ");
        strcat(message, jogadores[client_socket].nome);
        strcat(message, " pediu truco!");
        for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
        strcpy(message, "1 1 - Aceitar | 2 - Pedir seis | 3 - Correr ");
        sendToClient(jogadores[adversario].clt_skt, message);
        strcpy(player_choice, receiveFromClient(jogadores[adversario].clt_skt));
        player_choice_int = player_choice[0] - '0';
        if (player_choice_int == 1){
            printf("O jogador %s aceitou o truco!\n", jogadores[adversario].nome);
            strcpy(message, "1 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " aceitou o truco!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            *pontosMesa = 3;
            jogada(client_socket, rodadaAtual, &pontosMesa);
        } else if (player_choice_int == 2){
            printf("O jogador %s pediu seis!\n", jogadores[adversario].nome);
            strcpy(message, "0 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " pediu seis!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            truco(adversario, rodadaAtual, pontosMesa, 3);
        } else {
            printf("O jogador %s correu!\n", jogadores[adversario].nome);
            strcpy(message, "0 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " correu!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            if (client_socket == 0 || client_socket == 2) {
                dupla1.pontos += 3;
                partida(rand() % 4);
            } else {
                dupla2.pontos += 3;
                partida(rand() % 4);
            }
        }
        break;
    case 3:
        printf("SEIS!\nO jogador %s pediu seis!\n1 - Aceitar\n2 - Pedir seis\n3 - Correr\n", jogadores[client_socket].nome);
        adversario = (client_socket > 0) ? client_socket-1 : client_socket+3;
        strcpy(message, "0 SEIS!");
        for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
        strcpy(message, "0 O jogador ");
        strcat(message, jogadores[client_socket].nome);
        strcat(message, " pediu seis!");
        for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
        strcpy(message, "1 1 - Aceitar | 2 - Pedir nove | 3 - Correr");
        sendToClient(jogadores[adversario].clt_skt, message);
        strcpy(player_choice, receiveFromClient(jogadores[adversario].clt_skt));
        player_choice_int = player_choice[0] - '0';
        if (player_choice_int == 1){
            printf("O jogador %s aceitou o seis!\n", jogadores[adversario].nome);
            strcpy(message, "1 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " aceitou o seis!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            *pontosMesa = 6;
            jogada(client_socket, rodadaAtual, *pontosMesa);
        } else if (player_choice_int == 2){
            printf("O jogador %s pediu nove!\n", jogadores[adversario].nome);
            strcpy(message, "0 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " pediu nove!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            truco(adversario, rodadaAtual, pontosMesa, 6);
        } else {
            printf("O jogador %s correu!\n", jogadores[adversario].nome);
            strcpy(message, "0 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " correu!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            if (client_socket == 0 || client_socket == 2) {
                dupla1.pontos += 6;
                partida(rand() % 4);
            } else {
                dupla2.pontos += 6;
                partida(rand() % 4);
            }
        }
        break;
    case 6:
        printf("NOVE!\nO jogador %s pediu nove!\n1 - Aceitar\n2 - Pedir doze\n3 - Correr\n", jogadores[client_socket].nome);
        adversario = (client_socket < NUMERO_CLIENTS - 1) ? client_socket+1 : client_socket - 3;
        strcpy(message, "0 NOVE!");
        for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
        strcpy(message, "0 O jogador ");
        strcat(message, jogadores[client_socket].nome);
        strcat(message, " pediu nove!");
        for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
        strcpy(message, "1 1 - Aceitar | 2 - Pedir doze | 3 - Correr ");
        sendToClient(jogadores[adversario].clt_skt, message);
        strcpy(player_choice, receiveFromClient(jogadores[adversario].clt_skt));
        player_choice_int = player_choice[0] - '0';
        if (player_choice_int == 1){
            printf("O jogador %s aceitou o nove!\n", jogadores[adversario].nome);
            strcpy(message, "1 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " aceitou o nove!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            *pontosMesa = 9;
            jogada(client_socket, rodadaAtual, *pontosMesa);
        } else if (player_choice_int == 2){
            printf("O jogador %s pediu nove!\n", jogadores[adversario].nome);
            strcpy(message, "0 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " pediu nove!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            truco(adversario, rodadaAtual, pontosMesa, 9);
        } else {
            printf("O jogador %s correu!\n", jogadores[adversario].nome);
            strcpy(message, "0 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " correu!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            if (client_socket == 0 || client_socket == 2) {
                dupla1.pontos += 6;
                partida(rand() % 4);
            } else {
                dupla2.pontos += 6;
                partida(rand() % 4);
            }
        }
        break;
    case 9:
        printf("DOZE!\nO jogador %s pediu doze!\n1 - Aceitar\n2 - Correr\n", jogadores[client_socket].nome);
        adversario = (client_socket > 0) ? client_socket-1 : client_socket+3;
        strcpy(message, "0 DOZE!");
        for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
        strcpy(message, "0 O jogador ");
        strcat(message, jogadores[client_socket].nome);
        strcat(message, " pediu doze!");
        for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
        strcpy(message, "1 1 - Aceitar | 2 - Correr");
        sendToClient(jogadores[adversario].clt_skt, message);
        strcpy(player_choice, receiveFromClient(jogadores[adversario].clt_skt));
        player_choice_int = player_choice[0] - '0';
        if (player_choice_int == 1){
            printf("O jogador %s aceitou o doze!\n", jogadores[adversario].nome);
            strcpy(message, "1 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " aceitou o doze!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            *pontosMesa = 12;
            jogada(client_socket, rodadaAtual, *pontosMesa);
        } else {
            printf("O jogador %s correu!\n", jogadores[adversario].nome);
            strcpy(message, "0 O jogador ");
            strcat(message, jogadores[adversario].nome);
            strcat(message, " correu!");
            for (i = 0; i < NUMERO_CLIENTS; i++) sendToClient(jogadores[i].clt_skt, message);
            if (client_socket == 0 || client_socket == 2) {
                dupla1.pontos += 12;
                partida(0);
            } else {
                dupla2.pontos += 12;
                partida(0);
            }
        }
    default:
        break;
    }
}

void jogada(int client_socket, int rodadaAtual, int pontosMesa) {
    printf("\033[0;31mJogada...\033[0;37m\n");
    printf("Vez de %s\n", jogadores[client_socket].nome);

    char message[500] = "0 Sua vez de jogar! \n";
    sendToClient(jogadores[client_socket].clt_skt, message);

    strcpy(message, "1 Suas cartas: ");
    for (i = 0; i < TAMANHO_MAO; i++){
        strcat(message, jogadores[client_socket].mao[i].carta);
        strcat(message, " ");
    }
    sendToClient(jogadores[client_socket].clt_skt, message);
    printf("Aguardando jogada de %s...\n", jogadores[client_socket].nome);
    char *player_choice = receiveFromClient(jogadores[client_socket].clt_skt);
    int player_choice_int = player_choice[0] - '0';
    if(player_choice_int == 0) truco(client_socket, rodadaAtual, &pontosMesa, 1);
    else rodadas[rodadaAtual].rodada[client_socket] = jogadores[client_socket].mao[player_choice_int-1];
    printf(" - %d - ", player_choice_int);
    free(player_choice);
}

void verificarVencedor(Mesa mesa, int pontosMesa){
    printf("\033[0;31mVerificando vencedor...\033[0;37m\n");
    
    int maiorCarta = 0;
    int jogadorVencedor = 0;
    for (i = 0; i < NUMERO_CLIENTS; i++){
        if (mesa.rodada[i].valor > maiorCarta){
            maiorCarta = mesa.rodada[i].valor;
            jogadorVencedor = i;
        }
    }
    mesa.maiorCarta = maiorCarta;
    strcpy(mesa.jogadorVencedor, jogadores[jogadorVencedor].nome);
    printf("%s venceu a rodada com %s\n", mesa.jogadorVencedor, mesa.rodada[jogadorVencedor].carta);

    char message[500];
    strcpy(message, "0 ");
    strcat(message, "O jogador ");
    strcat(message, mesa.jogadorVencedor);
    strcat(message, " venceu a rodada com ");
    strcat(message, mesa.rodada[jogadorVencedor].carta);
    for (i = 0; i < NUMERO_CLIENTS; i++){
        sendToClient(jogadores[i].clt_skt, message);
    }
    jogadores[jogadorVencedor].pontos += 1;
    for (i = 0; i < NUMERO_CLIENTS; i++){
        strcpy(mesa.rodada[i].carta, "");
        mesa.rodada[i].valor = 0;
    }
    int D1 = jogadores[0].pontos + jogadores[2].pontos;
    int D2 = jogadores[1].pontos + jogadores[3].pontos;
    if (D1 == 2){
        dupla1.pontos += pontosMesa;

        strcpy(message, "0 Dupla 1 venceu a rodada!");
        for (i = 0; i < NUMERO_CLIENTS; i++){
            sendToClient(jogadores[i].clt_skt, message);
        }
        strcpy(message, "+");
        strcat(message, pontosMesa);
        strcat(message, " Pontos");
        for (i = 0; i < NUMERO_CLIENTS; i+2){
            sendToClient(jogadores[i].clt_skt, message);
        }

        printf("Dupla 1 venceu a rodada!\n");
        partida(jogadorVencedor);
    } else if (D2 == 2){
        dupla2.pontos += pontosMesa;

        strcpy(message, "0 Dupla 2 venceu a rodada!");
        for (i = 0; i < NUMERO_CLIENTS; i++){
            sendToClient(jogadores[i].clt_skt, message);
        }
        strcpy(message, "+");
        strcat(message, pontosMesa);
        strcat(message, " Pontos");
        for (i = 1; i < NUMERO_CLIENTS; i+2){
            sendToClient(jogadores[i].clt_skt, message);
        }

        printf("Dupla 2 venceu a rodada!\n");
        partida(jogadorVencedor);
    }
}

void rodada(int jogadas, int rodadaAtual, int jogadorAtual, int pontosMesa){
    printf("\033[0;31mRodada...\033[0;37m\n");
    
    if (jogadas == NUMERO_CLIENTS){
        verificarVencedor(rodadas[rodadaAtual], pontosMesa);
        rodadaAtual++;
    }
    printf("Rodada %d\n", jogadas+1);
    jogada(jogadorAtual, rodadaAtual, pontosMesa);
    printf("Mesa: ");
    char message[500];
    strcpy(message, "0 Mesa: ");
    for (i = 0; i < jogadas; i++){
        printf("%s ", rodadas[rodadaAtual].rodada[i].carta);
        strcat(message, rodadas[rodadaAtual].rodada[i].carta);
        strcat(message, " ");
    }
    sendToClient(jogadores[jogadorAtual].clt_skt, message);
    printf("\n");

    rodada(jogadas+1, rodadaAtual, (jogadorAtual < NUMERO_CLIENTS -1) ? jogadorAtual+1 : jogadorAtual - 3, pontosMesa);
}

// Funcao que inicia a partida
void partida(int ultimoJogador){
    printf("\033[0;31mPartida...\033[0;37m\n");
    
    if (dupla1.pontos >= PONTOS_PARA_VITORIA || dupla2.pontos >= PONTOS_PARA_VITORIA){
        printf("Fim de jogo!\n");
        if (dupla1.pontos >= PONTOS_PARA_VITORIA){
            // Um for() economizaria linhas mais nao o suficiente para valer a pena
            printf("Dupla 1 venceu!\n");
            sendToClient(jogadores[0].clt_skt, "2 Voces venceram a partida!\n");
            sendToClient(jogadores[1].clt_skt, "2 Voces perderam a partida!\n");
            sendToClient(jogadores[2].clt_skt, "2 Voces venceram a partida!\n");
            sendToClient(jogadores[3].clt_skt, "2 Voces perderam a partida!\n");
        } else {
            sendToClient(jogadores[0].clt_skt, "2 Voces perderam a partida!\n");
            sendToClient(jogadores[1].clt_skt, "2 Voces venceram a partida!\n");
            sendToClient(jogadores[2].clt_skt, "2 Voces perderam a partida!\n");
            sendToClient(jogadores[3].clt_skt, "2 Voces venceram a partida!\n");
        }
        exit(0);
    }
    int pontosMesa = 0; // Pontos que define se é truco ou nao
    embaralhar();
    distribuir(0);
    // Printa as cartas de cada jogador no servidor e envia para os clientes
    for (i = 0; i < NUMERO_CLIENTS; i++){
        char message[500];
        strcpy(message, "0 Suas cartas: ");

        printf("Jogador %d: %s\n", i+1, jogadores[i].nome);

        for (j = 0; j < TAMANHO_MAO; j++){
            printf("%s ", jogadores[i].mao[j].carta);
            strcat(message, jogadores[i].mao[j].carta);
            strcat(message, " ");
        }
        sendToClient(jogadores[i].clt_skt, message);
        printf("\n");
    }
    rodada(0, 0, ultimoJogador, pontosMesa);
}

int main() {
    
    // Criar socket do servidor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar socket do servidor");
        exit(EXIT_FAILURE);
    }
    
    // Configurar opções do socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Falha ao configurar opções do socket");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Vincular o socket à porta e ao endereço
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address))<0) {
        perror("Falha ao vincular o socket");
        exit(EXIT_FAILURE);
    }

    system("hostname -I");
    
    // Esperar por conexões
    if (listen(server_fd, 3) < 0) {
        perror("Erro ao esperar por conexões");
        exit(EXIT_FAILURE);
    }
    
    printf("Servidor esperando por conexões...\n\n");
    
    // Aceitar até 4 clientes
    int client_sockets[NUMERO_CLIENTS];
    for (int i = 0; i < NUMERO_CLIENTS; ++i) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen))<0) {
            perror("Erro ao aceitar conexão");
            exit(EXIT_FAILURE);
        }
        
        client_sockets[i] = new_socket;
        printf("Cliente %d conectado\n", i+1);
        jogadores[i].clt_skt = new_socket;
    }

    system("clear");

    // Receber dados dos clientes e imprimir na tela
    for (int i = 0; i < NUMERO_CLIENTS; ++i) {
        valread = read(client_sockets[i], buffer, sizeof(buffer));
        printf("Cliente %d: %s\n", i+1, buffer);
        strcpy(jogadores[i].nome, buffer);
    }
    printf("\n");

    gerarBaralho();
    definirDuplas(jogadores);
    partida(0);

    return 0;
}
