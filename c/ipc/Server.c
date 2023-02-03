#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAXLEN 256
#define NUMATMP 5
#define WAITING	do {				\
	semop(semid, &semops, 1);		\
	semop(semid, &semops, 1);		\
} while (0);						\


int nstrcmp (const char *str1, const char *str2);
void let_analysis (char *word, char *shmaddr);

int main (int argc, char *argv[]) {
	FILE *F = NULL;
	if (argc == 2)
		F = fopen(argv[1], "r");
	key_t key;
	int shmid, semid, i, head[2] = {1, 1};
	struct sembuf semops;
	char word[MAXLEN], *shmaddr;

	key = ftok("file.txt", 'S');
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
// ожидаем готовность клиента
	semops.sem_op = -1;
	semop(semid, &semops, 1);
	if (!F)
		sprintf(shmaddr, "%d %d", -1, 0);
	else {
		fscanf(F, "%s", word);
		while (!feof(F)) {
// сообщаем клиенту количество букв в слове и количество возможных попыток для угадывания
			sprintf(shmaddr, "%d %d", (int) strlen(word), NUMATMP);
// позволяем клиенту продолжить работу
// ожидаем попытку угадать букву или слово
			WAITING;
// анализируем пришедшее сообщение от клиента
			for (i = 0; i < NUMATMP; i++) {
				if (!nstrcmp(shmaddr, "END")) {
					sprintf(shmaddr, "%d ", -3);
					i = NUMATMP - 1;
				}
				else if (!nstrcmp(shmaddr, word)) {	  // игрок угадал слово
					sprintf(shmaddr, "%d ", -2);
					i = NUMATMP;
				}
				else if (strlen(shmaddr) == 1) {	// игрок угадывает букву
					if (i != NUMATMP - 1) {			// если еще есть попытки
						let_analysis(word, shmaddr);
						WAITING;
					}
					else
						sprintf(shmaddr, "%d ", 0);
				}	
				else {	// игрок не угадал слово 
					sprintf(shmaddr, "%d ", -1);
					i = NUMATMP - 1;
				}
				if (i == NUMATMP - 1)
					strcat(shmaddr, word);
			}
			WAITING;
// смотрим, что прислал клиент по поводу конца игры
// если 0, то завершаем игру
			sscanf(shmaddr, "%d", &i);
			if (!i) {
				fseek(F, 0, SEEK_END);
				getc(F);	// читаем EOF
			}
			else {
				fscanf(F, "%s", word);
				if (feof(F)) {
					sprintf(shmaddr, "%d %d", -1, 0);
					semop(semid, &semops, 1);
				}
			}
		}
	}
// отключаемся от разделяемой памяти
	shmdt(shmaddr);
// уничтащаем семафор
	if (head[0])
		semctl(semid, 0, IPC_RMID, (int) 0);
// уничтажаем разделяемую память
	if (head[1])
		shmctl(shmid, IPC_RMID, NULL);
	fclose(F);
	return 0;
}

// аналог strcmp, но работает без учета региста
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

void let_analysis (char *word, char *shmaddr) {
	int k = 0;
	char c = *shmaddr, d = toupper(c), tmp[sizeof(int) + 2];
	for (int i = 0; word[i] != '\0'; i++)
		if (word[i] == c || word[i] == d)
			k++;
	sprintf(shmaddr, "%d ", k);
	for (int i = 0; word[i] != '\0'; i++)
		if (word[i] == c || word[i] == d) {
			sprintf(tmp, "%d ", i+1);
			strcat(shmaddr, tmp);
		}
}