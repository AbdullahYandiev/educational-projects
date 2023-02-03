// Яндиев А, 211 группа, ДЗ-5

#include <iostream>
#include <cmath>
using namespace std;

#define ABS(x) x >= 0 ? x : -x

class Matrix {
	double **x;
	int m, n;		// размер (m строк, n столбцов)
public:
// ---------------------индексация---------------------
// выход за границы матрицы отдельно не рассматривается,
// так как все равно будет выдана ошибка сегментации
	double * operator[] (int i) {return x[i];}
	const double * operator[] (int i) const {return x[i];}
//-----------------------------------------------------					
	Matrix () : m(0), n(0), x(NULL) {};
	Matrix (int sm, int sn, double a);
	Matrix (const Matrix &A);
	Matrix & operator= (const Matrix &A);
	~Matrix();
	double norm () const;
	friend istream & operator>> (istream &s, Matrix &X);
	friend ostream & operator<< (ostream &s, const Matrix &X);
	Matrix operator! () const;							// транспонирование матрицы
	Matrix operator+ (const Matrix &A) const;			// сложение матриц
	Matrix operator- (const Matrix &A) const;			// вычитание матриц
	Matrix operator* (double alpha) const;				// умножение матрицы на число
	Matrix operator* (const Matrix &A) const;			// умножение матриц
	Matrix & operator+= (const Matrix &A);
	Matrix & operator-= (const Matrix &A);
	double det() const;									// определитель матрицы
};

// создается матрица размером sm x sn, инициализируется значением а
Matrix::Matrix (int sm, int sn, double a = 0) : m(sm), n(sn) {
	if (!m || !n) {
		m = n = 0;
		x = NULL;
		return;
	}
	int i, j;
	x = new double*[m];
	for (i = 0; i < m; ++i) {
		x[i] = new double[n];
		for (j = 0; j < n; ++j)
			x[i][j] = a;
	}
}

Matrix::Matrix (const Matrix &A) : m(A.m), n(A.n) {
	if (!m) {
		x = NULL;
		return;
	}
	int i, j;
	x = new double*[m];
	for (i = 0; i < m; ++i) {
		x[i] = new double[n];
		for (j = 0; j < n; ++j)
			x[i][j] = A[i][j];
	}
}

Matrix & Matrix::operator= (const Matrix &A) {
	if (!m) {
		x = NULL;
		return *this;
	}
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
				x[i][j] = A[i][j];
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
	if (!m)
		return -1;
	double tmp = 0;
	for (int i = 0, j; i < m; ++i)
		for (j = 0; j < n; ++j)
			tmp += x[i][j] * x[i][j];
	return sqrt(tmp);
}

istream & operator>> (istream &s, Matrix &X) {
	int j;
	for (j = 0; j < X.m; ++j)
		delete[] X[j];
	delete[] X.x;	
	cout << "Matrix size (m, n - separated by space):  ";
	s >> X.m >> X.n;
	if (X.m < 0 || X.n < 0) {
		cerr << "Invalid matrix size" << endl;
		exit(1);
	}
	else if (!X.m || !X.n) {
		X.m = X.n = 0;	// чтобы потом не было противоречий, что один из размеров ненулевой
		X.x = NULL;
		return s;
	}
	X.x = new double*[X.m];
	cout << "Matrix:" << endl;
	for (int i = 0; i < X.m; ++i) {
		X.x[i] = new double[X.n];
		for (j = 0; j < X.n; ++j)
			s >> X[i][j];
	}
	return s;
}

ostream & operator<< (ostream &s, const Matrix &X) {
	int i, j;
	if (!X.m) {
		s << " EMPTY";
		return s;
	}
	for (i = 0; i < X.m; ++i) {
		for (j = 0; j < X.n; ++j)
			s << X[i][j] << '\t';
		if (i != X.m - 1)
			s << endl;
	}
	return s;
}

Matrix Matrix::operator! () const {
	int i, j;
	Matrix T(n, m);
	for (i = 0; i < T.m; ++i)
		for (j = 0; j < T.n; ++j)
			T[i][j] = x[j][i];
	return T;
}

Matrix Matrix::operator+ (const Matrix &A) const {
	if (m != A.m || n != A.n) {
		cerr << "Invalid matrix size for addition" << endl;
		exit(1);
	}
	int i, j;
	Matrix C(m, n); 	// C = 0, то есть массив инициализирован нулями 
	for (i = 0; i < m; ++i)
		for (j = 0; j < n; ++j)
			C[i][j] = x[i][j] + A[i][j];
	return C;
}

Matrix Matrix::operator- (const Matrix &A) const {
	if (m != A.m || n != A.n) {
		cerr << "Invalid matrix size for addition" << endl;
		exit(1);
	}
	int i, j;
	Matrix C(m, n); 	// C = 0, то есть массив инициализирован нулями 
	for (i = 0; i < m; ++i)
		for (j = 0; j < n; ++j)
			C[i][j] = x[i][j] - A[i][j];
	return C;
}


Matrix Matrix::operator* (double alpha) const {
	int i, j;
	Matrix C(*this);
	for (i = 0; i < m; ++i)
		for (j = 0; j < n; ++j)
			C[i][j] *= alpha;
	return C;
}

Matrix Matrix::operator* (const Matrix &A) const {
	if (n != A.m) {
		cerr << "Invalid matrix size for multiplication" << endl;
		exit(1);
	}
	int i, j, k;
	Matrix C(m, A.n); 	// C = 0, то есть массив инициализирован нулями 
	for (i = 0; i < C.m; ++i)
		for (j = 0; j < C.n; ++j)
			for (k = 0; k < n; ++k)
				C[i][j] += x[i][k] * A[k][j];
	return C;
}

Matrix & Matrix::operator+= (const Matrix &A) {
	*this = *this + A;
 	return *this;
}

Matrix & Matrix::operator-= (const Matrix &A) {
	*this = *this - A;
	return *this;
}

// метод Гаусса с выделением главного элемента
double Matrix::det() const {
	if (m != n) {
		cerr << "Invalid matrix size for calculating determinant" << endl;
		exit(1);
	}

	int i, j, k, max_pos;
	double max, tmp, det = 1;
	Matrix A(*this);

	for (k = 0; k < m; ++k) {
// ищем наибольший по модулю элемент k-ой строки 	
		for (max = 0, j = k; j < n; ++j)
			if (abs(max) < abs(A[k][j])) {
				max = A[k][j];
				max_pos = j;
			}

// встретилась нулевая строка -> определитель равен нулю
		if (!max)
			return 0;

// если max находится не в k-ом столбце, то изменяем знак определителя
// и меняем местами столбцы: k-ый и тот, в котором содержится max
		if (max_pos != k) {
			det *= -1;
			for (i = k; i < m; ++i) {
				tmp = A[i][max_pos];
				A[i][max_pos] = A[i][k];
				A[i][k] = tmp;
			}
		}

// делим все элементы k-ой строки на максимальный, а определитель умножаем на это число
		det *= A[k][k];
		for (j = n - 1; j >= k; --j)
			A[k][j] /= A[k][k];

// вычитаем из оставшихся строк k-ую, умноженную на k-ый элемент соответствующей строки
		for (i = m - 1; i > k; --i)
			for (j = n - 1; j >= k; --j)
				A[i][j] -= A[k][j] * A[i][k];
	}
	return det;
}

int main() {
// демонстрируется лишь часть функций, так как многие из них подобны друг другу
	Matrix A, B;

// на произвольной матрице показывается работа функций транспонирования, вычисления нормы и вычитания матриц
	cout << "Enter matrix A:" << endl;
	cin >> A;
	cout << "\nA^T =\n" << !A << endl << "\n||A|| = " << A.norm() << endl;
	A -= A;
	cout << "\nA - A =\n" << A << endl;

// на квадратной матрице показывается работа функций поиска определителя, сложения и умножения матриц
	cout << "\nEnter square matrix B (n x n):" << endl;
	cin >> B;
	cout << "\ndet(B) = " << B.det() << endl << "\n2B =\n" << B + B << endl << "\nB^2 =\n" << B * B << endl;

// отдельная демонстрация работы двойной индексации (в программе она используется неоднократно)
	Matrix C(3, 3, 1);
	cout << "\nC =\n" << C << endl;
	C[1][1] = 9;
	cout << "\nC[2][2] = 9 ->\nC =\n" << C << endl;

/* ошибочная ситуация, ибо вызовется константная операция [], которая вернет
   указатель на CONST double, что не позволит, в итоге, произвести присваивание */
//	const Matrix D;
//	D[1][1] = 9;
	return 0;
}