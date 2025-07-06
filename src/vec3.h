/***********************************************************************************
* Заголовочный файл vec3.h определяет класс vec3 и минимальный набор математики
* для работы с векторами в трехмерном пространстве.
* 
* Класс vec3 описывает объект - трехмерный вектор, который  служит для хранения 
* и передачи информации о цвете пикселя, местоположений и направлений объектов в
* пространстве, и т.д.
*
* Типы color и point3 являются псевдонимами для vec3.
* 
* Объект типа vec3 может быть инициализирован в любой точке пространства или в 
* точке начала системы координат.
* 
* Определены функции получения значений компонентов объекта: x() ,y(), z().
* 
* Переопределены операторы: - (скалярное умножение на -1), [] (получение значения/
* ссылки на компонент по индексу), +=,*=,/= (стандартные операторы присваения, 
* возвращающие ссылку на левый операнд - текущее(измененное) состояние объекта 
* vec3).
* 
* Определены функции расчета величины (длины) векторов - "Т.П.": функция 
* length(), функция квадрта длины: length_squared().
* 
* Дополнительно заголовочный файл содержит переопределение операторов: << (опера-
* тор вставки, возвращает ссылку на выходной поток - левый операнд), + (сложение 
* векторов), - (разница векторов), * (как умножение компонентов векторв и как ска-
* ляроное умножение для операций: скалярного умножение/скалярного деление).
* 
* Определены функции: скалярное умножение в декартовой с.к.: dot(), 
* перекрестное умножение: cross(). 
* 
* Опредлена функция нахождения единиченого вектора: unitv().
* 
* Определный функции: random_unit_vector() и random_on_hemisphere() для вычисления
* отрежений лучей от диффузной поверхности с помощью метода отбрасывания и
* ламбертова распределения, для генерации отраженных лучей внутри области 
* единичной сферы.
* 
* > Метод отбрасывания -- равномерное распределение.
* 
* Все функции определены как inline - встраиваемые функции.
***********************************************************************************/

#ifndef VEC3_H
#define VEC3_H

class vec3
{																					
public:
	double e[3];

	vec3() : e {0,0,0} {}
	vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

	double  operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(double t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator/=(double t) { return *this *= 1 / t; }
	
	/* magnitude */
	double length() const { return std::sqrt(length_squared()); }
	
	double length_squared() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }

	static vec3 random() { return vec3(random_double(), random_double(), random_double()); }
	static vec3 random(double min, double max) { return vec3(random_double(min, max), random_double(min, max), random_double(min, max)); }

	bool near_zero() const
	{
		double s = 1e-8; // 0.0000001
		return(std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) <s );
	}

};

using point3 = vec3;

/* print */
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

/* стандартные операции */
inline vec3 operator+(const vec3& u, const vec3& v) 
{
	return vec3(
		u.e[0] + v.e[0],
		u.e[1] + v.e[1],
		u.e[2] + v.e[2]
	);
}

inline vec3 operator-(const vec3& u, const vec3& v)
{
	return vec3(
		u.e[0] - v.e[0],
		u.e[1] - v.e[1],	
		u.e[2] - v.e[2]
	);
}

inline vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(
		u.e[0] * v.e[0],
		u.e[1] * v.e[1],
		u.e[2] * v.e[2]
	);
}

/* скалярное умножение */
inline vec3 operator*(double t, const vec3& v)
{
	return vec3(
		t*v.e[0],
		t*v.e[1],
		t*v.e[2]
	);
}

inline vec3 operator*(const vec3& v, double t) { return t * v; }

inline vec3 operator/(const vec3& v, double t) { return (1/t) * v; }

/* фунции произведения векторов в декартовой с.к.: dot(), cross() */
inline double dot(const vec3& u, const vec3& v)
{
	return u.e[0] * v.e[0]
		+  u.e[1] * v.e[1]
		+  u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

/* функция нахождения единичегого вектора - нормализация */
inline vec3 unitv(const vec3& v) { return v/v.length(); } 

/* Random method by reflection */
inline vec3 random_unit_vector()
{
	for (;;) {
		vec3 p = vec3::random(-1,1);                                 // Создаем случайный вектор в области единичной сфер
		double lensq = p.length_squared();
		if (1e-160 < lensq && lensq <= 1) { return p/sqrt(lensq); }  // Нормализуем до единичного вектора 
																	 // 10^-160 чтобы избежать фиктивного вектора при векторе,
																	 // компоненты которого очень близки к нулю.
	}
}

inline vec3 random_on_hemisphere(const vec3& normal)
{
	vec3 rndv_in_unit_sp = random_unit_vector();
	if (dot(rndv_in_unit_sp, normal) > 0.0) { return rndv_in_unit_sp; } // Если случайный вектор находится в правильной 
																		// полусфере, то скалярное произведение случайного 
																		// вектора на нормаль будет положительно
	else { return -rndv_in_unit_sp; }
}

inline vec3 reflect(const vec3& v, const vec3& n) { return v - 2*dot(v,n)*n; }

#endif