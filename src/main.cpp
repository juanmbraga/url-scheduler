#include "escalonador.h"
#include "memlog.h"
#include "msgassert.h"
#include <iostream>
#include <getopt.h>     //getopt()
#include <string.h>     //strcpy()
#include <fstream>      //fstream, istream, ostream

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;

//Mapeamento de comandos do escalonador
#define ADD_URLS 1
#define ESCALONA_TUDO 2
#define ESCALONA 3
#define ESCALONA_HOST 4
#define VER_HOST 5
#define LISTA_HOSTS 6
#define LIMPA_HOST 7
#define LIMPA_TUDO 8

//Extensao da entrada a ser recebida, e pelo que substituir na saida
#define EXTENSAOENTRADA ".txt"
#define EXTENSAOSAIDA "-out.txt" 

//Variaveis globais para registro de parametros de entrada
bool deveRegistrarMemoria, ativaBreadthFirst, ativaBestFirst;
char arquivoRegistro[25] = "registro_de_acessos.txt";

//Variaveis globais para uso na impressao pela estrutura e pela funcao parseArgs()
char arquivoEntrada[100];
std::ofstream arquivoSaida;

// Funcoes auxiliares
// Em um editor de textos compativel (como o VSCode da Microsoft),
// posicione o ponteiro do mouse sobre o nome da funcao para ler sua descricao.

void menu(void);
void parseArguments(int argc, char** argv);
string nomeSaida(char* nomeEntrada);
bool terminaCom(string& nomeEntrada, string termino);
int pegaComando(ifstream& entrada);


int main(int argc, char* argv[]){

    //Variaveis
    ifstream entrada;
    string endereco;
    int comando, quantidade;

    //Estrutura de dados
    ListaHost lista;

    //Coleta o nome do arquivo de entrada e interpreta os parametros recebidos
    parseArguments(argc,argv);
    
    //Abre o arquivo
    entrada.open(arquivoEntrada);
    erroAssert(entrada.is_open(), "escalonador - nao foi possivel abrir o arquivo de entrada");
    arquivoSaida.open(nomeSaida(arquivoEntrada));
    erroAssert(arquivoSaida.is_open(), "escalonador - nao foi possivel abrir o arquivo de saida");

    //Inicia Memlog
    iniciaMemLog(arquivoRegistro);

    //Define se devera ser produzido um registro dos acessos
    if(deveRegistrarMemoria){
        ativaMemLog();
    } else {
        desativaMemLog();
    }

    //Enquanto houverem linhas a serem lidas no arquivo
    while(!entrada.eof()){
        //A primeira capturada deve ser um comando
        comando = pegaComando(entrada);

        switch(comando){
            case ADD_URLS:
                entrada >> quantidade;
                //Insercao ehfeita apenas se quantidade for adequada, senao apenas ignore
                if(quantidade > 0){
                    defineFaseMemLog(0);
                    for(int i=0; (i<quantidade) && (quantidade>0) ; i++){
                        entrada >> endereco;
                        lista.insere(endereco);
                    }
                }
                break;

            case ESCALONA_TUDO:
                if(ativaBreadthFirst){
                    lista.escalonaTudoBreadthFirst();
                } else if(ativaBestFirst){
                    lista.escalonaTudoBestFirst();
                } else {
                    defineFaseMemLog(1);
                    if(deveRegistrarMemoria) lista.acessa();
                    lista.escalonaTudo();
                }
                break;

            case ESCALONA:
                entrada >> quantidade;
                if(ativaBreadthFirst){
                    lista.escalonaBreadthFirst(quantidade);
                } else if(ativaBestFirst){
                    lista.escalonaBestFirst(quantidade);
                } else {
                    lista.escalona(quantidade);
                }
                break;

            case ESCALONA_HOST:
                entrada >> endereco;
                entrada >> quantidade;
                lista.escalona(endereco,quantidade);
                break;

            case VER_HOST:
                entrada >> endereco;
                lista.verHost(endereco);
                break;

            case LISTA_HOSTS:
                lista.listaHosts();
                break;

            case LIMPA_HOST:
                entrada >> endereco;
                lista.limpa(endereco);
                break;

            case LIMPA_TUDO:
                lista.limpaTudo();
                break;

            default:
                //Comandos incorretos ou mal-formados nao interrompem a execucao
                break;
        }
    }

    if(deveRegistrarMemoria){
        defineFaseMemLog(1);
        lista.acessa();
        lista.imprime();
    }

    //Finaliza o registro de memoria
    finalizaMemLog();

    return 0;
}


/**
 * \brief Exibe no terminal as opcoes de parametro que o programa suporta ao ser chamado.
 */
void menu(){
    cout << "Opcoes:" << endl;
    cout << "\t <entrada.txt \t\t (nome do arquivo de entrada)" << endl;
    cout << "\t -m \t\t (ativar registro de acessos a memoria para 'registro_de_acessos.txt')" << endl;
    cout << "\t -d \t\t (ativar estrategia 'Breadth-First' no escalonamento)" << endl;
    cout << "\t -t \t\t (ativar estrategia 'Best-First' no escalonamento)" << endl;
}


/**
 * \brief Processa os argumentos e parametros recebidos pelo programa.
 * \param argc inteiro que indica o numero de argumentos recebidos pela funcao main.
 * \param argv vetor de ponteiros para os argumentos em formato string.
 */
void parseArguments(int argc, char ** argv){

    // Variavel auxiliar
    int option_index = -1;
    
    // Inicializando variaveis globais
    deveRegistrarMemoria = false;
    ativaBreadthFirst = false;
    ativaBestFirst = false;

    // Processa os argumentos passados na chamada do programa
    while((option_index = getopt(argc,argv,"mdt")) != EOF){
        switch(option_index){
            case 'm':
                deveRegistrarMemoria = true;
                break;
            case 'd':
                ativaBreadthFirst = true;
                break;
            case 't':
                ativaBestFirst = true;
                break;
            default:
                menu();
                exit(-1);
        }
    }

    //ehpreciso ter recebido um argumento com o nome
    erroAssert(optind < argc,"escalonador - nome de arquivo de entrada tem que ser definido");

    // Se ele foi recebido, cheque o tamanho da string
    option_index = strlen(argv[optind]);    //Reutilizando a variavel
    erroAssert(option_index > 0,"escalonador - nome de arquivo de entrada tem que ser definido");
    erroAssert(option_index < int(sizeof(arquivoEntrada)),"escalonador - nome de arquivo de entrada ehmaior que o suportado");
    
    //Se parametro foi passado e ehde tamanho adequado, colete-o
    strcpy(arquivoEntrada, argv[optind]);    
    
    // Demais verificacoes para inegridade da execucao
    erroAssert(!(ativaBreadthFirst && ativaBestFirst),"escalonador - apenas um desafio pode ser ativado por vez");
}


/**
 * \brief Transforma o nome do arquivo de entrada no de saida esperado.
 * \param nomeEntrada string com o nome do arquivo que foi aberto.
 * \return String com o nome acrescido de -out antes da extensao ".txt", quando presente.
 */
string nomeSaida(char* nomeEntrada){
    erroAssert(strlen(nomeEntrada) > 0,"escalonador - nome de arquivo de entrada tem que ser definido ou nao foi recebido");

    //Gera uma nova string com o nome a processar
    string saida(nomeEntrada);
    string::size_type posicao;

    //Se a extensao for ".txt", remova-a
    if(terminaCom(saida,EXTENSAOENTRADA)){
        posicao = saida.find_last_of(EXTENSAOENTRADA);
        size_t tamanho = strlen(EXTENSAOENTRADA) -1;
        saida.erase(posicao-tamanho, strlen(EXTENSAOENTRADA));
    }
    
    //Acrescenta a extensao desejada no fim da string e a devolve
    return (saida + EXTENSAOSAIDA);
}


/**
 * \brief Verifica se string termina com uma segunda string.
 * \param nomeEntrada ehuma string que tera seus ultimos caracteres analisados.
 * \param termino ehuma string que contem o conteudo a se procurar no fim da primeira string.
 * \return True se a segunda string for encontrada no fim da primeira.
 */
bool terminaCom(string& nomeEntrada, string termino){
    //Credito: https://stackoverflow.com/questions/874134
    if (nomeEntrada.length() >= termino.length()) {
        return (0 == nomeEntrada.compare (nomeEntrada.length() - termino.length(), termino.length(), termino));
    } else {
        return false;
    }
}


/**
 * \brief Coleta e identifica um comando do arquivo de entrada.
 * \param entrada eho arquivo a se extrair o comando.
 * \return Valores de 1-8 correspondentes aos comandos disponiveis, ou zero caso o comando nao for identificado.
 */
int pegaComando(ifstream& entrada){
    erroAssert(entrada.is_open(),"escalonador - nao foi possivel abrir o arquivo de entrada");

    //Variaveis
    string comando;
    int opcao;

    //Coleta um comando
    entrada >> comando;

    //Compare o comando obtido com as opcoes validas
    if       (comando == "ADD_URLS"){
        opcao = 1;
    } else if(comando == "ESCALONA_TUDO"){
        opcao = 2;
    } else if(comando == "ESCALONA"){
        opcao = 3;
    } else if(comando == "ESCALONA_HOST"){
        opcao = 4;
    } else if(comando == "VER_HOST"){
        opcao = 5;
    } else if(comando == "LISTA_HOSTS"){
        opcao = 6;
    } else if(comando == "LIMPA_HOST"){
        opcao = 7;
    } else if(comando == "LIMPA_TUDO"){
        opcao = 8;

    //O valor ehzero caso for recebido um comando mal-formado ou uma url coletada por engano
    } else {
        opcao = 0;
    }

    return opcao;
}