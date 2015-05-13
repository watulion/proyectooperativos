#include <iostream>
#include <string>
#include<fstream>
#include<sstream>
#include <ctime>
#include <math.h>
#include <cmath>
#include <time.h>
using namespace std;


//para leer archivo de texto

ifstream File;
string FileRead;

//Timer para LRU

int marcosrealtimestamps[256] = {-1}; //(para calcular tiempo en el que se modifico)
int cputime = 0;

//variables que manejan marcos

int marcosreal[256] = {-1}; //(numero de proceso que utiliza el marco)
int marcosrealmodificado[256] = {0}; //(0 o 1, modificado o no)
int marcosvirtual[512] = {-1}; //(apunta al proceso del marco real que tenia la memoria real)
int cantMarcos = 0;
int cuentaVirtual = 0;
int mintimestamp = 0; //cuando ya no haya m�s paginas que hayan sido creadas en tiempo 0 se elevara +1


void cargarProceso()
{
//Formato (P n p)
double n = 0;
int p = 0;
int residuo = 0; //utilizado para saber cuantos marcos de paginaci�n hay que reemplazar cuando se llene la memoria
int lleno = 0; // cuando se pase de 256 paginas/marcos, este estara arriba de 1


//leer n
File>>FileRead;
istringstream (FileRead) >>n;

//leer p
File>>FileRead;
istringstream (FileRead) >>p;

//Debug de direcciones
//cout << "Proceso: " << p << " Direcciones: " << n << endl;

//Empezar a meter datos

int introducidas = ceil (n/8);
int paginas = cantMarcos + introducidas;
lleno = paginas/256;
residuo = introducidas % 256;

int ultimoMarco = 0;

//Si hay espacio en memoria real
if(lleno == 0)
{
    ultimoMarco = (residuo + cantMarcos) ;
    //cout << residuo << " " <<cantMarcos << " " <<ultimoMarco << endl;
    for(int i = cantMarcos; i < ultimoMarco; i++)
    {
        //asignar cputime para el timestamp
        marcosrealtimestamps[i] = cputime;
        marcosreal[i] = p;
        marcosvirtual[cuentaVirtual] = p;
        //subir el cputime
        cputime++;
    }
    cantMarcos += residuo;
}
else
{
    cout << "El proceso " << p <<" llenara la memoria" << endl;
    int faltantes = introducidas;
    //checar si quedan espacios libres
    if (cantMarcos < 256)
    {
        ultimoMarco = 256;
        int restanteMarcos = 255-cantMarcos;

        for(int i = cantMarcos; i < ultimoMarco; i++)
        {
          marcosrealtimestamps[i] = cputime;
          marcosreal[i] = p;
          //subir el cputime
          cputime++;

        }
        cantMarcos = 255;
        cout << "Quedaron fuera " << restanteMarcos << " paginas. Se aplicara LRU" << endl;
    }
    //Empezar LRU para insertar


}




//Ver si se superaron los 256 marcos con texto
cout << "Paginas: " << cantMarcos << " Lleno: " << lleno << " Paginas introducidas: " << residuo << " hecho por el proceso: " << marcosreal[ultimoMarco]<< " tiempo entrada: " << marcosrealtimestamps[ultimoMarco] << endl;

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

    // buscar en memoria real los marcos que pertenecen al id del proceso
    for (int i=0; i<256; i++) {
        if (marcosreal[i]==idProceso){
            marcosreal[i] = -1; // -1 es el valor default de memoria representando vacio
            marcosrealmodificado[i] = 0; // indicar como no modificado
            // actualizar timestamp?
            // registrar los cambios
            marcosLiberadosReal[cantidadMarcosLiberadosReal] = i;
            cantidadMarcosLiberadosReal++;
        }
    }

    // liberar los marcos de pagina en memoria secundaria de swapping
    for (int i=0; i<512; i++){
        if (marcosvirtual[i]==idProceso){
            marcosvirtual[i] = -1; // valor default para indicar que esta vacio
            //registrar los cambios
            marcosLiberadosVirtual[cantidadMarcosLiberadosVirtual] = i;
            cantidadMarcosLiberadosVirtual++;
        }
    }
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

void Debug()
{
    int proceso = -1;
    for (int i = 0; i < 256; i++)
    {
        if(marcosreal[i] != proceso)
        {
            cout << "Del proceso : " << marcosreal[i] << endl;
            proceso = marcosreal[i];
        }

        cout << marcosrealtimestamps[i] << endl;
    }
}

int main()
{
    //Cargar archivo de texto
    File.open("texto.txt");
    while (!File.eof()) {

        File>>FileRead; //agarra cada caracter separado por un espacio.

        if(FileRead == "P" || FileRead == "p"){cargarProceso();}
        //else if(FileRead == "A" || FileRead == "a"){A();}
        else if(FileRead == "L" || FileRead == "l"){liberarPaginas();}
        //else if(FileRead == "E" || FileRead == "e"){E();}
        //else if(FileRead == "F" || FileRead == "f"){F();}
        else{}//cout<<FileRead<<endl;}

}//while
    File.close();

    //Ver si estan correcto los datos
    //Debug();


    return 0;
}
