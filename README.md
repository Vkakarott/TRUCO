<h1 align="center">TRUCO<h1>

<p align="center">Jogo de cartas multiplayer<p>

<h4 align="center"> Conexao Cliente-servidor via socket TCP <h4>

##

### Features
<p>Truco online<p>

- [x] Permite jogar singleplayer e multiplayer
- [x] Conecao LAN com protocolo TCP
- [x] Totalmente desenvolvido em C

##

### 🛠️ TECNOLOGIAS

 <img height="40" width="50" src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/c/c-original.svg" />
 <h3>#include </h3>

    - <arpa/inet.h> -> fornece funções para manipular endereços da Internet
    - <stdlib.h> -> valores aleatorios
    - <time.h> -> definir tempo (necessario para função rand)
    - <unistd.h> -> sleep no linux
    - <string.h> -> manipulação de strings
 
 <h3>FUNÇÕES</h3>
 
```c
    // envia mensagem ao cliente
    void sendToClient(int client_socket, const char *message) {
      send(client_socket, message, strlen(message), 0);
    sleep(1);
    }

    // recebe mensagem do cliente
    char* receiveFromClient(int client_socket) {
      char buffer[MAX_BUFFER_SIZE] = {0};
      valread = read(client_socket, buffer, sizeof(buffer));
      return strdup(buffer);
    }
```

## 

para jogar multyplayer precisa compilar o arquivo cliente.c no computador dos players a mais

## Autores
<div align="center">
  <a align="center" href="https://github.com/vkakarott/TRUCO/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=vkakarott/TRUCO" />
  </a>
</div>
