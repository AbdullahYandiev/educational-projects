// Яндиев А, 211 группа, ДЗ-10.2.4

// Проверка, возобновляет ли работу функция wait(), если во время ожидания пришел сигнал

// Вывод:
// В моей системе функция wait() успешно возобновляет работу после прихода сигнала

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handler(int s) {};

int main () {
	pid_t p;
	signal(SIGUSR1, handler);
	if ((p = fork()) < 0) {
		perror("fork");
		return 0; 
	}
	if (p) {
// ждем завершение сыновьего процесса		
		if ((wait(NULL) == -1))
			printf("Функция wait() завершается с ошибкой\n");
		else
			printf("Функция wait() возобновляет работу после обработки сигнала\n");
	} 
	else {
// чтобы родитель успел вызвать wait() до посылки сигнала
		usleep(100000);
		kill(getppid(), SIGUSR1);
	}
	return 0;
}