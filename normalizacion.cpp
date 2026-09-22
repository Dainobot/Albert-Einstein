#include <iostream>
#include <cstdio>
#include <cstring>
#include "buffet.h"
using namespace std;

void mostrarMozos() {
    FILE* f = fopen("datos/mozos.dat", "rb"); // O "datos/mozos.dat" según donde lo guardaste
    if (!f) {
        cout << "No se pudo abrir mozos.dat" << endl;
        return;
    }

    Mozo m;
    cout << "--- LISTADO DE MOZOS NORMALIZADOS ---" << endl;
    while (fread(&m, sizeof(Mozo), 1, f) == 1) {
        cout << "ID: " << m.idMozo 
             << " | Nombre: " << m.nombre 
             << " | Clave cifrada: " << m.password 
             << " | Total Comision: $" << m.totalComision << endl;
    }

    fclose(f);
}


void encriptarPassword(const char* passOriginal, char* passDestino, int k) {
    int i = 0;
    while (passOriginal[i] != '\0') {
        passDestino[i] = passOriginal[i] + k; // Cifrado César por desplazamiento
        i++;
    }
    passDestino[i] = '\0';
}

int buscarMozoPorNombre(const Mozo mozos[], int cantMozos, const char* nombre) {
    for (int i = 0; i < cantMozos; i++) {
        if (strcmp(mozos[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

int generarArchivoMozos (Mozo mozos[], int maxMozos){
    FILE* fHistoricas = fopen("datos/comandas_historicas.dat", "rb");
    if (!fHistoricas) {
        cout <<"ERROR : no se pudo abrir comandas_historicas.dat" << endl;
        return 0;
    } 
    int cantMozos = 0;
    ComandaHistorica reg;

    while (fread(&reg, sizeof(ComandaHistorica), 1, fHistoricas) == 1) {
        int idx = buscarMozoPorNombre(mozos, cantMozos, reg.nombreMozo);
        if (idx == -1) {
            if (cantMozos < maxMozos) {
                mozos[cantMozos].idMozo = cantMozos + 1;
                strcpy(mozos[cantMozos].nombre, reg.nombreMozo);
                mozos[cantMozos].totalComision = reg.comision;

                // Clave por defecto: su idMozo como texto, luego encriptado con +K
                char clavePlana[20];
                sprintf(clavePlana, "%d", mozos[cantMozos].idMozo);
                encriptarPassword(clavePlana, mozos[cantMozos].password, K_ENCRIPTACION);

                cantMozos++;
            }
        } else {
            mozos[idx].totalComision += reg.comision;
        }
    }
    fclose(fHistoricas);
    
    FILE* fMozos = fopen("datos/mozos.dat", "wb");
    if (!fMozos) {
        cout << "Error: no se pudo crear mozos.dat" << endl;
        return 0;
    }
    fwrite(mozos, sizeof(Mozo), cantMozos, fMozos);
    fclose(fMozos);

    cout << "[Tarea 1] mozos.dat generado con exito. Total: " << cantMozos << " mozos." << endl;
    return cantMozos;
}


int main() {
    Mozo listaMozos[100];
    int cantMozos = generarArchivoMozos(listaMozos, 100);
    mostrarMozos();
}
