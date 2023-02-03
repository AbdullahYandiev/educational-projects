// Задание:
// Сортировка файлов sort [ -r -b -f -o] файлы
// Сортировать строки каждого файла в лексикографическом порядке.
// По умолчанию результат сортировки выводится на стандартный вывод, исходный файл при этом не меняется.
// sort -r файлы - сортировка в обратном порядке.
// sort -b файлы – сортировать файл, игнорируя начальные пробелы
// sort -f файлы – при сравнении преобразовывать малые буквы в большие (т.е. сравнение без учета регистра).
// sort -o выходной_файл файлы - результат направляется не на стандартный вывод (как происходит по умолчанию), а в выходной_файл, который может (но не обязан) совпадать с одним из исходных.

// Замечания по программе:
// 1) файлы сортируются по отдельности
// 2) каждый отсортированный файл выводится с соответствующим сообщением
// 3) если файл пуст или он не открылся, то об этом тоже сообщается
// 4) если задана опция -o и лишь 1 входной файл, то он сортируется, то есть вызовы
// ./sort -o [-r -b -f] text.txt и ./sort -o [-r -b -f] text.txt text.txt работают одинаково
// (оригинальная команда sort в случае вызова sort -o [-r -b -f] text.txt не работает)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R 3		// 00000011
#define B 12	// 00001100
#define F 48	// 00110000
#define O 192 	// 11000000

#define K 4     // for realloc

#define to_up(c) (((c)>='a' && (c)<='z') ? (c)+'A'-'a' : (c))

char check_options (char **arr, int n, int *i);
char **create_arr (FILE *T, int *n_str, int *n_emp);
int n_strcmp(const void *str1, const void *str2);
int r_strcmp(const void *str1, const void *str2);
int b_strcmp(const void *str1, const void *str2);
int f_strcmp(const void *str1, const void *str2);
int r_b_strcmp(const void *str1, const void *str2);
int r_f_strcmp(const void *str1, const void *str2);
int b_f_strcmp(const void *str1, const void *str2);
int r_b_f_strcmp(const void *str1, const void *str2);
void print_file (FILE *T, char opt, char **a, int s, int e);

int main (int argc, char *argv[]) {
	int i;
	char opt=check_options(argv, argc, &i);
	if (i == argc)
		printf("There is no input file included\n");
	else {
		int j, n_emp, n_str, n_emp_o, n_str_o, n_out = i-1;
		char **arr_str=NULL, **arr_str_o=NULL, c, flag=0, opened=1;
		FILE *T, *OUTP = stdout;

// отдельно разбираем случай, когда подключена опция -о		
		if (opt & O) {
			n_out++;
			for (i=n_out+1; i<argc && strcmp(argv[i], argv[n_out]); i++);
			if (i<argc || n_out == argc-1) {
				T = fopen(argv[n_out], "r");
				if (T) 
					arr_str_o = create_arr(T, &n_str_o, &n_emp_o);
				else
					opened--;	// не открылся
				fclose(T);
				if (n_out == argc-1) { 
	// если задан всего один файл (он же выходной), сортируем и выводим его	
					T = fopen(argv[n_out], "w");
					fprintf(T, "\t/// Sorted file: %s ///\n", argv[n_out]);
					if (opened) {
						if (arr_str_o) {
							print_file(T, opt, arr_str_o, n_str_o, n_emp_o);
							for (j=0; j<n_str_o; j++)
								free(arr_str_o[j]);
							free(arr_str_o);
							arr_str_o = NULL;
						}
						else 
							fprintf(T, " - File is empty");
					}
					else
						fprintf(T, " - File did not opened\n");
					fclose(T);
				}
				else {				
					OUTP = fopen(argv[n_out], "w");
					flag++;
				}	
			}
			else // выходной файл не совпадает с входными
				OUTP = fopen(argv[n_out], "w");
		}

// сортируем и выводим отсортированные файлы
		for (i=n_out+1; i<argc; i++) {
			fprintf(OUTP, "\t/// Sorted file: %s ///\n", argv[i]);
			if (strcmp(argv[i], argv[n_out])) {
				T = fopen(argv[i], "r");
				if (T) {
					arr_str = create_arr(T, &n_str, &n_emp);
					if (arr_str) {
						print_file(OUTP, opt, arr_str, n_str, n_emp);
						for (j=0; j<n_str; j++)
							free(arr_str[j]);
						free(arr_str);
					}
					else
						fprintf(OUTP, " - File is empty\n");
				}
				else 
					fprintf(OUTP, " - File did not opened\n");
				fclose(T);
			}	
			else
				if (opened)
					if (arr_str_o)
						print_file(OUTP, opt, arr_str_o, n_str_o, n_emp_o);
					else
						fprintf(OUTP, " - File is empty\n");
				else {
					fprintf(OUTP, " - File did not opened\n");
				}
		}
		if (flag)
			fclose(OUTP);
		if (arr_str_o) {
			for (j=0; j<n_str_o; j++)
				free(arr_str_o[j]);
			free(arr_str_o);
		}
	}
	return 0;
}

char check_options (char **arr, int n, int *i) {
	char opt=0, flag=1;
	for ((*i)=0; flag && (*i)<n-1; (*i)++) {
		if(arr[(*i)+1][0]=='-' && strlen(arr[(*i)+1])==2)
			if (arr[(*i)+1][1]=='r')
				opt |= R;
			else if (arr[(*i)+1][1]=='b')
				opt |= B;
			else if (arr[(*i)+1][1]=='f')
				opt |= F;
			else if (arr[(*i)+1][1]=='o')
				opt |= O;
			else
				flag--;
		else
			flag--;
	}	
	if (flag)
		(*i)++;	
	return opt;
}

char **create_arr (FILE *T, int *n_str, int *n_emp) {
	char **a=NULL, c=getc(T);
	if (!feof(T)) {
		fseek(T, 0, SEEK_SET);
		int i, n;
		*n_emp = 0; *n_str = 0;
		while (!feof(T)) {
			while ((c = getc(T))=='\n')
				(*n_emp)++;
			if (!feof(T)) {
				a = (char **)realloc(a, ((*n_str)+1)*sizeof(char *));
				n = 0; a[*n_str] = NULL;
				while (c!='\n' && !feof(T)) {
					a[*n_str] = (char *)realloc(a[*n_str], (n+K)*sizeof(char));
					for (i=0; i<K && c!='\n' && !feof(T); c=getc(T), i++)
						a[*n_str][n++] = c;
				}
				if (i==K)
					a[*n_str] = (char *)realloc(a[*n_str], (n+1)*sizeof(char));
				a[*n_str][n] = '\0';
				(*n_str)++;
			}
			else
				(*n_emp)++;
		}
	}
	return a;
}

int n_strcmp(const void *str1, const void *str2) {
	const char *arr1, *arr2;
	arr1 = * (char **) str1;
	arr2 = * (char **) str2;
	return strcmp(arr1, arr2);
}

int r_strcmp(const void *str1, const void *str2) {
	const char *arr1, *arr2;
	arr1 = * (char **) str1;
	arr2 = * (char **) str2;
	return -strcmp(arr1, arr2);
}

int b_strcmp(const void *str1, const void *str2) {
	const char *arr1, *arr2;
	arr1 = * (char **) str1;
    arr2 = * (char **) str2;
	while ((*arr1)==' ')
		arr1++;
	while ((*arr2)==' ')
		arr2++;
	return strcmp(arr1, arr2);
}

int f_strcmp(const void *str1, const void *str2) {
	int i=0;
	const char *arr1, *arr2;
	arr1 = * (char **) str1;
	arr2 = * (char **) str2;
	char c1=arr1[0], c2=arr2[0];
	while (c1!='\0' && c2!='\0' && (to_up(c1) == to_up(c2))) {
		c1 = arr1[++i];
		c2 = arr2[i];
	}
	if (to_up(c1) > to_up(c2))
		return 1;
	else if (to_up(c1) < to_up(c2))
		return -1;
	return 0;
}

int r_b_strcmp(const void *str1, const void *str2) {
	const char *arr1, *arr2;
	arr1 = * (char **) str1;
	arr2 = * (char **) str2;
	while ((*arr1)==' ')
		arr1++;
	while ((*arr2)==' ')
		arr2++;
	return -strcmp(arr1, arr2);
}

int r_f_strcmp(const void *str1, const void *str2) {
	int i=0;
	const char *arr1, *arr2; 
	arr1 = * (char **) str1;
	arr2 = * (char **) str2;
	char c1=arr1[0], c2=arr2[0];
	while (c1!='\0' && c2!='\0' && (to_up(c1) == to_up(c2))) {
		c1 = arr1[++i];
		c2 = arr2[i];
	}
	if (to_up(c1) > to_up(c2)) 
		return -1;
	else if (to_up(c1) < to_up(c2))
		return 1;
	return 0;
}

int b_f_strcmp(const void *str1, const void *str2) {
	int i=0;
	const char *arr1, *arr2; 
	arr1 = * (char **) str1;
	arr2 = * (char **) str2;
	while ((*arr1)==' ')
		arr1++;
	while ((*arr2)==' ')
		arr2++;
	char c1=arr1[0], c2=arr2[0];
	while (c1!='\0' && c2!='\0' && (to_up(c1) == to_up(c2))) {
		c1 = arr1[++i];
		c2 = arr2[i];
	}
	if (to_up(c1) > to_up(c2)) 
		return 1;
	else if (to_up(c1) < to_up(c2))
		return -1;
	return 0;
}

int r_b_f_strcmp(const void *str1, const void *str2) {
	int i=0;
	const char *arr1, *arr2; 
	arr1 = * (char **) str1;
	arr2 = * (char **) str2;
	while ((*arr1)==' ')
		arr1++;
	while ((*arr2)==' ')
		arr2++;
	char c1=arr1[0], c2=arr2[0];
	while (c1!='\0' && c2!='\0' && (to_up(c1) == to_up(c2))) {
		c1 = arr1[++i];
		c2 = arr2[i];
	}
	if (to_up(c1) > to_up(c2)) 
		return -1;
	else if (to_up(c1) < to_up(c2))
		return 1;
	return 0;
}

void print_file (FILE *T, char opt, char **a, int s, int e) {
	int i;
	if (opt & R) {
		if (opt & B)
			if (opt & F)
				qsort(a, s, sizeof(char *), r_b_f_strcmp);
			else
				qsort(a, s, sizeof(char *), r_b_strcmp);
		else
			if (opt & F)
				qsort(a, s, sizeof(char *), r_f_strcmp);
			else
				qsort(a, s, sizeof(char *), r_strcmp);
		for (i=0; i<s; i++)
			fprintf(T, "%s\n", a[i]);
		for (i=0; i<e; i++)
			putc('\n', T);
	}	
	else {
		if (opt & B)
			if (opt & F)
				qsort(a, s, sizeof(char *), b_f_strcmp);
			else
				qsort(a, s, sizeof(char *), b_strcmp);
		else
			if (opt & F)
				qsort(a, s, sizeof(char *), f_strcmp);
			else
				qsort(a, s, sizeof(char *), n_strcmp);
		for (i=0; i<e; i++)
			putc('\n', T);
		for (i=0; i<s; i++)
			fprintf(T, "%s\n", a[i]);
	}
}