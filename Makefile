# ======================
# INSA Lyon, Département Informatique
# TP 3IF : Programmation concurrente
# Binome : B3229
# ======================

MODE=NORMAL
CC=g++
CFLAGS= -I /shares/public/tp/tp-multitache/
LDFLAGS= /shares/public/tp/tp-multitache/libtp.a #librairie statiques
PATHEXEC=bin/
EXEC=Parking
TARGET=$(addprefix $(PATHEXEC), $(EXEC))
LIBS = -ltcl -lncurses #-ltp		#librairies systèmes

ifeq ($(MODE),DEBUG)
	CFLAGS:=$(CFLAGS) -g
endif

SRC=Mere.cpp
OBJ=$(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(PATHEXEC)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

run: $(TARGET)
	@./$(TARGET)

#test: $(TARGET)

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(TARGET)
