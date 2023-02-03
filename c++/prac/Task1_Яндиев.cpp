// Яндиев А, 211 группа, ДЗ-1

// 1) к структуре Date написаны только нужные конструкторы,
// ибо конструктор копирования, деструктор и операция присваивания
// работают тривиально, а сама структура Date лишь вспомогательная
// 2) отладочная печать есть только к спец. функциям (основного) класса Event

#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;

#define STD "Unnamed event"
#define P2D(x, y) do {					\
	if (x >= 0 && x < 10)				\
		cout << 0;						\
	cout << x << y;						\
} while (0)								\

struct Date {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	Date ();
	Date (const char *T); 
	void print_date() const;
};

Date::Date () : year(2021), month(1), day(1), hour(12), minute(0) {};

// Нет проверки на вводимую строку, так что формат строки нужно строго соблюдать
Date::Date (const char *T) {	// формат строки: HH:MM DD/MM/YYYY
	char *tmp = new char[5];
	int i, *a = new int [5];
	for (i = 0; i < 4; i++)
		tmp[i] = '0';
	tmp[i] = '\0';
	for (i = 0; i < 5; i++) {
		if (i == 4) {
			tmp[0] = *(T++);
			tmp[1] = *(T++);
		}
		tmp[2] = *(T++);
		tmp[3] = *(T++);
		a[i] = atoi(tmp);
		T++;
	}
	hour = a[0];
	minute = a[1];
	day = a[2];
	month = a[3];
	year = a[4];
	delete[] a;
	delete[] tmp;
}

void Date::print_date() const {
	cout << "DATE:  ";
	P2D(hour, ':');
	P2D(minute, ' ');
	P2D(day, '/');
	P2D(month, '/');
	cout << year << endl;
}

enum status {FREE, PAID};

class Event {
	char *name;
	Date date;
	status st;
	int price;
	int tickets;
public:
	Event ();
	Event (const char *T);
	Event (const char *N, Date D, status S, int P, int T);
	Event (const Event &X);
	Event & operator= (const Event &X);
	~Event ();
	void print_name() const;
	void print_event() const;
};

Event::Event () : st(FREE), price(0), tickets(50) {
	cout << "Работает конструктор по умолчанию класса Event\n";
	name = new char[strlen(STD) + 1];
	strcpy(name, STD);
}

Event::Event (const char *T) : st(FREE), price(0), tickets(50) {
	cout << "Работает конструктор преобразования класса Event\n";
	name = new char [strlen(T) + 1];
	strcpy(name, T);
}

Event::Event (const char *N, Date D, status S = FREE, int P = 0, int T = 50) {
	cout << "Работает конструктор с 2-5 параметрами класса Event\n";
	name = new char[strlen(N) + 1];
	strcpy(name, N);
	date = D; 
	st = S;
	price = P;
	tickets = T;
}

Event::Event (const Event &X) : date(X.date), st(X.st), price(X.price), tickets(X.tickets) {
	cout << "Работает конструктор копирования класса Event\n";
	name = new char[strlen(X.name) + 1];
	strcpy(name, X.name);
}

Event & Event::operator= (const Event &X) {
	cout << "Работает операция присваивания класса Event\n";
	if (this != &X) {
		delete[] name;
		name = new char[strlen(X.name) + 1];
		strcpy(name, X.name);
		date = X.date;
		st = X.st;
		price = X.price;
		tickets = X.tickets;
	}
	return *this;
}

Event::~Event () {
	cout << "Работает деструктор класса Event\n";
	delete[] name;
}

void Event::print_name() const {
	cout << "EVENT:  " << name << endl;
}

void Event::print_event() const {
	print_name();
	date.print_date();
	if (st == FREE)
		cout << "FOR FREE" << endl;
	else
		cout << "PRICE:  " << price << endl;
	if (tickets == 0)
		cout << "SOLD OUT" << endl;
	else
		cout << tickets << " TICKETS LEFT" << endl;
	cout << "-------------------------" << endl;

}

int main() {
// работа конструктора по умолчанию
	Event X;
	X.print_event();

// работа конструктора преобразования
	Event Z("Film Wonder Woman");
	Z.print_event();

// работа конструктора с 2 параметрами
	Event S("Press-Conference", "14:00 18/03/2021");
	S.print_event();

// работа конструктора копирования
	Event R = S;
	R.print_event();	

// работа конструктора с 5 параметрами
	Event Y = Event("MUN vs MCI", "19:30 20/03/2021", PAID, 300, 78000);
	Y.print_event();

// работа операции присваивания
	X = Y;
	X.print_event();

	return 0;
// работа деструкторов для каждого объявленного объекта класса Event
}