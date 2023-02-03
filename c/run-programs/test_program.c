// Яндиев А, 211 группа, ДЗ-9 (для теста)

// теперь exit возвращает 0

// 3000 раз печатает pid
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	pid_t p = getpid();
	int i, j;
	printf("\n\n\n");
	for (i=0; i<300; i++) {
		for (j=0; j<10; j++)
			printf("%d\t", p);
		putchar('\n');
	}
	exit(0);
}