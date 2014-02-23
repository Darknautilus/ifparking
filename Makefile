# ======================
# INSA Lyon, Département Informatique
# TP 3IF : Programmation concurrente
# Binome : B3229
# ======================

MODE=DEBUG
CC=gcc
INCPATH= -I /include
LIBPATH= -L /lib
CFLAGS= -o $@ $(INCPATH) -c
LDFLAGS= -o $@ $(LIBPATH)
PATHEXEC=bin/
EXEC=Parking
LIBS= #-ltp -lncurses -ltcl 
TARGET=$(addprefix $(PATHEXEC), $(EXEC))

ifeq ($(MODE),DEBUG)
	CFLAGS:=$(CFLAGS) -g
endif

SRC=hello.cpp Mere.cpp
OBJ=$(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(PATHEXEC)
#	$(CC) -o $@ $^ $(LDFLAGS)
	$(CC) $(LDFLAGS) $^ $(LIBS)

%.o: %.cpp Config.h
#	$(CC) -o $@ -c $< $(CFLAGS)
	$(CC) $(CFLAGS) $(<)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@make -C tests

clean:
	@rm -rf *.o
	@make clean -C tests

mrproper: clean
	@rm -rf $(TARGET)
