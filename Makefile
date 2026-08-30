# Makefile — Practica 1 ADA (FB + BT unificados en un unico binario)
#
# Por que existe: la linea de compilacion literal del enunciado
# (g++ -std=c++17 -O2 -o ada_p1 src/main.cpp src/*.cpp) falla con
# "multiple definition of main" porque el glob src/*.cpp ya incluye a
# main.cpp. Este Makefile compila con la linea que si funciona.
#
# Uso:
#   make          -> compila ./ada_p1
#   make test     -> compila y corre tests/test_ada_p1.sh
#   make clean    -> borra el binario

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic
TARGET   := ada_p1
SRC      := $(wildcard src/*.cpp)

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

test: all
	bash tests/test_ada_p1.sh

clean:
	rm -f $(TARGET)
