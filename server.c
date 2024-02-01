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
char buffer[1024] = {0};

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
    sleep(2);
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

void definirDuplas(Jogador *jogadores){
    printf("\033[0;31mDefinindo duplas...\033[0;37m\n");
    sleep(2);
    strcpy(dupla1.jogador1, jogadores[0].nome);
    strcpy(dupla1.jogador2, jogadores[2].nome);
    strcpy(dupla2.jogador1, jogadores[1].nome);
    strcpy(dupla2.jogador2, jogadores[3].nome);
    printf("Dupla 1: %s e %s\n", dupla1.jogador1, dupla1.jogador2);
    printf("Dupla 2: %s e %s\n", dupla2.jogador1, dupla2.jogador2);
    dupla1.pontos = 0;
    dupla2.pontos = 0;
}

void embaralhar(){
    printf("\033[0;31mEmbaralhando...\033[0;37m\n");
    sleep(2);
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

void distribuir(Jogador *jogadores, int jogadorAtual){
    printf("\033[0;31mDistribuindo...\033[0;37m\n");
    sleep(2);
    if (jogadorAtual == NUMERO_CLIENTS) return;
    for (int i = 0; i < TAMANHO_MAO; i++){
        jogadores[jogadorAtual].mao[i] = baralho[j];
        j++;
    }
    jogadores[jogadorAtual].nCartas = TAMANHO_MAO;
    distribuir(jogadores, jogadorAtual+1);
}

void reorganizarMao(Jogador jogador, int indiceSelecionado, int jogadas) {
    printf("\033[0;31mReorganizando mão...\033[0;37m\n");
    sleep(2);
    for (int i = indiceSelecionado; i < jogador.nCartas; i++) {
        jogador.mao[i] = jogador.mao[i + 1];
    }

    strcpy(jogador.mao[jogador.nCartas - 1].carta, "");
    jogador.mao[jogador.nCartas - 1].valor = 0;
    jogador.nCartas--;
}

void jogada(Jogador *jogadores, int client_socket) {
    printf("\033[0;31mJogada...\033[0;37m\n");
    sleep(2);

    printf("Vez de %s\n", jogadores[client_socket].nome);
    const char *message = "Sua vez de jogar!";
    sendToClient(jogadores[client_socket].clt_skt, message);
    printf("Aguardando jogada de %s...\n", jogadores[client_socket].nome);
    char *player_choice = receiveFromClient(jogadores[client_socket].clt_skt);
    printf("%s jogou %s\n", jogadores[client_socket].nome, player_choice);
    free(player_choice);
}

void verificarVencedor(Mesa mesa, Jogador *jogadores, int pontosMesa){
    printf("\033[0;31mVerificando vencedor...\033[0;37m\n");
    sleep(2);
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
    jogadores[jogadorVencedor].pontos += 1;
    for (i = 0; i < NUMERO_CLIENTS; i++){
        strcpy(mesa.rodada[i].carta, "");
        mesa.rodada->valor = 0;
    }
    int D1 = jogadores[0].pontos + jogadores[2].pontos;
    int D2 = jogadores[1].pontos + jogadores[3].pontos;
    if (D1 == 2){
        dupla1.pontos += pontosMesa;
        printf("Dupla 1 venceu a rodada!\n");
        partida(jogadorVencedor);
    } else if (D2 == 2){
        dupla2.pontos += pontosMesa;
        printf("Dupla 2 venceu a rodada!\n");
        partida(jogadorVencedor);
    }
}

void rodada(int jogadas, Mesa *rodadas, int rodadaAtual, int jogadorAtual, int pontosMesa){
    printf("\033[0;31mRodada...\033[0;37m\n");
    sleep(2);
    if (jogadas == NUMERO_CLIENTS){
        verificarVencedor(rodadas[rodadaAtual], jogadores, pontosMesa);
    }
    printf("Rodada %d\n", jogadas+1);
    jogada(jogadores, jogadorAtual);
    printf("Mesa: ");
    for (i = 0; i < jogadas; i++){
        printf("%s ", rodadas[rodadaAtual].rodada[i].carta);
    }
    printf("\n");

    rodada(jogadas+1, rodadas, rodadaAtual, (jogadorAtual < NUMERO_CLIENTS -1) ? jogadorAtual+1 : jogadorAtual - 3, pontosMesa);
}

void partida(ultimoJogador){
    printf("\033[0;31mPartida...\033[0;37m\n");
    sleep(2);
    if (dupla1.pontos >= PONTOS_PARA_VITORIA || dupla2.pontos >= PONTOS_PARA_VITORIA){
        printf("Fim de jogo!\n");
        if (dupla1.pontos >= PONTOS_PARA_VITORIA){
            printf("Dupla 1 venceu!\n");
        } else {
            printf("Dupla 2 venceu!\n");
        }
        exit(0);
    }
    int pontosMesa = 0;
    embaralhar();
    distribuir(jogadores, 0);
    for (i = 0; i < NUMERO_CLIENTS; i++){
        printf("Jogador %d: %s\n", i+1, jogadores[i].nome);
        for (j = 0; j < TAMANHO_MAO; j++){
            printf("%s ", jogadores[i].mao[j].carta);
        }
        printf("\n");
    }
    rodada(0, &rodadas[0], 0, ultimoJogador, pontosMesa);
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
    
    // Esperar por conexões
    if (listen(server_fd, 3) < 0) {
        perror("Erro ao esperar por conexões");
        exit(EXIT_FAILURE);
    }
    
    printf("Servidor esperando por conexões...\n");
    
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

    // Receber dados dos clientes e imprimir na tela
    for (int i = 0; i < NUMERO_CLIENTS; ++i) {
        valread = read(client_sockets[i], buffer, sizeof(buffer));
        printf("Cliente %d: %s\n", i+1, buffer);
        strcpy(jogadores[i].nome, buffer);
    }

    gerarBaralho();
    definirDuplas(jogadores);
    partida(0);

    return 0;
}
