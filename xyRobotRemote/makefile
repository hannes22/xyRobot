SRC = main.c
SRC += serial.c
SRC += x11.c
SRC += util.c

TARGET = remote

HEADERS = x11.h
HEADERS += util.h
HEADERS += serial.h

# Define all object files.
OBJ = $(SRC:.c=.o) 

LIBPATH = /usr/X11R6/lib
LIB = X11
CC = gcc

# ---------

LDFLAGS = -L$(LIBPATH) -l$(LIB)
CFLAGS = -g

# Default target.
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJ)
	rm $(TARGET)
