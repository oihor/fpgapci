CC := gcc
PROGRAM = fpgapci_test_util
SRC := main.c
CFLAGS := -Wall -I../common
LDFLAG := -lm

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAG) -o $(PROGRAM)

clean:
	rm -fr $(PROGRAM) $(PROGRAM).o
	