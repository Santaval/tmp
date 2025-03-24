# Compiladores
CC  := gcc
CXX := g++

# Flags de compilación
CFLAGS   := -g -c
CXXFLAGS := -g -c

# Dependencias
DEPS := socks/Socket.hh socks/VSocket.hh Holder.hpp Server.hpp

# Archivos de salida
TARGETS := tests/testHolder.out tests/testServer.out 

# Archivos objeto (con rutas correctas)
OBJS := socks/VSocket.o socks/Socket.o VServer.o Holder.o Server.o

# Debug info
$(info $$TARGETS = $(TARGETS))

# Regla principal
all: $(TARGETS)

# Regla genérica para compilar cualquier .cc en la raíz
%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Regla para compilar archivos en `tests/`
tests/%.o: tests/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Regla para compilar archivos en `socks/`
socks/%.o: socks/%.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Reglas para generar ejecutables
tests/testHolder.out: tests/testHolder.o $(OBJS) Holder.o
	$(CXX) -g $^ -o $@

tests/testServer.out: tests/testServer.o $(OBJS) Server.o
	$(CXX) -g $^ -o $@

# Regla de limpieza
clean:
	rm -f *.o tests/*.o socks/*.o tests/*.out
