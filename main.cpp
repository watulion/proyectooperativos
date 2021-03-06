/*
Autores: Gerardo Daniel Rodriguez, Rodolfo Cantu Ortiz, Eduardo Andrade Martinez, Cesar Rodriguez
Descripcion: Este programa sirve para simular el funcionamiento de memoria fisica y secundaria
            a la hora de administrar procesos.
*/
#include <iostream>
#include <string>
#include<fstream>
#include<sstream>
#include <ctime>
#include <math.h>
#include <cmath>
#include <time.h>
#include <ctype.h>
using namespace std;
//para leer archivo de texto
ifstream File;
string FileRead;
//Timer para LRU
int marcosrealtimestamps[256] = {0}; //(para calcular tiempo en el que se modifico)
int marcosvirtualtimestamps[512] = {0}; //(para relacionar marcos virtuales con reales)
int procesoinitimestamp[5328001] = {0};
int procesoendimestamp[5328001] = {0};
int cputime = 0;
int swaps = 0;
//variables que manejan marcos
int marcosreal[256]; //(numero de proceso que utiliza el marco)
int procesotam[5328001]; //saber el tama�p del proceso
int marcosrealmodificado[256] = {0}; //(0 o 1, modificado o no)
int marcosrealreferenciado[256] = {0}; //(0 o 1, referenciado o no)
int marcosvirtual[512];//(apunta al proceso del marco real que tenia la memoria virtual)

int cantMarcos = 0;
int cuentaVirtual = 0;
int mintimestamp = 0; //cuando ya no haya m�s paginas que hayan sido creadas en tiempo 0 se elevara +1
int pagefaults = 0;

// Esta funcion compacta la memoria virtual despues de que un proceso es liberado
void compactaMemoriaVirtual(){
    int empiezaHueco=-1; // primera posicion donde hay un -1 (despues de liberar)
    int terminaHueco=-1; // ultima posicion donde hay un -1 (despues de liberar)
    int empiezaVacio=-1; // posicion a partir de la cual no hay procesos en memoria virtual

    // buscar en memoria virtual por las posiciones declaradas anteriormente
    for (int i = 0; i<512; i++){
        if (marcosvirtual[i]==-1){
            empiezaHueco = i;
            for (int j=empiezaHueco+1; j<512; j++){ // buscar el final del hueco a partir de la posicion inicial del hueco
                if (marcosvirtual[j]==-1 && marcosvirtual[j+1]!= -1){ // identificar que el hueco se termine en esta posicion
                    terminaHueco = j;
                    break;
                }
            }
            break;
        }
    }
    // buscar la posicion inicial de la memoria virtual disponible (despues del hueco causado por la liberacion del proceso)
    for (int i=terminaHueco+1; i<512; i++) {
        if(marcosvirtual[i]==-1){
            empiezaVacio = i;
            break;
        }
    }
    // esta condicion se cumple cuando al buscar el inicio de los vacios no entra al ciclo ya que
    // el fin del hueco es justamente el fin de la memoria virtual (ya esta compactado)
    if (empiezaVacio!=-1){
        int espaciosRecorrer = (terminaHueco - empiezaHueco)+1; // numero de marcos que se debe de reposicionar cada pagina
        cuentaVirtual -= espaciosRecorrer;
        for (int i=terminaHueco+1; i<empiezaVacio; i++){ // recorrer las paginas despues del hueco liberado
            {
             marcosvirtual[i-espaciosRecorrer] = marcosvirtual[i];
             marcosvirtualtimestamps[i-espaciosRecorrer] = marcosvirtualtimestamps[i];
            }
        }
        // marcar como libres (-1) la cantidad de marcos en el hueco al final de los marcos ocupados
        for (int i=empiezaVacio-espaciosRecorrer; i<empiezaVacio; i++){
            marcosvirtual[i] = -1;
            marcosvirtualtimestamps[i] = -0;
        }
    }
}
// metodo para verificar la existencia de un proceso en memoria. Este recorre la memoria en busca de su id
bool existeProcesoEnMemoria(int idProceso){
    for (int i=0; i<512; i++){
        if (marcosvirtual[i] == idProceso) {
            return true;
        }
    }
    return false;
}

// metodo para cargar en memoria las paginas de un proceso
void cargarProceso()
{
    //Formato (P n p)
    double n = 0;
    int p = 0;
    int residuo = 0; //utilizado para saber cuantos marcos de paginaci�n hay que reemplazar cuando se llene la memoria
    bool lleno = 0; // avisa si esta lleno
    //leer n
    File>>FileRead;
    istringstream (FileRead) >>n;
    //leer p
    File>>FileRead;
    istringstream (FileRead) >>p;

    int introducidas = ceil (n/8);
    cout << "Numero de paginas introducidas " << introducidas << endl;
    int paginas = cantMarcos + introducidas;
    lleno = paginas/256;
    residuo = introducidas % 256;
    int contadorpaginas = 0;
    int ultimoMarco = 0;
    int primero = 0; // contador para saber cuales el primer elemento en uno corrido
    int numeroPrimero = 0; // numero del marco utilizado en una corrida (PRIMERO)
    int numeroFinal = 0; // numero del marco utilizado en una corrida (Final)

    // aqui empieza el verdadero cout
    cout << "P " << n << " " << p << endl;

    //decir en que cputime empezo el proceso
    procesoinitimestamp[p] = cputime;
    procesotam[p] = introducidas -1 ;

    if (cantMarcos < 256)
    {
       for(int i = 0; i < 256 && contadorpaginas < introducidas ; i++)
    {
        if (marcosreal[i] == -1)
        {
            primero++;
            if (primero == 1) numeroPrimero = i;
            //asignar cputime para el timestamp
            marcosrealtimestamps[i] = cputime;
            marcosvirtualtimestamps[i] = cputime;
            marcosreal[i] = p;
            marcosvirtual[cuentaVirtual] = p;

            //subir el cputime, pagefaults, cantMArcos y cuenta Virtual
            cuentaVirtual++;
            cputime++;
            pagefaults++;
            contadorpaginas++;
            //estos solo se cuentan cuando no esta llena

            cantMarcos++;
            ultimoMarco = i;
            if(cantMarcos >= 256 && contadorpaginas < introducidas)
                {
                    lleno = true;
                    break;
                }
            }
    }
    int ini = 0, final = -1;
    int proceso = -2;

    for (int i = 0; i < 256; i++)
    {
        if(marcosreal[i] != proceso || i == 255 )
        {
            if (i != 0)
            {
                final = i -1;
            if (marcosreal[final] == p)
            {
                if (i != 255){
                        cout << "Se asignaros los marcos " << ini << "-" << final << " al proceso " << p  << endl;
                }
              else{
                    cout << "Se asignaros los marcos " << ini << "-" << final+1 << " al proceso " << p  << endl;
              }
            }
            ini = i;
            }
            proceso = marcosreal[i];
        }

    }
    cout << "En el tiempo " << procesoinitimestamp[p] << endl;
    }
    if (cantMarcos >= 256)
    {
        cout << "Esta llena la memoria, se hara swapping con LRU " << endl;
        //empezar a hacer LRU aqui
        int marcoLRU = 0;
        bool minfound;

        //siempre y cuando falten paginas que introducir
        while (contadorpaginas < introducidas)
        {
        minfound = false;
        //cout << "empezo el show" << endl;
        //Primero revisar si alguna marco es el LRU
        while (!minfound)
        {
            for (int i = 0; i < 256; i++)
            {
                if(mintimestamp == marcosrealtimestamps[i])
                {
                    minfound = true;
                    marcoLRU = i;
                    break;
                }
            }
            if (!minfound) mintimestamp++;
        }
        //Ahora si cambiamos el marco
        int tempProceso = marcosreal[marcoLRU];
        //asignar cputime para el timestamp
            marcosrealtimestamps[marcoLRU] = cputime;
            marcosrealmodificado[marcoLRU] = 0;
            marcosrealreferenciado[marcoLRU] = 0;
            marcosvirtualtimestamps[cuentaVirtual] = cputime;
            marcosreal[marcoLRU] = p;
            marcosvirtual[cuentaVirtual] = p;
            swaps++;
            //buscar con que memoria virtual se swapio
            cout << "<El marco " << marcoLRU << " en mem real ahora le pertenece al proceso " << p << " (previo: " << tempProceso << ")>"<< endl;
            cout << "La pagina esta en la posicion " << cuentaVirtual << " en la memoria virtual " << endl;
            // inserta arriba la p�gina en la que est�
            contadorpaginas++;
            cuentaVirtual++;
            cputime++;
            pagefaults++;
        }//acabamos de meter paginas

        cout << " al proceso " << p << endl;
        cout << "En el tiempo " << procesoinitimestamp[p] << endl;
    }
}

void liberarPaginas()
{
    int idProceso; // id del proceso a liberar
    int marcosLiberadosReal[256]; // arreglo para guardar la posicion de los marcos que se liberaron de memoria real
    int cantidadMarcosLiberadosReal = 0; // contador de marcos liberados de memoria real

    int marcosLiberadosVirtual[512]; // arreglo para guardar la posicion de los marcos que se liberaron de memoria virtual
    int cantidadMarcosLiberadosVirtual = 0; // contador de marcos liberados de memoria virtual

    // leer el id del proceso a liberar de memoria
    File>>FileRead;
    istringstream (FileRead) >>idProceso;

    cout << "L " << idProceso << endl;
    // revisar que el proceso exista en memoria
    if (!existeProcesoEnMemoria(idProceso)){
        cout << "Este proceso no existe en memoria" << endl;
    } else {
        //Acabo proceso
        procesoendimestamp[idProceso] = cputime;

        cout << "El proceso acabo en el tiempo " << cputime << endl;

        int restaramarcos = 0;
        // buscar en memoria real los marcos que pertenecen al id del proceso
        for (int i=0; i<256; i++) {
            if (marcosreal[i]==idProceso){
                marcosreal[i] = -1; // -1 es el valor default de memoria representando vacio
                marcosrealmodificado[i] = 0; // indicar como no modificado
                marcosrealreferenciado[i] = 0; // indicar como no referenciado
                restaramarcos++;
                // registrar los cambios
                marcosLiberadosReal[cantidadMarcosLiberadosReal] = i;
                cantidadMarcosLiberadosReal++;
            }
        }
        cantMarcos -= restaramarcos;

        // liberar los marcos de pagina en memoria secundaria de swapping
        for (int i=0; i<512; i++){
            if (marcosvirtual[i]==idProceso){
                marcosvirtual[i] = -1; // valor default para indicar que esta vacio
                //registrar los cambios
                marcosLiberadosVirtual[cantidadMarcosLiberadosVirtual] = i;
                cantidadMarcosLiberadosVirtual++;
            }
        }
        // compactar los huecos en memoria virtual
        compactaMemoriaVirtual();
        // desplegar resultados
        // memoria principal
        if (cantidadMarcosLiberadosReal>0){
            cout << "Estos son los marcos de pagina que se liberaron en memoria real: ";
            for (int i=0; i<cantidadMarcosLiberadosReal; i++){
                cout << marcosLiberadosReal[i] <<", ";
            }
            cout << endl;
        } else {
            cout << "No hay marcos en memoria real con ese ID" << endl;
        }
        // memoria virtual
        if (cantidadMarcosLiberadosVirtual>0){
            cout << "Estos son los marcos de pagina que se liberaron en memoria virtual: ";
            for (int i=0; i<cantidadMarcosLiberadosVirtual; i++){
                cout<< marcosLiberadosVirtual[i]<< ", ";
            }
            cout << endl;
        } else {
            cout << "No hay marcos en memoria virtual con ese ID" << endl;
        }
    }
}
// metodo para desplegar los contenidos en memoria real y secundaris
void Debug()
{
   int ini = 0, final = -1;
   cout << "Memoria real" << endl;
    int proceso = -2;
    for (int i = 0; i < 256; i++)
    {
        if(marcosreal[i] != proceso || i == 255 )
        {
            if (i != 0)
            {
                final = i -1;
            if (marcosreal[final] != -1)
            {
                if (i != 255)
              {cout << "Proceso : " << marcosreal[final] << " ocupa marcos " << ini << "-" << final << " en mem real" << endl;}
              else
              {cout << "Proceso : " << marcosreal[final] << " ocupa marcos " << ini << "-" << final+1 << " en mem real" << endl;}
            }
                else
                {
                    if (i != 255)
                    {cout << "Del marco " << ini << " al marco " << final << " no hay nada" << endl;}
                    else
                    {cout << "Del marco " << ini << " al marco " << final+1 << " no hay nada" << endl;}
                }

        ini = i;

        }
        proceso = marcosreal[i];

        }

    }
    ini = 0; final = -1;
    proceso = -2;
    cout << "Memoria virtual" << endl;
    for (int i = 0; i < 512; i++)
    {
         if(marcosvirtual[i] != proceso || i == 511)
        {
            if (i != 0)
            {
                final = i -1;

                if (marcosvirtual[final] != -1)
                {
                    if (i != 511)
                  {cout << "Proceso : " << marcosvirtual[final] << " ocupa marcos " << ini << "-" << final << " en mem virtual" << endl;}
                  else
                 {
                   cout << "Proceso : " << marcosvirtual[final] << " ocupa marcos " << ini << "-" << final+1 << " en mem virtual" << endl;
                 }
                }
                else
                {
                    if (i != 511)
                {cout << "Del marco " << ini << " al marco " << final << " no hay nada" << endl;}
                    else
                {cout << "Del marco " << ini << " al marco " << final+1 << " no hay nada" << endl;}
                }
            }
            ini = i;

        }
         proceso = marcosvirtual[i];
    }
}

// metodo para buscar y accesar una direccion real en base a una direccion virtual de un proceso
void accesarVirtual()
{
    int d, p, m;
    bool virtualarealexiste = false;
    //leer d
    File>>FileRead;
    if (isdigit(FileRead[0]) && FileRead.size() > 0)
    istringstream (FileRead) >>d;
    else
    {
        cout << "comando no valido" << endl;
      return;
    }

    //leer p
    File>>FileRead;
    if (isdigit(FileRead[0]) && FileRead.size() > 0)
    istringstream (FileRead) >>p;
    else
    {
        cout << "comando no valido" << endl;
      return;
    }

    //leer m
    File>>FileRead;
    if (isdigit(FileRead[0]) && FileRead.size() > 0)
    istringstream (FileRead) >>m;
    else
    {
        cout << "comando no valido" << endl;
      return;
    }


    int marcovirtualbuscado = d/8;
    int virtualencontrado = -1;

    cout << "A " << d << " " << p << " " << m << endl;

    if (procesotam[p] < marcovirtualbuscado )
    {
        cout << "Ese numero de pagina es excedente" << endl;
        return;
    }

    for(int i = 0; i < 512; i++ )
    {
        if(marcosvirtual[i] == p)
        {

           virtualencontrado = i;
           break;
        }
    }
    if(virtualencontrado != -1)
    {
        int contador = 0;
       for(int i = virtualencontrado; i < 512; i++)
       {
           //se encuentra el marco buscado del proceso
         if(contador == marcovirtualbuscado)
         {
           for (int j = 0; j < 256; j++)
           {
               if (marcosrealtimestamps[j] == marcosvirtualtimestamps[i])
               {
                   cout << "Se encontro la pagina " << marcovirtualbuscado << " del proceso " << p << " en el marco " << virtualencontrado+marcovirtualbuscado << " de la memoria virtual" << endl;
                   cout << "Se encuentra en el marco " << j << " de la memoria real" << endl;
                  if(marcosrealtimestamps[i] == mintimestamp)
                  {
                    mintimestamp++;
                    cout << "El marco " << j << " en real solia ser el LRU. Ya no lo es " << endl;
                  }
                  virtualarealexiste = true;
                  marcosrealtimestamps[j] =  cputime;
                  marcosvirtualtimestamps[i] = cputime; //new
                  cputime++;
                  //si m = 0, referencia
                  if (m == 0)
                    marcosrealreferenciado[j] = 1;
                  //si m = 1, modificacion
                  else
                    marcosrealmodificado[j] = 1;
                    break;
               }
           }
         }
         contador++;
        }
        if (!virtualarealexiste)
           {

    cout << "Esta llena la memoria, se hara swapping con LRU " << endl;
    //empezar a hacer LRU aqui
    int marcoLRU = 0;
    bool minfound;

    //siempre y cuando falten paginas que introducir
    minfound = false;
    //cout << "empezo el show" << endl;

    //Buscar marco LRU
    while (!minfound)
    {
        for (int i = 0; i < 256; i++)
        {
            if(mintimestamp == marcosrealtimestamps[i])
            {
                minfound = true;
                marcoLRU = i;
                // cout << "Se encontro el marco LRU. Era " << marcoLRU << endl;
                break;
            }
        }
        if (!minfound) mintimestamp++;
    }
    //Ahora si cambiamos el marco
    int tempProceso = marcosreal[marcoLRU];
    //asignar cputime para el timestamp
        marcosrealtimestamps[marcoLRU] = cputime;
        marcosrealmodificado[marcoLRU] = 0;
        marcosrealreferenciado[marcoLRU] = 0;
        marcosvirtualtimestamps[marcovirtualbuscado] = cputime; //cambiar
        marcosreal[marcoLRU] = p;
        marcosvirtual[marcovirtualbuscado] = p;
        swaps++;
        //buscar con que memoria virtual se swapio
        cout << "La pagina esta en la posicion " << marcovirtualbuscado << " en la memoria virtual " << endl;
        //cout << "La pagina ahora se ubica en el marco " << marcoLRU << " en la memoria real" << endl;
        cout << "<El marco " << marcoLRU << " en mem real ahora le pertenece al proceso " << p << " (previo: " << tempProceso << ")>"<< endl;

        // inserta arriba la p�gina en la que est�
        cputime++;
        pagefaults++;
    }//acabamos de meter paginas

    }

    else
    {
       cout << "El proceso " << p << " no existe." << endl;
    }


}
// metodo para desplegar las estadisticas de ejecucion despues de recibir el comando F
void finalizar()
{
    //nota: no finaliza, es el F
    cout << "F" << endl;
    cout << "Info de memorias" << endl;

    cout << "------" << endl;
    //Ver si estan correcto los datos
    Debug();
    cout << "------" << endl;

    int turnaroundcount = 0;
    int turnaroundsum = 0;
    for(int i = 0; i < 5328001; i++)
    {
        if (procesoinitimestamp[i] != -1 && procesoendimestamp[i] != -1)
        {
            turnaroundsum += procesoendimestamp[i] - procesoinitimestamp[i];
            turnaroundcount++;
            cout << "Tiempo de retorno del proceso " << i << "  es: " << procesoendimestamp[i] << " - " << procesoinitimestamp[i] <<" = "<<procesoendimestamp[i] - procesoinitimestamp[i] << endl;
        }
    }

    //promedio turnaround
    if (turnaroundcount > 0)
    cout << "El tiempo promedio de turnaround es " << turnaroundsum/turnaroundcount << endl;
    else
    {
        cout << "Ningun proceso fue finalizado en esta secuencia" << endl;
    }

    //swaps
    cout << "Huvieron " << swaps << " swaps" << endl;

    //lo ultimo, reinicio todo lo necesario
    std::fill_n(marcosreal, 256, -1);
    std::fill_n(marcosrealmodificado, 256, -1);
    std::fill_n(marcosrealreferenciado, 256, -1);
    std::fill_n(marcosvirtual, 512, -1);
    std::fill_n(marcosrealtimestamps, 256, 0);
    std::fill_n(marcosvirtualtimestamps, 512, 0);
    std::fill_n(procesoinitimestamp, 5328001, -1);
    std::fill_n(procesoendimestamp, 5328001, -1);
    std::fill_n(procesotam, 5328001, 0);

    cantMarcos = 0;
    cputime = 0;
    cuentaVirtual = 0;
    mintimestamp = 0;
    pagefaults = 0;
    swaps = 0;

}

int main()
{
    string step;
    std::fill_n(marcosreal, 256, -1);
    std::fill_n(marcosvirtual, 512, -1);
    std::fill_n(procesoinitimestamp, 5328001, -1);
    std::fill_n(procesoendimestamp, 5328001, -1);
    std::fill_n(procesotam, 5328001, 0);
    //Cargar archivo de texto
    File.open("texto.txt");
    while (!File.eof()) {
        File>>FileRead; //agarra cada caracter separado por un espacio.
        if(FileRead == "P" || FileRead == "p"){cout << "------" << endl; cargarProceso();}
        else if(FileRead == "A" || FileRead == "a"){cout << "------" << endl;accesarVirtual();}
        else if(FileRead == "L" || FileRead == "l"){cout << "------" << endl;liberarPaginas();}
        else if(FileRead == "E" || FileRead == "e"){break;}
        else if(FileRead == "F" || FileRead == "f"){cout << "------" << endl;finalizar();}
        else{ cout << "------" << endl; cout << "comando no aceptado" << endl;}//cout<<FileRead<<endl;}
        std::cin.get();

    }
    File.close();
    cout << "------" << endl;
    //Ver si estan correcto los datos
    Debug();
    return 0;
}
