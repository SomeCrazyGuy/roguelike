CC	= gcc
CFLAGS	= -Wall -Wextra -O0 -g3
PROGNAME= game
SOURCE	= main.c

all: default
default:
	$(CC) $(CFLAGS) -o $(PROGNAME) $(SOURCE)
clean:
	rm $(PROGNAME)
