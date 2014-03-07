# ======================
# INSA Lyon, Département Informatique
# TP 3IF : Programmation concurrente
# Binome : B3229
# ======================

MODE=NORMAL
CC=g++
CFLAGS= -I ./include
LDFLAGS=

# liaison de libtp suivant si l'on est à l'INSA ou non
LIBTPPATH=/shares/public/tp/tp-multitache/libtp.a
ifeq ($(wildcard $(LIBTPPATH)),)
	LDFLAGS+= ./lib/libtp.a
else
	LDFLAGS+= $(LIBTPPATH)
endif

LDFLAGS+= -ltcl -lncurses

PATHEXEC=bin/
EXEC=Parking
TARGET=$(addprefix $(PATHEXEC), $(EXEC))

ifeq ($(MODE),DEBUG)
	CFLAGS:=$(CFLAGS) -g
endif

SRC=Mere.cpp GererClavier.cpp BarriereEntree.cpp BarriereSortie.cpp
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
