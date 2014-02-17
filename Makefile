# ======================
# INSA Lyon, Dértement Informatique
# TP 3IF : Programmation concurrente
# Binome : B3229
# ======================

MODE=DEBUG
CC=g++
CFLAGS=
LDFLAGS=
PATHEXEC=bin/
EXEC=Parking
TARGET=$(addprefix $(PATHEXEC), $(EXEC))

ifeq ($(MODE),DEBUG)
	CFLAGS:=$(CFLAGS) -g
endif

SRC=hello.cpp
OBJ=$(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(PATHEXEC)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp Config.h
	$(CC) -o $@ -c $< $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@make -C tests

clean:
	@rm -rf *.o
	@make clean -C tests

mrproper: clean
	@rm -rf $(TARGET)
