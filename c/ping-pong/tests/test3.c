// Яндиев А, 211 группа, ДЗ-10.2.3

// Проверка, получает ли сигнал, отправленный всей группе, сам процесс

// Вывод:
// В моей системе процесс-отправитель также получает сигнал, отправленный всей группе

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int i = 0;

void handler (int s) {
	signal(SIGUSR1, handler);
	i++;
	return;
}	

int main () {
	signal(SIGUSR1, handler);
// Посылаем сигнал всей группе текущего процесса
	kill(0, SIGUSR1);
	usleep(100000); 	// чтобы сигнал гарантированно успел дойти 
	if (i) // i = 1 => сигнал дошел до искомого процесса (и обработался)
		printf("Процесс, отправивший сигнал всей группе, получил его сам\n");
	else   // i = 0 => текущий процесс не получил сигнала SIGUSR1
		printf("Процесс, отправивший сигнал всей группе, НЕ получил его\n");
	return 0;
} 