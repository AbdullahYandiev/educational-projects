#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define K 4 // для realloc

enum tokens {EOL, OR, OR2, AND, AND2, GR, GR2, L, T3, LP, RP, NAME};
enum type_of_next {NXT, NOR, NAND};

typedef struct token_lexeme {
	enum tokens token;
	int len;
} token_lexeme;

typedef struct info_tree {
	char **argv;				// список из имени команды и аргументов
	char *in_file;				// файл для перенаправления стандартного ввода
	char *out_file;				// файл для перенаправления стандартного вывода
	int mode_a;					// =1, если ">>" (в конец файла)
	struct info_tree *extra;	// команда в скобках
	struct info_tree *cnvr;		// следующая команда после "|"
	struct info_tree *next;		// следующая после ";" или после "&"
	int backgrnd;				// =1, если фоновый режим
	int flag;					// для фонового режима в конвейере
	enum type_of_next type;		// связь со следующей командой через ; или && или ||
} info_tree;

char * make_input_str (void);
int nstrcmp (const char *str1, const char *str2);
// -----------------------------------------------
token_lexeme get_next_token (char *str, int *index);
void put_back_token (int *index, token_lexeme token);
int move_through (char *str, int index, char c);
int is_special (char sym);
// -----------------------------------------------
info_tree * make_tree (char *str);
info_tree * start_tree (void);
void print_tree (info_tree *tree);
void del_tree (info_tree *tree);
// -----------------------------------------------
// следующие функции возвращают -1 в случае ошибки
int cmdsh (char *str, int *index, token_lexeme token, info_tree **tree);
int cmdif (char *str, int *index, token_lexeme token, info_tree **tree);
int cmd (char *str, int *index, token_lexeme token, info_tree **tree);
int inout (char *str, int *index, token_lexeme token, info_tree **tree);
int rein (char *str, int *index, token_lexeme token, info_tree **tree);
int reout (char *str, int *index, token_lexeme token, info_tree **tree);
int conv (char *str, int *index, token_lexeme token, info_tree **tree);
// -----------------------------------------------
char ** get_name (char *str, int i, int name_len);
char * get_file_name (char *str, int i, int name_len);
// -----------------------------------------------
int hand_tree (info_tree *tree);
int hand_conv (info_tree *tree);
void redirection (info_tree *tree);

int main () {
	char *input_str;
	info_tree *tree;
	printf("...$ "); fflush(stdout);
	while (nstrcmp(input_str = make_input_str(), "exit")) {
		if (input_str[0] != '\0') {
			tree = make_tree(input_str);
			if (!tree)
				printf("...: syntax error\n"); 
			else {
				hand_tree(tree);
				del_tree(tree);
			}	
		}
		free(input_str);
		printf("...$ "); fflush(stdout);
	}	
	free(input_str);
	return 0;
}

// возвращает си-строку
char * make_input_str (void) {
	int len = 0, i;
	char c, *str = NULL;
	read(0, &c, sizeof(char));
	if (c != '\n') {
		while (c != '\n') {
			str = realloc(str, (len + K) * sizeof(char));
			for (i=0; c!='\n' && i<K; i++) {
				str[len++] = c;
				read(0, &c, sizeof(char));
			}
		}
	}	
	if (i == K || !len)
		str = realloc(str, (len + 1) * sizeof(char));
	str[len] = '\0';	
	return str;	
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

token_lexeme get_next_token (char *str, int *index) {
	token_lexeme token;
// смотрим, каков следующий токен
	switch (str[*index]) {
		case '\0':
			token.token = EOL;
			token.len = 0;
			break;
		case '|':
			if (str[(*index) + 1] != '|') {
				token.token = OR;
				token.len = 1;
			}
			else {
				token.token = OR2;
				token.len = 2;
			}
			break;
		case '&':
			if (str[(*index) + 1] != '&') {
				token.token = AND;
				token.len = 1;
			}
			else {
				token.token = AND2;
				token.len = 2;
			}
			break;
		case '>':
			if (str[(*index) + 1] != '>') {
				token.token = GR;
				token.len = move_through(str, *index+1, 'F') + 1;
			}
			else {
				token.token = GR2;
				token.len = move_through(str, *index+2, 'F') + 2;
			}
			break;
		case '<':
			token.token = L;
			token.len = move_through(str, *index+1, 'F') + 1;
			break;
		case ';':
			token.token = T3;
			token.len = 1;
			break;
		case '(':
			token.token = LP;
			token.len = 1;
			break;
		case ')':
			token.token = RP;
			token.len = 1;
			break;
		case ' ':
			(*index)++;
			token = get_next_token(str, index);
			(*index) -= token.len;
			break;
		default:
			token.token = NAME;
			token.len = move_through(str, *index, 0);
			break;
	}
	(*index) += token.len;
	return token;
}

void put_back_token (int *index, token_lexeme token) {
	*index -= token.len;
}

int move_through (char *str, int index, char c) {
	int len = 0;
	while (str[index] == ' ') {
		len++;
		index++;
	}
	while (!(is_special(str[index]))) {
		if (c == 'F' && str[index] == ' ')
			return len;
		len++;
		index++;
	}
	return len;
}

int is_special (char sym) {
	switch (sym) {
		case '\0':
		case '|':
		case '&':
		case '>':
		case '<':
		case ';':
		case '(':
		case ')':
			return 1;
		default:
			return 0;
	}
}

info_tree * make_tree (char *str) {
	int i = 0;
	info_tree *tree;
	token_lexeme token = get_next_token(str, &i);
	if (token.token != EOL)
		if (cmdsh(str, &i, token, &tree) == -1) {
			del_tree(tree);
			return NULL;
		}
	return tree;
}

info_tree * start_tree (void) {
	info_tree *tree = NULL;
	tree = (info_tree *)malloc(sizeof(info_tree));
	tree->argv = NULL;
//	tree->argv = (char **)malloc(sizeof(char *));
//	tree->argv[0] = NULL;
	tree->in_file = NULL;
	tree->out_file = NULL;
	tree->mode_a = 0;
	tree->backgrnd = 0;
	tree->flag = 1;
	tree->extra = NULL;
	tree->cnvr = NULL;
	tree->next = NULL;
	tree->type = NXT;
	return tree;
}

void print_tree (info_tree *tree) {
	int i;
	if (!tree)
		return;
	if (tree->argv) {
		printf("Команда: %s\n", tree->argv[0]);
		if (tree->argv[1]) {
			printf("Опции: ");
			for (i=1; tree->argv[i]; i++)
				printf("%s ", tree->argv[i]);
			putchar('\n');
		}
		if (tree->in_file)
			printf("Input file: %s\n", tree->in_file);
		if (tree->out_file) {
			printf("Output file: %s\n", tree->out_file);
			if (tree->mode_a)
				printf("Вывод в конец файла\n");
			else
				printf("Вывод в файл с перезаписью\n");
		}
		if (tree->backgrnd)
			printf("В фоновом режиме\n");
		if (tree->cnvr)
			if (tree->cnvr->argv[0])
				printf("Следующая команда: %s\n", tree->cnvr->argv[0]);
		if (tree->next)
			if (tree->next->argv[0])
				printf("Следующая команда: %s\n", tree->next->argv[0]);
		putchar('\n');
	}
	else {
		printf("Следующая команда %s (в отдельном экземпляре Шелла)\n", tree->extra->argv[0]);
		if (tree->backgrnd)
			printf("В фоновом режиме\n");
		print_tree(tree->extra);
	}
	print_tree(tree->cnvr);
	print_tree(tree->next);
}

void del_tree (info_tree *tree) {
	if (tree) {
		del_tree(tree->extra);
		del_tree(tree->cnvr);
		del_tree(tree->next);
		if (tree->argv) {
			for (int i = 0; tree->argv[i]; i++)
				free(tree->argv[i]);
			free(tree->argv);
		}	
		if (tree->in_file)
			free(tree->in_file);
		if (tree->out_file)
			free(tree->out_file);
		free(tree);
	}
}

int cmdsh (char *str, int *index, token_lexeme token, info_tree **tree) {
	switch (token.token) {
		case EOL:
			return 0;
		case GR:
		case GR2:
		case L:
		case NAME:
		case LP:
			(*tree) = start_tree();
			if (cmdif(str, index, token, &(*tree)) == -1)
				return -1;
			break;
		default:
			return -1;
			break;
	}
	token = get_next_token(str, index);
	switch (token.token) {
		case EOL:
			return 0;
		case AND:
			(*tree)->backgrnd = 1;
		case T3:
			token = get_next_token(str, index);
			if (token.token != RP) {
				if (cmdsh(str, index, token, &((*tree)->next)) == -1)
					return -1;
			}
			else
				put_back_token(index, token);
			break;
		case RP:
			put_back_token(index, token);
			break;	
		default:
			return -1;
	}
	return 0;
}

int cmdif (char *str, int *index, token_lexeme token, info_tree **tree) {
	if (cmd(str, index, token, &(*tree)) == -1)
		return -1;
	token = get_next_token(str, index);
	switch (token.token) {
		case EOL:
			return 0;
		case OR2:
			token = get_next_token(str, index);
			if (token.token != EOL) {
				(*tree)->type = NOR;
				if (cmdsh(str, index, token, &((*tree)->next)) == -1)
					return -1;
			}
			else 
				return -1;
			break;
		case AND2:
			token = get_next_token(str, index);
			if (token.token != EOL) {
				(*tree)->type = NAND;
				if (cmdsh(str, index, token, &((*tree)->next)) == -1)
					return -1;
			}
			else 
				return -1;
			break;
		default:
			put_back_token(index, token);
			break;
	}
	return 0;
}

int cmd (char *str, int *index, token_lexeme token, info_tree **tree) {
	switch (token.token) {
		case GR:
		case GR2:
		case L:
			if (inout(str, index, token, &(*tree)) == -1)
				return -1;
			token = get_next_token(str, index);
			switch (token.token) {
				case NAME:
					if (conv(str, index, token, &(*tree)) == -1)
						return -1;
					break;
				case LP:
					if (cmd(str, index, token, &(*tree)) == -1)
						return -1;
					break;
				default:
					return -1;
			}
			break;
		case NAME:
			if (conv(str, index, token, &(*tree)) == -1)
				return -1;
			break;
		case LP:
			token = get_next_token(str, index);
			if (cmdsh(str, index, token, &((*tree)->extra)) == -1)
				return -1;
			token = get_next_token(str, index);
			if (token.token != RP)
				return -1;
		default: 
			break;
	}	
	token = get_next_token(str, index);
	switch (token.token) {
		case GR:
		case GR2:
		case L:
			if (inout(str, index, token, &(*tree)) == -1)
				return -1;
			break;
		default:
			put_back_token(index, token);
	}
	return 0;
}

int inout (char *str, int *index, token_lexeme token, info_tree **tree) {
	switch (token.token) {
		case L:
			if (rein(str, index, token, &(*tree)) == -1)
				return -1;
			token = get_next_token(str, index);
			switch (token.token) {
				case GR:
				case GR2:
					if (reout(str, index, token, &(*tree)) == -1)
						return -1;
					break;
				default:
					put_back_token(index, token);
					break;
			}
			break;
		case GR:
		case GR2:
			if (reout(str, index, token, &(*tree)) == -1)
				return -1;
			token = get_next_token(str, index);
			switch (token.token) {
				case L:
					if (rein(str, index, token, &(*tree)) == -1)
						return -1;
					break;
				default:
					put_back_token(index, token);
					break;
			}
		default: ;
	}
	return 0;
}

int rein (char *str, int *index, token_lexeme token, info_tree **tree) {
	if (!((*tree)->in_file = get_file_name(str, *index, token.len)))
	 	return -1;
	return 0;
}

int reout (char *str, int *index, token_lexeme token, info_tree **tree) {
	if (token.token == GR2)
		(*tree)->mode_a = 1;
	if (!((*tree)->out_file = get_file_name(str, *index, token.len)))
		return -1;
	return 0;
}

int conv (char *str, int *index, token_lexeme token, info_tree **tree) {
	(*tree)->argv = get_name(str, *index, token.len);
	token = get_next_token(str, index);	
	switch (token.token) {
		case OR:
			token = get_next_token(str, index);
			if (token.token == NAME) {
				(*tree)->cnvr = start_tree();
				if (cmd(str, index, token, &((*tree)->cnvr)) == -1)
					return -1;
			}
			else		
				return -1;		
			break;
		default:
			put_back_token(index, token);
	}
	return 0;
}

// создает массив строк (для execv) c NILL на конце
char ** get_name (char *str, int i, int name_len) {
	int n = 0, len, j, k;
	i -= name_len;
	char **name = NULL;
	for (k=0; n < name_len; k++) {
		name = realloc(name, (k+1) * sizeof(char *));
		name[k] = NULL;
		len = 0;
		while (str[i] == ' ') {
			i++; n++;
		}
		if (n < name_len) {
			while (n < name_len && str[i] != ' ') {
				name[k] = realloc(name[k], (len + K) * sizeof(char));
				for (j=0; n < name_len && str[i] != ' ' && j<K; j++, n++)
					name[k][len++] = str[i++];
			}
			if (j == K)
				name[k] = realloc(name[k], (len + 1) * sizeof(char));
			name[k][len] = '\0';
		}
	}
	name = realloc(name, (k+1) * sizeof(char *));
	name[k] = NULL;
	return name;
}

// возвращает имя файла
char * get_file_name (char *str, int i, int name_len) {
	i -= (name_len - 1);
	if (str[i] == '>')
		i++;
	while (str[i] == ' ')
		i++;
	if (is_special(str[i]))
		return NULL;
	char *name = NULL;
	int len = 0, j;
	while (!is_special(str[i]) && str[i] != ' ') {
		name = realloc(name, (len + K) * sizeof(char));
		for (j=0; !is_special(str[i]) && str[i] != ' ' && j<K; j++)
			name[len++] = str[i++];
	}
	if (j == K)
		name = realloc(name, (len + 1) * sizeof(char));
	name[len] = '\0';
	return name;
}

int hand_tree (info_tree *tree) {
	if (!tree)
		return 0;
	int st, in, std_in;
	pid_t p;

// Oбработка конвейра
	if (tree->cnvr && (!tree->backgrnd || !tree->flag)) {
		std_in = dup(0); 			// запомнили стандартый поток ввода
		st = hand_conv(tree);
		dup2(std_in, 0);			// возвращаем стандартный ввод на место
		if (tree->flag && tree->next && (tree->type == NXT || (tree->type == NOR && (!WIFEXITED(st) || st)) || (tree->type == NAND && !WEXITSTATUS(st))))
			st = hand_tree(tree->next);
		return st;
	}

//  Отдельный экземпляр Шелла
	if ((p = fork()) < 0) {
		perror("fork");
		exit(0); 
	}
	if (!p) { // сын
		redirection(tree);
		if (tree->backgrnd) { 			// настраиваем фоновый режим
			signal(SIGINT, SIG_IGN);
			dup2(open("/dev/null", O_RDONLY), 0);
//			if (!tree->out_file)
//				dup2(open("/dev/null", O_WRONLY), 1);
			if ((p = fork()) < 0) {
				perror("fork");
				exit(0); 
			}
			if (!p) { // внук
				if (tree->cnvr) {
					tree->flag = 0;
					st = hand_tree(tree);
				}
				else if (tree->argv) {
					if (execvp(tree->argv[0], tree->argv) == -1)
						perror("exec");
				}
				else
					st = hand_tree(tree->extra);
				exit(0);
			}
			else // сын убивает себя
				exit(0);
		}
		else { 	// не фоновый режим
			if (tree->argv) {
				if (execvp(tree->argv[0], tree->argv) == -1)
					perror("exec");
				st = 1;
			}
			else 
				st = hand_tree(tree->extra);
			if (st)		// ошибка
				exit(1); 
			else
				exit(0);
		}
	}	
	else {  // отец
		wait(&st);
		if (tree->next && (tree->type == NXT || (tree->type == NOR && (!WIFEXITED(st) || st)) || (tree->type == NAND && !WEXITSTATUS(st))))
			st = hand_tree(tree->next);
		return st;
	}
	return 0;
}

int hand_conv (info_tree *tree) {
	int st, fd[2];
	pid_t p;
	while (tree) {
		pipe(fd);
		if ((p = fork()) < 0) {
			perror("fork");
			exit(0); 
		}
		if (!p) {  // сын
			redirection(tree);
			if (tree->cnvr) 	// не последняя команда в конвейере
				dup2(fd[1], 1); // перенаправляем стандартный вывод в канал
			close(fd[0]); close(fd[1]);
			if (tree->argv)
				if (execvp(tree->argv[0], tree->argv) == -1)
					perror(tree->argv[0]);
			exit(0);
		}
		// отец
		dup2(fd[0], 0);	// перенаправляем стандартный ввод в канал 
		close(fd[0]); close(fd[1]);
		tree = tree->cnvr;
	}
	while (wait(&st) != -1);
	return st;
}

void redirection (info_tree *tree) {
	int fd, MODE;
	if (tree->in_file) {
		if ((fd = open(tree->in_file, O_RDONLY, 0660)) == -1)
			printf("File %s did not open\n", tree->in_file);
		else
			dup2(fd, 0);
		close(fd);
	}
	if (tree->out_file) {
		if (tree->mode_a)
			MODE = O_APPEND;
		else
			MODE = O_TRUNC;
		if ((fd = open(tree->out_file, O_CREAT | MODE | O_WRONLY, 0660)) == -1)
			printf("File %s did not open\n", tree->out_file);
		else
			dup2(fd, 1);
		close(fd);
	}
}