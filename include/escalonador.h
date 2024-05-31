#ifndef LISTALISTAS_ESCALONADOR
#define LISTALISTAS_ESCALONADOR

#include <iostream>

using std::string;


/* Em um editor de textos compativel (como o VSCode da Microsoft),
 * posicione o ponteiro do mouse sobre o nome de uma funcao/metodo/classe/variavel para ler sua descricao.
 */


/**
 * \class Url
 * \brief Armazena um endereco web.
 * Tambem se comporta como no para uma lista encadeada de Urls.
 */
class Url {

    protected:
    string _endereco;       /** Contem um endereco web em formato string.  */
    int _nivel;    /** Nivel de um endereco Url, caracterizado pela quantidade de barras '/' apos o Host. */
    Url* _proxima;          /** Armazena o endereco para proxima Url. ehinicializada como nula. */
    bool _valida;           /** Armazena dado informando se o enereco atende as especificacoes. Valor definido na construcao. */

    public:

    //Construtor

    Url(string& endereco);

    bool eValida();
    string extraiHost();
    void imprime();
    int nivel();

    friend class Host;
    friend class ListaHost;

    private:
    void processa();
};



/**
 * \class Host
 * \brief Lista de enderecos do tipo Url. (Para uso exclusivo da Classe ListaHosts)
 * Tambem se comporta como no para uma lista encadeada de Hosts.
 */
class Host {

    protected:
    Url* _lista;            /** Armazena ponteiro para o primeiro item de uma lista de Urls. */
    string _nome;           /** Endereco do Host em formato string. */
    int _tamanho;  /** Armazena o numero de enderecos do tipo Url contidos na lista. */
    Host* _proximo;         /** Armazena ponteiro para o proximo elemento da fila de Hosts. */
    
    //Construtores e destrutor

    Host(Url* endereco);
    ~Host();

    // Insercao, escalonamento e exibicao

    void insere(Url* url);
    void escalona();
    void verEnderecos();
    void verHost();

    //Metodos para recuperar valores

    string& nome();
    bool vazio();
    int tamanho();
    Host* proximo();

    //Auxiliares

    void esvazia();
    void imprimeEnderecos();
    double acessaEnderecos();

    friend class ListaHost;
    friend class Url;
};



/**
 * \class ListaHost
 * \brief Fila encadeada de elementos do tipo Host.
 */
class ListaHost {
    private:
    Host* _hosts;           /** Armazena um ponteiro para o primeiro no de uma lista de elementos do tipo Host. */
    int _tamanho;  /** Informa o numero de Hosts contidos na lista. */

    public:
    //Construtor e destrutor

    ListaHost() : _hosts(nullptr), _tamanho(0) {};
    ~ListaHost();

    //Insercao, escalonamentos e exibicao

    void insere(string& url);
    void escalona(int quantidade);
    void escalona(string& host, int quantidade);
    void escalonaTudo();
    void verHost(string& host);
    void listaHosts();

    //Desafios

    void escalonaTudoBreadthFirst();
    void escalonaBreadthFirst(int quantidade);
    void escalonaTudoBestFirst();
    void escalonaBestFirst(int quantidade);

    //Limpeza

    void limpa(string& host);
    void limpaTudo();

    //Auxiliares

    Host* procuraHost(string& host);
    void imprime();
    double acessa();

    friend class Host;
    friend class Url; 
};

#endif