// Яндиев А, 211 группа, ДЗ-2
// исправил 160 строку, когда функция insert() работает со списком из 1 элемента

#include <iostream>
using namespace std;

#define REVERSE true

class List {
	struct Node {
		Node *next;
		Node *prev;
		int elem;
	} *head, *tail; // начало и конец списка соответсвенно
public:
// конструктор умолчания, копирования, операция присваивания, деструктор	
	List ();
	List (const List &L);
	List & operator= (const List &L);
	~List ();

// функции копирования и удаления списка (очищение памяти)
// они нужны для локаничного написания конструкторов	
	void lstcpy(const List &L);
	void delete_lst();

// обязательный набор методов работы со списком
	void push_front (int data);
	void push_back (int data);
	int front () const;
	int back () const;
	void pop_front ();
	void pop_back ();
	void insert (int p, int data);
	void erase (int p);
	bool empty () const;
	int size () const;
	void print (bool rev) const;		
};

List::List () : head(NULL), tail(NULL) {};

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

void List::push_front (int data) {
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

void List::push_back (int data) {
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

// front() и back() возвращают -1, если список пустой
int List::front () const {
	if (head)
		return head->elem;
	else
		return -1;
}

int List::back () const {
	if (tail)
		return tail->elem;
	else
		return -1;
}

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

// если p < 1, то добавляется новый элемент перед первым элементов
// если p > size, то добавляется элемент перед последним
// если список пуст, то ничего не происходит
void List::insert (int p, int data) {
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

// если p < 1, то удаляется первый элемент
// если p > size, то удаляется последний
// если список пуст, то ничего не происходит
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

// в этом методе добавлен параметр со значением по умолчанию
// print() / print(false) 	    - обычный вывод
// print(REVERSE) / print(true) - вывод элементов списка в обратном порядке
void List::print (bool rev = false) const {
	if (empty()) {
		cout << "List is empty" << endl;
		return;
	}
	
	Node *q;;
	if (!rev) {
		cout << "List:  ";
		q = head;
		while (q) {
			cout << q->elem << "   ";
			q = q->next;
		}
	}	
	else {
		cout << "List (in reverse order):  ";
		q = tail;
		while (q) {
			cout << q->elem << "   ";
			q = q->prev;
		}
	}	
	cout << endl;
}

int main () {
	List L;
	int k;

// создание списка:
// демонстрация работы функций push_back(), push_front(), print(), а также size();
// числа записываются по порядку в список, а нуль добавляется в начало списка
// если вводится только нуль, то список остается пустым (для демонстрации
// работы с пустым списком)	
	cout << "Enter list elements (numbers, zero at the end):" << endl;
	for (cin >> k; k; cin >> k)
		L.push_back(k);
	if (L.size())
		L.push_front(k);
	L.print();
	if (L.size())
		L.print(REVERSE);

// демонстрация работы функций front() и back():
	cout << "First and last list elements:" << endl;
	cout << L.front() << '\t';
	cout << L.back() << endl;

// демонстрация работы функций pop_back() и pop_front():
	cout << "Deleting first and last elements:" << endl;
	L.pop_back();
	L.pop_front();
	L.print();

// демонстрация работы функций insert() и erase():
// введите номер элемента (num), перед которым необходимо добавить
// новый элемент, а замем и сам элемент	(elem)
// далее удаляется элемент, перед которым добавился новый (при "хорошем" num)
	int num, elem;
	cout << "Enter an element number before which you want to add a new element:  ";
	cin >> num;
	cout << "Enter a new element:  ";
	cin >> elem;
	L.insert(num++, elem);
	L.print();
// при "плохом"	num или пустого списка вывод не очень красивый, но смысл все равно ясен
	cout << "Deleting " << num << "th element" << endl;
	L.erase(num);
	L.print();

// демонстрация работы функции empty():	
	if (!L.empty())
		cout << "Size of List:  "<< L.size() << endl;

// демонстрация конструкторов умолчания и копирования
	cout << "Some new lists:" << endl;
	List X = List(L), Z;
	cout << "1 ";
	X.print();
	cout << "2 ";
	Z.print();

//демонстрация операции присваивания
	Z = X;
	cout << "2 ";
	Z.print(); 
	return 0;
}