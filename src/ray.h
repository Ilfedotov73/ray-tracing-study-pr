/***********************************************************************************
* Класс ray основан на идеи функции линейной интерполяции: P(t) = A + tb, где
* > P - это позиция (точка) вдоль линии в трех-мерном пространстве;
* > A - это источник испускания луча;
* > b - направление луча;
* > t - параметр функции, действительное число. Изменяя значение t, можно пере-
* мещать точку вдоль линии на интервале [-inf, +inf].
* Функция at(double t) в ray, рассчитывает значение P(t).
* 
* Поведение/свойства объекта типа ray():
* В классе ray определены два приватных поля: point3 orig и vec3 dir
* > т.е. начало луча и его направление.
* Инициализация объекта типа ray происходит через конструктор ray(), который
* в этом случае orig, dir = (0,0,0), перегрузка ray(const point3& origin, 
*													cont vec3& direction) {} 
* инициализирует ray с заданными значениями orig и dir через списки инициализации.
* Также для объекта типа ray определены функции получения константных ссылок на 
* поля orig и dir.
***********************************************************************************/

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray
{
private:
	point3 orig; //(0,0,0) 
	vec3 dir;    //(0,0,0) 
	double tm;
public:
	ray() {}
	ray(const point3& origin, const vec3& direction, double time) : orig(origin), dir(direction), tm(time) {}
	ray(const point3& origin, const vec3& direction) : ray(origin, direction, 0) {}

	const point3& origin() const { return orig; }
	const vec3& direction() const { return dir; }
	double time() const { return tm; }

	point3 at(double t) const { return orig + t*dir;}
};
#endif