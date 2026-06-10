#include <iostream>
#include <string>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

using namespace nlohmann;

// Callback para escribir la respuesta
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

class ClimaLanús {
private:
    const double lat = -34.7036;
    const double lon = -58.3977;
    
    std::string construirURL() {
        return "https://api.open-meteo.com/v1/forecast?latitude=" + 
               std::to_string(lat) + 
               "&longitude=" + std::to_string(lon) + 
               "&current_weather=true&daily=temperature_2m_max,temperature_2m_min&timezone=auto";
    }
    
public:
    void obtenerClima() {
        CURL* curl;
        CURLcode res;
        std::string response_string;
        
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        
        if(curl) {
            std::string url = construirURL();
            
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            
            // Realizar petición
            res = curl_easy_perform(curl);
            
            if(res == CURLE_OK) {
                try {
                    // Parsear JSON
                    json datos = json::parse(response_string);
                    
                    // Extraer datos
                    double temp_actual = datos["current_weather"]["temperature"];
                    double temp_max = datos["daily"]["temperature_2m_max"][0];
                    double temp_min = datos["daily"]["temperature_2m_min"][0];
                    
                    // Mostrar resultados
                    std::cout << "\n========================================" << std::endl;
                    std::cout << "     CLIMA EN LANÚS - HOY" << std::endl;
                    std::cout << "========================================" << std::endl;
                    std::cout << "🌡️  Temperatura actual: " << temp_actual << "°C" << std::endl;
                    std::cout << "📈  Máxima del día:    " << temp_max << "°C" << std::endl;
                    std::cout << "📉  Mínima del día:    " << temp_min << "°C" << std::endl;
                    std::cout << "========================================" << std::endl;
                    std::cout << "✅ Datos de Open-Meteo (gratis, sin registro)" << std::endl;
                    
                } catch (const std::exception& e) {
                    std::cout << "Error al parsear JSON: " << e.what() << std::endl;
                }
            } else {
                std::cout << "Error en petición HTTP: " << curl_easy_strerror(res) << std::endl;
            }
            
            curl_easy_cleanup(curl);
        }
        
        curl_global_cleanup();
    }
};

int main() {
    ClimaLanús clima;
    clima.obtenerClima();
    
    return 0;
}

