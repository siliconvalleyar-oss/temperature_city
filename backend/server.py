#!/usr/bin/env python3
"""Weather API server — serves REST endpoint + static web frontend."""

import http.server
import json
import urllib.request
import urllib.error
import os
import sys


CITIES = [
    {"name": "Buenos Aires (Congreso)", "lat": -34.6099, "lon": -58.3924},
    {"name": "Catamarca", "lat": -28.4696, "lon": -65.7852},
    {"name": "Resistencia (Chaco)", "lat": -27.4512, "lon": -58.9868},
    {"name": "Rawson (Chubut)", "lat": -43.3002, "lon": -65.1023},
    {"name": "Córdoba", "lat": -31.4201, "lon": -64.1888},
    {"name": "Corrientes", "lat": -27.4692, "lon": -58.8306},
    {"name": "Paraná (Entre Ríos)", "lat": -31.7311, "lon": -60.5238},
    {"name": "Formosa", "lat": -26.1849, "lon": -58.1731},
    {"name": "San Salvador de Jujuy", "lat": -24.1858, "lon": -65.2995},
    {"name": "Santa Rosa (La Pampa)", "lat": -36.6167, "lon": -64.2833},
    {"name": "La Rioja", "lat": -29.4131, "lon": -66.8556},
    {"name": "Mendoza", "lat": -32.8895, "lon": -68.8458},
    {"name": "Posadas (Misiones)", "lat": -27.3666, "lon": -55.8969},
    {"name": "Neuquén", "lat": -38.9516, "lon": -68.0591},
    {"name": "Viedma (Río Negro)", "lat": -40.8134, "lon": -62.9967},
    {"name": "Salta", "lat": -24.7821, "lon": -65.4232},
    {"name": "San Juan", "lat": -31.5375, "lon": -68.5364},
    {"name": "San Luis", "lat": -33.2968, "lon": -66.3291},
    {"name": "Río Gallegos (Santa Cruz)", "lat": -51.6226, "lon": -69.2181},
    {"name": "Santa Fe", "lat": -31.6333, "lon": -60.7000},
    {"name": "Santiago del Estero", "lat": -27.7834, "lon": -64.2672},
    {"name": "Ushuaia (Tierra del Fuego)", "lat": -54.8019, "lon": -68.3030},
    {"name": "San Miguel de Tucumán", "lat": -26.8083, "lon": -65.2176},
    {"name": "Mar del Plata", "lat": -38.0055, "lon": -57.5426},
    {"name": "La Plata", "lat": -34.9205, "lon": -57.9536},
    {"name": "Bahía Blanca", "lat": -38.7196, "lon": -62.2724},
    {"name": "Tandil", "lat": -37.3217, "lon": -59.1332},
    {"name": "El Calafate", "lat": -50.3408, "lon": -72.2761},
    {"name": "Bariloche", "lat": -41.1335, "lon": -71.3103},
    {"name": "Lanús", "lat": -34.7036, "lon": -58.3977},
]

API_BASE = "https://api.open-meteo.com/v1/forecast"


def fetch_weather_all():
    results = []
    for c in CITIES:
        url = (
            f"{API_BASE}?latitude={c['lat']}&longitude={c['lon']}"
            f"&current=temperature_2m,relative_humidity_2m,apparent_temperature,"
            f"weather_code,wind_speed_10m"
            f"&daily=temperature_2m_max,temperature_2m_min,precipitation_sum"
            f"&timezone=auto&forecast_days=1"
        )
        try:
            req = urllib.request.Request(url, headers={"User-Agent": "temperature-city/1.0"})
            with urllib.request.urlopen(req, timeout=10) as resp:
                data = json.loads(resp.read().decode())
                current = data["current"]
                daily = data["daily"]
                results.append({
                    "name": c["name"],
                    "lat": c["lat"],
                    "lon": c["lon"],
                    "temp": current["temperature_2m"],
                    "feels_like": current["apparent_temperature"],
                    "humidity": current["relative_humidity_2m"],
                    "wind": current["wind_speed_10m"],
                    "weather_code": current["weather_code"],
                    "temp_max": daily["temperature_2m_max"][0],
                    "temp_min": daily["temperature_2m_min"][0],
                    "precipitation": daily["precipitation_sum"][0],
                })
        except (urllib.error.URLError, urllib.error.HTTPError, json.JSONDecodeError, KeyError) as e:
            results.append({
                "name": c["name"],
                "lat": c["lat"],
                "lon": c["lon"],
                "error": str(e),
            })
    return results


WEB_DIR = os.path.join(os.path.dirname(__file__), "..", "web")


class WeatherHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def do_GET(self):
        if self.path == "/api/weather":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.end_headers()
            data = fetch_weather_all()
            self.wfile.write(json.dumps(data, ensure_ascii=False).encode())
        else:
            super().do_GET()

    def log_message(self, fmt, *args):
        sys.stderr.write(f"[{self.log_date_time_string()}] {args[0]} {args[1]} {args[2]}\n")


def main():
    port = int(os.environ.get("PORT", 8000))
    with socketserver.TCPServer(("", port), WeatherHandler) as httpd:
        print(f"Serving at http://localhost:{port}")
        print(f"API: http://localhost:{port}/api/weather")
        httpd.serve_forever()


if __name__ == "__main__":
    import socketserver
    main()
