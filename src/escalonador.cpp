#include "escalonador.h"
#include "msgassert.h"
#include "memlog.h"
#include <iostream>
#include <regex>
#include <fstream>

using std::cout;
using std::string;
using std::regex;
using std::ofstream;
using std::endl;

//Protocolo a ser aceito
#define PROTOCOLO "http://"

//Identificadores das entidades para registro de memoria
#define URL_STRING_ID 0
//#define URL_CLASS_ID 1
#define HOST_STRING_ID 2
//#define HOST_CLASS_ID 3

//Variaveis globais para escrita no arquivo
//Devem ser inicializadas na main()
extern char* arquivoEntrada;
extern ofstream arquivoSaida;


Url* criaUrl(string& endereco);

/**
 * \brief Destroi a lista e todo o seu conteudo.
 */
ListaHost::~ListaHost(){
    limpaTudo();
}


/**
 * \brief Insere uma Url dentro da lista do respectivo Host.
 * \param nova e uma string contendo o endereco web.
 */
void ListaHost::insere(string& url){

    Host *anterior, *atual;
    string nome;
    Url* nova;
    bool encontrado = false;

    //Cria uma Url a partir do endereco
    nova = criaUrl(url);

    //Urls invalidas nao devem interromper o programa
    if(nova->eValida() == false){
        delete nova;
        return;
    }

    //Extrai o Host do endereco
    nome = nova->extraiHost();

    //Se a lista estiver vazia, insira um novo Host
    if(_tamanho < 1){
        this->_hosts = new Host(nova);
        erroAssert(this->_hosts != nullptr, "escalonador - nao foi possivel alocar memoria");
        //ESCREVEMEMLOG((long int)(nova),sizeof(double),HOST_CLASS_ID);
        _tamanho = 1;
    
    } else {
        //Se houver ao menos um elemento na lista, procure pelo Host
        atual = this->_hosts;
        while(atual != nullptr && !encontrado){
            //LEMEMLOG((long int)(atual),sizeof(double),HOST_CLASS_ID);
            LEMEMLOG((long int)(atual->nome().front()),sizeof(double),HOST_STRING_ID);
            if(atual->nome() == nome){
                encontrado = true;
            } else {
                anterior = atual;
                atual = atual->proximo();
            }
        }
        
        //Se ha uma lista do Host do endereco
        if(encontrado){
            atual->insere(nova);
        
        //Senao, o fim da lista foi atingido
        } else {
            atual = new Host(nova);
            erroAssert(atual != nullptr, "escalonador - nao foi possivel alocar memoria");
            //ESCREVEMEMLOG((long int)(atual),sizeof(double),HOST_CLASS_ID);
            anterior->_proximo = atual;
            _tamanho++;
        }
    }
}


/**
 * \brief Auxilia na criacao de uma Url dinamica.
 * \param endereco e uma string que contem o endereco web a se armazenar.
 * \return Ponteiro para tipo Url armazenando o endereco web.
 */
Url* criaUrl(string& endereco){
    erroAssert(!endereco.empty(),"escalonador - endereco nao pode ser encontrado");
    Url* nova = new Url(endereco);
    erroAssert(nova != nullptr, "escalonador - o programa nao pode alocar memoria");
    
    //Registra o endereco de criacao da classe
    //ESCREVEMEMLOG((long int)(nova),sizeof(double),URL_CLASS_ID);
    
    return nova;
}


/**
 * \brief Percorre a fila de Hosts na e escalona as Urls na ordem de profundidade, de cada Host na ordem em que foram conhecidos.
 * \param quantidade e um inteiro com o numero de Urls a escalonar.
 */
void ListaHost::escalona(int quantidade){
    //Se quantidade estiver incorreta ou nao houver hosts, nao faca nada
    if(quantidade < 1 || this->_tamanho < 1) return;

    Host* atual = this->_hosts;
    erroAssert(atual != nullptr, "escalonador - lista de hosts nao encontrada");

    //Solicita o escalonamento para todos os hosts, um a um
    while(quantidade > 0 && atual != nullptr){
        //LEMEMLOG((long int)(atual),sizeof(double),HOST_CLASS_ID);
        if(atual->vazio()){
            atual = atual->proximo();
        } else {
            atual->escalona();
            quantidade--;
        }
    }
}


/**
 * \brief Escalona uma quantidade de enderecos de um dado Host.
 * \param host e uma string com o endereco do host.
 * \param quantidade e um inteiro com o numero de Urls a escalonar.
 */
void ListaHost::escalona(string& host, int quantidade){
    //Nao faca nada caso nao houverem hosts, nenhum escalonamento a ser feito ou host incorreto
    if(host.empty() || quantidade < 1 || this->_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    //Procura por host com mesmo nome
    Host* encontrado = this->procuraHost(host);

    //Se nao houver host com o nome, encerre o metodo
    if(encontrado == nullptr) return;

    //Se houver, escalone a quantidade desejada
    for(int i=0; (i<quantidade && !encontrado->vazio()); i++){
        encontrado->escalona();
    }
}


/**
 * \brief Escalona todos os enderecos armazenados em todos os Hosts.
 */
void ListaHost::escalonaTudo(){
    // Nao e necessario escalonar se nao houverem hosts
    if(this->_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    Host* atual = this->_hosts;

    //Escalona enderecos dos hosts um a um, ate atingir o fim da lista
    while(atual != nullptr){
        //LEMEMLOG((long int)(atual),sizeof(double),HOST_CLASS_ID);
        if(atual->vazio()){
            atual = atual->proximo();
        } else {
            atual->escalona();
        }
    }
}


/**
 * \brief Imprime no arquivo os enderecos de um dado Host.
 * \param host e uma string contendo o nome do host.
 */
void ListaHost::verHost(string& host){

    //Se nao houverem hosts ou o host passado estiver incorreto, saia
    if(host.empty() || this->_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    //Procura por host com mesmo nome
    Host* encontrado = this->procuraHost(host);

    if(encontrado == nullptr) return;

    //Solicita a visualizacao 
    encontrado->verEnderecos(); 
}


/**
 * \brief Imprime no arquivo o nome de todos os Hosts na ordem em que foram conhecidos.
 */
void ListaHost::listaHosts(){

    //Saia caso nao houverem hosts
    if(this->_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    Host* atual = this->_hosts;

    while(atual != nullptr){
        //LEMEMLOG((long int)(atual),sizeof(double),HOST_CLASS_ID);
        atual->verHost();
        atual = atual->proximo();
    }
}


/**
 * \brief Escalona todos os enderecos armazenados, removendo um de cada Host por vez.
 */
void ListaHost::escalonaTudoBreadthFirst(){
    // Nao e necessario escalonar se nao houverem hosts
    if(this->_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    Host* atual;
    bool existeHostNaoVazio;

    //Enquanto houver Hosts com elementos
    do{
        atual = this->_hosts;
        erroAssert(atual != nullptr, "escalonador - lista de hosts nao encontrada");
        existeHostNaoVazio = false;
        //Escalona enderecos dos hosts um a um, ate atingir o fim da lista
        while(atual != nullptr){
            if(atual->vazio() == false){
                atual->escalona();
                if(atual->vazio() == false){
                    existeHostNaoVazio = true;
                }
            }
            atual = atual->proximo();
        }
    } while(existeHostNaoVazio);
}


/**
 * \brief Percorre a fila de Hosts e escalona um por vez, ate escalonar a quantidade informada.
 * \param quantidade e um inteiro com o numero de Urls a escalonar.
 */
void ListaHost::escalonaBreadthFirst(int quantidade){
    //Se quantidade estiver incorreta ou nao houver hosts, nao faca nada
    if(quantidade < 1 || this->_tamanho < 1) return;

    Host* atual;
    bool existeHostNaoVazio;

    //Enquanto houver Hosts com elementos
    do{
        atual = this->_hosts;
        erroAssert(atual != nullptr, "escalonador - lista de hosts nao encontrada");
        existeHostNaoVazio = false;
        //Escalona enderecos dos hosts um a um, ate atingir o fim da lista
        while(atual != nullptr && quantidade > 0){
            if(atual->vazio() == false){
                atual->escalona();
                if(atual->vazio() == false) {
                    existeHostNaoVazio = true;
                }
                quantidade--;
            }
            atual = atual->proximo();
        }
    } while((existeHostNaoVazio) && (quantidade > 0));
}


/**
 * \brief Escalona todos os enderecos armazenados, removendo um do primeiro Host que tiver mais elementos por vez.
 */
void ListaHost::escalonaTudoBestFirst(){
    // Nao e necessario escalonar se nao houverem hosts
    if(this->_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    Host *atual, *melhor;
    int maior;

    //Enquanto houver Hosts com elementos
    do{
        atual = this->_hosts;
        erroAssert(atual != nullptr, "escalonador - lista de hosts nao encontrada");
        maior = 0;
        //Procura pelo primeiro Host com o maior numero de elementos
        while(atual != nullptr){
            if(atual->tamanho() > maior){
                maior = atual->tamanho();
                melhor = atual;
            }
            atual = atual->proximo();
        }

        //Se Houver host com algum elemento, escalone-o
        if(maior != 0){
            melhor->escalona();
        }
    } while(maior != 0);
}


/**
 * \brief Percorre a fila de Hosts e escalona o primeiro que tiver mais enderecos, ate escalonar a quantidade informada.
 * \param quantidade e um inteiro com o numero de Urls a escalonar.
 */
void ListaHost::escalonaBestFirst(int quantidade){
    //Se quantidade estiver incorreta ou nao houver hosts, nao faca nada
    if(quantidade < 1 || this->_tamanho < 1) return;

    Host* atual, *melhor;
    int maior;

    //Enquanto houver Hosts com elementos
    do{
        atual = this->_hosts;
        erroAssert(atual != nullptr, "escalonador - lista de hosts nao encontrada");
        maior = 0;

        //Escalona enderecos dos hosts um a um, ate atingir o fim da lista
        while(atual != nullptr){
            if(atual->tamanho() > maior){
                maior = atual->tamanho();
                melhor = atual;
            }
            atual = atual->proximo();
        }

        if(maior != 0){
            melhor->escalona();
            quantidade--;
        }
    } while((maior != 0) && (quantidade > 0));
}


/**
 * \brief Limpa a lista de enderecos de um dado Host.
 * \param host e uma string contendo o nome do Host.
 */
void ListaHost::limpa(string& host){
    if(host.empty() || this->_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    //Procura por host com mesmo nome
    Host* encontrado = this->procuraHost(host);

    if(encontrado == nullptr) return;

    encontrado->esvazia();
}


/**
 * \brief Limpa todos os Hosts e seus enderecos.
 */
void ListaHost::limpaTudo(){
    if(_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    Host* aux;

    do{
        aux = this->_hosts;
        this->_hosts = aux->proximo();
        aux->~Host(); //Chama o destrutor
    } while(this->_hosts != nullptr);

    _tamanho = 0;
}


/**
 * \brief Busca na fila um Host com o mesmo nome que o fornecido.
 * \param host e uma string com o nome do Host.
 * \return Ponteiro para um Host na Fila, se existir. Caso contrario, retorna nullptr.
 */
Host* ListaHost::procuraHost(string& host){
    //Host nao existe se lista estiver vazia, ou nome recebido estiver vazio
    if(this->_tamanho < 1 || host.empty()) return nullptr;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    Host* atual = this->_hosts;
    bool encontrado = false;

    //Tenta encontrar Host
    while(atual != nullptr && !encontrado){
        //LEMEMLOG((long int)(atual),sizeof(double),HOST_CLASS_ID);
        LEMEMLOG((long int)(atual->nome().front()),sizeof(double),HOST_STRING_ID);
        if(atual->nome() == host){
            encontrado = true;
        } else {
            atual = atual->proximo();
        }
    }

    return atual;
}


/**
 * \brief Exibe no terminal toda a estrutura, incluindo Hosts e Urls.
 */
void ListaHost::imprime(){
    
    //Saia caso nao houverem hosts
    if(this->_tamanho < 1) return;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    Host* atual = this->_hosts;

    while(atual != nullptr){
        //LEMEMLOG((long int)(atual),sizeof(double),HOST_CLASS_ID);
        LEMEMLOG((long int)(atual->_nome.front()),sizeof(double),HOST_STRING_ID);
        cout << atual->_nome << endl;
        atual->imprimeEnderecos();
        atual = atual->proximo();
    }
}


/**
 * \brief Acessa todos os Hosts e enderecos
 */
double ListaHost::acessa(){
    
    //Saia caso nao houverem hosts
    if(this->_tamanho < 1) return 0;
    erroAssert(this->_hosts != nullptr, "escalonador - lista de hosts nao encontrada");

    Host* atual = this->_hosts;
    double qualquer=0;

    while(atual != nullptr){
        //LEMEMLOG((long int)(atual),sizeof(double),HOST_CLASS_ID);
        LEMEMLOG((long int)(atual->_nome.front()),sizeof(double),HOST_STRING_ID);
        qualquer += atual->_nome.front();
        qualquer += atual->acessaEnderecos();
        atual = atual->proximo();
    }

    return qualquer;
}


/**
 * \brief Constroi um objeto Host e insere um endereco.
 * \param endereco e uma Url.
 */
Host::Host(Url* endereco){
    //Como construtor e protegido e deve ser usado apenas pelo metodo de insercao da ListaHost, erro nao deve ocorrer
    erroAssert(endereco->eValida(), "escalonador - endereco incorreto recebido por metodo interno");

    this->_lista = endereco;
    this->_nome = endereco->extraiHost();
    ESCREVEMEMLOG((long int)(this->_nome.front()),sizeof(double),HOST_STRING_ID);
    this->_tamanho = 1;
    this->_proximo = nullptr;
}


/**
 * \brief Destroi a lista e todos os seus elementos
 */
Host::~Host(){
    this->esvazia();
}


/**
 * \brief Insere uma Url no Host de acordo com sua profundidade.
 * \param endereco e uma Url contendo o endereco.
 */
void Host::insere(Url* nova){
    //Como o metodo deve ser usado pela classe ListaHost, algumas assercoes devem ser feitas
    erroAssert(nova->extraiHost() == this->nome(),"escalonador - erro - Url foi inserida em Host incorreto");
    //Como o metodo deve ser usado apenas pela classe ListaHost, assert() e mais adequado que if()
    erroAssert(nova->eValida(),"escalonador - tentativa de insercao de Url invalida");

    Url *atual, *anterior;
    bool encontrada = false, repetida = false;

    //Nao existam elementos no Host
    if(this->tamanho() < 1){
        this->_lista = nova;
        this->_tamanho = 1;

    //Caso houverem
    } else {
        atual = this->_lista;

        //Procura por uma Url de nivel maior que aquela a ser inserida
        while(atual != nullptr && !encontrada && !repetida){
            //LEMEMLOG((long int)(atual),sizeof(double),URL_CLASS_ID);

            //Se o nivel da Url atual for maior que o da nova
            if(atual->nivel() > nova->nivel()){
                encontrada = true;

            //Se o nivel da Url atual for menor, cheque a proxima
            } else if(atual->nivel() < nova->nivel()){
                anterior = atual;
                atual = atual->_proxima;

            //Se os niveis forem iguais
            } else {
                //E elas tiverem o mesmo endereco, pare
                //Decidiu-se que comparacoes nao seriam contempladas na analise de espaco
                if(atual->_endereco == nova->_endereco){
                    repetida = true;
                
                //Se ela nao for repetida, continue
                } else {
                    anterior = atual;
                    atual = atual->_proxima;
                }
            }
        }

        //Se a Url nao for repetida
        if(!repetida){

            //E se o elemento for a primeira Url da lista
            if(atual == this->_lista){
                nova->_proxima = this->_lista;
                this->_lista = nova;
                
            //Se ela estiver em qualquer outro ponto
            } else {
                nova->_proxima = atual;
                anterior->_proxima = nova;
            }

            this->_tamanho++;

        //Se ela for repetida, apague-a
        } else {
            delete nova;
        }
    }
}


/**
 * \brief Escalona uma Url do Host e a remove em seguida.
 */
void Host::escalona(){
    //Caso nao haja o que escalonar
    if(this->tamanho() < 1) return;
    erroAssert(this->_lista != nullptr, "escalonador - Nao foi possivel encontrar a lista de Urls");

    //Captura a primeira Url da lista
    Url* aux = this->_lista;
    this->_lista = aux->_proxima;

    //Exibe a primeira Url da lista, e a remove
    //LEMEMLOG((long int)(aux),sizeof(double),URL_CLASS_ID);
    aux->imprime();
    delete aux;

    this->_tamanho--;
}


/**
 * \brief Exibe todos os enderecos de um Host.
 */
void Host::verEnderecos(){
    //Caso nao houver o que se exibir
    if(this->tamanho() < 1) return;
    erroAssert(this->_lista != nullptr, "escalonador - Nao foi possivel encontrar a lista de Urls");

    Url* atual = this->_lista;

    //Exibe cada um dos enderecos da lista
    while(atual != nullptr){
        //LEMEMLOG((long int)(atual),sizeof(double),URL_CLASS_ID);
        atual->imprime();
        atual = atual->_proxima;
    }
}


/**
 * \brief Exibe o nome do Host que o invocou.
 */
void Host::verHost(){
    //Declara em escopo local a variavel global para escrita no arquivo de saida
    extern ofstream arquivoSaida;

    //O arquivo deve ter sido inicializado na main()
    erroAssert(arquivoSaida.is_open(),"escalonador - arquivo de saida nao pode ser acessado");

    LEMEMLOG((long int)(this->_nome.front()),sizeof(double),HOST_STRING_ID);

    //Escreve o nome do Host
    arquivoSaida << this->nome() << endl;
}


/**
 * \brief Metodo para recuperar o nome de um Host.
 * \return referencia string com o nome do Host.
 */
string& Host::nome(){
    LEMEMLOG((long int)(this->_nome.front()),sizeof(double),HOST_STRING_ID);
    return this->_nome;
}


/**
 * \brief Checa se nao ha Urls no Host.
 * \return true se nao houverem elementos.
 */
bool Host::vazio(){
    return (this->tamanho() < 1);
}


/**
 * \brief Metodo para recuperar o tamanho do Host.
 * \return int com o numero de Urls contidas.
 */
int Host::tamanho(){
    return this->_tamanho;
}


/**
 * \brief Metodo para recuperar o endereco do proximo Host na fila.
 * \return ponteiro para classe Host.
 */
Host* Host::proximo(){
    return this->_proximo;
}


/**
 * \brief Remove todas as Urls do Host.
 */
void Host::esvazia(){
    Url* aux;

    while(this->_lista != nullptr){
        aux = this->_lista;
        this->_lista = aux->_proxima;
        delete aux;
    }

    this->_tamanho = 0;
}


/**
 * \brief Imprime no terminal cada um dos enderecos de um Host
 */
void Host::imprimeEnderecos(){
    //Caso nao houver o que se exibir
    if(this->tamanho() < 1) return;
    erroAssert(this->_lista != nullptr, "escalonador - Nao foi possivel encontrar a lista de Urls");

    Url* atual = this->_lista;

    //Imprime no terminal cada um dos enderecos da lista
    while(atual != nullptr){
        //LEMEMLOG((long int)(atual),sizeof(double),URL_CLASS_ID);
        LEMEMLOG((long int)(atual->_endereco.front()),sizeof(double),URL_STRING_ID);
        cout << atual->_endereco << endl;
        atual = atual->_proxima;
    }
}


/**
 * \brief Acessa os enderecos de um Host
 * \return numero de caracteres 
 */
double Host::acessaEnderecos(){
    //Caso nao houver o que se exibir
    if(this->tamanho() < 1) return 0;
    erroAssert(this->_lista != nullptr, "escalonador - Nao foi possivel encontrar a lista de Urls");

    Url* atual = this->_lista;
    double qualquer=0;

    //Acessa cada um dos enderecos da lista
    while(atual != nullptr){
        //LEMEMLOG((long int)(atual),sizeof(double),URL_CLASS_ID);
        LEMEMLOG((long int)(atual->_endereco.front()),sizeof(double),URL_STRING_ID);
        qualquer += atual->_endereco.front();
        atual = atual->_proxima;
    }
    return qualquer;
}


/**
 * \brief Constroi uma Url.
 * \param endereco e uma string com o endereco.
 */
Url::Url(string& endereco){
    this->_endereco = endereco;
    ESCREVEMEMLOG((long int)(this->_endereco.front()),sizeof(double),URL_STRING_ID);
    this->_proxima = nullptr;
    this->processa();
}


/**
 * \brief Checa se Url e valida.
 * \return true se formato do endereco estiver dentro das especificacoes.
 */
bool Url::eValida(){
    return this->_valida;
}


/**
 * \brief Extrai da Url o endereco do Host.
 * \return string com o nome do Host. String vazia caso o endereco nao seguir as especificacoes.
 */
string Url::extraiHost(){
    string nomeHost;
    string::size_type posicao;

    //Caso Url nao for valida, retorne a string vazia
    if(!this->eValida()) return nomeHost;
    erroAssert(this->_endereco.find(PROTOCOLO) != string::npos, "escalonador - erro de validacao da Url");

    //Caso contrario, extraia a partir da primeira posicao apos "http://"
    nomeHost = this->_endereco.substr(7);

    //Encontra a posicao da primeira barra '/' que marca o fim do host, ou atinge o fim da lista
    posicao = nomeHost.find('/');

    //Retorna a string do inicio do nome do Host ate a primeira barra ('/') ou o fim (npos)
    return nomeHost.substr(0,posicao);
}


/**
 * \brief Imprime no arquivo o endereco da Url.
 */
void Url::imprime(){
    //Declara no escopo a variavel global para escrita no arquivo de saida
    extern ofstream arquivoSaida;

    //O arquivo deve ser inicializado na main()
    erroAssert(arquivoSaida.is_open(),"escalonador - arquivo de saida nao pode ser acessado");
    erroAssert(this->eValida(),"escalonador - Url invalida foi armazenada");

    //Escreve o endereco
    LEMEMLOG((long int)(this->_endereco.front()),sizeof(double),URL_STRING_ID);
    arquivoSaida << this->_endereco << endl;
}


/**
 * \return Nivel de uma Url de acordo com numero de barras, ou -1 se o endereco for invalido.
 */
int Url::nivel(){
    if(this->eValida()){
        return this->_nivel;
    } else {
        return -1;
    }
}


/**
 * \brief Processa uma Url na construcao para definir se e valida, o seu nivel e remover partes dispensaveis.
 */
void Url::processa(){

    //Especificacoes da expressao regular
    const regex formatoUrl("(http:\\/\\/)(www\\.)?([\\da-z\\.-]+)\\.([a-z\\.]{2,6})([\\/\\w \\.\\\?#~=-]*)*\\/?");
    const regex extensoesIndesejadas("((.*\\.jpg)|(.*\\.gif)|(.*\\.mp3)|(.*\\.avi)|(.*\\.doc)|(.*\\.pdf))");

    //Auxiliar
    string::size_type posicao;

    //Se string nao estiver vazia, tiver formato de Url, enao tiver extensoes a rejeitar
    if((!(this->_endereco.empty())) && (std::regex_match(this->_endereco, formatoUrl)) && (!(std::regex_match(this->_endereco, extensoesIndesejadas)))){

        //Ela e valida
        this->_valida = true;

    //Caso contrario, ela nao e valida e nao ha mais o que se fazer
    } else {
        this->_valida = false;
        return;
    }

    //Se for valida, 
    //Remova tudo a partir do fragmento ('#'), se houver
    posicao = this->_endereco.find("#");
    if(posicao != string::npos){
        this->_endereco = this->_endereco.substr(0,posicao);
    }

    //Remova "www.", supondo Url bem formatada (como monitor afirmou)
    posicao = this->_endereco.find("www.");
    if(posicao != string::npos){
        this->_endereco.erase(posicao,4);
    }

    //Remove o ultimo elemento se for uma barra '/' (mesmo que tenha varias)
    while(this->_endereco.back() == '/'){
        this->_endereco.pop_back();
    }

    //Calcule a profundidade
    //O numero de barras ('/') deduzido das duas barras iniciais (em "https://") define a profundidade
    this->_nivel = std::count(this->_endereco.begin(), this->_endereco.end(), '/') - 2;
    erroAssert(this->_nivel > -1,"escalonador - numero de barras calculadas incorreto");
}
