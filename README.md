# temperature_city

Herramientas para consultar el clima de ciudades de Argentina desde la terminal y el navegador.

## Estructura

```
src/                  - Código fuente C++
  main_temp_bs_as_rpi.cpp   - Clima de 40 ciudades del interior bonaerense
  main_temp_arg_rpi.cpp     - Clima de capitales provinciales (menú interactivo)
  main_clima_rpi.cpp        - Clima de Lanús (versión simple con libcurl)
  main_temp_rpi.cpp         - Clima de Lanús (versión con OpenSSL nativo)
qt/                   - Aplicación de escritorio Qt (C++/Qt5)
  weather.pro               - Archivo de proyecto qmake
  main.cpp                  - Punto de entrada
  mainwindow.h/cpp          - Ventana principal con grilla de ciudades
  weathercard.h/cpp         - Widget de tarjeta de clima
python/               - Scripts Python
  temperature_whater.py     - Clima de Lanús (wttr.in / Open-Meteo)
backend/              - Servidor web Python
  server.py                 - API REST + servidor de archivos estáticos
web/                  - Frontend visual
  index.html                - Dashboard HTML
  style.css                 - Estilos (dark theme, responsive)
  app.js                    - Lógica de fetch y renderizado
scripts/              - Scripts auxiliares
  get_calafate.sh           - Clima de El Calafate por curl
nlohmann/             - Librería JSON header-only (nlohmann/json)
bin/                  - Ejecutables compilados (gitignored)
venv/                 - Entorno virtual Python (gitignored)
```

## Requisitos

### C++ (libcurl)
```bash
sudo apt install g++ libcurl4-openssl-dev
```

### Qt5 (aplicación de escritorio)
```bash
sudo apt install qtbase5-dev
```

### Python
```bash
pip install requests
```

## Uso

### C++ (terminal)
```bash
make
./bin/app                    # 40 ciudades del interior bonaerense
./bin/main_temp_arg_rpi      # Capitales provinciales (menú interactivo)
./bin/main_clima_rpi         # Lanús
```

### Qt5 (aplicación de escritorio)
```bash
make qt-build
./qt/weather                 # 20 ciudades argentinas con interfaz gráfica
```

### Servidor web (recomendado)
```bash
python3 backend/server.py
# Abrir http://localhost:8000
```

### Scripts individuales
```bash
python3 python/temperature_whater.py
bash scripts/get_calafate.sh
```

## API

El servidor web expone un endpoint REST:

```
GET /api/weather  →  JSON con todas las ciudades
```

Datos incluidos: temperatura actual, sensación térmica, humedad, viento,
código meteorológico, temperatura máxima/mínima y precipitación.

## APIs externas

- [Open-Meteo](https://open-meteo.com/) — gratuita, sin registro
- [wttr.in](https://wttr.in/) — gratuita, sin registro

## Skill

Este proyecto incluye un skill para opencode en `.opencode/skills/temperature-city.md`.
