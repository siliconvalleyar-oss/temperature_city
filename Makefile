CXX       = g++
CXXFLAGS  = -std=c++11 -Wall -Wextra -I.
LIBS      = -lcurl

SRC       = src/main_temp_bs_as_rpi.cpp
TARGET    = bin/app

all: $(TARGET) bin/main_temp_arg_rpi bin/main_clima_rpi qt-build

$(TARGET): src/main_temp_bs_as_rpi.cpp | bin
	$(CXX) -o $@ $< $(LIBS) $(CXXFLAGS)

bin/main_temp_arg_rpi: src/main_temp_arg_rpi.cpp | bin
	$(CXX) -o $@ $< $(LIBS) $(CXXFLAGS)

bin/main_clima_rpi: src/main_clima_rpi.cpp | bin
	$(CXX) -o $@ $< $(LIBS) $(CXXFLAGS)

qt-build:
	$(MAKE) -C qt

bin/weather: qt-build
	cp qt/weather bin/

bin:
	mkdir -p bin

clean:
	rm -f bin/app bin/main_temp_arg_rpi bin/main_clima_rpi
	$(MAKE) -C qt clean

distclean: clean
	rm -f bin/weather

.PHONY: all clean distclean qt-build
