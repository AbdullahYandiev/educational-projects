// Яндиев А, 211 группа, ДЗ-3
// Вариант 2

#include <iostream>
#include <cstring>
using namespace std;

// максимальная длина вводимых для заполнения полей класса строк
// нужна для ввода данных с клавиатуры
#define MAXLEN 100

// максимальный размер очереди по умолчанию
#define MAXSIZE 10

// ------------------Элемент данных------------------
// счетчик заказов увеличивается только после внесения существенной информации
// если же существенная информация (имя, номер) не внесена, то номер заказа считается -1
class Data {
	char *name, *phone;
	int num;
	static int count;
public:	
	Data ();
	Data (const Data &X);
	Data (const char *N, const char *P);
	Data & operator= (const Data &X);
	friend istream & operator>> (istream &s, Data &X);
	friend ostream & operator<< (ostream &s, const Data &X);
	~Data ();
	void datacpy(const Data &X);
};

Data::Data () : num(-1) {
	name = new char[MAXLEN];
	phone = new char[MAXLEN];
	*name = '\0';
	*phone = '\0';
}

Data::Data (const Data &X) : num(X.num) {
	datacpy(X);
}

Data::Data (const char *N, const char *P = "None") {
	name = new char[strlen(N) + 1];
	phone = new char[strlen(P) + 1];
	strcpy(name, N);
	strcpy(phone, P);
	num = ++count;
}

Data & Data::operator= (const Data &X) {
	if (this != &X) {
		delete[] name;
		delete[] phone;
		datacpy(X);
		num = X.num;
	}	
	return *this;
}

// необходимо ввести и название, и телефон
// если телефона нет, то можно ввести, например, "None"
istream & operator>> (istream &s, Data &X) {
	s >> X.name >> X.phone;
	X.num = ++X.count;
	return s;
}

ostream & operator<< (ostream &s, const Data &X) {
	s << "Order № " << X.num << "\nFirm: " << X.name << "\nPhone: " << X.phone;
	return s;
}

Data::~Data () {
	delete[] name;
	delete[] phone;
}

void Data::datacpy(const Data &X) {
	name = new char[strlen(X.name) + 1];
	phone = new char[strlen(X.phone) + 1];
	strcpy(name, X.name);
	strcpy(phone, X.phone);
}

int Data::count = 0;

// ----------------------Список----------------------
// в списке изменился лишь тип элемента (int -> Data)
class List {
	Data elem;
	struct Node {
		Node *next;
		Node *prev;
		Data elem;
	} *head, *tail;
public:
	List ();
	List (const List &L);
	List & operator= (const List &L);
	~List ();
	void lstcpy(const List &L);
	void delete_lst();
	void push_front (const Data &data);
	void push_back (const Data &data);
	Data front () const;
	Data back () const;
	void pop_front ();
	void pop_back ();
	void insert (int p, const Data &data);
	void erase (int p);
	bool empty () const;
	int size () const;
	void print () const;
};

List::List () : head(NULL), tail(NULL) {}

List::List (const List &L) {
	lstcpy(L);
}

List & List::operator= (const List &L) {
	if (this == &L)
		return *this;
	delete_lst();
	lstcpy(L);
	return *this;
}

List::~List () {
	delete_lst();
}

void List::lstcpy (const List &L) {
	Node *p = L.head;
	if (p)
		tail = head = new Node;
	else
		tail = head = NULL;
	for (; p; p = p->next, tail = tail->next) {
		tail->elem = p->elem;
		if (p->next)
			tail->next = new Node;
		else
			tail->next = NULL;
	}
}

void List::delete_lst () {
	Node *p = head;
	while (p) {
		head = head->next;
		delete p;
		p = head;
	}
}

void List::push_front (const Data &data) {
	Node *p = new Node;
	p->elem = data;
	if (head) {
		p->next = head;
		head->prev = p;
	}	
	else {
		p->next = NULL;
		tail = p;
	}
	p->prev = NULL;
	head = p;
}

void List::push_back (const Data &data) {
	Node *p = new Node;
	p->elem = data;
	if (tail) {
		p->prev = tail;
		tail->next = p;
	}	
	else {
		p->prev = NULL;
		head = p;
	}
	p->next = NULL;
	tail = p;
}
// теперь нет проверки на пустоту списка, ибо непонятно, что можно вернуть
// в этом случае, поэтому эту проверку должен проводить сам пользователь
//---------------------------------------
Data List::front () const {
	return head->elem;
}

Data List::back () const {
	return tail->elem;
}
//---------------------------------------

void List::pop_front () {
	if (!head) 
		return;
	Node *p = head;
	if (head->next) {
		head = head->next;
		head->prev = NULL;
	}
	else
		head = tail = NULL;
	delete p;
}

void List::pop_back () {
	if (!tail)
		return;
	Node *p = tail;
	if (tail->prev) {
		tail = tail->prev;
		tail->next = NULL;
	}
	else
		head = tail = NULL;
	delete p;
}

void List::insert (int p, const Data &data) {
	if (empty())
		return;
	int s = size();
	if (p <= 1 || s == 1) {
		push_front(data);
		return;
	}
	if (p > s)
		p = s;
	Node *q = new Node, *d = head;
	q->elem = data;
	for (--p; p; --p, d = d->next);
	q->prev = d->prev;
	d->prev->next = q;
	d->prev = q;
	q->next = d;
}

void List::erase (int p) {
	if (empty())
		return;
	if (p <= 1) {
		pop_front();
		return;
	}
	if (p >= size()) {
		pop_back();
		return;
	}
	Node *q, *d = head;
	for (--p; p; --p, d = d->next);
	q = d->prev;	
	q->next = d->next;
	d->next->prev = q;
	delete d;
}

bool List::empty () const {
	return (head == NULL);
}

int List::size () const {
	if (empty())
		return 0;
	if (head == tail)
		return 1;
	Node *q = head;
	int i;
	for (i = 0; q; i++, q = q->next);
	return i; 
}

void List::print () const {
	if (empty()) {
		cout << "EMPTY" << endl;
		return;
	}
	Node *q;
	q = head;
	while (q) {
		cout << q->elem << endl;
		q = q->next;
	}
}

//----------------------Очередь----------------------
class Queue: protected List {
	int len;
public:
	Queue ();
	Queue (int maxlen);
	Queue (const Queue &Q);
	Queue & operator= (const Queue &Q);
	~Queue ();
	void back (const Data &data);
	Data front () const;
	void pop ();
	bool empty () const;
	int size () const;
	bool full () const;
	void print () const;
};

Queue::Queue () : len(MAXSIZE) {};

Queue::Queue (int maxlen) : len(maxlen) {};

Queue::Queue (const Queue &Q) : List(Q) {
	len = Q.len;
}

Queue & Queue::operator= (const Queue &Q) {
	if (this == &Q)
		return *this;
	List::operator=(Q);
	len = Q.len;
	return *this;
}

Queue::~Queue () {}

void Queue::back (const Data &data) {
	if (!full())
		push_back(data);
//	else
//		cout << "Queue is full" << endl;
// закомментировал, так как в большом цикле без проверки на
// переполненность очереди печать может сильно перегрузить вывод	
// таким образом, при попытке записать в заполенную очередь ничего не происходит
}

Data Queue::front () const {
	return List::front();
}

void Queue::pop () {
	pop_front();
}

bool Queue::empty () const {
	return List::empty();
}

int Queue::size () const {
	return List::size();
}

bool Queue::full () const {
	return (len == size());
}

void Queue::print () const {
	List::print();
}

int main () {
// особой демонстрации работы со списом нет, ибо тип List не претерпел существенных изменений
	int k;
	cout << "Enter max size of 1st queue (Q1):  ";
	cin >> k;
	
	List L;
	Queue Q1(k), Q2;
	Data D;

// демонстрация работы функций full() и back()
// а также перегруженной операции >> и конструктора с 2 параметрами класса Data
	cout << "Enter number of 1st queue elements:  ";
	for (cin >> k; k && !Q1.full(); --k) {
		cin >> D;
		Q1.back(D);
	}

	cout << "Enter number of list elements:  ";
	for (cin >> k; k; --k) {
		cin >> D;
		L.push_back(D);
	}
// добавляем перед последним элеметом списка новое звено	
	L.insert(L.size(), Data("Amazon", "+7712345"));

	cout << "Enter number of 2st queue elements:  ";
	for (cin >> k; k && !Q2.full(); --k) {
		cin >> D;
		Q2.back(D);
	}

// демонстрация работы функции print()
	cout << "\n---------------------------\n1st QUEUE:" << endl;
	Q1.print();
	cout << "\n---------------------------\nLIST:" << endl;
	L.print();
	cout << "\n---------------------------\n2nd QUEUE:" << endl;
	Q2.print();

// демонстрация работы функций front(), pop(), size() и empty()
// а также перегруженной операции << класса Data	
	cout << "\nSize of 1st queue: " << Q1.size() << endl;
	if (!Q1.empty()) {
		cout << "Its 1st element:\n" << Q1.front() << "\nDeleting 1st element..." << endl;
		Q1.pop();
	}	
	cout << "\n1st QUEUE:" << endl;
	Q1.print();

// демонстрация конструктора копирования и операции присваивания
	Queue Q3(Q1);
	cout << "\n---------------------------\n3rd QUEUE:" << endl;
	Q3.print();
	while (!Q3.empty())	// очистили Q3
		Q3.pop();
	cout << "\n---------------------------\n3rd QUEUE:" << endl;
	Q3.print();
	Q3 = Q2;
	cout << "\n---------------------------\n3rd QUEUE:" << endl;
	Q3.print();
	return 0;
}