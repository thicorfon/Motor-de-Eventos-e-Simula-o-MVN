#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"

#ifndef MYBOOLEAN_H
#define MYBOOLEAN_H

#define false 0
#define true 1
typedef int bool; // or #define bool int

#define MEM_TAM 0xFFF //definicao do tamanho da memoria. Caso se necessite de um espaco de memoria menor, recomenda-se diminuir este espaco para facilitar a visualizacao da maquina

#endif

/**Definição do evento e seus parametros**/

typedef struct evento{ //Definicao do evento e seus parametros relevantes

    /**Espaco reservado para a criacao dos parametros necessarios no
       para os eventos. Alguns parametros comuns ja foram implementados**/

        char nome;      //identificador do evento
        int tempo;      //tempo previsto de simulacao
        int tarefa;     //tarefa ao qual o evento esta associado
        bool breakpoint;

    /**Termino da Implementacao**/

        struct evento * next;
} evento_t;


/**Definição dos objetos a serem simulados**/


/**Declaracao de funcoes**/

void pop_evento(evento_t ** head, evento_t ** retorno);
void seletorDeEvento (evento_t * head, evento_t *atual, bool* acompanhamento, bool* fim);
void menuDePausa(evento_t * listaDeEventos, bool *acompanhamento, bool *fim);

/**Reacoes a eventos especificos referentes aos objetos simulados**/



/** Operacoes basicas do simulador**/

void print_list(evento_t * head){ //Printa a lista de eventos
    evento_t * it  = head;
    printf("\nA lista atual de eventos e:");
    while (it->next != NULL){

        /**Implementar aqui como printar os eventos**/

        printf("\nNome: %c    Tempo: %d    Tarefa: T%d",it->next->nome,it->next->tempo,it->next->tarefa);

        /**Termino da Implementacao**/

        it = it->next;
    }
    printf("\n");
}

void inserir_evento (evento_t * head, char nome, int tempo, int tarefa, bool breakpoint){ //Insere um evento na lista de acordo com o tempo de execucao dele

    evento_t *temp, *left, *right;
    right = head;

    do{
        left = right;
        right = right->next;
        if (right == NULL){
            break;
        }
    } while(right->tempo<tempo);

    temp = malloc(sizeof(evento_t));

    /**Implemente aqui os parametros do seu evento**/

    temp->nome = nome;
    temp->tempo = tempo;
    temp->tarefa = tarefa;
    temp->breakpoint = breakpoint;

    /**Termino da Implementacao**/

    left->next = temp;
    left = temp;
    left->next = right;

}

void load_lista(evento_t * head, char * filename){ //Carrega a lista inicial de eventos a partir de um arquivo texto

    FILE *fp;
    fp = fopen(filename, "r"); //ver o formato do arquivo no relatorio


    int n = 1000;
    char buf[1000];

    /** Inserir aqui os parametros do seu evento **/

    char mne;
    int tempo;
    int tarefa;
    bool breakpoint;

    /** Termino **/

    char * parametro;


    /**Definir como será feita a leitura do arquivo de eventos e em quais parametros aquilo que foi lido sera alocado**/

    while (fgets(buf,n,fp) != NULL){
        parametro = strtok (buf," ");
        mne = parametro[0];
        parametro = strtok (NULL, " ");
        tempo = strtol(parametro,&parametro,10);
        parametro = strtok(NULL, " ");
        tarefa = strtol(parametro, &parametro, 10);
        parametro = strtok(NULL, " ");
        breakpoint = strtol(parametro,&parametro, 2);
        inserir_evento(head,mne,tempo,tarefa,breakpoint);
    }

    /**Termino**/


    fclose (fp);
}

void loopDeSimulacao(evento_t *listaDeEventos,bool *acompanhamento,bool *fim){ //Loop de Simulacao. Retira o primeiro elemento da lista e o resolve.
    evento_t *atual = malloc(sizeof(evento_t));
    atual->next = malloc(sizeof(evento_t));
    atual->next->next = NULL;
    while (listaDeEventos->next != NULL & *fim == false){ //caso fim seja true, encerra a simulacao
        pop_evento(&listaDeEventos,&atual);
        seletorDeEvento(listaDeEventos,atual, acompanhamento,fim);
        if (*acompanhamento == true){ //caso o acompanhamento passo a passo esteja ligado, pausa a simulacao.
            menuDePausa(listaDeEventos,acompanhamento,fim);
        }
    }
    printf("\nAcabei!");
}

void menu(evento_t * listaDeEventos, bool *acompanhamento, bool *fim){ //Menu Inicial de Simulacao
    printf("\nMenu Inicial");
    printf("\n1. Carregar lista de eventos iniciais");
    printf("\n2. Imprimir lista atual de eventos e estado atual da simulacao");
    printf("\n3. Adicionar eventos adicionais");
    if (*acompanhamento == false)
        printf("\n4. Ligar acompanhamento passo a passo");
    else
        printf("\n4. Desligar acompanhamento passo a passo");
    printf("\n5. Incluir ponto de pausa na simulacao");
    printf("\n6. Iniciar simulacao");
    printf("\n7. Finalizar simulacao");
    printf("\nDigite sua opcao:");
    char opcao[100];
    scanf("%s",opcao);
    switch(opcao[0]){
        case '1':
            printf("\nDigite o nome do arquivo que contem a lista inicial de eventos: ");
            char file[100];
            scanf("%s",file);
            load_lista(listaDeEventos,file);
            menu(listaDeEventos,acompanhamento,fim);
        break;

        case '2':
            print_list(listaDeEventos);
            /**Colocar aqui a funcao de estado do objeto simulado**/

            /**Termino**/
            menu(listaDeEventos,acompanhamento,fim);
        break;

        case '3':

            /**Implementar como que um evento deve ser inserido**/

            printf("\nDigite o nome, tempo e tarefa do evento (exemplo C 1 1):");
            char evento[100];
            char * parametro;
            char mne;
            int tempo;
            int tarefa;
            scanf("%s",evento);
            parametro = strtok(evento," ");
            mne = parametro[0];
            parametro = strtok (NULL, " ");
            tempo = strtol(parametro,&parametro,10);
            parametro = strtok(NULL, " ");
            tarefa = strtol(parametro, &parametro, 10);
            inserir_evento(listaDeEventos,mne,tempo,tarefa,false);

            /**Termino**/

            menu(listaDeEventos,acompanhamento,fim);
        break;

        case '4':
            if (*acompanhamento == false)
                *acompanhamento = true;
            else
                *acompanhamento = false;
            printf("\nAcompanhamento agora e %d",*acompanhamento);
            menu(listaDeEventos,acompanhamento,fim);
        break;

        case '5':
            printf("\nDigite o tempo no qual uma pausa deva ser inserida:");
            int tempop;
            scanf("%d",&tempop);
            inserir_evento(listaDeEventos,'S',tempop,0,false);
            menu(listaDeEventos,acompanhamento,fim);
        break;

        case '6':
            loopDeSimulacao(listaDeEventos,acompanhamento,fim);
        break;

        case '7':
            print_list(listaDeEventos);
            /**Chamar a função de estado dos objetos simulados**/

            /**Termino**/
        break;

        default:
            printf("\n\nOpção Invalida");
            menu(listaDeEventos,acompanhamento,fim);
    }

}

void menuDePausa(evento_t * listaDeEventos, bool *acompanhamento,bool *fim){ //Menu de Pausa
    printf("\nSimulacao pausada");
    printf("\n1. Imprimir lista atual de eventos e estado atual da simulacao");
    printf("\n2. Adicionar evento adicional");
    if (*acompanhamento == false)
        printf("\n3. Ligar acompanhamento passo a passo");
    else
        printf("\n3. Desligar acompanhamento passo a passo");
    printf("\n4. Incluir ponto de pausa na simulacao");
    printf("\n5. Continuar simulacao");
    printf("\n6. Finalizar Simulacao");
    printf("\nDigite sua opcao:");
    char opcao[100];
    scanf("%s",opcao);
    switch(opcao[0]){
        case '1':
            print_list(listaDeEventos);
            /**Chamar a função de estado dos objetos simulados**/

            /**Termino**/
            menuDePausa(listaDeEventos,acompanhamento,fim);
        break;

        case '2':
            /** Implementar como o Evento deve ser inserido **/

            printf("\nDigite o nome, tempo e tarefa do evento (exemplo C 1 1):");
            char evento[100];
            char * parametro;
            char mne;
            int tempo;
            int tarefa;
            scanf("%s",evento);
            parametro = strtok(evento," ");
            mne = parametro[0];
            parametro = strtok (NULL, " ");
            tempo = strtol(parametro,&parametro,10);
            parametro = strtok(NULL, " ");
            tarefa = strtol(parametro, &parametro, 10);
            inserir_evento(listaDeEventos,mne,tempo,tarefa,false);
            menuDePausa(listaDeEventos,acompanhamento,fim);

            /**Termino**/

        break;

        case '3':
            if (*acompanhamento == false)
                *acompanhamento = true;
            else
                *acompanhamento = false;
            menuDePausa(listaDeEventos,acompanhamento,fim);
        break;

        case '4':
            printf("\nDigite o tempo no qual uma pausa deva ser inserida:");
            int tempop;
            scanf("%d",tempop);
            inserir_evento(listaDeEventos,'S',tempop,0,false);
            menuDePausa(listaDeEventos,acompanhamento,fim);
        break;

        case '5':
            loopDeSimulacao(listaDeEventos,acompanhamento,fim);
        break;

        case '6':
            print_list(listaDeEventos);
            /**Chamar a função de estado dos objetos simulados**/

            /**Termino**/
            *fim = true;
        break;

        default:
            printf("\n\nOpção Invalida");
            menuDePausa(listaDeEventos,acompanhamento,fim);
    }

}

void pop_evento(evento_t ** head, evento_t ** retorno){ //Retira um elemento da lista e armazena os seus valores por referencia
    evento_t * proximo_evento = NULL;

    if (*head == NULL){
        printf("Lista Vazia");
        return;
    }

    proximo_evento = (*head)->next;
    (*retorno)->next = malloc(sizeof(evento_t));

    /** Implemente aqui os parâmetros do seu evento**/

    (*retorno)->next->nome = proximo_evento->nome;
    (*retorno)->next->tempo = proximo_evento->tempo;
    (*retorno)->next->tarefa = proximo_evento->tarefa;
    (*retorno)->next->breakpoint = proximo_evento->breakpoint;

    /**Termino da Implementacao**/

    (*retorno)->next->next = NULL;
    free(*head);
    *head = proximo_evento;

    return;

}

void append_evento(evento_t * head, char nome, int tempo, int tarefa, bool breakpoint){ //Adiciona um evento ao final da lista (OS PARAMETROS MUDAM DE ACORDO COM OS PARAMETROS DO EVENTO)

    evento_t * it = head;
    while (it->next != NULL){
        it = it->next;
    }

    it->next= malloc(sizeof(evento_t));

    /**Implemente aqui os parametros do seu evento**/

    it->next->nome = nome;
    it->next->tempo = tempo;
    it->next->tarefa =  tarefa;
    it->next->breakpoint = breakpoint;

    /**Termino da Implementacao**/

    it->next->next = NULL;
}

void remover_evento (evento_t ** head, evento_t ** retorno, int posicao){ //Remove um evento da lista de acordo com o tempo de execucao dele e, por referencia, retorna outro evento
    evento_t *aux,*left, *right;

    if (*head == NULL){
        printf("Lista Vazia");
        return;
    }

    aux = *head;
    right = aux->next;

    int i;
    for (i = 0; i<posicao; i++){
        left = aux;
        aux = aux->next;
        right = aux->next;
    }

    (*retorno)->next = malloc(sizeof(evento_t));

    /** Implemente aqui os parâmetros do seu evento**/

    (*retorno)->next->nome = aux->nome;
    (*retorno)->next->tempo = aux->tempo;
    (*retorno)->next->tarefa = aux->tarefa;
    (*retorno)->next->breakpoint = aux->breakpoint;

    /**Termino da Implementacao**/

    (*retorno)->next->next = NULL;
    left->next = right;

    free(aux);

    return;

}

void seletorDeEvento (evento_t * head, evento_t *atual, bool *acompanhamento, bool *fim){ //Lê qual o tipo de evento e toma a ação adequada.

    /**Nesta parte deve ser implementado como o evento deve ser identificado a partir de seus parâmetros e como o simulador deve reagir a cada evento**/

    char op;
    op = atual->next->nome;
    switch(op){

        case 'P': //Print
            print_list(head);
            /**Chamar a função de estado dos objetos simulados**/

            /**Termino**/
        break;

        case 'S'://Stop
            menuDePausa(head,acompanhamento,fim);

        default:
            printf("\nEvento Invalido");

    }

    /**Término da implementação**/
}

/** Programa principal **/

int main(){

    evento_t * listaDeEventos;

    listaDeEventos = malloc(sizeof(evento_t));
    listaDeEventos->next = NULL;

    bool acompanhamento = false;
    bool fim = false;

    /**Inicializar Objetos Simulados **/

    menu(listaDeEventos,&acompanhamento,&fim);

    return 0;
}
