#include <stdio.h>  // Para entrada e saída (printf, scanf)
#include <stdlib.h> // Para funções úteis (rand, srand, exit)
#include <time.h>   // Para inicializar o gerador aleatório (time)

// --- Definições Globais e Constantes ---

// Define o tamanho máximo da fila e da pilha
#define FILA_TAM 5
#define PILHA_TAM 3

// Contador global para garantir que cada peça tenha um ID único
int proximoId = 0;

// --- Estrutura das Peças ---

// 'struct' é como um molde para criar uma variável que agrupa outras.
// Aqui, uma Peca tem um nome (char) e um id (int).
typedef struct {
    char nome; // 'I', 'O', 'T', 'L'
    int id;    // Número único (0, 1, 2, ...)
} Peca;


// --- Estrutura da Fila Circular ---

// Nossa fila terá um array de Peças e três 'ponteiros' (índices)
// para saber onde começa, onde termina e quantas peças temos.
typedef struct {
    Peca itens[FILA_TAM]; // Onde guardamos as peças
    int inicio;           // Índice de quem vai sair (frente)
    int fim;              // Índice de quem acabou de entrar (traseira)
    int total;            // Quantas peças temos na fila
} FilaCircular;


// --- Estrutura da Pilha Linear ---

// Nossa pilha é mais simples. Só precisamos de um array
// e um 'ponteiro' (índice) para saber quem está no topo.
typedef struct {
    Peca itens[PILHA_TAM]; // Onde guardamos as peças
    int topo;             // Índice do topo. Se for -1, está vazia.
} Pilha;


// --- Variáveis Globais ---
// Para "calouros", é mais fácil declarar as estruturas como globais.
// Assim, não precisamos passar ponteiros para todas as funções.
FilaCircular filaDePecas;
Pilha pilhaDeReserva;


// --- Protótipos das Funções ---
// Avisamos ao C quais funções existem antes de usá-las.

// Funções de inicialização
void inicializarFila();
void inicializarPilha();
void popularFilaInicial();

// Funções de verificação (retornam 1 para Verdadeiro, 0 para Falso)
int filaEstaCheia();
int filaEstaVazia();
int pilhaEstaCheia();
int pilhaEstaVazia();

// Função que cria uma nova peça
Peca gerarPeca();

// Operações da Fila
void enfileirar(Peca p); // Adiciona no FIM
Peca desenfileirar();    // Remove do INÍCIO

// Operações da Pilha
void empilhar(Peca p); // Adiciona no TOPO
Peca desempilhar();   // Remove do TOPO

// Funções do Menu (Ações do Jogo)
void acaoJogar();
void acaoReservar();
void acaoUsarReservada();
void acaoTrocarAtual();
void acaoTrocaMultipla();

// Funções de UI (Interface do Usuário)
void exibirEstado();
void exibirMenu();
void limparBufferEntrada(); // Função útil para limpar o 'Enter' do scanf

// Função de lógica do jogo
void reabastecerFila(); // Regra especial: preenche a fila se sair uma peça


// --- Função Principal (main) ---
// É aqui que o programa começa.

int main() {
    // Inicializa o gerador de números aleatórios
    // Isso garante que as peças 'I', 'O', 'T', 'L' mudem a cada execução
    srand(time(NULL));

    // Prepara as estruturas para o jogo
    inicializarFila();
    inicializarPilha();
    
    // Regra do jogo: "Inicializar a fila... com um número fixo (5)"
    popularFilaInicial();

    int opcao; // Variável para guardar a escolha do usuário

    // loop 'do-while' significa: "Faça o que está dentro,
    // e depois verifique se a condição (opcao != 0) é verdadeira
    // para repetir."
    do {
        // system("clear"); // No Linux/Mac. Descomente se quiser limpar a tela.
        // system("cls");   // No Windows. Descomente se quiser limpar a tela.

        printf("\n============================================\n");
        printf("       GERENCIADOR ESTRATEGICO DE PECAS\n");
        printf("============================================\n");
        
        // Mostra o estado atual antes de pedir a próxima ação
        exibirEstado();
        exibirMenu();
        
        printf("\nOpcao escolhida: ");
        
        // Lê a opção do usuário
        // O `scanf` pode deixar um '\n' (Enter) "sujo" no buffer.
        // Se o programa se comportar de forma estranha (pulando menus),
        // é por causa disso. A função limparBufferEntrada() ajuda.
        if (scanf("%d", &opcao) != 1) {
            // Se o usuário não digitou um número, limpamos a bagunça
            limparBufferEntrada();
            opcao = -1; // Força a ser uma opção inválida
        } else {
            // Se ele digitou um número, só limpamos o Enter
            limparBufferEntrada(); 
        }

        printf("\n--- ACAO ---\n");

        // 'switch' é como um monte de 'if'/'else if'
        switch (opcao) {
            case 1:
                acaoJogar();
                break;
            case 2:
                acaoReservar();
                break;
            case 3:
                acaoUsarReservada();
                break;
            case 4:
                acaoTrocarAtual();
                break;
            case 5:
                acaoTrocaMultipla();
                break;
            case 0:
                printf("Encerrando o programa. Ate mais!\n");
                break;
            default:
                printf("Opcao invalida! Por favor, escolha um numero do menu.\n");
                break;
        }

        if (opcao != 0) {
            printf("\n(Pressione Enter para continuar...)");
            getchar(); // Pausa o programa até o usuário pressionar Enter
        }

    } while (opcao != 0); // Repete enquanto a opção não for 0 (Sair)

    return 0; // Termina o programa com sucesso
}


// --- Implementação das Funções ---

// Zera a fila
void inicializarFila() {
    filaDePecas.inicio = 0;
    filaDePecas.fim = -1; // -1 indica que está vazia
    filaDePecas.total = 0;
}

// Zera a pilha
void inicializarPilha() {
    pilhaDeReserva.topo = -1; // -1 indica que está vazia
}

// Enche a fila com as 5 primeiras peças do jogo
void popularFilaInicial() {
    printf("Gerando pecas iniciais...\n");
    for (int i = 0; i < FILA_TAM; i++) {
        Peca novaPeca = gerarPeca();
        enfileirar(novaPeca);
    }
}

// Verifica se a fila está cheia
int filaEstaCheia() {
    return (filaDePecas.total == FILA_TAM);
}

// Verifica se a fila está vazia
int filaEstaVazia() {
    return (filaDePecas.total == 0);
}

// Verifica se a pilha está cheia
int pilhaEstaCheia() {
    // O topo é um índice (começa em 0). 
    // Se o tamanho é 3, os índices são 0, 1, 2.
    // Então, a pilha está cheia quando o topo for (PILHA_TAM - 1).
    return (pilhaDeReserva.topo == PILHA_TAM - 1);
}

// Verifica se a pilha está vazia
int pilhaEstaVazia() {
    return (pilhaDeReserva.topo == -1);
}

// Cria e retorna uma nova peça
Peca gerarPeca() {
    Peca p;
    char tipos[] = {'I', 'O', 'T', 'L'};
    
    // 'rand() % 4' gera um número aleatório: 0, 1, 2 ou 3
    int tipoAleatorio = rand() % 4; 
    
    p.nome = tipos[tipoAleatorio]; // Pega o nome ('I', 'O', 'T' ou 'L')
    p.id = proximoId;              // Pega o ID global
    
    proximoId++; // Incrementa o ID para a próxima peça ser única
    
    return p;
}

// Adiciona uma peça no FIM da fila
void enfileirar(Peca p) {
    if (filaEstaCheia()) {
        // Isso não deveria acontecer por causa da nossa regra de reabastecer
        printf("ERRO: Fila cheia!\n");
        return;
    }
    
    // A mágica da fila circular: (indice + 1) % TAMANHO
    // Se 'fim' é 4 e TAMANHO é 5: (4 + 1) % 5 = 5 % 5 = 0. Volta pro início!
    filaDePecas.fim = (filaDePecas.fim + 1) % FILA_TAM;
    filaDePecas.itens[filaDePecas.fim] = p;
    filaDePecas.total++;
}

// Remove uma peça do INÍCIO da fila
Peca desenfileirar() {
    // Se a fila estiver vazia, retornamos uma "peça nula" para evitar erros
    if (filaEstaVazia()) {
        printf("Fila esta vazia! Nao ha pecas para desenfileirar.\n");
        Peca pecaNula = {'?', -1};
        return pecaNula;
    }

    Peca pecaDaFrente = filaDePecas.itens[filaDePecas.inicio];
    
    // A mágica de novo: move o 'inicio' para frente
    filaDePecas.inicio = (filaDePecas.inicio + 1) % FILA_TAM;
    filaDePecas.total--;
    
    return pecaDaFrente;
}

// Adiciona uma peça no TOPO da pilha
void empilhar(Peca p) {
    if (pilhaEstaCheia()) {
        printf("ERRO: Pilha cheia!\n");
        return;
    }
    
    // Avança o topo...
    pilhaDeReserva.topo++;
    // ...e coloca a peça lá.
    pilhaDeReserva.itens[pilhaDeReserva.topo] = p;
}

// Remove uma peça do TOPO da pilha
Peca desempilhar() {
    if (pilhaEstaVazia()) {
        printf("Pilha de reserva esta vazia! Nao ha pecas para usar.\n");
        Peca pecaNula = {'?', -1};
        return pecaNula;
    }

    // Pega a peça do topo...
    Peca pecaDoTopo = pilhaDeReserva.itens[pilhaDeReserva.topo];
    // ...e "abaixa" o topo, "esquecendo" a peça que estava lá.
    pilhaDeReserva.topo--;
    
    return pecaDoTopo;
}


// --- Funções de Ação ---

// REGRA: "Gerar uma nova peça... a cada remoção ou envio à pilha"
// Esta função é chamada sempre que uma peça sai da fila.
void reabastecerFila() {
    if (!filaEstaCheia()) {
        Peca novaPeca = gerarPeca();
        enfileirar(novaPeca);
        printf("(Uma nova peca [%c %d] foi gerada no fim da fila.)\n", novaPeca.nome, novaPeca.id);
    }
}

// Ação 1: Jogar Peça
void acaoJogar() {
    if (filaEstaVazia()) {
        printf("Fila de pecas esta vazia! Nao pode jogar.\n");
        return;
    }

    Peca p = desenfileirar(); // Tira da frente da fila
    
    printf("Peca jogada (removida do jogo): [%c %d]\n", p.nome, p.id);
    
    // Regra especial: reabastece a fila
    reabastecerFila();
}

// Ação 2: Reservar Peça
void acaoReservar() {
    // Verificações primeiro
    if (filaEstaVazia()) {
        printf("Fila de pecas esta vazia! Nao pode reservar.\n");
        return;
    }
    if (pilhaEstaCheia()) {
        printf("Pilha de reserva esta cheia! Nao pode reservar.\n");
        return;
    }

    Peca p = desenfileirar(); // Tira da frente da fila
    empilhar(p);             // Coloca no topo da pilha
    
    printf("Peca movida para a reserva: [%c %d]\n", p.nome, p.id);

    // Regra especial: reabastece a fila
    reabastecerFila();
}

// Ação 3: Usar Peça Reservada
void acaoUsarReservada() {
    if (pilhaEstaVazia()) {
        printf("Pilha de reserva esta vazia! Nao pode usar.\n");
        return;
    }
    
    Peca p = desempilhar(); // Tira do topo da pilha
    
    printf("Peca usada da reserva (removida do jogo): [%c %d]\n", p.nome, p.id);
    
    // Nota: Usar da pilha NÃO reabastece a fila, conforme os requisitos.
}

// Ação 4: Trocar peça da frente da fila com o topo da pilha
void acaoTrocarAtual() {
    // Verificações primeiro
    if (filaEstaVazia()) {
        printf("Fila esta vazia! Troca impossivel.\n");
        return;
    }
    if (pilhaEstaVazia()) {
        printf("Pilha esta vazia! Troca impossivel.\n");
        return;
    }

    // Pega uma CÓPIA de cada peça (sem remover)
    Peca pecaFila = filaDePecas.itens[filaDePecas.inicio];
    Peca pecaPilha = pilhaDeReserva.itens[pilhaDeReserva.topo];

    // Agora, coloca a peça da pilha na fila
    filaDePecas.itens[filaDePecas.inicio] = pecaPilha;
    // E a peça da fila na pilha
    pilhaDeReserva.itens[pilhaDeReserva.topo] = pecaFila;

    printf("Troca realizada:\n");
    printf("- Fila recebeu: [%c %d]\n", pecaPilha.nome, pecaPilha.id);
    printf("- Pilha recebeu: [%c %d]\n", pecaFila.nome, pecaFila.id);
}


// Ação 5: Trocar os 3 primeiros da fila com as 3 peças da pilha
void acaoTrocaMultipla() {
    // Verificações
    // 'filaDePecas.total < 3' -> fila não tem 3 peças
    // 'pilhaDeReserva.topo < 2' -> pilha não tem 3 peças (índices 0, 1, 2)
    if (filaDePecas.total < 3 || pilhaDeReserva.topo < 2) {
        printf("Troca multipla falhou! Ambas as estruturas devem ter 3 pecas.\n");
        printf("(Fila tem: %d | Pilha tem: %d)\n", filaDePecas.total, pilhaDeReserva.topo + 1);
        return;
    }

    // Lógica da troca (como no exemplo):
    // Fila [A B C]... e Pilha [X Y Z] (X é o topo)
    // Fila vira [X Y Z]... e Pilha vira [C B A] (C é o topo)
    //
    // O jeito mais simples (para um calouro) é usar um loop
    // e trocar Fila[inicio+i] com Pilha[topo-i].
    //
    // Vamos seguir o exemplo:
    // Fila Antes: [I 0] [L 1] [T 2] ...
    // Pilha Antes: [O 8] [L 7] [T 6] (Topo->Base)
    //
    // Fila Depois: [O 8] [L 7] [T 6] ...
    // Pilha Depois: [T 2] [L 1] [I 0] (Topo->Base)
    //
    // Isso mostra que:
    // Fila[inicio+0] (I 0) trocou com Pilha[topo-2] (T 6)
    // Fila[inicio+1] (L 1) trocou com Pilha[topo-1] (L 7)
    // Fila[inicio+2] (T 2) trocou com Pilha[topo-0] (O 8)

    printf("Realizando troca multipla...\n");

    Peca temp; // Variável temporária para a troca

    for (int i = 0; i < 3; i++) {
        // Índice da Fila (circular)
        int idxFila = (filaDePecas.inicio + i) % FILA_TAM;
        
        // Índice da Pilha (linear, mapeando 0->2, 1->1, 2->0)
        // O item 'i' da fila (0, 1, 2)
        // troca com o item 'i' da pilha (topo-2, topo-1, topo)
        int idxPilha = (pilhaDeReserva.topo - 2) + i;

        // Troca (swap)
        temp = filaDePecas.itens[idxFila];
        filaDePecas.itens[idxFila] = pilhaDeReserva.itens[idxPilha];
        pilhaDeReserva.itens[idxPilha] = temp;
    }
    
    printf("Acao: troca realizada entre os 3 primeiros da fila e os 3 da pilha.\n");
}


// --- Funções de Interface ---

// Mostra o estado atual das duas estruturas
void exibirEstado() {
    printf("\nEstado Atual:\n");

    // 1. Exibir Fila
    printf("Fila de pecas (Inicio -> Fim):\n");
    if (filaEstaVazia()) {
        printf("[ Fila Vazia ]\n");
    } else {
        // Percorre a fila circular do início até o fim
        // 'i' é só um contador, 'idxReal' é o índice no array
        int idxReal;
        printf("   ");
        for (int i = 0; i < filaDePecas.total; i++) {
            idxReal = (filaDePecas.inicio + i) % FILA_TAM;
            printf("[%c %d] ", filaDePecas.itens[idxReal].nome, filaDePecas.itens[idxReal].id);
        }
        printf("\n");
    }

    // 2. Exibir Pilha
    printf("Pilha de reserva (Topo -> Base):\n");
    if (pilhaEstaVazia()) {
        printf("[ Pilha Vazia ]\n");
    } else {
        // Percorre a pilha de cima para baixo
        printf("   ");
        for (int i = pilhaDeReserva.topo; i >= 0; i--) {
            printf("[%c %d] ", pilhaDeReserva.itens[i].nome, pilhaDeReserva.itens[i].id);
        }
        printf("\n");
    }
}

// Mostra as opções
void exibirMenu() {
    printf("\nOpcoes disponiveis:\n");
    printf(" 1. Jogar peca (remove da frente da fila)\n");
    printf(" 2. Reservar peca (fila -> pilha)\n");
    printf(" 3. Usar peca reservada (remove do topo da pilha)\n");
    printf(" 4. Trocar peca atual (frente-fila <-> topo-pilha)\n");
    printf(" 5. Troca multipla (3 da fila <-> 3 da pilha)\n");
    printf(" 0. Sair\n");
}

// Limpa o "lixo" do buffer de entrada (o 'Enter' que sobra do scanf)
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
