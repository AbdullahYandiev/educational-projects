// Яндиев А, 211 группа, ДЗ-4 (2 версия)
// в методах ввода вектора и матрицы сделал нулевыми указатели в случае нулевого размера

#include <iostream>
#include <cmath>
using namespace std;

//------------------Абстрактный класс------------------
//----------------Линейное пространство----------------
class Linear_space {
public:
	virtual ~Linear_space() {};
	virtual double norm () const = 0;			// ||A|| = ...
	virtual void multiply (double alpha) = 0;	// A = alpha * A
	virtual void read () = 0;					// чтение объекта с клавиатуры
	virtual void write () const = 0;			// вывод объекта на экран
};

//------------------Производные классы-----------------
//------------------------Число------------------------
class Number: public Linear_space {
	double x;
public:
	Number (double a = 0) : x(a) {};
	Number (const Number &N) : x(N.x) {};
	Number & operator= (const Number &N) {x = N.x; return *this;};
	~Number() {};
	double norm () const;
	void multiply (double alpha);
	void read ();
	void write () const;
};

double Number::norm () const {
	return x >= 0 ? x : -x;
}

void Number::multiply (double alpha) {
	x *= alpha;
}

void Number::read () {
	cout << "Number:  ";
	cin >> x;
}

void Number::write () const {
	cout << "Number:  " << x << endl;
}

//-----------------------Вектор-----------------------
class Vector: public Linear_space {
	double *x;
	int size;
public:
	Vector () : size(0), x(NULL) {};
	Vector (const Vector &V);
	Vector & operator= (const Vector &V);
	~Vector();
	double norm () const;
	void multiply (double alpha);
	void read ();
	void write () const;
};

Vector::Vector (const Vector &V) {
	size = V.size;
	x = new double[size];
	for (int i = 0; i < size; ++i)
		x[i] = V.x[i];
}

Vector & Vector::operator= (const Vector &V){
	if (this != &V) {
		delete[] x;
		size = V.size;
		x = new double[size];
		for (int i = 0; i < size; ++i)
			x[i] = V.x[i];
	}
	return *this;
}

Vector::~Vector() {
	delete[] x;
}

// если вектор пустой, то возвращаем -1
double Vector::norm () const {
	if (!size)
		return -1;
	double tmp = 0;
	for (int i = 0; i < size; ++i)
		tmp += x[i] * x[i];
	return sqrt(tmp);
}

void Vector::multiply (double alpha) {
	for (int i = 0; i < size; ++i)
		x[i] *= alpha; 
}

void Vector::read () {
	cout << "Vector size:  ";
	cin >> size;
	delete[] x;
	if (size < 0) {
		cerr << "Invalid vector size" << endl;
		exit(1);
	}
	else if (size) {
		x = new double[size];
		cout << "Vector:  ";
		for (int i = 0; i < size; ++i)
			cin >> x[i];
	}
	else
		x = NULL;
}

void Vector::write () const {
	cout << "Vector: ";
	if (!size)
		cout << "EMPTY" << endl;
	else {
		for (int i = 0; i < size; ++i)
			cout << x[i] << ' ';
		cout << endl;
	}	
}

//-----------------------Матрица-----------------------
class Matrix: public Linear_space {
	double **x;
	int m, n;     // размер (m строк, n столбцов)
public:
	Matrix () : m(0), n(0), x(NULL) {};
	Matrix (const Matrix &A);
	Matrix & operator= (const Matrix &A);
	~Matrix();
	double norm () const;
	void multiply (double alpha);
	void read ();
	void write () const;
};

Matrix::Matrix (const Matrix &A) {
	int i, j;
	m = A.m;
	n = A.n;
	x = new double*[m];
	for (i = 0; i < m; ++i) {
		x[i] = new double[n];
		for (j = 0; j < n; ++j)
			x[i][j] = A.x[i][j];
	}	
}

Matrix & Matrix::operator= (const Matrix &A) {
	int i, j;
	if (this != &A) {
		for (i = 0; i < m; ++i)
			delete[] x[i];
		delete[] x;
		m = A.m;
		n = A.n;
		x = new double*[m];
		for (i = 0; i < m; ++i) {
			x[i] = new double[n];
			for (j = 0; j < n; ++j)
				x[i][j] = A.x[i][j];
		}
	}
	return *this;
}

Matrix::~Matrix() {
	for (int i = 0; i < m; ++i)
		delete[] x[i];
	delete[] x;
}

// если матрица пустая, то возвращаем -1
double Matrix::norm () const {
	if (!m) // аналогично if (!n) {...}, т.к., если хотя бы один размер 0, то матрица пустая
		return -1;
	double tmp = 0;
	for (int i = 0, j; i < m; ++i)
		for (j = 0; j < n; ++j)
			tmp += x[i][j] * x[i][j];
	return sqrt(tmp);
}

void Matrix::multiply (double alpha) {
	int i, j;
	for (i = 0; i < m; ++i)
		for (j = 0; j < n; ++j)
			x[i][j] *= alpha; 
}

void Matrix::read () {
	int j;
	for (j = 0; j < m; ++j)
		delete[] x[j];
	delete[] x;
	cout << "Matrix size (m, n - separated by space):  ";
	cin >> m >> n;
	if (m < 0 || n < 0) {
		cerr << "Invalid matrix size" << endl;
		exit(1);
	}
	else if (!m || !n) {
		m = n = 0;	// чтобы потом не было противоречий, что один из размеров ненулевой
		x = NULL;
		return;
	}
	x = new double*[m];
	cout << "Matrix:" << endl;
	for (int i = 0; i < m; ++i) {
		x[i] = new double[n];
		for (j = 0; j < n; ++j)
			cin >> x[i][j];
	}
}

void Matrix::write () const {
	int i, j;
	cout << "Matrix:";
	if (!m)
		cout << " EMPTY" << endl;
	else {
		cout << endl;
		for (i = 0; i < m; ++i) {
			for (j = 0; j < n; ++j)
				cout << x[i][j] << '\t';
			cout << endl;
		}
	}	
}


//------------------Независимый класс------------------
class Work {
	Linear_space **x;
public:
	Work ();
	Work(const Work &W);
	Work & operator= (const Work &W);
	~Work();
	void print_norm () const;
	void multiply (int alpha);
	void read ();
	void write () const;
};

Work::Work() {
	x = new Linear_space*[3];
	x[0] = new Number;
	x[1] = new Vector;
	x[2] = new Matrix;
}

Work::Work(const Work &W) {
	x = new Linear_space*[3];
	x[0] = new Number;
	x[1] = new Vector;
	x[2] = new Matrix;
	for (int i = 0; i < 3; ++i)
		x[i] = W.x[i];
}

Work & Work::operator= (const Work &W) {
	int i;
	if (this != &W) {
		for (i = 0; i < 3; ++i)
			delete x[i];
		delete[] x;
		x = new Linear_space*[3];
		x[0] = new Number;
		x[1] = new Vector;
		x[2] = new Matrix;
		for (i = 0; i < 3; ++i)
			x[i] = W.x[i];
	}
	return *this;
}

Work::~Work() {
	for (int i = 0; i < 3; ++i)
		delete x[i];
	delete[] x;
}

void Work::print_norm () const {
	cout << "Norms:" << endl;
	for (int i = 0; i < 3; ++i)
		cout << i+1 << ") " << x[i]->norm() << endl;
}

void Work::multiply (int alpha) {
	for (int i = 0; i < 3; ++i)
		x[i]->multiply(alpha);
}

void Work::read () {
	for (int i = 0; i < 3; ++i)
		x[i]->read();
}
void Work::write () const {
	for (int i = 0; i < 3; ++i)
		x[i]->write();
}

//--------------------Функция main()-------------------
int main() {
// объявляем объект независимого "рабочего" класса  и демонстрируем работу	
	Work X;

// считываем информацию в массив
	X.read();

// печатаем норму соответсвующих объектов
	X.print_norm();

// вводим альфа, на которое умножаются все объекты и, соответственно, умножаем их
	int alpha;	
	cout << "----------------------\nalpha = ";
	cin >> alpha;
	X.multiply(alpha);

// выводим получившиеся объекты
	X.write();
	return 0;
}