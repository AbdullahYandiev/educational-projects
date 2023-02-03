// Яндиев А, 211 группа, ДЗ-10.2.2 (а, б)
// В командной строке нужно указать букву a или b: ./prog a или ./prog b

// Проверка, блокируется ли на время обработчика сигнала данный и другие сигналы

// Вывод:
// В моей системе сигналы не блокируются ни в том, ни в ином случае

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int fd[2][2], i = 0;

void handler (int s);

int main (int argc, char *argv[]) {
	if (argc == 1) {
		printf("Please enter operating mode (a or b)\n");
		return 0;
	}
	char mode = argv[1][0];
	if ((mode != 'a' && mode != 'b') || argc != 2) {
		printf("Mode can be only a or b\n");
		return 0;
	}
	int status;
	char c;
	pid_t p;
	pipe(fd[0]); pipe(fd[1]);
	signal(SIGUSR1, handler);
	if ((p = fork()) < 0) {
		perror("fork");
		return 0; 
	}
	if (p) {
		close(fd[0][1]); close(fd[1][0]);
		write(fd[1][1], &c, sizeof(char));
		kill (p, SIGUSR1);
		read(fd[0][0], &c, sizeof(char));
// проверяем блокируется ли тот же сигнал		
		if (mode == 'a') {
			kill (p, SIGUSR1);
			usleep(100000);	// чтобы гарантированно сигнал дошел во время обработки прошлого
			write(fd[1][1], &c, sizeof(char));
		}
// проверяем блокируется ли другой сигнал		
		else {
			kill (p, SIGINT);
			usleep(100000);
			wait(&status);
// проверяем, что вызвало завершение сына: необработанный сигнал или что-то другое			
			if (WIFSIGNALED(status))
				printf("Другой сигнал НЕ блокируется на время работы обработчика\n");
			else
				printf("Другой сигнал блокируется на время работы обработчика\n");
		}
		close(fd[0][0]); close(fd[1][1]);
	}
	else {
		read(fd[1][0], &c, sizeof(char));
		if (i == 1) // сигнал обработался 1 раз
			printf("Тот же сигнал блокируется на время работы обработчика\n");
		else		// сигнал обработался 2 раза
			printf("Тот же сигнал НЕ блокируется на время работы обработчика\n");
		close(fd[0][1]); close(fd[1][0]);
	}
	return 0;
}

void handler (int s) {
	signal(SIGUSR1, handler);
	char c;
	i++;
	if (i == 1) {
		close(fd[0][0]); close(fd[1][1]);
		write(fd[0][1], &c, sizeof(char));
		read(fd[1][0], &c, sizeof(char));
	}
	return;
}