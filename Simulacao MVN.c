#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"

#ifndef MYBOOLEAN_H
#define MYBOOLEAN_H

#define false 0
#define true 1
typedef int bool; // or #define bool int

#define MEM_TAM 0x10000 //definicao do tamanho da memoria. Caso se necessite de um espaco de memoria menor, recomenda-se diminuir este espaco para facilitar a visualizacao da maquina
#define NUM_PROC 0x10
#define NUM_PROG 0x20
#define NUM_DEVICE 0x10

#endif

/**Defini��o do evento e seus parametros**/

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


/**Defini��o dos objetos a serem simulados**/

typedef struct LinhaMapaDeProcessos{
    int blocoMemoriaFisica;
   	long blocoDisco;
}LinhaMapaDeProcessos_t;

typedef struct MapaDeProcessos{
    LinhaMapaDeProcessos_t linha[NUM_PROC];
}MapaDeProcessos_t;

typedef struct LinhaMapaDeMemoria{
    int idProgramaOcupando;
    bool busy;
    int tempoLU;
}LinhaMapaDeMemoria_t;

typedef struct MapaDeMemoria{
    LinhaMapaDeMemoria_t linha[16];
}MapaDeMemoria_t;

typedef struct LinhaListaDeProgramas{
    MapaDeProcessos_t MapaDeProcessos;
    int idPrograma;
    bool carregadoNoDisco;
    int ultimoCi;
    int ultimoAcc;
    bool ultimoModoIndireto;
}LinhaListaDeProgramas_t;

typedef struct ListaDeProgramas{
    LinhaListaDeProgramas_t linha[NUM_PROG];
    int fim;
}ListaDeProgramas_t;

typedef struct BlocoDisco{
	uint8_t posicao[0xFFF];
	bool ocupado;
}BlocoDisco_t;

typedef struct Disco{
	BlocoDisco_t bloco[NUM_PROG*NUM_PROC];
	long ultimoBlocoOcupado;
}Disco_t;

typedef struct DispositivoDeEntrada{
    uint8_t reg;
    bool uso;
    bool termino;
    int tempo;
    int valor;
}DispositivoDeEntrada_t;

typedef struct DispositivoDeSaida{
    uint8_t reg;
    bool uso;
    bool termino;
    int tempo;
}DispositivoDeSaida_t;

typedef struct MVN{
    uint8_t memoria[MEM_TAM]; //memoria
   	Disco_t disco;
    uint16_t acc; //acumulador
    uint16_t ci;  //contador de instrucoes
    uint16_t arg; //op da instrucao
    uint16_t instrucao; //instrucao a ser executada
    char buffer[2];
    long cursorEntrada;
    bool modoIndireto;
    bool modoSupervisor;
    int programaAtivo;
    ListaDeProgramas_t ListaDeProgramas;
    MapaDeMemoria_t MapaDeMemoria;
    DispositivoDeEntrada_t DispositivosDeEntrada[NUM_DEVICE];
    DispositivoDeSaida_t  DispositivosDeSaida[NUM_DEVICE];
}MVN_t;


/**Declaracao de funcoes**/

void pop_evento(evento_t ** head, evento_t ** retorno);
void seletorDeEvento (evento_t * head, evento_t *atual, MVN_t * MVN, bool* acompanhamento, bool* fim);
void menuDePausa(evento_t * listaDeEventos, MVN_t * MVN, bool *acompanhamento,bool *fim);
int procurarPrograma(MVN_t * MVN, int id);
void inserir_evento (evento_t * head, char nome, int tempo, int tarefa, bool breakpoint);

/**Reacoes a eventos especificos referentes aos objetos simulados**/

void inicializarMVN(MVN_t * MVN){
    int i;
    for (i = 0; i < MEM_TAM; i++){
        MVN->memoria[i] = 0;
    }
    MVN->acc = 0;
    MVN->ci = 0;
    MVN->arg = 0;
    MVN->instrucao = 0;
    MVN->cursorEntrada = 0;
    MVN->modoIndireto = false;
    MVN->modoSupervisor = false;
    MVN->programaAtivo = -1;

    LinhaMapaDeMemoria_t * linhaMemoriaAtual;

    for (i = 0; i < 16; i++)
    {
        linhaMemoriaAtual = &MVN->MapaDeMemoria.linha[i];
        linhaMemoriaAtual->busy = false;
        linhaMemoriaAtual->idProgramaOcupando = 0;
    }


    LinhaListaDeProgramas_t *linhaProgramaAtual;
    LinhaMapaDeProcessos_t *linhaProcessoAtual;

    MVN->ListaDeProgramas.fim = -1;

    for (i = 0; i < NUM_PROG; i++)
    {
        linhaProgramaAtual = &MVN->ListaDeProgramas.linha[i];
        linhaProgramaAtual->idPrograma = 0;
        linhaProgramaAtual->carregadoNoDisco = 0;
        linhaProgramaAtual->ultimoCi = 0;
        linhaProgramaAtual->ultimoAcc = 0;
        linhaProgramaAtual->ultimoModoIndireto = false;
        int j;

        for (j = 0; j < NUM_PROC; j++){
            linhaProcessoAtual = &linhaProgramaAtual->MapaDeProcessos.linha[j];
            linhaProcessoAtual->blocoMemoriaFisica = -1;
            linhaProcessoAtual->blocoDisco = -1;
        }
    }

    DispositivoDeEntrada_t *deviceEntradaAtual;
    DispositivoDeSaida_t *deviceSaidaAtual;

    for (i = 0; i < NUM_DEVICE; i++){
        deviceEntradaAtual = &MVN->DispositivosDeEntrada[i];
        deviceSaidaAtual = &MVN->DispositivosDeSaida[i];
        deviceEntradaAtual->reg = 0;
        deviceSaidaAtual->reg = 0;
        deviceEntradaAtual->tempo = NUM_DEVICE - i;
        deviceSaidaAtual->tempo = NUM_DEVICE - i;
        deviceEntradaAtual->termino = false;
        deviceSaidaAtual->termino = false;
        deviceEntradaAtual->uso = false;
        deviceSaidaAtual->uso = false;
        deviceEntradaAtual->valor = i+1;
    }

    MVN->disco.ultimoBlocoOcupado = -1;


    // MVN->MapaDeMemoria.linha[3].busy = true;
    // MVN->MapaDeMemoria.linha[3].idProgramaOcupando = 6;

    // MVN->ListaDePro.gramas.linha[10].carregadoNoDisco = 1;
    // MVN->ListaDeProgramas.linha[10].idPrograma = 6;

    // MVN->ListaDeProgramas.linha[10].MapaDeProcessos.linha[3].blocoDisco = 0;
    // MVN->ListaDeProgramas.linha[10].MapaDeProcessos.linha[3].blocoMemoriaFisica = 3;

}

void imprimirMemoria(MVN_t* MVN, int inicio, int fim){
        printf("\nMemoria:");
        int i;
        printf("\n      %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x",0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
        for (i = inicio; i<fim; i+=16){
            printf("\n%03x   %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x", i/0x10,MVN->memoria[i],MVN->memoria[i+1],MVN->memoria[i+2],MVN->memoria[i+3],MVN->memoria[i+4],MVN->memoria[i+5],MVN->memoria[i+6],MVN->memoria[i+7],MVN->memoria[i+8],MVN->memoria[i+9],MVN->memoria[i+10],MVN->memoria[i+11],MVN->memoria[i+12],MVN->memoria[i+13],MVN->memoria[i+14],MVN->memoria[i+15]);
        }
}

void imprimirMapaDeMemoria(MVN_t* MVN){
    int i;
    printf("\nBanco  Ocupado  Id_Programa");
    LinhaMapaDeMemoria_t linhaAtual;
    for (i=0; i < 16; i++){
        linhaAtual = MVN->MapaDeMemoria.linha[i];
        printf("\n%5x %6d  %11d", i, linhaAtual.busy, linhaAtual.idProgramaOcupando);
    }
}

void imprimirMapaDeProcessos(MVN_t* MVN, int idPrograma){
	int linhaPrograma = procurarPrograma(MVN, idPrograma);

	if (linhaPrograma == NUM_PROG){
        printf("\n\nNAO ACHEI O PROGRAMA\n\n");
    }
    else{
    	int i;

    	MapaDeProcessos_t mapa = MVN->ListaDeProgramas.linha[linhaPrograma].MapaDeProcessos;
    	LinhaMapaDeProcessos_t linhaAtual;
    	printf("\nBlocoDisco  BlocoMemoriaFisica");

    	for (i = 0; i < NUM_PROC; i++){
    		linhaAtual = mapa.linha[i];
    		printf("\n%10x  %18x",linhaAtual.blocoDisco, linhaAtual.blocoMemoriaFisica);
    	}
    }
}

void imprimirListaDeProgramas(MVN_t* MVN){
	int i;
	printf("\nidPrograma  CarregadoNoDisco  UltimoCi  UltimoAcc  UltimoModoIndireto");
	LinhaListaDeProgramas_t linhaAtual;
	for (i = 0; i < NUM_PROG; i++)
	{
		linhaAtual = MVN->ListaDeProgramas.linha[i];
		printf("\n%10x  %16x  %8x  %9x  %18x", linhaAtual.idPrograma, linhaAtual.carregadoNoDisco, linhaAtual.ultimoCi, linhaAtual.ultimoAcc, linhaAtual.ultimoModoIndireto);
	}
	int programa = 1;
	while(programa != 0){
		printf("\n\nDigite o id de um programa para imprimir o Mapa de Processos ou 0 para sair: ");
		scanf("%d", &programa);
		if (programa){
			imprimirMapaDeProcessos(MVN, programa);
		}
	}
}

void imprimirDisco(MVN_t* MVN){
	int programa = 1;
	int i;
	BlocoDisco_t bloco;
	while(programa != -1){
		printf("\n\nDigite o bloco do disco ou -1 para sair: ");
		scanf("%d", &programa);
		if (programa != -1){
			bloco = MVN->disco.bloco[programa];
			printf("\n     %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x    %2x",0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
			for (i = 0; i<0x1000; i+=16){
            	printf("\n%02x   %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x    %02x", i/0x10,bloco.posicao[i],bloco.posicao[i+1],bloco.posicao[i+2],bloco.posicao[i+3],bloco.posicao[i+4],bloco.posicao[i+5],bloco.posicao[i+6],bloco.posicao[i+7],bloco.posicao[i+8],bloco.posicao[i+9],bloco.posicao[i+10],bloco.posicao[i+11],bloco.posicao[i+12],bloco.posicao[i+13],bloco.posicao[i+14],bloco.posicao[i+15]);
        	}
		}
	}
}

void estado_maquina(MVN_t * MVN){ //Printa o conte�do do acumulador, Contador de Instru��es, Instru��o realizada, Operando e o conte�do na mem�ria
    printf("\nO estado atual da maquina eh: ");
    printf("\nAcumulador: %04x",MVN->acc);
    printf("\nC.I.: %03x ", MVN->ci);
    printf("\nInstrucao: %04x", MVN->instrucao);
    printf("\nOP: %04x",MVN->arg);
    printf("\nSupervisor: %1d", MVN->modoSupervisor);
    printf("\nProgramaAtivo: %2d", MVN->programaAtivo);
    char op[100];
    printf("\n\nMostrar Memoria?(y/n) ");
    scanf("%s",op);
    if (op[0] == 'y'){
        int banco;
        printf("\nDigite 0-15 para mostrar um dos bancos de memoria ou -1 para mostrar todos: ");
        scanf("%d",&banco);
        if (banco == -1){
            imprimirMemoria(MVN, 0, MEM_TAM);
        }
        else{
            imprimirMemoria(MVN, (banco)*0x1000, ((banco+1)*0x1000));
        }
    }

    printf("\n\nMostrar Mapa de Memoria?(y/n)");
    scanf("%s",op);
    if (op[0] == 'y'){
        imprimirMapaDeMemoria(MVN);
    }

    printf("\n\nMostrar Lista de Programas?(y/n)");
    scanf("%s",op);
    if (op[0] == 'y'){
        imprimirListaDeProgramas(MVN);
    }

    printf("\n\nMostrar Disco?(y/n)");
    scanf("%s",op);
    if (op[0] == 'y'){
        imprimirDisco(MVN);
    }
}

int adicionarProgramaNaListaDeProgramas(MVN_t * MVN, int tarefa, int enderecoInicial){
	MVN->ListaDeProgramas.fim++;
	int indice = MVN->ListaDeProgramas.fim;
	LinhaListaDeProgramas_t * linhaProgramaAtual = &MVN->ListaDeProgramas.linha[indice];
	linhaProgramaAtual->carregadoNoDisco = true;
	linhaProgramaAtual->idPrograma = tarefa;
	linhaProgramaAtual->ultimoCi = enderecoInicial;


	return indice;
}


int alocarProcessoNoDisco(MVN_t * MVN, int tarefa, int endereco, int indiceListaDeProgramas){
	LinhaListaDeProgramas_t *linhaProgramaAtual = &MVN->ListaDeProgramas.linha[indiceListaDeProgramas];
	LinhaMapaDeProcessos_t *processoProgramaAtual = &linhaProgramaAtual->MapaDeProcessos.linha[endereco/0x1000];
	int indiceBlocoAAlocar;
	if (processoProgramaAtual->blocoDisco == -1){
		MVN->disco.ultimoBlocoOcupado++;
		indiceBlocoAAlocar = MVN->disco.ultimoBlocoOcupado;
		BlocoDisco_t *blocoAtual = &MVN->disco.bloco[indiceBlocoAAlocar];
		blocoAtual->ocupado = 1;

		processoProgramaAtual->blocoDisco = indiceBlocoAAlocar;
	}

	else{
		indiceBlocoAAlocar = processoProgramaAtual->blocoDisco;
	}
	return indiceBlocoAAlocar;
}


void carregarPrograma (MVN_t * MVN, char* filename, int tarefa){ //Carrega dados para a mem�ria e posiciona o CI no valor inicial informado no arquivo texto.
    FILE *fp;
    fp = fopen(filename, "r"); //ver o formato do arquivo no relatorio

    int endereco;
    int instrucao;
    char * parametro;

    int n = 1000;
    char buf[1000];

    int processoAnterior = 16;
    int blocoAtual;

    fgets(buf,n,fp);
    parametro = strtok(buf," ");
    endereco = strtol(parametro,&parametro,16);
    int indiceProgramaNaLista = adicionarProgramaNaListaDeProgramas(MVN, tarefa, endereco);


    while (fgets(buf,n,fp)!= NULL){
        parametro = strtok (buf," ");
        endereco = strtol(parametro,&parametro,16);
	    if ((processoAnterior == 16) || ((endereco/0x1000) != processoAnterior)){
	    	blocoAtual = alocarProcessoNoDisco(MVN, tarefa, endereco, indiceProgramaNaLista);
	    }
        parametro = strtok (NULL, " ");
        instrucao = strtol(parametro,&parametro,16);
        MVN->disco.bloco[blocoAtual].posicao[endereco%0x1000] = instrucao/0x100;
        MVN->disco.bloco[blocoAtual].posicao[endereco%0x1000 + 1] = instrucao%0x100;
        processoAnterior = endereco/0x1000;
    }
}

int procurarPrograma(MVN_t * MVN, int id){
    int i;
    LinhaListaDeProgramas_t linhaProgramaAtual;
    for (i = 0; i < NUM_PROG ; i++)
    {
        linhaProgramaAtual = MVN->ListaDeProgramas.linha[i];
        if (linhaProgramaAtual.carregadoNoDisco && linhaProgramaAtual.idPrograma == id){
            return i;
        }
    }
    return NUM_PROG;
}

int procurarBlocoMemoriaVago(MVN_t * MVN){
	MapaDeMemoria_t mapa = MVN->MapaDeMemoria;
	LinhaMapaDeMemoria_t linha;
	int i;
	int maisAntigo = -1;
	int linhaMaisAntiga = 0;

	for (i=0; i < NUM_PROC; i++){
		linha = mapa.linha[i];
		if (!linha.busy)
			return i;
		else{
			if ((maisAntigo == -1) || (linha.tempoLU < maisAntigo)){
				maisAntigo = linha.tempoLU;
				linhaMaisAntiga = i;
			}
		}
	}

	return linhaMaisAntiga;
}

void copiarDiscoParaMemoria(MVN_t *MVN, int blocoMemoria, int blocoDisco){
	int enderecoBaseMemoria = blocoMemoria * 0x1000;
	BlocoDisco_t bloco = MVN->disco.bloco[blocoDisco];
	int i;

	for (i=0; i < 0x1000; i++){
		MVN->memoria[enderecoBaseMemoria + i] = bloco.posicao[i];
	}
}

void alocarProcessoNaMemoria (MVN_t * MVN, int tarefa, int processo, int indiceListaDeProgramas){
	LinhaListaDeProgramas_t* programaAtual = &MVN->ListaDeProgramas.linha[indiceListaDeProgramas];
	int blocoDaMemoriaAOcupar = procurarBlocoMemoriaVago(MVN);
	int blocoDisco = programaAtual->MapaDeProcessos.linha[processo].blocoDisco;

	copiarDiscoParaMemoria(MVN, blocoDaMemoriaAOcupar, blocoDisco);

	programaAtual->MapaDeProcessos.linha[processo].blocoMemoriaFisica = blocoDaMemoriaAOcupar;

	MVN->MapaDeMemoria.linha[blocoDaMemoriaAOcupar].idProgramaOcupando = tarefa;
	MVN->MapaDeMemoria.linha[blocoDaMemoriaAOcupar].busy = true;
}

void iniciarPrograma (MVN_t * MVN, int tarefa){ //Inicia a execucao do programa, perguntando se o usuario quer mudar o valor do CI
	int indiceProgramaNaLista = procurarPrograma(MVN, tarefa);
	if (indiceProgramaNaLista == NUM_PROG){
		printf("\n\nNAO ACHEI O PROGRAMA\n\n");
	}
	else{
		LinhaListaDeProgramas_t* programaAtual = &MVN->ListaDeProgramas.linha[indiceProgramaNaLista];
		int enderecoLogico = programaAtual->ultimoCi;
		int processoInicial = enderecoLogico/0x1000;
		if (programaAtual->MapaDeProcessos.linha[processoInicial].blocoMemoriaFisica == -1){
			alocarProcessoNaMemoria(MVN, tarefa, processoInicial, indiceProgramaNaLista);
		}
		MVN->ci = (programaAtual->MapaDeProcessos.linha[processoInicial].blocoMemoriaFisica * 0x1000) + (enderecoLogico%0x1000);
	}


    printf("\nO valor atual no CI eh: %04x", MVN->ci);
    printf("\nDeseja altera-lo?[y/n]");
    char resp[100];
    scanf("%s",resp);
    if (resp[0] == 'y'){
        printf("Digite o novo valor no CI em hexadecimal:");
        scanf("%x",&MVN->ci);
    }
}

int procurarEnderecoProcesso(MVN_t* MVN, int idPrograma, int processo ,  evento_t* head, evento_t* atual){
    int i;
    int linhaPrograma = procurarPrograma(MVN, idPrograma);
    //printf("\n%d %d",linhaPrograma, processo);
    if (linhaPrograma == NUM_PROG){
        printf("\n\nNAO ACHEI O PROGRAMA\n\n");
        return MEM_TAM;
    }
    else{
        MapaDeProcessos_t mapaDeProcessos = MVN->ListaDeProgramas.linha[linhaPrograma].MapaDeProcessos;
        LinhaMapaDeProcessos_t linhaAtualProcesso = mapaDeProcessos.linha[processo];

        if (linhaAtualProcesso.blocoMemoriaFisica != -1){
        	return linhaAtualProcesso.blocoMemoriaFisica * 0x1000;
        }
        inserir_evento(head,atual->next->nome,atual->next->tempo,atual->next->tarefa,atual->next->breakpoint);
        inserir_evento(head,'A', atual->next->tempo -1 , atual->next->tarefa, atual->next->breakpoint);
        MVN->memoria[MEM_TAM-1] = processo ;
        MVN->memoria[MEM_TAM-2] = linhaPrograma ;
        return -1;
    }
}


int calcularEnderecoEfetivo(MVN_t* MVN,  evento_t* head, evento_t* atual){
    int i;
    int banco = MVN->ci/0x1000;
    int progId;
    if (!MVN->MapaDeMemoria.linha[banco].busy){
        printf("\n\nO BANCO NAO TA SENDO USADO\n\n");
        return 0;
    }
    progId = MVN->MapaDeMemoria.linha[banco].idProgramaOcupando;
    int procId = MVN->arg/0x1000;
    int enderecoOrigem = procurarEnderecoProcesso(MVN, progId, procId, head, atual);
    if (enderecoOrigem == MEM_TAM){
        return 0;
    }
    else if (enderecoOrigem == -1){
      return -1;
    }
    else{
        MVN->arg = MVN->arg%0x1000 + enderecoOrigem;
        return 1;
    }
}

int procurarProcesso(MVN_t* MVN,int linhaPrograma, int banco){
    int i;
    for (i = 0; i < NUM_PROC; i++){
        if (MVN->ListaDeProgramas.linha[linhaPrograma].MapaDeProcessos.linha[i].blocoMemoriaFisica == banco)
            return i;
    }
    return NUM_PROC;
}

int calcularEnderecoLogico(MVN_t* MVN, int enderecoFisico){
    //printf("\n\n O ENDEREÇO FISICO EH %04x E O MOD 0X1000 EH %04x", enderecoFisico, enderecoFisico%0x1000);
    int banco = enderecoFisico/0x1000;
    int idPrograma = MVN->MapaDeMemoria.linha[banco].idProgramaOcupando;
    int linhaPrograma = procurarPrograma(MVN,idPrograma);
    int processo = procurarProcesso(MVN, linhaPrograma, banco);
    return (processo*0x1000 + enderecoFisico%0x1000);
}

void fetch (MVN_t *MVN){ // Armazena em MVN->instrucao a proxima instrucao a ser executada
    MVN->instrucao = MVN->memoria[MVN->ci]*0x100 + MVN->memoria[MVN->ci+1];
}

void decodificar(MVN_t *MVN, evento_t* head, evento_t* atual){ //Separa a instrucao em instrucao e argumento
    uint16_t aux;
    uint16_t aux2;
    bool processoEncontrado;
    aux = MVN->instrucao;
    aux2 = MVN->instrucao;
    int banco = (MVN->ci/0x1000) * 0x1000;

    MVN->instrucao = aux/0x1000;
    MVN->arg = (aux2 & 0x0FFF);
    if (MVN->modoIndireto){
        MVN->arg = MVN->memoria[banco + MVN->arg]*0x100 + MVN->memoria[banco + MVN->arg+1];
        processoEncontrado = calcularEnderecoEfetivo(MVN, head, atual);
        MVN->arg = MVN->arg;
        if (processoEncontrado == 0){
            printf("\n\n************** PROCESSO OU PROGRAMA NAO ENCONTRADO **************\n\n");
        }
        else if (processoEncontrado == 1){
            inserir_evento(head,'E',atual->next->tempo+1,atual->next->tarefa,false);
        }
        else {
            //printf("\n\nNECESSARIO ALOCAR PROCESSO\n\n");
            MVN->instrucao = aux;
        }
    }
    else{
      MVN->arg = banco + MVN->arg;
      inserir_evento(head,'E',atual->next->tempo+1,atual->next->tarefa,false);
    }
}

void executar (MVN_t *MVN, int idPrograma){ //Executa a instrucao

    FILE *fp;
    char arquivo [100];
    char numero[5];
    char byte[2];
    int banco = (MVN->ci/0x1000) * 0x1000;
    int linhaPrograma = procurarPrograma(MVN, idPrograma);
    MVN->modoIndireto = false;
    MVN->ListaDeProgramas.linha[linhaPrograma].ultimoModoIndireto = MVN->modoIndireto;

    switch(MVN->instrucao){
        case 0x0: // Jump Incoditional
            MVN->ci = MVN->arg;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->arg);
        break;

        case 0x1:
            if (MVN->acc == 0){ // Jump if Zero
                MVN->ci = MVN->arg;
                MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->arg);
            }
            else{
                MVN->ci = MVN->ci + 2;
                MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->ci);
            }
        break;

        case 0x2:
            if ((MVN->acc/0x8000) == 0x1){ // Jump if Negative
                MVN->ci = MVN->arg;
                MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->arg);
            }
            else{
                MVN->ci = MVN->ci + 2;
                MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->ci);
            }
        break;

        case 0x3: // Load Value
            MVN->acc = MVN->arg & 0x0FFF;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoAcc = MVN->acc;
            MVN->ci = MVN->ci + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0x4: // Add
            MVN->acc = MVN->acc + MVN->memoria[MVN->arg];
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoAcc = MVN->acc;
            MVN->ci = MVN->ci +2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi =  calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0x5: // Sub
            MVN->acc = MVN->acc + (~(MVN->memoria[MVN->arg])+1);
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoAcc = MVN->acc;
            MVN->ci = MVN->ci + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi =  calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0x6: // Multiply
            MVN->acc = MVN->acc * MVN->memoria[MVN->arg];
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoAcc = MVN->acc;
            MVN->ci = MVN->ci + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0x7: // Divide
            MVN->acc = MVN->acc/MVN->memoria[MVN->arg];
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoAcc = MVN->acc;
            MVN->ci = MVN->ci + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi =  calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0x8: //Load from memory
            MVN->acc = MVN->memoria[MVN->arg];
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoAcc = MVN->acc;
            MVN->ci = MVN->ci + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0x9: //Store
            MVN->memoria[MVN->arg] = (0x00FF & MVN->acc);
            MVN->ci = MVN->ci + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0xA: // Call subroutine
            MVN->memoria[MVN->arg] = ((MVN->ci+2)/0x100);
            MVN->memoria[MVN->arg + 1] = (0x00FF & (MVN->ci+2));
            MVN->ci = MVN->arg + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0xB: // Entrar Modo Indireto
            MVN->modoIndireto = true;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoModoIndireto= MVN->modoIndireto;
            MVN->ci = MVN->ci +2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi =  calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0xC: // Halt Machine
            estado_maquina(MVN);
            printf("\nPressione qualquer tecla para continuar\n");
            getch();
            MVN->ci = MVN->arg;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi = calcularEnderecoLogico(MVN,MVN->arg);
        break;

        case 0xD: // Input
            switch ((MVN->arg - banco)/0x100){
                case 0:
                    printf("\nO que colocar no acumulador?");
                    uint16_t k;
                    scanf("%x",&k);
                    MVN->acc = k;
                break;

                case 1:
                    strcpy(arquivo,"file");
                    itoa((MVN->arg - 0x100*(MVN->arg/0x100)),numero,10);
                    strcat(arquivo,numero);
                    strcat(arquivo,".txt");
                    fp = fopen(arquivo,"rb");
                    fseek(fp,MVN->cursorEntrada,SEEK_SET);
                    MVN->acc = getc(fp);
                    MVN->cursorEntrada = ftell(fp);
                    fclose(fp);
                break;

                case 2:
                    MVN->cursorEntrada = 0;
                break;

                case 3:
                    scanf("%s",MVN->buffer);
                break;

                case 4:
                    MVN->acc = MVN->buffer[0];
                    MVN->buffer[0] = MVN->buffer[1];
                break;

            }
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoAcc = MVN->acc;
            MVN->ci = MVN->ci + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi =  calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0xE: // Output
            switch ((MVN->arg - banco)/0x100){
                case 0:
                    printf("%c",MVN->acc);
               break;

                case 1:
                    strcpy(arquivo,"file");
                    itoa((MVN->arg - 0x100*(MVN->arg/0x100)),numero,10);
                    strcat(arquivo,numero);
                    strcat(arquivo,".txt");
                    fp = fopen(arquivo,"ab");
                    putc(MVN->acc,fp);
                    fclose(fp);
                break;

                case 2:
                    printf("%d",MVN->acc);
                break;
            }
            MVN->ci = MVN->ci + 2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi =  calcularEnderecoLogico(MVN,MVN->ci);
        break;

        case 0xF: // OS
            MVN->ci = MVN->ci+2;
            MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi =  calcularEnderecoLogico(MVN,MVN->ci);
        break;
    }
}

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


    /**Definir como ser� feita a leitura do arquivo de eventos e em quais parametros aquilo que foi lido sera alocado**/

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

void loopDeSimulacao(evento_t *listaDeEventos, MVN_t * MVN,bool *acompanhamento,bool *fim){ //Loop de Simulacao. Retira o primeiro elemento da lista e o resolve.
    evento_t *atual = malloc(sizeof(evento_t));
    atual->next = malloc(sizeof(evento_t));
    atual->next->next = NULL;
    while (listaDeEventos->next != NULL & *fim == false){ //caso fim seja true, encerra a simulacao
        pop_evento(&listaDeEventos,&atual);
        seletorDeEvento(listaDeEventos,atual,MVN, acompanhamento,fim);
    }
    printf("\nAcabei!");
}

void menu(evento_t * listaDeEventos, MVN_t * MVN, bool *acompanhamento, bool *fim){ //Menu Inicial de Simulacao
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
            menu(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '2':
            print_list(listaDeEventos);

            /**Colocar aqui a funcao de estado do objeto simulado**/

            estado_maquina(MVN);

            /**Termino**/
            menu(listaDeEventos,MVN,acompanhamento,fim);
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
            menu(listaDeEventos,MVN,acompanhamento,fim);

            /**Termino**/
        break;

        case '4':
            if (*acompanhamento == false){
                *acompanhamento = true;
                printf("\nO acompanhamento agora esta ligado");
            }
            else{
                *acompanhamento = false;
                printf("\nO acompanhamento agora esta ligado");
            }
            menu(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '5':
            printf("\nDigite o tempo no qual uma pausa deva ser inserida:");
            int tempop;
            scanf("%d",&tempop);
            inserir_evento(listaDeEventos,'S',tempop,0,false);
            menu(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '6':
            loopDeSimulacao(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '7':
            print_list(listaDeEventos);

            /**Chamar a funo de estado dos objetos simulados**/

            estado_maquina(MVN);

            /**Termino**/

        break;

        default:
            printf("\n\nOpcao Invalida");
            menu(listaDeEventos,MVN,acompanhamento,fim);
    }

}

void menuDePausa(evento_t * listaDeEventos, MVN_t * MVN, bool *acompanhamento,bool *fim){ //Menu de Pausa
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
            estado_maquina(MVN);
            menuDePausa(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '2':
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
            menuDePausa(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '3':
            if (*acompanhamento == false)
                *acompanhamento = true;
            else
                *acompanhamento = false;
            menuDePausa(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '4':
            printf("\nDigite o tempo no qual uma pausa deva ser inserida:");
            int tempop;
            scanf("%d",&tempop);
            inserir_evento(listaDeEventos,'S',tempop,0,false);
            menuDePausa(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '5':
            loopDeSimulacao(listaDeEventos,MVN,acompanhamento,fim);
        break;

        case '6':
            print_list(listaDeEventos);
            estado_maquina(MVN);
            *fim = true;
        break;

        default:
            printf("\n\nOpcao Invalida");
            menuDePausa(listaDeEventos,MVN,acompanhamento,fim);
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

    /** Implemente aqui os par�metros do seu evento**/

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

    /** Implemente aqui os par�metros do seu evento**/

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

void seletorDeEvento (evento_t * head, evento_t *atual, MVN_t * MVN, bool *acompanhamento, bool *fim){ //L� qual o tipo de evento e toma a a��o adequada.

    /**Nesta parte deve ser implementado como o evento deve ser identificado a partir de seus par�metros e como o simulador deve reagir a cada evento**/

    char op;
    op = atual->next->nome;
    int tarefa = atual->next->tarefa;
    if (op == 'T' || op == 'C'){
        switch(op){
            case 'C'://Carregar Programa e iniciar execu��o na MVN
                printf("\nDigite o nome do arquivo que contem o programa a ser carregado: ");
                char file[100];
                scanf("%s",file);
                MVN->modoSupervisor = true;
                MVN->programaAtivo = tarefa;
                carregarPrograma(MVN, file, tarefa);
                //estado_maquina(MVN);
            break;

            case 'T':
                MVN->programaAtivo = tarefa;
                int linhaPrograma = procurarPrograma(MVN, tarefa);
                int enderecoProcesso = procurarEnderecoProcesso(MVN, tarefa, MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi/0x1000, head, atual);
                if (enderecoProcesso !=1 && enderecoProcesso != MEM_TAM){
                    MVN->acc = MVN->ListaDeProgramas.linha[linhaPrograma].ultimoAcc;
                    MVN->modoIndireto = MVN->ListaDeProgramas.linha[linhaPrograma].ultimoModoIndireto;
                    MVN->ci = enderecoProcesso + (MVN->ListaDeProgramas.linha[linhaPrograma].ultimoCi & 0x0FFF);
                    inserir_evento(head, 'F', atual->next->tempo + 1, tarefa,false);
                }
            break;
        }
    }
    else if (MVN->programaAtivo == tarefa){
        switch(op){
            case 'A': // Alocar
                MVN->modoSupervisor = true;
                alocarProcessoNaMemoria(MVN, atual->next->tarefa, MVN->memoria[MEM_TAM-1],MVN->memoria[MEM_TAM-2]);
            break;

            // case 'N':
            // 	fetch(MVN);
            // 	decodificar(MVN);
            // 	executar(MVN);
            // 	inserir_evento(head,'N',atual->next->tempo+1,atual->next->tarefa,false);
            // break;

            case 'D':
                MVN->modoSupervisor = false;
                decodificar(MVN, head, atual);//Decodificar
                //estado_maquina(MVN);
            break;

            case 'E': // ExecutarMVN,
                MVN->modoSupervisor = false;
                executar(MVN, atual->next->tarefa);
                //estado_maquina(MVN);
                inserir_evento(head,'F', atual->next->tempo+1, atual->next->tarefa,false);
                // if (*acompanhamento == true){ //caso o acompanhamento passo a passo esteja ligado, pausa a simulacao.
                //     menuDePausa(head,MVN,acompanhamento,fim);
                //}
            break;

            case 'I'://Iniciar Programa
                MVN->modoSupervisor = true;
                iniciarPrograma(MVN, tarefa);
                inserir_evento(head,'F',atual->next->tempo+1, atual->next->tarefa,false);
            break;

            case 'F': //Fetch
                MVN->modoSupervisor = false;
                fetch(MVN);
                //estado_maquina(MVN);
                inserir_evento(head,'D',atual->next->tempo+1,atual->next->tarefa,false);
            break;

            case 'P': //Print
                print_list(head);
                estado_maquina(MVN);
            break;

            case 'S'://Stop
                menuDePausa(head,MVN,acompanhamento,fim);
            break;

            default:
                printf("\nEvento Invalido");


        }
    }

    if (*acompanhamento == true){ //caso o acompanhamento passo a passo esteja ligado, pausa a simulacao.
            menuDePausa(head,MVN,acompanhamento,fim);
    }

    /**T�rmino da implementa��o**/
}

/** Programa principal **/

int main(){

    evento_t * listaDeEventos;

    listaDeEventos = malloc(sizeof(evento_t));
    listaDeEventos->next = NULL;

    MVN_t * MVN;
    MVN = malloc(sizeof(MVN_t));
    inicializarMVN(MVN);

    bool acompanhamento = false;
    bool fim = false;

    menu(listaDeEventos,MVN,&acompanhamento,&fim);

    return 0;
}
