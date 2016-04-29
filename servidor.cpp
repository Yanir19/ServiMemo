#include "servidor.h"


Servidor::Servidor(){
 list=NULL;
 contadorMem=1;
 pthread_mutex_init(&m,NULL);
}

bool Servidor::crear_Socket(int puerto,string tipo)
{
   if(tipo=="tcp"){
        descriptor = socket(AF_INET,SOCK_STREAM,0); //El protocolo utilizado es TCP y 0 para que el sistema asigne la IP

    }else{
        descriptor = socket(AF_INET,SOCK_DGRAM,0); //El protocolo utilizado es UDP y 0 para que el sistema asigne la IP

    }
      if(descriptor < 0){
        return false;
    }

    info.sin_family = AF_INET;
    info.sin_addr.s_addr = INADDR_ANY;
    info.sin_port = htons(puerto);
    memset(&info.sin_zero,0,sizeof(info.sin_zero));
    return true;
}//4050


bool Servidor::fusionar_kernel(string tipo){
    if((bind(descriptor,(sockaddr * )&info,(socklen_t)sizeof(info))) < 0){
        return false;
    }
    if(tipo=="tcp"){
         listen(descriptor,10);
    }

    return true;
}

void Servidor::run(int puerto,string tipo){
    tip=tipo;
    if(!crear_Socket(puerto,tipo)){
         cout << "Error al crearlo" <<endl;
    }else{
         cout << "Lo creo bien" <<endl;
    }
    if(!fusionar_kernel(tipo)){
     cout << "Error al Fusionar" <<endl;
    }else{
        cout << "Fusiono Bien" <<endl;
    }

    if(tipo=="tcp"){
        while(true){
            cout << "Esperando un nuevo cliente" <<endl;
            dataSocket data;
            socklen_t tam = sizeof(data.info);
            data.descriptor = accept(descriptor,(sockaddr*)&data.info,&tam);
            data.ser=this;
            if(data.descriptor < 0){
                cout << "Error al aceptar el cliente" <<endl;
            }else{
                cout << "Tama;o de clientes>" <<endl;
                cout << clientes.size() <<endl;
                clientes.push_back(data.descriptor);
                cout << "Dada.decriptor" <<endl;
                cout << data.descriptor <<endl;
                pthread_t hilo;
                pthread_create(&hilo,0,Servidor::controladorCliente, (void *)&data);
                pthread_detach(hilo);
            }
        }
        close(descriptor);
    }else{
         dataSocket data;
         data.info=info;
         data.descriptor =descriptor ;
         data.ser=this;
         controladorClienteUDP((void *)&data);
    }

}

void * Servidor::controladorCliente (void *obj)
{
    bool yes=true;
    dataSocket *data = (dataSocket*)obj;
    pthread_mutex_lock(&data->ser->m);
         int descrip=data->descriptor;
pthread_mutex_unlock(&data->ser->m);
    while(yes){
        string mensaje;
        while(1){
            char buffer[1024] = {0};

            int bytes = recv (descrip,buffer,1024,0); //Aqui se recibe el mensaje y segun
            // el primer byte que puede ser 1, 2,3, 4 segun la opcion que sea hago lo demas.
            cout << bytes <<endl;
            mensaje.append(buffer,strlen("1"));

            if(bytes <= 0){
                cout << "Entro en el area de bytes 0" <<endl;
                close(descrip);
                pthread_exit(NULL);
            }
            if(bytes < 1024){

   pthread_mutex_lock(&data->ser->m);

                void *recv;
                recv=malloc(bytes);
                memcpy(recv,buffer,bytes);
                int resp=0;
                 int *buf;


                if(bytes==1){ //si solo me mando 1 byte entonces no sirve y retorno que no
                    buf=&resp;
                    send(descrip,buf,sizeof(int),0);
                    break;
                }

                if(mensaje=="1"){ // Si el char es 1 entonces hago el CREATE

                    int bandera1=1;
                    void * dat;

                    dat=malloc(bytes-strlen("1")); //Reservo espacio para el resto del buffer


                    memcpy(dat,recv+strlen("1"),bytes-strlen("1")); //Guardo el resto del bufer
                    //En la variable dat


                    lista *nuevo=new lista(); //Creo un nuevo elemento de la lista
                    nuevo->puntero=dat; //Guardo el dato en el puntero de la estructura que esta en la lista
                    nuevo->sgt=NULL;
                    nuevo->id=data->ser->contadorMem;
                    nuevo->tam=bytes-strlen("1");

                    lista *aux; //Aux para recorrer la lista

                    aux=data->ser->list;
                  if(aux==NULL){

                      data->ser->list=nuevo;
                  }else{

                    while(aux->sgt!=NULL){

                        aux=aux->sgt;
                    }

                        aux->sgt=nuevo; //Agrego el elemento a la lista
                  }



                    buf=&data->ser->contadorMem; //Retorno el identificador del nuevo elemento

                     send(descrip,buf,sizeof(int),0); //Se lo retorno como un entero
                     //Alla el cliente lo recibe
                     data->ser->contadorMem=data->ser->contadorMem+1;//Esto es una varibale para el identificador

                }



               if(mensaje=="2"){
                   int * dat;

                   dat=(int *)malloc(bytes-strlen("1"));


                   memcpy(dat,recv+strlen("1"),bytes-strlen("1"));

                   lista *aux;


                   aux=data->ser->list;

                   if(aux==NULL){
                       buf=&resp;
                       send(descrip,buf,sizeof(int),0);
                   }else{
                       while(aux!=NULL){
                           if(aux->id==*dat){
                               break;
                           }else{
                            aux=aux->sgt;
                           }
                       }

                   }

                   if(aux!=NULL){ // VER QUE RETORNAR SI NO LO ENCUENTRA
                       send(descrip,aux->puntero,aux->tam,0);
                   }else{
                       buf=&resp;
                       send(descrip,buf,sizeof(int),0);
                   }


               }



               if(mensaje=="3"){
                   int * dat;

                   dat=(int *)malloc(bytes-strlen("1"));


                   memcpy(dat,recv+strlen("1"),bytes-strlen("1"));

                   lista *aux;
                   int banderaD=0;


                   aux=data->ser->list;

                   if(aux==NULL){
                       buf=&resp;
                       send(descrip,buf,sizeof(int),0);
                   }else{

                       if(aux->id==*dat){
                           data->ser->list=aux->sgt;
                           free(aux);
                           banderaD=1;
                       }else{

                       while(aux->sgt!=NULL){
                           if(aux->sgt->id==*dat){

                               lista *sg ;
                               sg = aux->sgt;
                               aux->sgt=sg->sgt;

                               free(sg);
                               banderaD=1;
                               break;
                           }else{
                            aux=aux->sgt;
                           }
                       }
                       }

                   }


                   if(banderaD){

                       resp=1;
                       buf=&resp;
                       send(descrip,buf,sizeof(int),0);
                       resp=0;
                   }else{
                       buf=&resp;
                       send(descrip,buf,sizeof(int),0);
                   }


               }


               if(mensaje=="4"){
                   int bandera4=0;
                   int * datInt;
                   void * datDat;
                   int tamDat=bytes-strlen("1")-sizeof(int);

                   datInt=(int *)malloc(sizeof(int)); //Reservo espacio para el resto del buffer
                   datDat=malloc(tamDat);

                   memcpy(datInt,recv+strlen("1"),sizeof(int)); //Guardo el resto del bufer
                   //En la variable dat
                    memcpy(datDat,recv+strlen("1")+sizeof(int),tamDat);


                    lista *nu=new lista(); //Creo un nuevo elemento de la lista
                    nu->puntero=datDat; //Guardo el dato en el puntero de la estructura que esta en la lista
                    nu->tam=tamDat;

                    lista *aux; //Aux para recorrer la lista

                    aux=data->ser->list;

                    if(aux==NULL){
                        bandera4=0;
                    }else{
                        if(aux->id==*datInt){
                            data->ser->list=nu;
                            nu->sgt=aux->sgt;
                            nu->id=aux->id;
                            free(aux);
                            bandera4=1;
                        }else{

                        while(aux->sgt!=NULL){
                            if(aux->sgt->id==*datInt){

                                lista *sg ;
                                sg = aux->sgt;

                                nu->sgt=sg->sgt;
                                nu->id=sg->id;

                                aux->sgt=nu;

                                free(sg);
                                bandera4=1;
                                break;
                            }else{
                             aux=aux->sgt;
                            }
                        }
                        }
                    }

                    if(bandera4){

                        resp=1;
                        buf=&resp;
                        send(descrip,buf,sizeof(int),0);
                        resp=0;
                    }else{
                        buf=&resp;
                        send(descrip,buf,sizeof(int),0);
                    }


               }


 pthread_mutex_unlock(&data->ser->m);

                break;
            }
        }
        data->ser->imprimirLista();

    }

    close(descrip);
    pthread_exit(NULL);
}

void * Servidor::controladorClienteUDP (void *obj)
{
    bool yes=true;
    dataSocket *data = (dataSocket*)obj;

    while(yes){
        string mensaje;
        while(1){
            cout << "Esperando recibir un mensaje" <<endl;
            /* Contendrá los datos del que nos envía el mensaje */
            struct sockaddr_in Cliente;


            int longitudCliente = sizeof(Cliente);
            char buffer[1024] = {0};

            int bytes = recvfrom (descriptor,buffer,1024,0,(struct sockaddr *)&Cliente,(socklen_t *)&longitudCliente); //Aqui se recibe el mensaje y segun
            // el primer byte que puede ser 1, 2,3, 4 segun la opcion que sea hago lo demas.
            cout << bytes <<endl;
            mensaje.append(buffer,strlen("1"));

            if(bytes <= 0){
                cout << "entro en el area de bytes 0" <<endl;

                pthread_exit(NULL);
            }
            if(bytes < 1024){



                void *recv;
                recv=malloc(bytes);
                memcpy(recv,buffer,bytes);
                int resp=0;
                 int *buf;


                if(bytes==1){ //si solo me mando 1 byte entonces no sirve y retorno que no
                    buf=&resp;
                    sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente);
                    break;
                }

                if(mensaje=="1"){ // Si el char es 1 entonces hago el CREATE

                    int bandera1=1;
                    void * dat;

                    dat=malloc(bytes-strlen("1")); //Reservo espacio para el resto del buffer


                    memcpy(dat,recv+strlen("1"),bytes-strlen("1")); //Guardo el resto del bufer
                    //En la variable dat


                    lista *nuevo=new lista(); //Creo un nuevo elemento de la lista
                    nuevo->puntero=dat; //Guardo el dato en el puntero de la estructura que esta en la lista
                    nuevo->sgt=NULL;
                    nuevo->id=data->ser->contadorMem;
                    nuevo->tam=bytes-strlen("1");

                    lista *aux; //Aux para recorrer la lista

                    aux=data->ser->list;
                  if(aux==NULL){

                      data->ser->list=nuevo;
                  }else{

                    while(aux->sgt!=NULL){

                        aux=aux->sgt;
                    }

                        aux->sgt=nuevo; //Agrego el elemento a la lista
                  }



                    buf=&data->ser->contadorMem; //Retorno el identificador del nuevo elemento

                     sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente); //Se lo retorno como un entero
                     //Alla el cliente lo recibe
                     data->ser->contadorMem=data->ser->contadorMem+1;//Esto es una varibale para el identificador

                }



               if(mensaje=="2"){
                   int * dat;

                   dat=(int *)malloc(bytes-strlen("1"));


                   memcpy(dat,recv+strlen("1"),bytes-strlen("1"));

                   lista *aux;


                   aux=data->ser->list;

                   if(aux==NULL){
                       buf=&resp;
                       sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente);
                   }else{
                       while(aux!=NULL){
                           if(aux->id==*dat){
                               break;
                           }else{
                            aux=aux->sgt;
                           }
                       }

                   }

                   if(aux!=NULL){ // VER QUE RETORNAR SI NO LO ENCUENTRA
                       sendto(descriptor,aux->puntero,aux->tam,0, (struct sockaddr *)&Cliente, longitudCliente);
                   }else{
                       buf=&resp;
                       sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente);
                   }


               }



               if(mensaje=="3"){
                   int * dat;

                   dat=(int *)malloc(bytes-strlen("1"));


                   memcpy(dat,recv+strlen("1"),bytes-strlen("1"));

                   lista *aux;
                   int banderaD=0;


                   aux=data->ser->list;

                   if(aux==NULL){
                       buf=&resp;
                       sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente);
                   }else{

                       if(aux->id==*dat){
                           data->ser->list=aux->sgt;
                           free(aux);
                           banderaD=1;
                       }else{

                       while(aux->sgt!=NULL){
                           if(aux->sgt->id==*dat){

                               lista *sg ;
                               sg = aux->sgt;
                               aux->sgt=sg->sgt;

                               free(sg);
                               banderaD=1;
                               break;
                           }else{
                            aux=aux->sgt;
                           }
                       }
                       }

                   }


                   if(banderaD){

                       resp=1;
                       buf=&resp;
                       sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente);
                       resp=0;
                   }else{
                       buf=&resp;
                       sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente);
                   }


               }


               if(mensaje=="4"){
                   int bandera4=0;
                   int * datInt;
                   void * datDat;
                   int tamDat=bytes-strlen("1")-sizeof(int);

                   datInt=(int *)malloc(sizeof(int)); //Reservo espacio para el resto del buffer
                   datDat=malloc(tamDat);

                   memcpy(datInt,recv+strlen("1"),sizeof(int)); //Guardo el resto del bufer
                   //En la variable dat
                    memcpy(datDat,recv+strlen("1")+sizeof(int),tamDat);


                    lista *nu=new lista(); //Creo un nuevo elemento de la lista
                    nu->puntero=datDat; //Guardo el dato en el puntero de la estructura que esta en la lista
                    nu->tam=tamDat;

                    lista *aux; //Aux para recorrer la lista

                    aux=data->ser->list;

                    if(aux==NULL){
                        bandera4=0;
                    }else{
                        if(aux->id==*datInt){
                            data->ser->list=nu;
                            nu->sgt=aux->sgt;
                            nu->id=aux->id;
                            free(aux);
                            bandera4=1;
                        }else{

                        while(aux->sgt!=NULL){
                            if(aux->sgt->id==*datInt){

                                lista *sg ;
                                sg = aux->sgt;

                                nu->sgt=sg->sgt;
                                nu->id=sg->id;

                                aux->sgt=nu;

                                free(sg);
                                bandera4=1;
                                break;
                            }else{
                             aux=aux->sgt;
                            }
                        }
                        }
                    }

                    if(bandera4){

                        resp=1;
                        buf=&resp;
                        sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente);
                        resp=0;
                    }else{
                        buf=&resp;
                        sendto(descriptor,buf,sizeof(int),0, (struct sockaddr *)&Cliente, longitudCliente);
                    }


               }




                break;
            }
        }
        data->ser->imprimirLista();

    }

    close(data->descriptor);
    pthread_exit(NULL);
}






void Servidor::setMensaje(const char *msn)
{
    for(unsigned int i = 0; i <clientes.size() ; i++)
        cout << "bytes enviados"<<send(clientes[i],msn,strlen(msn),0);
}




int Servidor::imprimirLista(){
    lista *aux;
    aux=list;
if(aux==NULL){
      cout << " La Lista esta Vacia vaciaaaaaa " << endl;
}


cout <<  "!************************* IMPRIMIR LISTA *******************************! " << endl;
   while(aux!=NULL){

       cout << "!**** ID: " << aux->id << " ************! " << endl;

       cout << aux->tam << " Tamanio de elemento en la lista " << endl;
       aux=aux->sgt;
       cout <<  "   " << endl;
   }
return 1;
}


