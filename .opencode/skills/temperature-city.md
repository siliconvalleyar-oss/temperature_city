---
name: temperature-city
description: Proyecto de clima para ciudades de Argentina con frontend web visual
---

# temperature-city Skill

Proyecto de consulta climática para ciudades argentinas con interfaz C++ (terminal),
web (HTML/CSS/JS) y aplicación de escritorio (Qt5).

## Archivos principales

- `src/*.cpp` — Clientes C++ que consultan Open-Meteo API
- `qt/` — Aplicación de escritorio Qt5 con grilla de tarjetas
- `backend/server.py` — Servidor Python que sirve API REST + frontend web
- `web/` — Frontend visual HTML/CSS/JS
- `python/temperature_whater.py` — Script Python simple
- `scripts/get_calafate.sh` — Script shell para El Calafate
- `Makefile` — Compilación de binarios C++

## Uso

```bash
# C++ (terminal)
make && ./bin/app

# Qt5 (escritorio)
make qt-build && ./qt/weather

# Servidor web
python3 backend/server.py
# Abrir http://localhost:8000

# Script Python
python3 python/temperature_whater.py
```

## API externas

- [Open-Meteo](https://open-meteo.com/) — datos climáticos gratuitos sin registro
- [wttr.in](https://wttr.in/) — clima por curl
