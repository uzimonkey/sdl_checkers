CC = gcc
CFLAGS = -Wall -std=c11 -pedantic `pkg-config --cflags sdl2`
LDFLAGS = `pkg-config --libs sdl2` -lm

sdl_checkers: checkers.c checkers.h main.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

test: test.c tests.h checkers.c checkers.h
	$(CC) $(CFLAGS) -Imunit test.c munit/munit.c -o test

tests.h: test.c
	grep -o '^test(.\+)' test.c >tests.h

.phony: clean
clean:
	rm -f a.out test sdl_checkers tests.h

.phony: run
run: sdl_checkers
	./sdl_checkers
