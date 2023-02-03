#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAXLEN 256
#define WAITING do {					\
	semops.sem_op = 2;					\
	semop(semid, &semops, 1);			\
	semops.sem_op = 0;					\
	semop(semid, &semops, 1);			\
} while (0)								\

int nstrcmp (const char *str1, const char *str2);
int num_dig (int num);

int main () {
	key_t key;
	int shmid, semid, i, n, res, ok = 1, head[2] = {1, 1};
	struct sembuf semops;
	char word[MAXLEN], *shmaddr, *tmp, c;

	key = ftok("file.txt", 'S');
// создаем 1 семафор для синхранизации
	if ((semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL)) == -1) {
		semid = semget(key, 1, 0666);
		head[0]--;
	}
// инициализируем семафор нулем, если создали его
	else
		semctl(semid, 0, SETVAL, (int) 0);
// создаем разделяемую память на 256 элементов
	if ((shmid = shmget(key, MAXLEN, 0666 | IPC_CREAT | IPC_EXCL)) == -1) {
		shmid = shmget(key, MAXLEN, 0666);
		head[1]--;
	}
	semops.sem_num = 0;
	semops.sem_flg = 0;
// подключаемся к разделяемой памяти
	shmaddr = shmat(shmid, NULL, 0);
// начинаем игру
	printf("ДЛЯ НАЧАЛА ИГРЫ НАЖМИТЕ ENTER"); fflush(stdout);
	while ((c = getchar()) != '\n');
	printf("-------------------------------------\n");
	printf("НАЧАЛО ИГРЫ\n");
	while (ok) {
		printf("-------------------------------------\n");
		WAITING;
// n - количество букв в слове
// i - количество попыток
		sscanf(shmaddr, "%d %d", &n, &i);
		if (n == -1) {
			printf("К СОЖАЛЕНИЮ, ИГРА ОКОНЧЕНА ПО ТЕХНИЧЕСКИМ ПРИЧИНАМ\n");
			break;	  // выходим из цикла
		}
		printf("В СЛОВЕ %d БУКВ\n", n);
		printf("-------------------------------------\n");
		for (; i > 0; i--)	{
			printf("ОСТАЛОСЬ ПОПЫТОК: %d\nВВЕДИТЕ БУКВУ ИЛИ ВСЕ СЛОВО ЦЕЛИКОМ: ", i);
// записываем слово в разделяемую память
			scanf("%s", shmaddr);
			printf("-------------------------------------\n");
// ждем обработки
			WAITING;
// обрабатываем результат
			sscanf(shmaddr, "%d", &res);
			if (res == -3) {
				printf("МЫ ЗАГАДАЛИ СЛОВО - %s\n", shmaddr + num_dig(res) + 1);
				i = 1;
			}
			else if (res == -2) {  // слово угадано
				printf("СЛОВО УГАДАНО\nПОБЕДА\n");
				i = 1;
			}
			else if (res == -1) {  // слово не угадано
				printf("ЖАЛЬ, НО ВЫ НЕ УГАДАЛИ СЛОВО\nПОРАЖЕНИЕ\n");
				printf("МЫ ЗАГАДАЛИ СЛОВО - %s\n", shmaddr + num_dig(res) + 1);
				i = 1;
			}
			else {
				if (i == 1) {
					printf("УВЫ, НО НЕ ОСТАЛОСЬ НИ ОДНОЙ ПОПЫТКИ\nПОРАЖЕНИЕ\n");
					printf("МЫ ЗАГАДАЛИ СЛОВО - %s\n", shmaddr + num_dig(res) + 1);
				}
				else if (!res)
					printf("ВЫ НЕ УГАДАЛИ БУКВУ\n");
				else
					printf("ВАША БУКВА НАХОДИТСЯ НА ПОЗИЦИЯХ - %s\n", shmaddr + num_dig(res) + 1);
			}
			printf("-------------------------------------\n");
		}
		printf("КОНЕЦ ИГРЫ\n");
		printf("ХОТИТЕ СЫГРАТЬ ЕЩЕ РАЗ? (YES/NO) - ");
		scanf("%s", word);
		if (nstrcmp(word, "YES"))
			ok--;
// отправляем серверу 0, если хотим закончить
		sprintf(shmaddr, "%d", ok);
	}
	semops.sem_op = 2;
	semop(semid, &semops, 1);
// отключаемся от разделяемой памяти
	shmdt(shmaddr);
// уничтожаем семафор
	if (head[0])
		semctl(semid, 0, IPC_RMID, (int) 0);
// уничтожаем разделяемую память
	if (head[1])
		shmctl(shmid, IPC_RMID, NULL);
	return 0;
}

int nstrcmp (const char *str1, const char *str2) {
	int i = 0;
	while (str1[i] != '\0' && str2[i] != '\0' && toupper(str1[i]) == toupper(str2[i]))
		i++;	
	if (toupper(str1[i]) > toupper(str2[i]))
		return 1;
	else if (toupper(str1[i]) < toupper(str2[i]))
		return -1;
	return 0;
}

int num_dig (int num) {
	int k = 0;
	if (num <= 0)
		k++;
	while (num) {
		k++;
		num /= 10;
	}
	return k;
}