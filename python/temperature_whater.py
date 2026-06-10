import requests

# Por ahora, usemos una API pública sin registro (limitada pero funciona para probar)
# Esta es una API de prueba gratuita

def obtener_clima_por_api():
    # API pública de demostración (datos de ejemplo actualizados)
    # No necesita registro, pero son datos simulados de Buenos Aires
    
    # Opción 1: Usar wttr.in (API pública sin clave, datos reales)
    try:
        url = "https://wttr.in/Lanús?format=%t+%w+%h+%m"
        response = requests.get(url, timeout=10)
        if response.status_code == 200:
            print("\n=== CLIMA EN LANÚS ===")
            print(f"Datos: {response.text}")
            return
    except:
        pass
    
    # Opción 2: Usar Open-Meteo (API gratuita, sin registro, datos reales)
    try:
        # Coordenadas de Lanús: -34.7036, -58.3977
        url_meteo = "https://api.open-meteo.com/v1/forecast?latitude=-34.7036&longitude=-58.3977&current_weather=true&timezone=America/Argentina/Buenos_Aires"
        response = requests.get(url_meteo, timeout=10)
        
        if response.status_code == 200:
            datos = response.json()
            temp = datos["current_weather"]["temperature"]
            viento = datos["current_weather"]["windspeed"]
            
            print("\n=== CLIMA EN LANÚS (Open-Meteo) ===")
            print(f"🌡️ Temperatura actual: {temp}°C")
            print(f"💨 Velocidad del viento: {viento} km/h")
            print("✅ Datos reales - No necesita registro")
            return
    except:
        pass
    
    # Opción 3: Simular datos si no hay internet
    print("\n=== CLIMA EN LANÚS ===")
    print("🌡️ Temperatura: ~11°C")
    print("☁️ Nublado")
    print("📈 Máx: 16°C / Mín: 8°C")
    print("\n⚠️ Usando datos estimados (sin conexión a internet)")

# Ejecutar
if __name__ == "__main__":
    obtener_clima_por_api()
