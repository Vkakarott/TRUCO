#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define TAMANHO_BARALHO 27
#define TAMANHO_MAO 3
#define PONTOS_VITORIA 12
#define NUMERO_JOGADORES 4
#define BUFFER_SIZE 10000
#define STR_SIZE 20
#define PORT 8080
int NUMERO_BOTS = 3;

int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);

typedef struct {
    char carta[STR_SIZE];
    int valor;
} Carta ;

typedef struct {
    char name[STR_SIZE];
    int pontos;
    int num_cartas;
    int bot;
    int clt_skt;
    Carta mao[TAMANHO_MAO];
} Jogador ;

typedef struct {
    Carta rodada[NUMERO_JOGADORES];
    char jogadorVencedor[STR_SIZE];
    int maiorCarta;
} Mesa ;

typedef struct {
    char jogador1[STR_SIZE];
    char jogador2[STR_SIZE];
    int pontos;
} Dupla ;

// Globais
Jogador players[NUMERO_JOGADORES];
Carta baralho[TAMANHO_BARALHO];
Mesa rodadas[TAMANHO_MAO];
Dupla dupla1, dupla2;
int empate;
int i, j;

char name_bot[3][STR_SIZE] = {"Bot1", "Bot2", "Bot3"};

int Bot(){
    return rand() % 3;
}

// Organiza o baralho por ordem de força
int compare_cartas(const void *a, const void *b){
    const Carta *cartaA = (const Carta *)a;
    const Carta *cartaB = (const Carta *)b;

    if (cartaA->valor > cartaB->valor) {
        return 1;
    } else if (cartaA->valor < cartaB->valor) {
        return -1;
    } else {
        return 0;
    }
}

// gera baralho atraves de um arquivo binario
void gerarBaralho(){
    printf("\033[3;90mGerando baralho...\033[0;37m\n");
    
    FILE *arq;
    char linha[STR_SIZE];
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
    sleep(1.5);
    system("clear");
}

// Envia mensagem para o cliente
void sendToClient(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
    sleep(1);
}

// recebe mensagem do cliente
char* receiveFromClient(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    valread = read(client_socket, buffer, sizeof(buffer));
    return strdup(buffer);
}

// define as duplas
void definirDuplas(){
    printf("\033[3;90mDefinindo duplas...\033[0;37m\n");  

    strcpy(dupla1.jogador1, players[0].name);
    strcpy(dupla1.jogador2, players[2].name);
    strcpy(dupla2.jogador1, players[1].name);
    strcpy(dupla2.jogador2, players[3].name);
    
    printf("Dupla 1: %s e %s\n", dupla1.jogador1, dupla1.jogador2);
    
    if (!players[2].bot) 
        sendToClient(players[2].clt_skt, "0 Você faz parte da dupla 1");
    for (i = 1; i < NUMERO_JOGADORES; i+=2) 
        if (!players[i].bot) 
            sendToClient(players[i].clt_skt, "0 Você faz parte da dupla 2");

    dupla1.pontos = 0;
    dupla2.pontos = 0;

    sleep(2);
    system("clear");
}

// Embaralha as cartas
void embaralhar(){
    printf("\033[3;90mEmbaralhando...\033[0;37m\n");
    
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

    sleep(1.5);
    system("clear");
}

// Distribui as cartas
void distribuir(int jogadorAtual){
    if (jogadorAtual == NUMERO_JOGADORES) return;
    for (i = 0; i < TAMANHO_MAO; i++){
        players[jogadorAtual].mao[i] = baralho[j];
        j++;
    }
    players[jogadorAtual].num_cartas = TAMANHO_MAO;
    distribuir(jogadorAtual+1);
}

// Cria, e espera conecao com os clientes
void abrir_servidor (int num_clients){

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

    // Mostrar endereço IP
    system("hostname -I");
    
    // Esperar por conexões
    if (listen(server_fd, 3) < 0) {
        perror("Erro ao esperar por conexões");
        exit(EXIT_FAILURE);
    }
    
    printf("\033[3;90mServidor esperando por conexões...\033[0;37m\n\n");
    
    // Aceitar clientes
    int client_sockets[num_clients];
    for (int i = 0; i < num_clients; ++i) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen))<0) {
            perror("Erro ao aceitar conexão");
            exit(EXIT_FAILURE);
        }
        
        client_sockets[i] = new_socket;
        printf("Cliente %d conectado\n", i+1);
        players[i+1].clt_skt = new_socket;
        players[i+1].bot = 0;
    }

    system("clear");

    // Receber dados dos clientes e imprimir na tela
    for (int i = 0; i < num_clients; ++i) {
        char buffer[BUFFER_SIZE] = {0};
        valread = read(client_sockets[i], buffer, sizeof(buffer));
        printf("Client %d: %s\n", i+1, buffer);
        strcpy(players[i+1].name, buffer);
    }

    sleep(1.5);
    system("clear");
}

// limpa mesa antes e apos cada rodada
void zerarRodadas() {
    for (i = 0; i < TAMANHO_MAO; i++) {
        for (j = 0; j < NUMERO_JOGADORES; j++) {
            strcpy(rodadas[i].rodada[j].carta, "");
            rodadas[i].rodada[j].valor = 0;
        }

        strcpy(rodadas[i].jogadorVencedor, "");
        rodadas[i].maiorCarta = 0;
    }

    for (i = 0; i < NUMERO_JOGADORES; i++)
            players[i].pontos = 0;

}

// verifica quem venceu a rodada
void verificarVencedor(Mesa mesa, int pontosMesa, int rodadaAtual){
    printf("\033[3;90mVerificando vencedor...\033[0;37m\n");

    for(i = 0; i < NUMERO_JOGADORES; i++)
        printf("%s ", mesa.rodada[i].carta);

    printf("\n");

    sleep(1);

    int D1 = players[0].pontos + players[2].pontos;
    int D2 = players[1].pontos + players[3].pontos;
    
    int maiorCarta = 0;
    int jogadorVencedor = 0;
    for (i = 0; i < NUMERO_JOGADORES; i++){
        if (mesa.rodada[i].valor > maiorCarta){
            maiorCarta = mesa.rodada[i].valor;
            jogadorVencedor = i;
        }
    }

    char message[BUFFER_SIZE];
    strcpy(mesa.jogadorVencedor, players[jogadorVencedor].name);

    int advrs1 = (jogadorVencedor > 0) ? jogadorVencedor-1 : jogadorVencedor+3;
    int advrs2 = (jogadorVencedor < NUMERO_JOGADORES - 1) ? jogadorVencedor+1 : jogadorVencedor - 3;
    if ((mesa.rodada[advrs1].valor == mesa.rodada[jogadorVencedor].valor) ||(mesa.rodada[advrs2].valor == mesa.rodada[jogadorVencedor].valor)){
        printf("Empate!\n");
        empate = 1;

        strcpy(message, "0 Empate!");
        for (i = 1; i < NUMERO_JOGADORES; i++)
            if (!players[i].bot)
                sendToClient(players[i].clt_skt, message);

        if (rodadaAtual == 0){
            players[0].pontos ++;
            players[1].pontos ++;

            return;
        } else if (rodadaAtual == 1) {
            if (D1 > D2){
                dupla1.pontos += pontosMesa;
            } else if (D2 > D1){
                dupla2.pontos += pontosMesa;
            }
        }
    }
    if (!empate) {
        mesa.maiorCarta = maiorCarta;

        printf("%s venceu a rodada!\n", mesa.jogadorVencedor);

        strcpy(mesa.jogadorVencedor, players[jogadorVencedor].name);
        strcpy(message, "0 ");
        strcat(message, "O jogador ");
        strcat(message, mesa.jogadorVencedor);
        strcat(message, " venceu a rodada!");

        for (i = 1; i < NUMERO_JOGADORES; i++)
            if (!players[i].bot)
                sendToClient(players[i].clt_skt, message);

        players[jogadorVencedor].pontos += 1;
    
        D1 = players[0].pontos + players[2].pontos;
        D2 = players[1].pontos + players[3].pontos;
    }
    if (D1 == 2){
        dupla1.pontos += pontosMesa;

        printf("Vocês venceram a rodada!\n");
        printf("+%d Pontos\n", pontosMesa);

        strcpy(message, "0 Dupla 1 venceu a rodada!");
        for (i = 1; i < NUMERO_JOGADORES; i++)
            sendToClient(players[i].clt_skt, message);
        
        strcpy(message, "+");
        char pontosMesaStr[10];
        sprintf(pontosMesaStr, "%d", pontosMesa);
        strcat(message, pontosMesaStr);
        strcat(message, " Pontos");
        for (i = 1; i < NUMERO_JOGADORES; i += 2)
            sendToClient(players[i].clt_skt, message);

        zerarRodadas();

        partida(jogadorVencedor);

    } else if (D2 == 2){
        dupla2.pontos += pontosMesa;

        printf("A dupla 2 venceu a rodada!\n");

        strcpy(message, "0 Dupla 2 venceu a rodada!");

        for (i = 1; i < NUMERO_JOGADORES; i++)
            sendToClient(players[i].clt_skt, message);
        
        strcpy(message, "+");
        char pontosMesaStr[10];
        sprintf(pontosMesaStr, "%d", pontosMesa);
        strcat(message, pontosMesaStr);
        strcat(message, " Pontos");
        for (i = 1; i < NUMERO_JOGADORES; i += 2)
            sendToClient(players[i].clt_skt, message);

        zerarRodadas();

        partida(jogadorVencedor);
    }
}

// reorganiza a mao do jogador
void reorganizarMao(Jogador jogador, int indiceSelecionado) {
    printf("\033[3;90mReorganizando mão...\033[0;37m\n");
    system("pause");
    for (int i = indiceSelecionado; i < jogador.num_cartas; i++) {
        jogador.mao[i] = jogador.mao[i + 1];
    }

    jogador.num_cartas--;
}

void truco(int client_socket,int rodadaAtual, int *pontosMesa){
    printf("\033[3;90mTruco...\033[0;37m\n");

    int adversario;
    char message[BUFFER_SIZE];
    char player_choice[STR_SIZE];
    int player_choice_int;

    adversario = (client_socket < NUMERO_JOGADORES - 1) ? client_socket+1 : client_socket - 3;
        
    switch (*pontosMesa) {
        case 1:
            printf("TRUCO!\n");

            strcpy(message, "0 TRUCO!");
            for (i = 1; i < NUMERO_JOGADORES; i++)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, message);

            if (client_socket != 0)
                printf("O jogador %s pediu truco!\n", players[client_socket].name);

            if (adversario == 0) {
                printf("1 - Aceitar | 2 - Pedir seis | 3 - Correr\n");
                scanf("%d", &player_choice_int);
            }

            strcpy(message, "0 O jogador ");
            strcat(message, players[client_socket].name);
            strcat(message, " pediu truco!");
            for (i = 1; i < NUMERO_JOGADORES; i++)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, message);

            if (!players[adversario].bot && adversario != 0) {
                strcpy(message, "1 1 - Aceitar | 2 - Pedir seis | 3 - Correr ");
                sendToClient(players[adversario].clt_skt, message);
                strcpy(player_choice, receiveFromClient(players[adversario].clt_skt));
                player_choice_int = player_choice[0] - '0';
            } else {
                player_choice_int = (rand() % 3) + 1;
            } 

            if (player_choice_int == 1){
                *pontosMesa = 3;
                if(adversario == 0) {
                    printf("Você aceitou o truco!\n");
                } else {
                    printf("O jogador %s aceitou o truco!\n", players[adversario].name);
                    strcpy(message, "1 O jogador ");
                    strcat(message, players[adversario].name);
                    strcat(message, " aceitou o truco!");
                    for (i = 1; i < NUMERO_JOGADORES; i++) 
                        if (!players[i].bot) 
                            sendToClient(players[i].clt_skt, message);
                }
        
                jogada(client_socket, rodadaAtual, &pontosMesa);
            } else if (player_choice_int == 2){
                *pontosMesa = 3;
                if (adversario == 0)
                    printf("Você pediu seis!\n");
                else
                    printf("O jogador %s pediu seis!\n", players[adversario].name);
                    
                strcpy(message, "0 O jogador ");
                strcat(message, players[adversario].name);
                strcat(message, " pediu seis!");
                for (i = 1; i < NUMERO_JOGADORES; i++)
                    if (!players[i].bot) 
                        sendToClient(players[i].clt_skt, message);
            
                truco(adversario, rodadaAtual, &pontosMesa);
            } else {
                if (adversario == 0) {
                    printf("Você correu!\n");
                    dupla2.pontos += *pontosMesa;
                } else {
                    printf("O jogador %s correu!\n", players[adversario].name);
                    strcpy(message, "0 O jogador ");
                    strcat(message, players[adversario].name);
                    strcat(message, " correu!");

                    for (i = 1; i < NUMERO_JOGADORES; i++) 
                        if (!players[i].bot)
                            sendToClient(players[i].clt_skt, message);

                    if (adversario == 2) 
                        dupla2.pontos += *pontosMesa;
                    else
                        dupla1.pontos += *pontosMesa;
                }
                partida(rand() % 4);
            }
            break;

        case 3:
            printf("SEIS!\n");

            strcpy(message, "0 SEIS!");
            for (i = 1; i < NUMERO_JOGADORES; i++)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, message);

            if (client_socket != 0)
                printf("O jogador %s pediu seis!\n", players[client_socket].name);

            if (adversario == 0) {
                printf("1 - Aceitar | 2 - Pedir nove | 3 - Correr\n");
                scanf("%d", &player_choice_int);
            }

            strcpy(message, "0 O jogador ");
            strcat(message, players[client_socket].name);
            strcat(message, " pediu seis!");
            for (i = 1; i < NUMERO_JOGADORES; i++)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, message);

            if (!players[adversario].bot && adversario != 0) {
                strcpy(message, "1 1 - Aceitar | 2 - Pedir nove | 3 - Correr ");
                sendToClient(players[adversario].clt_skt, message);
                strcpy(player_choice, receiveFromClient(players[adversario].clt_skt));
                player_choice_int = player_choice[0] - '0';
            } else {
                player_choice_int = (rand() % 3) + 1;
            } 

            if (player_choice_int == 1){
                *pontosMesa = 6;
                if(adversario == 0) {
                    printf("Você aceitou o seis!\n");
                } else {
                    printf("O jogador %s aceitou!\n", players[adversario].name);
                    strcpy(message, "1 O jogador ");
                    strcat(message, players[adversario].name);
                    strcat(message, " aceitou!");
                    for (i = 1; i < NUMERO_JOGADORES; i++) 
                        if (!players[i].bot) 
                            sendToClient(players[i].clt_skt, message);
                }
        
                jogada(client_socket, rodadaAtual, &pontosMesa);
            } else if (player_choice_int == 2){
                *pontosMesa = 6;
                if (adversario == 0)
                    printf("Você pediu nove!\n");
                else
                    printf("O jogador %s pediu nove!\n", players[adversario].name);
                    
                strcpy(message, "0 O jogador ");
                strcat(message, players[adversario].name);
                strcat(message, " pediu nove!");
                for (i = 1; i < NUMERO_JOGADORES; i++)
                    if (!players[i].bot) 
                        sendToClient(players[i].clt_skt, message);
            
                truco(adversario, rodadaAtual, &pontosMesa);
            } else {
                if (adversario == 0) {
                    printf("Você correu!\n");
                    dupla2.pontos += *pontosMesa;
                } else {
                    printf("O jogador %s correu!\n", players[adversario].name);
                    strcpy(message, "0 O jogador ");
                    strcat(message, players[adversario].name);
                    strcat(message, " correu!");

                    for (i = 1; i < NUMERO_JOGADORES; i++) 
                        if (!players[i].bot)
                            sendToClient(players[i].clt_skt, message);

                    if (adversario == 2) 
                        dupla2.pontos += *pontosMesa;
                    else
                        dupla1.pontos += *pontosMesa;
                }
                partida(rand() % 4);
            }
            break;

        case 6:
            printf("NOVE!\n");

            strcpy(message, "0 NOVE!");
            for (i = 1; i < NUMERO_JOGADORES; i++)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, message);

            if (client_socket != 0)
                printf("O jogador %s pediu nove!\n", players[client_socket].name);

            if (adversario == 0) {
                printf("1 - Aceitar | 2 - Pedir doze | 3 - Correr\n");
                scanf("%d", &player_choice_int);
            }

            strcpy(message, "0 O jogador ");
            strcat(message, players[client_socket].name);
            strcat(message, " pediu nove!");
            for (i = 1; i < NUMERO_JOGADORES; i++)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, message);

            if (!players[adversario].bot && adversario != 0) {
                strcpy(message, "1 1 - Aceitar | 2 - Pedir doze | 3 - Correr ");
                sendToClient(players[adversario].clt_skt, message);
                strcpy(player_choice, receiveFromClient(players[adversario].clt_skt));
                player_choice_int = player_choice[0] - '0';
            } else {
                player_choice_int = (rand() % 3) + 1;
            } 

            if (player_choice_int == 1){
                *pontosMesa = 9;
                if(adversario == 0) {
                    printf("Você aceitou nove!\n");
                } else {
                    printf("O jogador %s aceitou!\n", players[adversario].name);
                    strcpy(message, "1 O jogador ");
                    strcat(message, players[adversario].name);
                    strcat(message, " aceitou!");
                    for (i = 1; i < NUMERO_JOGADORES; i++) 
                        if (!players[i].bot) 
                            sendToClient(players[i].clt_skt, message);
                }
        
                jogada(client_socket, rodadaAtual, &pontosMesa);
            } else if (player_choice_int == 2){
                *pontosMesa = 9;
                if (adversario == 0)
                    printf("Você pediu doze!\n");
                else
                    printf("O jogador %s pediu doze!\n", players[adversario].name);
                    
                strcpy(message, "0 O jogador ");
                strcat(message, players[adversario].name);
                strcat(message, " pediu doze!");
                for (i = 1; i < NUMERO_JOGADORES; i++)
                    if (!players[i].bot) 
                        sendToClient(players[i].clt_skt, message);
            
                truco(adversario, rodadaAtual, &pontosMesa);
            } else {
                if (adversario == 0) {
                    printf("Você correu!\n");
                    dupla2.pontos += *pontosMesa;
                } else {
                    printf("O jogador %s correu!\n", players[adversario].name);
                    strcpy(message, "0 O jogador ");
                    strcat(message, players[adversario].name);
                    strcat(message, " correu!");

                    for (i = 1; i < NUMERO_JOGADORES; i++) 
                        if (!players[i].bot)
                            sendToClient(players[i].clt_skt, message);

                    if (adversario == 2) 
                        dupla2.pontos += *pontosMesa;
                    else
                        dupla1.pontos += *pontosMesa;
                }
                partida(rand() % 4);
            }
            break;

        case 9:
            printf("DOZE!\n");

            strcpy(message, "0 DOZE!");
            for (i = 1; i < NUMERO_JOGADORES; i++)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, message);

            if (client_socket != 0)
                printf("O jogador %s pediu doze!\n", players[client_socket].name);

            if (adversario == 0) {
                printf("1 - Aceitar | 2 - Correr\n");
                scanf("%d", &player_choice_int);
            }

            strcpy(message, "0 O jogador ");
            strcat(message, players[client_socket].name);
            strcat(message, " pediu doze!");
            for (i = 1; i < NUMERO_JOGADORES; i++)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, message);

            if (!players[adversario].bot && adversario != 0) {
                strcpy(message, "1 1 - Aceitar | 2 - Correr ");
                sendToClient(players[adversario].clt_skt, message);
                strcpy(player_choice, receiveFromClient(players[adversario].clt_skt));
                player_choice_int = player_choice[0] - '0';
            } else {
                player_choice_int = (rand() % 3) + 1;
            } 

            if (player_choice_int == 1){
                *pontosMesa = 12;
                if(adversario == 0) {
                    printf("Você aceitou o doze!\n");
                } else {
                    printf("O jogador %s aceitou!\n", players[adversario].name);
                    strcpy(message, "1 O jogador ");
                    strcat(message, players[adversario].name);
                    strcat(message, " aceitou!");
                    for (i = 1; i < NUMERO_JOGADORES; i++) 
                        if (!players[i].bot) 
                            sendToClient(players[i].clt_skt, message);
                }
        
                jogada(client_socket, rodadaAtual, &pontosMesa);
            } else {
                if (adversario == 0) {
                    printf("Você correu!\n");
                    dupla2.pontos += *pontosMesa;
                } else {
                    printf("O jogador %s correu!\n", players[adversario].name);
                    strcpy(message, "0 O jogador ");
                    strcat(message, players[adversario].name);
                    strcat(message, " correu!");

                    for (i = 1; i < NUMERO_JOGADORES; i++) 
                        if (!players[i].bot)
                            sendToClient(players[i].clt_skt, message);

                    if (adversario == 2) 
                        dupla2.pontos += *pontosMesa;
                    else
                        dupla1.pontos += *pontosMesa;
                }
                partida(rand() % 4);
            }
            break;

        default:
            break;
    }
}

void jogada(int client_socket, int rodadaAtual, int *pontosMesa) {
    if (client_socket == 0){
        printf("Sua vez de jogar!\n");

        for (i = 0; i < players[client_socket].num_cartas; i++)
            printf("%d - %s\n", i+1, players[0].mao[i].carta);

        scanf("%d", &j);

        if (j == 0) truco(client_socket, rodadaAtual, &pontosMesa);
        else {
            rodadas[rodadaAtual].rodada[client_socket] = players[0].mao[j-1];
            reorganizarMao(players[0], j-1);
        }

    } else if (!players[client_socket].bot) {
        printf("Vez de %s\n", players[client_socket].name);

        char message[500] = "0 Sua vez de jogar! \n";
        sendToClient(players[client_socket].clt_skt, message);

        strcpy(message, "1 Suas cartas: ");
        for (i = 0; i < players[client_socket].num_cartas; i++){
            j = i+1;
            char j_str[STR_SIZE];
            sprintf(j_str, "%d", j);
            strcat(message, j_str);
            strcat(message, " - ");
            strcat(message, players[client_socket].mao[i].carta);
            strcat(message, " | ");
        }
        sendToClient(players[client_socket].clt_skt, message);
        printf("033[3;90mAguardando jogada...\033[0;37m\n");
        char *player_choice = receiveFromClient(players[client_socket].clt_skt);
        int player_choice_int = player_choice[0] - '0';
        if(player_choice_int == 0) truco(client_socket, rodadaAtual, &pontosMesa);
        else {
            rodadas[rodadaAtual].rodada[client_socket] = players[client_socket].mao[player_choice_int-1];
            reorganizarMao(players[client_socket], player_choice_int-1);
        }
        free(player_choice);
    } else {
        printf("Vez de %s\n", players[client_socket].name);
        int choice = Bot(client_socket, rodadaAtual, pontosMesa);
        rodadas[rodadaAtual].rodada[client_socket] = players[client_socket].mao[choice];
        reorganizarMao(players[client_socket], choice);
    }
}

void rodada(int jogadas, int rodadaAtual, int jogadorAtual, int pontosMesa){   
    if (jogadas == NUMERO_JOGADORES){
        verificarVencedor(rodadas[rodadaAtual], pontosMesa, rodadaAtual);
        rodadaAtual++;
        jogadas = 0;
        sleep(1.5);
        system("clear");
    }
    printf("\033[3;90mRodada %d\033[0;37m\n", jogadas+1);
    jogada(jogadorAtual, rodadaAtual, pontosMesa);
    system("clear");
    printf("Mesa: ");
    char message[500];
    strcpy(message, "0 Mesa: ");
    for (i = 0; i < NUMERO_JOGADORES; i++){
        printf("%s ", rodadas[rodadaAtual].rodada[i].carta);
        strcat(message, rodadas[rodadaAtual].rodada[i].carta);
        strcat(message, " ");
    }
    for (i = 1; i < NUMERO_JOGADORES; i++)
        if (!players[i].bot) 
            sendToClient(players[i].clt_skt, message);

    printf("\n");

    sleep(1);

    rodada(jogadas+1, rodadaAtual, (jogadorAtual < NUMERO_JOGADORES -1) ? jogadorAtual+1 : jogadorAtual - 3, pontosMesa);
}

// Funcao que inicia a partida
void partida(int ultimoJogador){
    printf("\033[3;90mIniciando partida...\033[0;37m\n");
    
    if (dupla1.pontos >= PONTOS_VITORIA || dupla2.pontos >= PONTOS_VITORIA){
        if (dupla1.pontos >= PONTOS_VITORIA){
            printf("Voces venceram a partida!\n");

            if (!players[2].bot) 
                sendToClient(players[2].clt_skt, "2 Voces venceram a partida!\n");
            for (i = 1; i < NUMERO_JOGADORES; i=+2) 
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, "2 Voces perderam a partida!\n");

        } else {
            printf("Dupla 2 venceu!\n");

            if (!players[2].bot) 
                sendToClient(players[2].clt_skt, "2 Voces perderam a partida!\n");
            for (i = 1; i < NUMERO_JOGADORES; i=+2)
                if (!players[i].bot) 
                    sendToClient(players[i].clt_skt, "2 Voces venceram a partida!\n");

        }
        printf("Fim de jogo!\n");
        exit(0);
    }

    int pontosMesa = 1; // Pontos que define se é truco ou nao

    embaralhar();
    
    distribuir(0);
    
    for (i = 0; i < NUMERO_JOGADORES; i++)
        qsort(players[i].mao, TAMANHO_MAO, sizeof(Carta), compare_cartas);

    // Printa as cartas do jogador
    printf("Suas cartas: ");
    for (i = 0; i < TAMANHO_MAO; i++){
        printf("%s ", players[0].mao[i].carta);
    }
    printf("\n");

    // Manda as cartas para os clientes
    for (i = 1; i < NUMERO_JOGADORES; i++){
        char message[500];
        strcpy(message, "0 Suas cartas: ");

        for (j = 0; j < TAMANHO_MAO; j++){
            strcat(message, players[i].mao[j].carta);
            strcat(message, " ");
        }
        if (!players[i].bot) 
            sendToClient(players[i].clt_skt, message);
    }
    rodada(0, 0, ultimoJogador, pontosMesa);
}

int main(){
    srand(time(NULL));
    printf("Digite seu nome: ");
    scanf("%s", players[0].name);

    system("clear");

    printf("Bem vindo %s!\n\n", players[0].name);

    gerarBaralho();

    int option = 0;
    while (!option){
        printf("Selecione o modo de jogo:\n");
        printf("1 - Singleplayer\n");
        printf("2 - Multiplayer\n");
        scanf("%d", &option);
        if(option > 2 || option < 1){
            printf("\033[2;49;31mOpção inválida\033[0;37m\n");
            option = 0;
        }
        system("clear");
    }

    switch (option) {
    case 1:
        for (int i = 1; i < NUMERO_JOGADORES; i++){
            strcpy(players[i].name, name_bot[i-1]);
            players[i].bot = 1;
        }

        definirDuplas();

        partida(0);
        break;
    case 2:
        int players;
        printf(" - Multiplayer\n");
        printf("Quantos jogadores?\n");
        printf("\033[3;90mNão inclui você (de 1 a 3)\033[0;37m\n");
        scanf("%d", &players);
        NUMERO_BOTS -= players;

        abrir_servidor(players);

        definirDuplas();

        partida(0);
        break;
    default:
        break;
    }

    return 0;
}