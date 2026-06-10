# Compilador
CXX = g++

# Flags de compilación
CXXFLAGS = -std=c++11

# Librerías
LIBS = -lcurl

# Archivo fuente
SRC = src/main_temp_bs_as_rpi.cpp

# Ejecutable
TARGET = bin/app

# Regla principal
all: $(TARGET)

# Compilación
$(TARGET): $(SRC)
	$(CXX) -o $(TARGET) $(SRC) $(LIBS) $(CXXFLAGS)

# Limpiar archivos compilados
clean:
	rm -f $(TARGET)
