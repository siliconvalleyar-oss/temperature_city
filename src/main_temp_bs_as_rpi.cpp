#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

using namespace nlohmann;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

int main() {
    std::vector<std::pair<std::string, std::pair<double, double>>> ciudades = {
        // 50 ciudades del interior bonaerense
        {"Mar del Plata", {-38.0055, -57.5426}},
        {"La Plata", {-34.9205, -57.9536}},
        {"Bahía Blanca", {-38.7196, -62.2724}},
        {"Tandil", {-37.3217, -59.1332}},
        {"Necochea", {-38.5545, -58.7366}},
        {"Olavarría", {-36.8927, -60.3225}},
        {"Junín", {-34.5850, -60.9584}},
        {"San Nicolás", {-33.3359, -60.2266}},
        {"Pergamino", {-33.8897, -60.5730}},
        {"Chivilcoy", {-34.8956, -60.0168}},
        {"Mercedes", {-34.6514, -59.4306}},
        {"Luján", {-34.5700, -59.1050}},
        {"Zárate", {-34.0981, -59.0286}},
        {"Campana", {-34.1681, -58.9589}},
        {"Azul", {-36.7833, -59.8500}},
        {"Chacabuco", {-34.6333, -60.4667}},
        {"Salto", {-34.3000, -60.2500}},
        {"Rojas", {-34.2000, -60.7500}},
        {"Pehuajó", {-35.8108, -61.8964}},
        {"Lincoln", {-34.8661, -61.5300}},
        {"Trenque Lauquen", {-35.9697, -62.7333}},
        {"Coronel Suárez", {-37.4667, -61.9167}},
        {"Pigüé", {-37.6000, -62.4000}},
        {"Tornquist", {-38.1000, -62.2333}},
        {"Monte Hermoso", {-38.9833, -61.3000}},
        {"Villa Gesell", {-37.2639, -56.9731}},
        {"Pinamar", {-37.1111, -56.8653}},
        {"Miramar", {-38.2667, -57.8333}},
        {"San Pedro", {-33.6789, -59.6669}},
        {"Baradero", {-33.8078, -59.5081}},
        {"San Antonio de Areco", {-34.2500, -59.4667}},
        {"Carmen de Areco", {-34.3775, -59.8258}},
        {"Arrecifes", {-34.0664, -60.1064}},
        {"Ramallo", {-33.4847, -60.0083}},
        {"Colón", {-33.9000, -61.1000}},
        {"General Villegas", {-35.0333, -63.0167}},
        {"Rivadavia", {-36.2833, -62.8333}},
        {"Sierra de la Ventana", {-38.1333, -61.7833}},
        {"Cariló", {-37.1667, -56.9000}}
    };

    CURL* curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::cout << "\n" << std::string(90, '=') << std::endl;
    std::cout << "        🌡️  CLIMA EN CIUDADES DEL INTERIOR DE BUENOS AIRES  🌡️" << std::endl;
    std::cout << std::string(90, '=') << std::endl;

    int count = 0;
    for (const auto& ciudad : ciudades) {
        std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" +
                         std::to_string(ciudad.second.first) +
                         "&longitude=" + std::to_string(ciudad.second.second) +
                         "&current_weather=true&daily=temperature_2m_max,temperature_2m_min&timezone=auto";

        std::string response_string;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        CURLcode res = curl_easy_perform(curl);

        if(res == CURLE_OK) {
            try {
                json datos = json::parse(response_string);
                double temp_actual = datos["current_weather"]["temperature"];
                double temp_max = datos["daily"]["temperature_2m_max"][0];
                double temp_min = datos["daily"]["temperature_2m_min"][0];

                std::cout << "\n📍 " << ciudad.first << std::endl;
                std::cout << "   🌡️  Actual: " << temp_actual << "°C  |  📈 Máx: " << temp_max << "°C  |  📉 Mín: " << temp_min << "°C" << std::endl;
                count++;
            } catch (...) {
                std::cout << "\n📍 " << ciudad.first << " - ⚠️ Error" << std::endl;
            }
        } else {
            std::cout << "\n📍 " << ciudad.first << " - ⚠️ Sin conexión" << std::endl;
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    std::cout << "\n" << std::string(90, '=') << std::endl;
    std::cout << "✅ Total ciudades reportadas: " << count << "/" << ciudades.size() << std::endl;
    std::cout << "📊 Datos actualizados - Open-Meteo API" << std::endl;
    std::cout << std::string(90, '=') << std::endl;

    return 0;
}
