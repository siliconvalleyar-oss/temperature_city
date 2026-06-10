#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

using namespace nlohmann;

// Callback para escribir la respuesta
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

struct Ciudad {
    std::string nombre;
    double lat;
    double lon;
};

class ClimaArgentina {
private:
    std::vector<Ciudad> ciudades;

    void inicializarCiudades() {
        ciudades = {
            // Capital Federal y Congreso
            {"Buenos Aires (Congreso)", -34.6099, -58.3924},

            // Provincias y sus capitales
            {"Catamarca (San Fernando del Valle)", -28.4696, -65.7852},
            {"Chaco (Resistencia)", -27.4512, -58.9868},
            {"Chubut (Rawson)", -43.3002, -65.1023},
            {"Córdoba (Córdoba)", -31.4201, -64.1888},
            {"Corrientes (Corrientes)", -27.4692, -58.8306},
            {"Entre Ríos (Paraná)", -31.7311, -60.5238},
            {"Formosa (Formosa)", -26.1849, -58.1731},
            {"Jujuy (San Salvador)", -24.1858, -65.2995},
            {"La Pampa (Santa Rosa)", -36.6167, -64.2833},
            {"La Rioja (La Rioja)", -29.4131, -66.8556},
            {"Mendoza (Mendoza)", -32.8895, -68.8458},
            {"Misiones (Posadas)", -27.3666, -55.8969},
            {"Neuquén (Neuquén)", -38.9516, -68.0591},
            {"Río Negro (Viedma)", -40.8134, -62.9967},
            {"Salta (Salta)", -24.7821, -65.4232},
            {"San Juan (San Juan)", -31.5375, -68.5364},
            {"San Luis (San Luis)", -33.2968, -66.3291},
            {"Santa Cruz (Río Gallegos)", -51.6226, -69.2181},
            {"Santa Fe (Santa Fe)", -31.6333, -60.7000},
            {"Santiago del Estero (Santiago)", -27.7834, -64.2672},
            {"Tierra del Fuego (Ushuaia)", -54.8019, -68.3030},
            {"Tucumán (San Miguel)", -26.8083, -65.2176}
        };
    }

    std::string construirURL(double lat, double lon) {
        return "https://api.open-meteo.com/v1/forecast?latitude=" +
               std::to_string(lat) +
               "&longitude=" + std::to_string(lon) +
               "&current_weather=true&daily=temperature_2m_max,temperature_2m_min&timezone=auto";
    }

    bool obtenerClimaCiudad(const Ciudad& ciudad, double& temp_actual, double& temp_max, double& temp_min) {
        CURL* curl;
        CURLcode res;
        std::string response_string;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if(curl) {
            std::string url = construirURL(ciudad.lat, ciudad.lon);

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Para evitar problemas SSL

            res = curl_easy_perform(curl);

            if(res == CURLE_OK) {
                try {
                    json datos = json::parse(response_string);
                    temp_actual = datos["current_weather"]["temperature"];
                    temp_max = datos["daily"]["temperature_2m_max"][0];
                    temp_min = datos["daily"]["temperature_2m_min"][0];
                    curl_easy_cleanup(curl);
                    curl_global_cleanup();
                    return true;
                } catch (const std::exception& e) {
                    curl_easy_cleanup(curl);
                    curl_global_cleanup();
                    return false;
                }
            }

            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
        return false;
    }

public:
    void mostrarClimaTodasLasProvincias() {
        inicializarCiudades();

        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "        CLIMA EN CAPITAL FEDERAL Y CAPITALES DE PROVINCIAS" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        for (const auto& ciudad : ciudades) {
            double temp_actual, temp_max, temp_min;

            if (obtenerClimaCiudad(ciudad, temp_actual, temp_max, temp_min)) {
                std::cout << "\n📍 " << ciudad.nombre << std::endl;
                std::cout << "   🌡️  Actual: " << temp_actual << "°C  |  📈 Máx: " << temp_max << "°C  |  📉 Mín: " << temp_min << "°C" << std::endl;
            } else {
                std::cout << "\n📍 " << ciudad.nombre << " - ⚠️ No se pudieron obtener datos" << std::endl;
            }
        }

        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "✅ Datos de Open-Meteo (gratis, sin registro)" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }

    void mostrarClimaProvinciaEspecifica(int opcion) {
        inicializarCiudades();

        if (opcion < 1 || static_cast<size_t>(opcion) > ciudades.size()) {
            std::cout << "Opción inválida" << std::endl;
            return;
        }

        const auto& ciudad = ciudades[opcion - 1];
        double temp_actual, temp_max, temp_min;

        std::cout << "\nObteniendo clima de " << ciudad.nombre << "..." << std::endl;

        if (obtenerClimaCiudad(ciudad, temp_actual, temp_max, temp_min)) {
            std::cout << "\n" << std::string(50, '=') << std::endl;
            std::cout << "     CLIMA EN " << ciudad.nombre << std::endl;
            std::cout << std::string(50, '=') << std::endl;
            std::cout << "🌡️  Temperatura actual: " << temp_actual << "°C" << std::endl;
            std::cout << "📈  Máxima del día:    " << temp_max << "°C" << std::endl;
            std::cout << "📉  Mínima del día:    " << temp_min << "°C" << std::endl;
            std::cout << std::string(50, '=') << std::endl;
        } else {
            std::cout << "❌ No se pudo obtener el clima para " << ciudad.nombre << std::endl;
        }
    }

    void mostrarMenu() {
        inicializarCiudades();

        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "        CLIMA EN ARGENTINA - MENÚ PRINCIPAL" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "1. Mostrar clima de todas las provincias" << std::endl;
        std::cout << "2. Buscar clima de una provincia específica" << std::endl;
        std::cout << "3. Salir" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Opción: ";
    }

    void listarProvincias() {
        inicializarCiudades();

        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "        CAPITALES DE PROVINCIAS ARGENTINAS" << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        for (size_t i = 0; i < ciudades.size(); i++) {
            std::cout << i + 1 << ". " << ciudades[i].nombre << std::endl;
        }
        std::cout << std::string(60, '=') << std::endl;
    }
};

int main() {
    ClimaArgentina clima;
    int opcion;

    do {
        clima.mostrarMenu();
        std::cin >> opcion;

        switch(opcion) {
            case 1:
                clima.mostrarClimaTodasLasProvincias();
                break;
            case 2:
                clima.listarProvincias();
                std::cout << "\nSeleccione el número de provincia: ";
                int seleccion;
                std::cin >> seleccion;
                clima.mostrarClimaProvinciaEspecifica(seleccion);
                break;
            case 3:
                std::cout << "\n¡Hasta luego!" << std::endl;
                break;
            default:
                std::cout << "\n❌ Opción inválida. Intente nuevamente." << std::endl;
        }

        if (opcion != 3) {
            std::cout << "\nPresione Enter para continuar...";
            std::cin.ignore();
            std::cin.get();
        }

    } while(opcion != 3);

    return 0;
}
