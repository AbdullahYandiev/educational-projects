// Яндиев А, 211 группа, ДЗ-10.2.1

// Проверка, сбрасывается ли обработчик сигнала

// Вывод:
// В моей системе обработчик сигнала НЕ сбрасывается, если этого не сделать явно

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void handler (int s) {}

int main () {
	pid_t p; 
	int status, fd[2], i;
	char c;
	signal(SIGUSR1, handler);
	pipe(fd);
	if ((p = fork()) < 0) {
		perror("fork");
		return 0; 
	}
	if (p) {
		close(fd[1]);
// отправляем 2 сигнала сыну
// канал для последовательной работы процессов
		for (i=0; i<2; i++) {
			read(fd[0], &c, sizeof(char));
			kill(p, SIGUSR1);
		}
		wait(&status);
// проверяем при каком условие завершился процесс
		if (WIFSIGNALED(status))
			printf("Обработка сигнала сбрасывается на значение по умолчанию\n");
		close(fd[0]);
	}
	else {
		close(fd[0]);
		for (i=0; i<2; i++) {
			write(fd[1], &c, sizeof(char));
			pause();
		}	
// сын доработал нормально, поэтому обработка сигнала не сбрасывается
		printf("Обработка сигнала НЕ сбрасывается на значение по умолчанию\n");
		close(fd[1]);
	}
	return 0; 
}