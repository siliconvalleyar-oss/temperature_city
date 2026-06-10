#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class ClimaLanús {
private:
    const double lat = -34.7036;
    const double lon = -58.3977;
    
    std::string construirURL() {
        return "/v1/forecast?latitude=" + std::to_string(lat) + 
               "&longitude=" + std::to_string(lon) + 
               "&current_weather=true&daily=temperature_2m_max,temperature_2m_min&timezone=auto";
    }
    
    std::string hacerPeticionHTTPS(const std::string& host, const std::string& path) {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
        
        SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
        if (!ctx) return "";
        
        struct hostent* server = gethostbyname(host.c_str());
        if (!server) return "";
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(443);
        memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);
        
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) return "";
        
        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            return "";
        }
        
        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        
        if (SSL_connect(ssl) <= 0) {
            SSL_free(ssl);
            close(sock);
            return "";
        }
        
        std::string request = "GET " + path + " HTTP/1.1\r\n" +
                              "Host: " + host + "\r\n" +
                              "Connection: close\r\n\r\n";
        
        SSL_write(ssl, request.c_str(), request.length());
        
        std::string response;
        char buffer[4096];
        int bytes;
        while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            response += buffer;
        }
        
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        
        // Encontrar el cuerpo de la respuesta (después de las cabeceras)
        size_t bodyPos = response.find("\r\n\r\n");
        if (bodyPos != std::string::npos) {
            return response.substr(bodyPos + 4);
        }
        
        return "";
    }
    
    // Función simple para extraer valores del JSON (sin librería externa)
    double extraerValor(const std::string& json, const std::string& clave) {
        std::string buscar = "\"" + clave + "\":";
        size_t pos = json.find(buscar);
        if (pos == std::string::npos) return 0;
        
        pos = json.find(":", pos);
        if (pos == std::string::npos) return 0;
        pos++;
        
        // Saltar espacios
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        
        std::string numero;
        while (pos < json.length() && (isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-')) {
            numero += json[pos];
            pos++;
        }
        
        return std::stod(numero);
    }
    
public:
    void obtenerClima() {
        std::string host = "api.open-meteo.com";
        std::string path = construirURL();
        
        std::cout << "Conectando a " << host << "..." << std::endl;
        
        std::string jsonRespuesta = hacerPeticionHTTPS(host, path);
        
        if (jsonRespuesta.empty()) {
            std::cout << "Error: No se pudo obtener datos" << std::endl;
            std::cout << "\n💡 Verifica tu conexión a internet" << std::endl;
            return;
        }
        
        try {
            double temp_actual = extraerValor(jsonRespuesta, "temperature");
            double temp_max = extraerValor(jsonRespuesta, "temperature_2m_max");
            double temp_min = extraerValor(jsonRespuesta, "temperature_2m_min");
            
            std::cout << "\n========================================" << std::endl;
            std::cout << "     CLIMA EN LANÚS - HOY" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "🌡️  Temperatura actual: " << temp_actual << "°C" << std::endl;
            std::cout << "📈  Máxima del día:    " << temp_max << "°C" << std::endl;
            std::cout << "📉  Mínima del día:    " << temp_min << "°C" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "✅ Datos de Open-Meteo" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error al procesar datos: " << e.what() << std::endl;
        }
    }
};

int main() {
    ClimaLanús clima;
    clima.obtenerClima();
    
    return 0;
}
