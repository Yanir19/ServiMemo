#ifndef SERVIDOR_H
#define SERVIDOR_H
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <list>
using namespace std;


struct datas{
    void *dato;
    int tipo;
};

struct lista{
  size_t tam;
  int id;
  void *puntero;
  lista *sgt;  
};

struct  dataSocket{
    int descriptor;
    sockaddr_in info;
    class Servidor *ser;
};

struct  segmentoMem{
    int indice;
    int tamano;
};

class Servidor
{
public:

    Servidor();
    void run(int puerto, string tipo);
    void setMensaje(const char *msn);

private:
    static int nodo;
    int contadorMem;
    int descriptor; //Variable que asigna el sistema operativo al crear un nuevo socket
    sockaddr_in info;//Estructura con informacion del tipo de socket y conexiones a realizar
    int imprimirLista();
    bool crear_Socket(int puerto,string tipo);
    bool fusionar_kernel(string tipo); //Funcion para combinar el el socket y el sistema, para retornar las llamadas al programa
    static void * controladorCliente(void *obj);
    void * controladorClienteUDP(void *obj);
    string mensaje;
    vector<int> clientes;
    lista *list;
    string tip;
     pthread_mutex_t m;


};

#endif // SERVIDOR_H
