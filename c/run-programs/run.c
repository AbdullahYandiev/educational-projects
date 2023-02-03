// Задание:
// В командной строке передаются имена двух исполняемых файлов, для каждого передается по одному параметру. Также должен быть передан параметр, определяющий режим запуска — параллельно (1) или последовательно (2).
// Командная строка выглядит примерно так:
// >./a.out prog1 arg11 prog2 arg21 1
// В зависимости от значения последнего параметра запустить эти программы
// с использованием векторной передачи данных для параллельного исполнения ( т. е. работают одновременно) .
// или запустить эти программы с использованием передачи данных в виде списка для последовательного исполнения ( работают строго одна после другой) .
// Определить, завершились ли они успешно (с точки зрения программы — узнать, что вернул exit, и с точки зрения системы).
// По окончании работы не должно остаться процессов «зомби».
// Для тестирования напишите программу (достаточно одной), которая должна работать не слишком быстро и осуществлять вывод на экран (пусть многократно выводит pid процесса), и запускайте ее.

// Замечания по программе:
// добавил проверку на то, что возвращает exit в различных случаях
// 1 - если не запустился exec
// 0 - если тестовая программа завершилась успешно

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char *argv[]) {
	if (argc!=6) {
		printf("Input is incorrect\n");
		return 0;
	}
	int st;
	char *arv1[3], *arv2[3], *mode = argv[argc-1];
	pid_t p1, p2;
// формируем вектор для execV
	if (!strcmp(mode, "1")) {
		arv1[0] = argv[1];
		arv1[1] = argv[2];
		arv1[2] = NULL;
		arv2[0] = argv[3];
		arv2[1] = argv[4];
		arv2[2] = NULL;
	}
	else if (strcmp(mode, "2")) {
		printf("Launch mode is incorrect\n");
		return 0;
	}
// порождаем 1-ый процесс и проверяем, получилось ли
	if ((p1 = fork()) < 0) {
		perror ("fork");
		exit(0);
	}
// запускаем первую программу
	if (!p1) { // сын
		if (!strcmp(mode, "1"))
			execvp(argv[1], arv1);
		else 
			execlp(argv[1], argv[1], argv[2], NULL);
		exit(1);    // сюда не должно дойти -> ошибка (exit вернет 1)
	}
// порождаем 2-ой процесс и проверяем, получилось ли
// если mode = 2, то ждем завершения 1-ой программы и проверяем успешность завершения
// иначе запускаем 2-ую программу параллельно 1-ой
	// родитель
	if (!strcmp(mode, "2")) {
		wait(&st);
		if (!WIFEXITED(st)) {
			printf("Program works incorrectly\n");
			exit(0);
		}
		else {
			if (WEXITSTATUS(st) == 0)
				printf("\nProgram works correctly\n");
			else if (WEXITSTATUS(st) == 1)
				printf("\nExec works incorrectly\nCheck name of your program\n");
			else
				printf("\nProgram works incorrectly\n");
		}
	}
	if ((p2 = fork()) < 0) {
		perror ("fork");
		exit(0);
	}
	if (!p2) { // сын
		if (!strcmp(mode, "1"))
			execvp(argv[3], arv2);
		else
			execlp(argv[3], argv[3], argv[4], NULL); 
		exit(1); // сюда не должно дойти -> ошибка (exit вернет 1)
	} 	// родитель
// далее, проверяем успешность выполняния программ
// при последовательном выполнении - успешность выполнения одной оставшейся
// при параллельном выполнении - успешность выполнения обеих
	if (!strcmp(mode, "1")) {
		wait(&st);
		if (!WIFEXITED(st)) {
			printf("\nProgram works incorrectly\n");
			exit(0);
		}
		else {
			if (WEXITSTATUS(st) == 0)
				printf("\nProgram works correctly\n");
			else if (WEXITSTATUS(st) == 1)
				printf("\nExec works incorrectly\nCheck name of your program\n");
			else
				printf("\nProgram works incorrectly\n");
		}
	}
	wait(&st);
	if (!WIFEXITED(st))
		printf("\nProgram works incorrectly\n");
	else {
		if (WEXITSTATUS(st) == 0)
			printf("\nProgram works correctly\n");
		else if (WEXITSTATUS(st) == 1)
			printf("\nExec works incorrectly\nCheck name of your program\n");
		else
			printf("\nProgram works incorrectly\n");
		}
	exit(0);
}