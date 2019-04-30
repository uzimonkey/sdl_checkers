CC = gcc
CFLAGS = -Wall -std=c11 -pedantic
SRC = checkers.c

test: test.c tests.h $(SRC)
	$(CC) $(CFLAGS) -Imunit test.c munit/munit.c -o test

tests.h: test.c
	grep -o '^test(.\+)' test.c >tests.h

.phony: clean
clean:
	rm -f a.out test tests.h
