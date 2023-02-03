// Задание:
// Вывод информации о файлах ls [-a -l -R] [файл]
// Если в качестве файла задано имя каталога, то выводится информация обо всех содержащихся в нем файлах, если задано имя файла — не каталога, то выводится информация только об этом файле. По умолчанию, когда файл не задан, выводится информация о файлах текущего каталога.
// -a – вывести имена всех файлов (т.е. в том числе и скрытых, имена которых начинаются с точки);
// -l - вывести подробную информацию о файлах. Имя файла, тип файла, права доступа, число ссылок на файл, размер файла в байтах - обязательно, имена владельца файла и владеющей группы, дата и время последнего изменения – по желанию;
// -R – рекурсивно обойти встретившиеся подкаталоги.

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/dir.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>

#define A 1		// 00000001
#define L 2		// 00000010
#define R 4		// 00000100
#define F 8		// 00001000
#define MAX_PATH 1024

char check_options (char **arr, int n);
void ls (char opt, char *name);
void print_dir (struct dirent *dp, char opt, char *name, int max_nlink, int max_size);
void print_file (int fd, char opt, char *name);
void print_mode (mode_t x);
void print_link (nlink_t  l, int max_l);
void print_user (uid_t u, gid_t g);
void print_time (time_t ctime);
void print_size (off_t s, int max_s);
int max_prm (DIR **dfd, int *link, char opt, char *name);

int main (int argc, char *argv[]) {
	char opt = check_options(argv, argc), *name;
	if (opt & F)   // явно указан путь к каталогу
		name = argv[argc-1];		
	else 
		name = ".";
	ls (opt, name);
	return 0;
}

char check_options (char **arr, int n) {
	char opt = 0;
	for (int i=1; i<n; i++) {
		if(arr[i][0]=='-' && strlen(arr[i])==2)
			if (arr[i][1]=='a')
				opt |= A;
			else if (arr[i][1]=='l')
				opt |= L;
			else if (arr[i][1]=='R')
				opt |= R;
			else {
				opt |= F;
				return opt;
			}
		else {
			opt |= F;
			return opt;
		}
	}
	return opt;
}

void ls (char opt, char *name) {
	DIR *dfd;
	if (!(dfd = opendir(name))) {
		int fd;
		if ((open(name, O_RDONLY, 0660)) == -1) {
			printf("%s: No such file or directory\n", name);
			return;	
		}
		else {
			print_file (fd, opt, name);
			close (fd);
			return;
		}
	}
	else {
		struct dirent *dp;
		int max_nlink, max_size = max_prm(&dfd, &max_nlink, opt, name);
		while ((dp = readdir(dfd))!=0) 
			print_dir (dp, opt, name, max_nlink, max_size);
		closedir(dfd);
		if (opt & R) {		
			if (!(dfd = opendir(name))) {
				printf("%s: No such file or directory\n", name);
				return;	
			}	
			struct stat stbuf;
			char dname[MAX_PATH];
			while ((dp = readdir(dfd))!=0) {
				if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
					continue; 
				if (strlen(name)+strlen(dp->d_name)+2 > sizeof(dname)) {
					printf("print_dir_r: too long name of path %s/%s\n", name, dp->d_name);
					return;
				}	
				sprintf(dname, "%s/%s", name, dp->d_name);
				if (stat(dname, &stbuf) == -1)
					perror("stat");
				else  {
					if ((stbuf.st_mode & S_IFMT) == S_IFDIR) 
						if (((opt & A) && dp->d_name[0] == '.') || dp->d_name[0] != '.') {
							printf("\n\n%s\n", dname);
							ls (opt, dname);
						}
				}
			}			
			closedir(dfd);
		}
	}
	return;
}

void print_dir (struct dirent *dp, char opt, char *name, int max_nlink, int max_size) {
	struct stat stbuf;
	char dname[MAX_PATH];
		if (!(opt & L)) {
			if (((opt & A) && dp->d_name[0] == '.') || dp->d_name[0] != '.') 
				printf("%s\n", dp->d_name);
		}
		else {
			if (strlen(name)+strlen(dp->d_name)+2 > sizeof(dname)) {
				printf("print_dir: too long name of path %s/%s\n", name, dp->d_name);
				return;
			}	
			sprintf(dname, "%s/%s", name, dp->d_name);
			if (stat(dname, &stbuf) == -1)
				perror("stat");
			else if (((opt & A) && dp->d_name[0] == '.') || dp->d_name[0] != '.') {
				print_mode(stbuf.st_mode);
				print_link(stbuf.st_nlink, max_nlink);
				print_user (stbuf.st_uid, stbuf.st_gid);
				print_size(stbuf.st_size, max_size);
				print_time(stbuf.st_ctime);
				printf("%s\n", dp->d_name);
			}
		}
	return; 
}

void print_file (int fd, char opt, char *name) {
	struct stat stbuf;	
	if (!(opt & L))
		printf("%s\n", name);
	else {
		if (stat(name, &stbuf) == -1)
				perror("stat");
		else {
			print_mode(stbuf.st_mode);
			printf("%d ", stbuf.st_nlink);
			print_user (stbuf.st_uid, stbuf.st_gid);
			printf("%lld ", stbuf.st_size);
			print_time(stbuf.st_ctime);
			printf("%s\n", name);
		}
	}
	return;
}

void print_mode (mode_t x) {
	int i;
// печать типа	
	if (S_ISLNK(x))
		putchar('l');
	else if (S_ISREG(x))
		putchar('-');
	else if (S_ISDIR(x))
		putchar('d');
	else if (S_ISCHR(x))
		putchar('c');
	else if (S_ISBLK(x))
		putchar('b');
	else if (S_ISFIFO(x))
		putchar('p');
	else if (S_ISSOCK(x))
		putchar('s');
// печать режимов доступа	
	for (i=0; i<3; i++) {
		if (x & 0400)
			putchar('r');
		else
			putchar('-');
		if (x & 0200)
			putchar('w');
		else
			putchar('-');
		if (x & 0100)
			putchar('x');
		else
			putchar('-');
		x <<= 3;
	}
	putchar (' ');
	return;
}

void print_link (nlink_t l, int max_l) {
	int x = l;
	int i = 0;
	while (x) {
		i++;
		x /= 10;
	}
	if (!l)
		i++;
	for (i = max_l - i; i; i--)
		putchar (' ');
	printf("%d ",  l);
	return;
} 

void print_user (uid_t u, gid_t g) {
	struct passwd *pbuf;
	struct group *gbuf;
	if ((pbuf = getpwuid(u)) == NULL)
		perror("getpwuid");
	else
		printf("%s  ", pbuf->pw_name);
	if ((gbuf = getgrgid(g)) == NULL)
		perror("getgrgid");
	else
		printf("%s  ", gbuf->gr_name);
	return;
}

void print_time (time_t ctime) {
	char buf[13]; // в дате ровно 12 символов
	struct tm *tbuf, *nbuf;
	time_t t = time(NULL); 	// чтобы узнать текущий год
	nbuf = localtime (&t);
	tbuf = localtime (&ctime);
// если текущий год, то печатаем в формате 13 Nov 15:05	
	if (tbuf->tm_year == nbuf->tm_year)
		strftime (buf, 13, "%d %b %H:%M", tbuf);
// если не текущий год, то печатаем в формате 13 Nov  2007	
	else
		strftime (buf, 13, "%d %b  %Y", tbuf);
	printf ("%s ", buf);
	return;
}

void print_size (off_t s, int max_s) {
	long long int x = s;
	int i = 0;
	while (x) {
		i++;
		x /= 10;
	}
	if (!s)
		i++;
	for (i = max_s - i; i; i--)
		putchar (' ');
	printf("%lld ",  s);
	return;
}

int max_prm (DIR **dfd, int *link, char opt, char *name) { 	
// возвращает порядок макс. размера файла в каталоге
// побочный эффект: возвр. порядок макс. числа ссылок	
	int max_l=0, k=0;
	long long int max_s=0;
	struct dirent *dp; 
	struct stat stbuf;
	char dname[MAX_PATH];
	*link = 0;
	while ((dp = readdir(*dfd))!=0) {
		if (strlen(name)+strlen(dp->d_name)+2 > sizeof(dname))
			break;
		sprintf(dname, "%s/%s", name, dp->d_name);
		if (stat(dname, &stbuf) != -1) {
			if (dp->d_name[0] == '.') {
				if (opt & A) {
					if (stbuf.st_size > max_s)
						max_s = stbuf.st_size;
					if (stbuf.st_nlink > max_l)
						max_l = stbuf.st_nlink;
				}
			}	
			else {
				if (stbuf.st_size > max_s)
					max_s = stbuf.st_size;
				if (stbuf.st_nlink > max_l)
					max_l = stbuf.st_nlink;
			}
		}
	}		
// определяем порядок макс. величин			
	while (max_s) {
		k++;
		max_s /= 10;
	}		
	while (max_l) {
		(*link)++;
		max_l /= 10;
	}
	closedir(*dfd);
	*dfd = opendir(name); 
	return k;
}