all:
	clang *.c -o build-results/dirlcnt -Wall -Wextra -pedantic -ggdb -std=c99 -Wno-missing-braces