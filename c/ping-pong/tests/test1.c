// Яндиев А, 211 группа, ДЗ-10.1.2

// "Пинг-понг" с 1 каналом и сигналами

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

pid_t p, elder;
int max, fd[2];

void F1 (int s);
void F2 (int s);

int main (int argc, char *argv[]) {
// проверка на количество введенных параметров	
	if (argc != 4) {
		printf("Number of parameters: %d is incorrect\n", argc-1);
		return 0;
	}
// считывание количества процессов N, проверка значения
	int N = atoi(argv[1]);
	if (N < 2) {
		printf("Number of processes N should be >= 2\n");
		return 0;
	}
// инициализация, считывание диапазона [a, b]
// если a > b, то программа просто завершается
	elder = getpid();
	int i, cur = atoi(argv[2]);
	max = atoi(argv[3]);
	if (cur > max)
		return 0;
// создание канала
	pipe(fd);
// установка обработчика сигнала 
	signal(SIGUSR1, F1);
	signal(SIGUSR2, F2);
	write(fd[1], &cur, sizeof(int));
	for (i=1; ; i++) {
		if (i < N) {
			if ((p = fork()) < 0) {
				perror("fork");
				close(fd[0]); close(fd[1]);
				exit(0); 
			}
		}
		else  // i = N. p = 0
			p = elder; // обращение к пра...прадеду (первому)
		if (p) {
			if (i!=N) {
				if (i == 2) {
					kill(getppid(), SIGUSR1);
					pause(); pause();
					kill(p, SIGUSR1);
				}
				else if (i > 2) {
					kill(getppid(), SIGUSR2);
					pause(); pause();
					kill(p, SIGUSR1);
				}
			}
			else {
				if (i == 2) 
					kill(getppid(), SIGUSR1);
				else
					kill(getppid(), SIGUSR2);
			}
			for (;;)
				pause();
		}
	}
	return 0;
}

void F1 (int s) {
	signal(SIGUSR1, F1);
	static char flag = 1;
	int cur;
	read(fd[0], &cur, sizeof(int));
	if (cur <= max)
		printf("pid = %d, number = %d\n", getpid(), cur++);
	write(fd[1], &cur, sizeof(int));
	if (!flag || (elder == getpid()) || (p == elder)) 
		kill(p, SIGUSR1);
	else
		flag--;
	if (cur > max) {
		close(fd[0]); close(fd[1]);
		exit(0);
	}
	return;
}

void F2 (int s) {
	signal(SIGUSR2, F2);
}