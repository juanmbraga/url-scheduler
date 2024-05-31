#ifndef LISTA_ESCALONADOR
#define LISTA_ESCALONADOR

#include <iostream>

using std::string;

class ListaHost {
    private:
    Host* hosts;
    unsigned int tamanho;

    public:
    ListaHost() : hosts(nullptr), tamanho(0) {}
    ~ListaHost();

    /**
     * \brief Insere url na lista de seu respectivo Host
     * \param url Endereço Web em formato string
     */
    void insere(string& url);

    
    void escalona(int quantidade);
    void escalona(string& host, int quantidade);
    void escalonaTudo();

    void verHost(string& host);
    void listaHosts();

    void limpa(string& host);
    void limpaTudo();

    
};

class Host {
    Url* lista;
    string nome;
    unsigned int tamanho;
    Host* proxima;

    Host() : lista(nullptr), tamanho(0), proxima(nullptr){}
    Host(Url&);
    ~Host();


};

class Url {
    string endereco;
    Url* proximo;
    
};

#endif