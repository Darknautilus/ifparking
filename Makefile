# ======================
# INSA Lyon, Département Informatique
# TP 3IF : Programmation concurrente
# Binome : B3229
# ======================

MODE=DEBUG
CC=g++
CFLAGS= -I ./include/
LDFLAGS= -L ./lib/
PATHEXEC=bin/
EXEC=Parking
TARGET=$(addprefix $(PATHEXEC), $(EXEC))

ifeq ($(MODE),DEBUG)
	CFLAGS:=$(CFLAGS) -g
endif

SRC=hello.cpp #Mere.cpp
OBJ=$(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(PATHEXEC)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

run: $(TARGET)
	@./$(TARGET)

#test: $(TARGET)

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(TARGET)
