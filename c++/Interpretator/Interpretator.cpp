#include <iostream>
#include <vector>
#include <stack>
using namespace std;

//---------------------------------Лексический анализ---------------------------------

enum type_of_lex {
	LEX_NULL, LEX_PROGRAM, LEX_STRUCT, LEX_AND, LEX_NOT, LEX_OR, LEX_TRUE, LEX_FALSE, LEX_BOOL, LEX_INT, LEX_STRING, LEX_WHILE, LEX_FOR, LEX_IF, LEX_ELSE, LEX_BREAK, LEX_GOTO, LEX_READ, LEX_WRITE, LEX_TAB, LEX_ENDL,
	LEX_LFBRACE, LEX_RFBRACE, LEX_COMMA, LEX_SEMICOLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_NEQ, LEX_LSS, LEX_GTR, LEX_LEQ, LEX_GEQ, LEX_PLUS, LEX_MINUS, LEX_UNMINUS, LEX_TIMES, LEX_SLASH,
	LEX_NUM, LEX_STR, LEX_ID,
	POLIZ_LABEL, POLIZ_ADDRESS, POLIZ_GO, POLIZ_FGO
};


class Lex {
	type_of_lex t_lex;
	int v_lex;
public:
	Lex (type_of_lex t = LEX_NULL, int v = 0): t_lex(t), v_lex(v) {}
	type_of_lex  get_type () const {return t_lex;}
	int get_value () const {return v_lex;}
	friend ostream & operator<< (ostream &s, Lex l);
};

class Ident {
	string name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value, num_struct, s_type;
public:
	Ident(): declare(false), assign(false), num_struct (-1) {}
	bool operator== (const string &s) const {return name == s;}
	Ident (const string s, int n): name(s), num_struct(n), declare(false), assign(false) {}
	string get_name () const {return name;}
	void put_name (string s) {name = s;} 
	bool get_declare () const {return declare;}
	void put_declare () {declare = true;}
	type_of_lex get_type () const {return type;}
	void put_type (type_of_lex t) {type = t;}
	bool get_assign () const {return assign;}
	void put_assign () {assign = true;}
	int  get_value () const {return value;}
	void put_value (int v) {value = v;}
	int  get_num_struct () const {return num_struct;}
	void put_num_struct (int n) {num_struct = n;}
	int  get_struct_type () const {return s_type;}
	void put_struct_type (int s) {s_type = s;}
};

vector<Ident> TID;

int put (const string &buf, int num_struct = -1) {
	vector<Ident>::iterator k;
	for (k = TID.begin(); k != TID.end(); k++)
		if (*k == buf && (*k).get_num_struct() == num_struct)
			return k - TID.begin();
	TID.push_back (Ident(buf, num_struct));
	return TID.size() - 1;
}

int change_TID (int pos) {
	string buf = TID[pos].get_name();
	vector<Ident>::iterator k = TID.begin() + pos;
	TID.erase(k);
	buf.erase(0, 1);
	return put(buf);
}

vector<string> TS;

int put_str (const string &buf) {
	vector<string>::iterator k;
	if ((k = find(TS.begin(), TS.end(), buf)) != TS.end())
		return k - TS.begin();
	TS.push_back(buf);
	return TS.size() - 1;
}

class Scanner {
	FILE *fp;
	char c;
	int look (const string buf, const char ** list);
	void gc () {c = fgetc(fp);}
public:
	static const char *TW[], *TD[];
	Scanner (const char *program) {
		if (!(fp = fopen (program, "r")))
			throw  "can’t open file" ;
	}
	Lex get_lex (bool in_struct, int num_struct);
	char get_sym () {
		for (gc(); c == ' ' || c == '\n' || c == '\r' || c == '\t'; gc());
		return c;
	}
	void put_sym (char sym)	{
		ungetc(sym, fp);
	}
	
};

int Scanner::look (const string buf, const char ** list) {
	int i = 0;
	while (list[i]) {
		if (buf == list[i])
			return i;
			++i;
	}
	return -1;
}

const char *Scanner::TW[] = {"", "program", "struct", "and", "not", "or", "true", "false", "bool", "int", "string", "while", "for", "if", "else", "break", "goto", "read", "write", "tab", "endl", NULL};
const char *Scanner::TD[] = {"{", "}", ",", ";", "=", "(", ")", "==", "!=", "<", ">", "<=", ">=", "+", "-","@", "*", "/", NULL};
 
Lex Scanner::get_lex (bool is_struct = false, int num_struct = -1) {
	enum state {H, IDENT, NUMB, STR, COM, ALE, NEQ};
	int d, j;
	string buf;
	state CS = H;
	for (;;) {
		gc();
		if (feof(fp))
			return Lex(LEX_NULL);
		switch (CS) {
			case H:
				if (c == ' ' || c == '\n' || c == '\r' || c == '\t'); 
				else if (isalpha(c)) {
					buf.push_back(c);
					CS = IDENT;
				}
				else if (isdigit(c)) {
					d = c - '0';
					CS = NUMB;
				}
				else if (c == '"')
					CS = STR;
				else if (c == '/') {
					gc();
					if (c == '*')
						CS  = COM;
					else {
						ungetc(c, fp);
						buf.push_back ('/');
						j = look(buf, TD);
						return Lex((type_of_lex)(j + (int)LEX_LFBRACE), j);
					}
				}
				else if (c == '<' || c == '>' || c == '=') {
					buf.push_back(c);
					CS  = ALE; 
				}
				else if (c == '!') {
					buf.push_back(c);
					CS  = NEQ;
				}
				else {				
					buf.push_back(c);
					if ((j = look(buf, TD)) != -1)
						return Lex((type_of_lex)(j + (int)LEX_LFBRACE), j);
					else
						throw c;
				}
				break;
			case IDENT:
				if (isalpha(c) || isdigit(c))
					buf.push_back(c); 
				else {
					ungetc(c, fp);
					if ((j = look (buf, TW)) != -1)
						return Lex((type_of_lex)j, j);
					else {
						if (is_struct) {
							if (num_struct == -1)
								buf = "0" + buf;
							else
								buf = to_string(num_struct) + buf;
						}	
						j = put(buf, num_struct);
						return Lex(LEX_ID, j);
					}
				}
				break;
			case NUMB:
				if (isdigit (c))
					d = d * 10 + (c - '0');
				else {
					ungetc (c, fp);
					return Lex (LEX_NUM, d);
				}
				break;
			case STR:
				if (c == '"') {
					j = put_str(buf);
					return Lex(LEX_STR, j);
				}
				else
					buf.push_back(c);
				break;
			case COM:
				if (c == '*') {
					gc();
					if (c == '/')
						CS = H;
				}
				break;
			case ALE:
				if (c == '=') 
					buf.push_back(c);
				else
					ungetc(c, fp);
				j = look(buf, TD);
				return Lex ((type_of_lex)(j + (int)LEX_LFBRACE), j);
				break;
			case NEQ:
				if (c == '=') {
					buf.push_back(c);
					j = look (buf, TD);
					return Lex (LEX_NEQ, j);
				}
				else
					throw '!';
				break;
		}
	}
}

ostream & operator<< (ostream &s, Lex l) {
	string t;
	if (l.t_lex <= LEX_ENDL)
		t = Scanner::TW[l.t_lex];
	else if (l.t_lex >= LEX_LFBRACE && l.t_lex <= LEX_SLASH)
		t = Scanner::TD[l.t_lex - LEX_LFBRACE];
	else if (l.t_lex == LEX_NUM)
		t = "NUMB";
	else if (l.t_lex == LEX_ID)
		t = TID[l.v_lex].get_name();
	else if (l.t_lex == LEX_STR)
		t = TS[l.v_lex];
	else if (l.t_lex == POLIZ_LABEL)
		t = "Label";
	else if (l.t_lex == POLIZ_ADDRESS)
		t = "&" + TID[l.v_lex].get_name();
	else if (l.t_lex == POLIZ_GO)
		t = "!";
    else if (l.t_lex == POLIZ_FGO) 
		t = "!F";
	else
		throw l;
	s << '[' << t << ", " << l.v_lex << "];";
	return s;
}
//---------------------------Синтактический анализ + ПОЛИЗ---------------------------

template <class T, class T_EL>
void from_st (T & st, T_EL & i) {
	i = st.top();
	st.pop();
}
 
class Parser {
	Lex curr_lex;
	type_of_lex c_type, tmp_type;
	int c_val, tmp_val, num_struct, s_type;
	bool in_loop, in_struct;
	Scanner scan;
	struct goto_label {
		int label;
		string name;
		goto_label (string N = "", int L = 0) : name(N), label(L) {}
	};
	stack <int> st_int;
	stack <int> st_break;
	stack <goto_label> st_goto;
	vector <goto_label> v_goto;
	stack <type_of_lex> st_lex;
	vector <Lex> :: iterator p;
	void  P ();
	void  Y ();
	void  D1 ();
	void  D ();
	void  I1 ();
	void  B ();
	void  S ();
	void  S1 ();
	void  E ();
	void  I ();
	void  A ();
	void  C ();
	void  T ();
	void  F ();
	void  goto_analyze ();
	void  dec (type_of_lex type);
	void  check_id ();
	void  check_op ();
	void  check_minus ();
	void  check_not ();
	void  eq_type (bool b);
	void  eq_bool ();
	void  assign_check_id ();
	void  simple_check_id ();
	void  gl () {
		curr_lex = scan.get_lex(in_struct, num_struct);
		c_type = curr_lex.get_type();
		c_val = curr_lex.get_value();
	}
public:
	vector <Lex> poliz;
	Parser (const char *program) : scan(program), in_loop(false), in_struct(false), num_struct(-1), s_type(-1) {}
	void analyze();
};

void Parser::analyze () {
	gl();
	P();
	gl();
	if (c_type != LEX_NULL)
		throw curr_lex;
	for (int k = 0; Lex l : poliz)
		cout << k++ << '\t' << l << endl;
	cout << endl << endl;
}

void Parser::P() {
	if (c_type != LEX_PROGRAM)
		throw curr_lex;
	gl();
	if (c_type != LEX_LFBRACE)
		throw curr_lex;  
	gl();
	Y();
	D1();
	B();
	if (c_type != LEX_RFBRACE)
		throw curr_lex;
	if (!st_goto.empty() || !v_goto.empty())
		goto_analyze();
}

void Parser::Y () {
	while (c_type == LEX_STRUCT) {
		in_struct = true;
		gl();
		if (c_type != LEX_ID)
			throw curr_lex;
		st_int.push(c_val);
		dec(c_type);
		num_struct = TID.size() - 1;
		in_struct = false;
		gl();
		if (c_type != LEX_LFBRACE)
			throw curr_lex;
		gl();
		if (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING)
			D();
		else
			throw curr_lex;
		if (c_type != LEX_SEMICOLON)
			throw curr_lex;
		gl();
		while (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING) {
			D();
			if (c_type != LEX_SEMICOLON)
				throw curr_lex;
			gl();
		}
		if (c_type != LEX_RFBRACE)
			throw curr_lex;
		gl();
		if (c_type != LEX_SEMICOLON)
			throw curr_lex;
		num_struct = -1;
		in_struct = true;
		gl();
		in_struct = false;
	}
}	

void Parser::D1 () {
	char c;
	while (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING || c_type == LEX_ID) {
		if (c_type == LEX_ID) {
			c = scan.get_sym();
			scan.put_sym(c);
			if (!isalpha(c)) {
				c_val = change_TID(c_val);
				break;
			}
		}
		D();
		if (c_type != LEX_SEMICOLON)
			throw curr_lex;
		in_struct = true;
		gl();
		in_struct = false;
	}
}

void Parser::D() {
	if (c_type == LEX_ID) {
		simple_check_id();
		s_type = c_val;
	}
	else
		s_type = -1;
	tmp_type = c_type;
	gl();
	tmp_val = c_val;
	I1();
	while (c_type == LEX_COMMA) {
		gl();
		I1();
	}
	dec(tmp_type);
}

void Parser::I1() {
	if (c_type != LEX_ID)
		throw curr_lex;
	if (s_type != -1) {
		string buf;
		TID[c_val].put_struct_type(s_type);
		for (int i = s_type + 1, j = TID.size(), size = j; i < j && TID[i].get_num_struct() == s_type; i++, size++) {
			buf = to_string(c_val) + TID[i].get_name();
			TID.push_back(TID[i]);
			TID[size].put_name(buf);
			TID[size].put_value(size);
			TID[size].put_num_struct(c_val);
		}
	}	
	st_int.push(c_val);
	if (!in_struct) {
		poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
		gl();
		if (c_type == LEX_ASSIGN) {
			dec(tmp_type);
			c_val = tmp_val;
			assign_check_id();
			gl();
			if ((tmp_type == LEX_INT && c_type == LEX_NUM) || (tmp_type == LEX_BOOL && (c_type == LEX_TRUE || c_type == LEX_FALSE)) || (tmp_type == LEX_STRING && c_type == LEX_STR))
				F();
			else
				throw "wrong types are in =";
			poliz.push_back(Lex(LEX_ASSIGN));
			poliz.push_back(Lex(LEX_SEMICOLON));
		}
		else
			poliz.pop_back();
	}	
}

void Parser::B () {
	while (c_type == LEX_IF || c_type == LEX_FOR || c_type == LEX_WHILE || c_type == LEX_BREAK || c_type == LEX_GOTO || c_type == LEX_READ || c_type == LEX_WRITE || c_type == LEX_LFBRACE || c_type == LEX_ID || c_type == LEX_NUM || c_type == LEX_TRUE || c_type == LEX_FALSE || c_type == LEX_NOT || c_type == LEX_LPAREN)
		S();
}
 
void Parser::S () {
	int pl0 = 0, pl1, pl2, pl3;
	bool glob_loop = false;
	char c;
	switch (c_type) { 
		case LEX_IF:
			gl();
			if (c_type != LEX_LPAREN)
				throw curr_lex;
			gl();
			E();
			if (c_type != LEX_RPAREN)
				throw curr_lex;
			poliz.pop_back();
			eq_bool();
			pl2 = poliz.size();
			poliz.push_back(Lex());
			poliz.push_back(Lex(POLIZ_FGO));
			gl();
			S();
			pl3 = poliz.size();
			poliz.push_back (Lex());
			poliz.push_back (Lex(POLIZ_GO));
			poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
			if (c_type != LEX_ELSE)
				throw curr_lex;
			gl();
			S();
			poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());
			break;     
		case LEX_FOR:
			if (in_loop)
				glob_loop = true;
			else
				in_loop = true;
			gl();
			if (c_type != LEX_LPAREN)
				throw curr_lex;
			gl();
			if (c_type != LEX_SEMICOLON) {
				E();
				if (c_type != LEX_SEMICOLON)
					throw curr_lex;
			}
			gl();
			pl2 = poliz.size();
			if (c_type != LEX_SEMICOLON) {
				E();
				if (c_type != LEX_SEMICOLON)
					throw curr_lex;
				poliz.pop_back();
				eq_bool();
				pl0 = poliz.size();
				poliz.push_back(Lex());
				poliz.push_back(Lex(POLIZ_FGO));
			}
			gl();
			if (c_type != LEX_RPAREN) {
				pl1 = poliz.size();
				poliz.push_back(Lex());
				poliz.push_back(Lex(POLIZ_GO));
				pl3 = poliz.size();
				E();
				poliz.push_back(Lex(POLIZ_LABEL, pl2));
				poliz.push_back(Lex(POLIZ_GO));
				if (c_type != LEX_RPAREN)
					throw curr_lex;
				poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
			}
			else
				pl3 = pl2;
			gl();
			S();
			poliz.push_back(Lex(POLIZ_LABEL, pl3));
			poliz.push_back(Lex(POLIZ_GO));
			pl1 = poliz.size();
			if (pl0)
				poliz[pl0] = Lex(POLIZ_LABEL, pl1);
			while (!st_break.empty()) {
				from_st(st_break, pl2);
				poliz[pl2] = Lex(POLIZ_LABEL, pl1);
			}
			if (!glob_loop)
				in_loop = false;
			break;
		case LEX_WHILE:
			if (in_loop)
				glob_loop = true;
			else
				in_loop = true;
			pl0 = poliz.size ();
			gl ();
			if (c_type != LEX_LPAREN)
				throw curr_lex;
			gl();
			E();
			poliz.pop_back();
			if (c_type != LEX_RPAREN) 
				throw curr_lex;
			eq_bool ();
			pl1 = poliz.size(); 
			poliz.push_back(Lex());
			poliz.push_back(Lex(POLIZ_FGO));
			gl();
			S();
			poliz.push_back(Lex(POLIZ_LABEL, pl0));
			poliz.push_back(Lex(POLIZ_GO));
			pl3 = poliz.size();
			poliz[pl1] = Lex(POLIZ_LABEL, pl3);
			while (!st_break.empty()) {
				from_st(st_break, pl2);
				poliz[pl2] = Lex(POLIZ_LABEL, pl3);
			}
			if (!glob_loop)
				in_loop = false;
			break;
		case LEX_BREAK:
			if (!in_loop)
				throw "break not in loop";
			gl();
			if (c_type != LEX_SEMICOLON)
				throw curr_lex;
			gl();
			st_break.push(poliz.size());
			poliz.push_back(Lex());
			poliz.push_back(Lex(POLIZ_GO));
			break;
		case LEX_GOTO:
			gl();
			if (c_type != LEX_ID)
				throw curr_lex;
			st_goto.push(goto_label(TID[c_val].get_name(), poliz.size()));
			poliz.push_back(Lex());
			poliz.push_back(Lex(POLIZ_GO));
			gl();
			if (c_type != LEX_SEMICOLON)
				throw curr_lex;
			gl();
			break;
		case LEX_READ:
			gl ();
			if (c_type != LEX_LPAREN)
				throw curr_lex;
			gl ();
			if (c_type != LEX_ID)
				throw curr_lex;
			I();
			if (TID[c_val].get_type() == LEX_ID)
				throw "struct in read";
			simple_check_id();
			poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
			gl();
			if (c_type != LEX_RPAREN) 
				throw curr_lex;
			gl();
			if (c_type != LEX_SEMICOLON) 
				throw curr_lex;
			gl();
			poliz.push_back(Lex(LEX_READ));
			break;
		case LEX_WRITE:
			gl();
			if (c_type != LEX_LPAREN)
				throw curr_lex;
			gl();
			if (c_type == LEX_ENDL || c_type == LEX_TAB) {
				poliz.push_back(Lex(c_type));
				gl();
			}
			else {
				E();
				poliz.pop_back();
				if (tmp_val != -1)
					poliz.push_back(Lex(LEX_WRITE));
			}	
			while (c_type == LEX_COMMA) {
				gl();
				if (c_type == LEX_ENDL || c_type == LEX_TAB) {
					poliz.push_back(Lex(c_type));
					gl();
				}
				else {
					E();
					poliz.pop_back();
					if (tmp_val != -1)
						poliz.push_back(Lex(LEX_WRITE));
				}	
			}
			if (c_type != LEX_RPAREN) 
				throw curr_lex;
			gl();
			if (c_type != LEX_SEMICOLON)
				throw curr_lex;
			gl();
			break;
		case LEX_LFBRACE:
			gl();
			B();
			if (c_type != LEX_RFBRACE)
				throw curr_lex;
			gl();
			break;
		case LEX_ID:
			c = scan.get_sym();
			if (c == ':') {
				v_goto.push_back(goto_label(TID[c_val].get_name(), poliz.size()));
				gl();
				S();
			}
			else {
				scan.put_sym(c);
				E();
				if (c_type != LEX_SEMICOLON)
					throw curr_lex;
				gl();
			}
			break;
		default:
			throw curr_lex;
	}
}

void Parser::E () {
	int i = 0;
	char c = '=', tmp;
	bool b = true;
	vector <Lex> :: iterator t;
	I();
	tmp_val = 0;
	p = poliz.end();
	while (c_type == LEX_ID && c == '=')
		if ((c = scan.get_sym()) == '=') {
			tmp = scan.get_sym();
			scan.put_sym(tmp);
			if (tmp == '=') {
				scan.put_sym('=');
				break;
			}
			else {
				check_id();
				if (TID[c_val].get_type() != LEX_ID) 
					poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
				else {
					b = false;
					for (t = p, i = c_val + 1, s_type = 1; i < TID.size() && TID[i].get_num_struct() == c_val; t += tmp_val + 1, p = t - 2 * s_type * (tmp_val + 1), i++, s_type++) {
						t += tmp_val;
						t = poliz.insert(t, Lex(POLIZ_ADDRESS, i));
						t = poliz.insert(t + 1, Lex(LEX_ASSIGN));
					}
				}
				gl();
				i = ++tmp_val;
				if (c_type == LEX_LPAREN)
					break; 
				I();
			}
		}
		else
			scan.put_sym(c);			
	A();
	for (; i; --i) {
		eq_type(false);
		if (b)
			poliz.push_back(Lex(LEX_ASSIGN));
	}
	if (b)
		poliz.push_back(Lex(LEX_SEMICOLON));

}

void Parser::I () {
	char c = scan.get_sym();
	int t_val = c_val;
	if (c == '.') {
		in_struct = true; num_struct = t_val;
		gl();
		if (c_type != LEX_ID)
			throw curr_lex;
		in_struct = false; num_struct = -1;
		simple_check_id();
	}
	else
		scan.put_sym(c);
}

void Parser::A () {
	C();
	if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GTR || c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ) {
		st_lex.push(c_type);
		gl();
		C(); 
		check_op();
	}
}

void Parser::C () {
	T();
	while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR) {
		st_lex.push(c_type);
		gl();
		T();
		check_op();
	}
}

void Parser::T () {
	F();
	while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND) {
		st_lex.push(c_type);
		gl();
		F();
		check_op();
	}
}

void Parser::F () {
	int i;
	vector <Lex> :: iterator t;
	switch (c_type) {
		case LEX_ID:
			I();
			check_id();
			if (TID[c_val].get_type() != LEX_ID)
				poliz.push_back(Lex(LEX_ID, c_val));
			else {
				for (t = p, i = c_val + 1, s_type = 1; i < TID.size() && TID[i].get_num_struct() == c_val; i++, s_type++) {
					t += tmp_val;
					t = poliz.insert(t, Lex(LEX_ID, i));
					t += tmp_val + 1;

					if (tmp_val) {
						t = poliz.insert(t, Lex(LEX_SEMICOLON)) + 1;
						p = t - 2 * s_type * (tmp_val + 1);
					}
					else {
						t = poliz.insert(t, Lex(LEX_WRITE)) + 1;
						t = poliz.insert(t, Lex(LEX_TAB)) + 1;
						p = t - 3 * s_type - 1;
					}
				}
				tmp_val = -1;
			}
			gl();
			break;
		case LEX_NUM:
			st_lex.push(LEX_INT);
			poliz.push_back(curr_lex);
			gl();
			break;
		case LEX_TRUE:
			st_lex.push(LEX_BOOL);
			poliz.push_back(Lex(LEX_TRUE, 1));
			gl();
			break;
		case LEX_FALSE:
			st_lex.push(LEX_BOOL);
			poliz.push_back(Lex(LEX_FALSE, 0));
			gl();
			break;
		case LEX_STR:
			st_lex.push(LEX_STRING);
			poliz.push_back(Lex(LEX_STR, c_val));
			gl();
			break;
		case LEX_MINUS:
			gl();
			F();
			check_minus();
			break;
		case LEX_NOT:
			gl(); 
			F(); 
			check_not();
			break;
		case LEX_LPAREN:
			gl();
			E();
			poliz.pop_back();
			if (c_type == LEX_RPAREN)
				gl();
			else 
				throw curr_lex;
			break;
		default:
			throw curr_lex;
	}		
}

void Parser::goto_analyze () {
	int i, j;
	goto_label tmp;
	if (!v_goto.empty())
		for (i = 0; i < v_goto.size() - 1; i++)
			for (j = i + 1; j < v_goto.size(); j++)
				if (v_goto[i].name == v_goto[j].name)
					throw "same labels";
	while (!st_goto.empty()) {
		from_st(st_goto, tmp);
		for (i = 0; i < v_goto.size() && v_goto[i].name != tmp.name; i++);
		if (i == v_goto.size())
			throw "unkrown label in goto";
		poliz[tmp.label] = Lex(POLIZ_LABEL, v_goto[i].label);
	}
}

void Parser::dec (type_of_lex type) {
	int i;
	while (!st_int.empty()) {
		from_st(st_int, i);
		if (TID[i].get_declare())
			throw "twice";
		else {
			TID[i].put_declare();
			TID[i].put_type(type);
		}
	}
}

void Parser::check_id () {
	if (TID[c_val].get_declare()) {
		st_lex.push(TID[c_val].get_type());
		if (TID[c_val].get_type() == LEX_ID) 
			st_int.push(TID[c_val].get_struct_type());
	}
	else
		throw "not declared";
}

void Parser::check_op () {
	type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
	from_st(st_lex, t2);
	from_st(st_lex, op);
	from_st(st_lex, t1);
	if (op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
		r = LEX_INT;
	else if (op == LEX_PLUS) {
		if (t1 == LEX_STRING) 
			r = t = LEX_STRING;
		else
			r = LEX_INT;
	}	
	if (op == LEX_EQ || op == LEX_NEQ || op == LEX_GTR || op == LEX_LSS) {
		if (t1 == LEX_STRING)
			t = LEX_STRING;
		else
			t = LEX_INT;
	}
	if (op == LEX_OR || op == LEX_AND)
		t = LEX_BOOL;
	if (t1 == t2  &&  t1 == t)
		st_lex.push(r);
	else
		throw "wrong types are in operation";
	poliz.push_back(Lex(op));
}

void Parser::check_minus () {
	if (st_lex.top() != LEX_INT)
		throw "wrong type is in -";
	else  
		poliz.push_back (Lex(LEX_UNMINUS));
}

void Parser::check_not () {
	if (st_lex.top() != LEX_BOOL)
		throw "wrong type is in not";
	else  
		poliz.push_back (Lex(LEX_NOT));
}

void Parser::eq_type (bool b = true) {
	int i;
	type_of_lex t;
	from_st (st_lex, t);
	if (t != st_lex.top())
		throw "wrong types are in =";
	if (t == LEX_ID) {
		from_st (st_int, i);
		if (i != st_int.top())
			throw "wrong types (structs) are in =";
		if (b)
			st_int.pop();
	}
	if (b)
		st_lex.pop();
}

void Parser::eq_bool() {
	if (st_lex.top() != LEX_BOOL)
		throw "expression is not boolean";
	st_lex.pop();
}

void Parser::assign_check_id () {
	if (TID[c_val].get_declare()) {
		if (TID[c_val].get_type() == LEX_ID)
			st_int.push(TID[c_val].get_struct_type());
	}
	else	
		throw "not declared";
}

void Parser::simple_check_id () {
	if (!TID[c_val].get_declare())
		throw "not declared";
}

//------------------------------------Интерпретация-----------------------------------

class Executer {
	struct data {
		int value;
		bool is_bool, is_str;
		data (int v = 0, bool b = false, bool bs = false) : value(v), is_bool(b), is_str(bs) {}
		data (const data &D) :  value(D.value), is_bool(D.is_bool), is_str(D.is_str) {}
		data & operator= (const data &D) {if (this != &D) {value = D.value; is_bool = D.is_bool, is_str = D.is_str;} return *this;}
		data operator! () {data D = *this; D.value = !value; return D;}
		data operator|| (const data &S) {data D = *this; D.value = value || S.value; return D;}
		data operator&& (const data &S) {data D = *this; D.value = value && S.value; return D;}
		data operator+ (const data &S) {data D = *this; if (is_str) D.value = put_str(TS[value] + TS[S.value]); else D.value += S.value ; return D;}
		data operator- () {data D = *this; D.value = -D.value ; return D;}
		data operator- (const data &S) {data D = *this; D.value -= S.value ; return D;}
		data operator* (const data &S) {data D = *this; D.value *= S.value ; return D;}
		data operator/ (const data &S) {data D = *this; D.value /= S.value ; return D;}
		bool operator== (const data &S) {return value == S.value;}
		bool operator!= (const data &S) {return value != S.value;}
		bool operator> (const data &S) {if (is_str) return TS[value] > TS[S.value]; else return value > S.value;}
		bool operator< (const data &S) {if (is_str) return TS[value] < TS[S.value]; else return value < S.value;}
		bool operator>= (const data &S) {return value >= S.value;}
		bool operator<= (const data &S) {return value <= S.value;}
		operator int() {return value;}
		friend ostream & operator<< (ostream &s, const data &D) {if (D.is_str) s << TS[D.value]; else if (D.is_bool) s << (D.value ? "true" : "false"); else s << D.value; return s;}
	};
	stack <data> args;
public:
    void execute (vector <Lex> & poliz);
};
 
void Executer::execute (vector <Lex> & poliz) {
	Lex pc_el;
	int index = 0, size = poliz.size();
	data i, j;
	while (index < size) {
		pc_el = poliz[index];
		switch (pc_el.get_type()) {
			case LEX_NUM: case POLIZ_LABEL:
				args.push(pc_el.get_value());
				break;
			case LEX_ID:
				if (!TID[pc_el.get_value()].get_assign())
					throw "POLIZ: indefinite identifier";
				switch (TID[pc_el.get_value()].get_type()) {
					case LEX_INT:
						args.push(data(TID[pc_el.get_value()].get_value()));
						break;
					case LEX_BOOL:
						args.push(data(TID[pc_el.get_value()].get_value(), true));
						break;
					case LEX_STRING:
						args.push(data(TID[pc_el.get_value()].get_value(), false, true));
						break;	
					default:
						break;
				}
				break;		
			case POLIZ_ADDRESS:
				switch (TID[pc_el.get_value()].get_type()) {
					case LEX_INT:
						args.push(data(pc_el.get_value()));
						break;
					case LEX_BOOL:
						args.push(data(pc_el.get_value(), true));
						break;
					case LEX_STRING:
						args.push(data(pc_el.get_value(), false, true));
						break;	
					default:
						break;
				}
				break;
			case LEX_TRUE: case LEX_FALSE:
				args.push(data(pc_el.get_value(), true));
				break;
			case LEX_STR:
				args.push(data(pc_el.get_value(), false, true));
				break;
			case LEX_NOT:
				from_st(args, i);
				args.push(!i);
				break;
			case LEX_OR:
				from_st(args, i); 
				from_st(args, j);
				args.push(j || i);
				break;
			case LEX_AND:
				from_st(args, i);
				from_st(args, j);
				args.push(j && i);
				break;
			case POLIZ_GO:
				from_st(args, i);
				index = int(i) - 1;
				break;
			case POLIZ_FGO:
				from_st(args, i);
				from_st(args, j);
				if (!j) 
					index = int(i) - 1;
				break;
			case LEX_WRITE:
				from_st(args, j);
				cout << j;	
				break;
			case LEX_TAB:
				cout << '\t';
				break;
			case LEX_ENDL:
				cout << endl;
				break;	
			case LEX_READ:
				int k;
				from_st (args, i);
				if (TID[i].get_type () == LEX_INT) {
					cout << "Input int value for " << TID[i].get_name() << endl;
					cin >> k;
				}
				else {
					string j;
					if (TID[i].get_type() == LEX_STRING) {
						cout << "Input string value for " << TID[i].get_name() << endl;
						cin >> j;
						k = put_str(j);
					}
					else
						while (1) {
							cout << "Input boolean value (true or false) for " << TID[i].get_name() << endl;
							cin >> j;
							if (j != "true" && j != "false") {
								cout << "Error in input: true/false" << endl;
								continue;
							}
							k = (j == "true") ? 1 : 0;
							break;
						}
				}
				TID[i].put_value(k);
				TID[i].put_assign();
				break;
			case LEX_PLUS:
				from_st(args, i);
				from_st(args, j);
				args.push(j + i);
				break;
			case LEX_TIMES:
				from_st(args, i);
				from_st(args, j);
				args.push(i * j);
				break;
			case LEX_MINUS:
				from_st(args, i);
				from_st(args, j);
				args.push(j - i);
				break;
			case LEX_UNMINUS:
				from_st(args, i);
				args.push(-i);
				break;
			case LEX_SLASH:
				from_st(args, i);
				from_st(args, j);
				if (!i)
					args.push (j / i);
				else
					throw "POLIZ:divide by zero";
				break;
			case LEX_EQ:
				from_st(args, i);
				from_st(args, j);
				args.push(i == j);
				break;
			case LEX_LSS:
				from_st(args, i);
				from_st(args, j);
				args.push(j < i);
				break;
			case LEX_GTR:
				from_st(args, i);
				from_st(args, j);
				args.push(j > i);
				break;
			case LEX_LEQ:
				from_st(args, i);
				from_st(args, j);
				args.push(j <= i);
				break;
			case LEX_GEQ:
				from_st(args, i);
				from_st(args, j);
				args.push(j >= i);
				break;
			case LEX_NEQ:
				from_st(args, i);
				from_st(args, j);
				args.push(j != i);
				break;
			case LEX_ASSIGN:
				from_st(args, i);
				from_st(args, j);
				TID[j].put_value(i);
				TID[j].put_assign();
				args.push(i);
				break;
			case LEX_SEMICOLON:
				args.pop();
				break;
			default:
				throw "POLIZ: unexpected elem";
		}
		++index;
	};
}

class Interpretator {
	Parser pars;
	Executer E;
public:
	Interpretator (const char *program): pars(program) {}
	void interpretation ();
};

void Interpretator::interpretation () {
	pars.analyze();
	E.execute(pars.poliz);
}

//----------------------------------------Main----------------------------------------

int main(int argc, char **argv) {
	try {
		if (argc != 2)
			throw "no input file";
		Interpretator I(argv[1]);
		I.interpretation();
		return 0;
	}
	catch (const char *s) {
		cout << s << endl;
		return 1;
	}
	catch (char c) {
		cout << "unexpected symbol " << c << endl;
		return 1;
	}
	catch (Lex l) {
		cout << "unexpected lexeme " << l << endl;
	return 1;
	}
}