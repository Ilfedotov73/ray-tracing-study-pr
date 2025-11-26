#ifndef AABB_H
#define AABB_H

 /**
  * @brief	Класс aabb определяет ограничивающий объем примитивов. Задание ограничивающего объема 
  * 		поддерживает три подхода: 
  * 			- задание интервалов для каждой из оси: x,y,z;
  * 			- задание через две экстремумы ограничивающего объемва;
  * 			- задание через объединение ограничивающиъ объемов.
  */
class aabb
{
private:
	/**
	 * @brief	Объект -- четырехугольник является плоским примитивом, поэтому их ограничивающая рамка,
	 *			выровненная по осям, будет иметь нулевую толщину по одной из осей. Что может привести к 
	 *			проблемам при создании ограничивающих объемов. Поэтому, ограничивабщая рамка четырехугольника
	 *			должна иметь почти нулевой размер по осям, в плоскости которых находится четырехугольник.  					 		 	
	 */
	void pad_to_minimus() {
		float delta = 0.0001f;
		if (x.size() < delta) { x = x.expand(delta); }
		if (y.size() < delta) { y = y.expand(delta); }
		if (z.size() < delta) { z = z.expand(delta); }
	}
public:
	interval x,y,z;
	aabb() {}
	aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) { pad_to_minimus(); }

	/**
	* @brief		Конструктор создания объекта ограничивающего объема.
	*
	*				Точки a и b рассматриваются как экстремумы для ограничивающего объема, что позволяет 
	* 				задать порядок координат (от минимального к максимальному) для ограничивающих плоскостей 
	* 				x,y,z.
	*
	* @param[in] a	точкам минимального (или максимального) экстремума функции. 
	* @param[in] b	точкам минимального (или максимального) экстремума функции. 
	*/
	aabb(const point3& a, const point3& b)
	{
		x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);

		pad_to_minimus();
	}

	// Объединение ограничивающих объемов. 
	aabb(const aabb& box0, const aabb& box1)
	{
		x = interval(box0.x, box1.x);
		y = interval(box0.y, box1.y);
		z = interval(box0.z, box1.z);
	}

	// Получение интервала плоскости. 
	const interval& axis_interval(int n) const
	{
		if (n == 1) { return y; }
		if (n == 2) { return z; }
		return x;
	}
	bool hit(const ray& r, interval ray_t) const
	{
		const point3& ray_orig = r.origin();
		const vec3&   ray_dir  = r.direction();

		for (int axis = 0; axis < 3; ++axis) {
			const interval& ax = axis_interval(axis);

			// Направление луча в измерении плоскости.
			const float adinv = 1.0f / ray_dir[axis]; 

			/**
			 * @brief	На примире вычисления точки пересечения ограничивающей плоскости x0:
			 * 			t0 = (x0-Qx)/dx, где Qx - начальная точка луча, dx - направление луча.
			 */
			float t0 = (ax.min - ray_orig[axis]) * adinv;
			float t1 = (ax.max - ray_orig[axis]) * adinv;

			/**
			 * @brief	Если луч направлен в обратном направлении, то точка t1 будет меньше,
			 * 			чем точка t2, потому значения необхоидмо инвертировать.
			 */
			if (t0 < t1) {
				/**
				 * @brief	После прохода по всем ограничивающем плоскостям, будут найдены
				 * 			ближайшие к ограничивающему объему (входящие в ограничивающий объем)
				 * 			точки t0 и t1, которые определят интервал (tmin,tmax) луча внутри 
				 * 			ограничивающего объема.
				 */
				if (t0 > ray_t.min) { ray_t.min = t0; }
				if (t1 < ray_t.max) { ray_t.max = t1; }
			}
			else {
				if (t1 > ray_t.min) { ray_t.min = t1; }
				if (t0 < ray_t.max) { ray_t.max = t0; }
			}

			if (ray_t.max <= ray_t.min) { return false; }
			return true;
		}
	}

	/**
	 * @brief	Функция longest_axis возвращает индекс наибольшей плоскости 
	 * 			ограничивающего объема.
	 */
	int longest_axis() const
	{
		if (x.size() > y.size()) { return x.size() > z.size() ? 0 : 2; }
		else return y.size() > z.size() ? 1 : 2;
	}

	static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

#endif