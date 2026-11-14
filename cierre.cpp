#include "buffet.h"
#include <iostream>
#include <cstdio>
#include <string>

struct Fecha {
    int dia;
    int mes;
    int anio;
};

// Avanza exactamente 1 dia real en el calendario
void avanzarUnDia(Fecha& f) {
    int maxDias = 31;
    if (f.mes == 4 || f.mes == 6 || f.mes == 9 || f.mes == 11) {
        maxDias = 30;
    } else if (f.mes == 2) {
        bool bisiesto = (f.anio % 4 == 0 && f.anio % 100 != 0) || (f.anio % 400 == 0);
        maxDias = bisiesto ? 29 : 28;
    }

    f.dia++;
    if (f.dia > maxDias) {
        f.dia = 1;
        f.mes++;
        if (f.mes > 12) {
            f.mes = 1;
            f.anio++;
        }
    }
}

std::string dosDigitos(int n) {
    return (n < 10 ? "0" : "") + std::to_string(n);
}

bool aparearArchivos(const char* rutaBase, const char* rutaNuevoDiario, const char* rutaSalida);

int main() {
    Fecha fecha;
    const std::string nombreTempBase = "temporal_base.dat";
    const std::string nombreTempSalida = "temporal_salida.dat";

    std::cout << "Decime la fecha del lunes (dia mes anio): ";
    std::cin >> fecha.dia >> fecha.mes >> fecha.anio;

    // Calcular numero de semana (restar 7 hasta que sea <= 0)
    int numSemana = 1;
    int tempDia = fecha.dia;
    while (tempDia - 7 > 0) {
        numSemana++;
        tempDia -= 7;
    }

    std::string nombreSemanal = "comandas_semana_s" + std::to_string(numSemana) + "-" + dosDigitos(fecha.mes) + ".dat";
    int archivosProcesados = 0;

    // 7 vueltas exactas: lunes (0) a domingo (6). Nunca se pasa a la semana siguiente.
    for (int i = 0; i < 7; i++) {
        std::string nombreDiario = "comandas_" + dosDigitos(fecha.dia) + "-" + dosDigitos(fecha.mes) + "-" + std::to_string(fecha.anio) + ".dat";

        FILE* testArch = fopen(nombreDiario.c_str(), "rb");
        if (testArch != NULL) {
            fclose(testArch);

            if (archivosProcesados == 0) {
                FILE* archDiario = fopen(nombreDiario.c_str(), "rb");
                FILE* archBase = fopen(nombreTempBase.c_str(), "wb");

                Comanda reg;
                while (fread(&reg, sizeof(Comanda), 1, archDiario) == 1) {
                    fwrite(&reg, sizeof(Comanda), 1, archBase);
                }

                fclose(archDiario);
                fclose(archBase);

                archivosProcesados++;
                std::cout << "-> Archivo " << nombreDiario << " establecido como base inicial." << std::endl;
            } else {
                std::cout << "-> Apareando " << nombreDiario << " con el acumulado semanal..." << std::endl;

                if (aparearArchivos(nombreTempBase.c_str(), nombreDiario.c_str(), nombreTempSalida.c_str())) {
                    remove(nombreTempBase.c_str());
                    rename(nombreTempSalida.c_str(), nombreTempBase.c_str());
                    archivosProcesados++;
                    std::cout << "-> Archivo " << nombreDiario << " integrado correctamente de forma ordenada." << std::endl;
                } else {
                    std::cout << "[ERROR] Fallo el procesamiento del archivo " << nombreDiario << "." << std::endl;
                }
            }
        } else {
            std::cout << "[AVISO] Sin actividad el " << dosDigitos(fecha.dia) << "/" << dosDigitos(fecha.mes) << " (no existe archivo)." << std::endl;
        }

        avanzarUnDia(fecha);
    }

    if (archivosProcesados > 0) {
        remove(nombreSemanal.c_str());
        if (rename(nombreTempBase.c_str(), nombreSemanal.c_str()) == 0) {
            std::cout << "\n¡Proceso terminado con exito! Se genero '" << nombreSemanal 
                      << "' con " << archivosProcesados << " dias trabajados." << std::endl;
        } else {
            std::cout << "\n[ERROR] No se pudo renombrar el archivo final a " << nombreSemanal << std::endl;
        }
    } else {
        std::cout << "\nNo se encontraron archivos para esa semana." << std::endl;
        remove(nombreTempBase.c_str());
    }

    return 0;
}

bool aparearArchivos(const char* rutaBase, const char* rutaNuevoDiario, const char* rutaSalida) {
    FILE* archBase = fopen(rutaBase, "rb");
    FILE* archDiario = fopen(rutaNuevoDiario, "rb");
    FILE* archSalida = fopen(rutaSalida, "wb");

    if (archBase == NULL || archDiario == NULL || archSalida == NULL) {
        if (archBase) fclose(archBase);
        if (archDiario) fclose(archDiario);
        if (archSalida) fclose(archSalida);
        return false;
    }

    Comanda regBase, regDiario;
    bool tieneBase = (fread(&regBase, sizeof(Comanda), 1, archBase) == 1);
    bool tieneDiario = (fread(&regDiario, sizeof(Comanda), 1, archDiario) == 1);

    while (tieneBase && tieneDiario) {
        if (regBase.idMozo <= regDiario.idMozo) {
            fwrite(&regBase, sizeof(Comanda), 1, archSalida);
            tieneBase = (fread(&regBase, sizeof(Comanda), 1, archBase) == 1);
        } else {
            fwrite(&regDiario, sizeof(Comanda), 1, archSalida);
            tieneDiario = (fread(&regDiario, sizeof(Comanda), 1, archDiario) == 1);
        }
    }

    while (tieneBase) {
        fwrite(&regBase, sizeof(Comanda), 1, archSalida);
        tieneBase = (fread(&regBase, sizeof(Comanda), 1, archBase) == 1);
    }

    while (tieneDiario) {
        fwrite(&regDiario, sizeof(Comanda), 1, archSalida);
        tieneDiario = (fread(&regDiario, sizeof(Comanda), 1, archDiario) == 1);
    }

    fclose(archBase);
    fclose(archDiario);
    fclose(archSalida);

    return true;
}

    return 0;
} 
