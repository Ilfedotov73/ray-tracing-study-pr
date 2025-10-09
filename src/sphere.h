#ifndef SPHERE_H
#define SPHERE_H

class sphere : public hittable {
private:
	ray center;
	double radius;
	shared_ptr<material> mat;
	aabb bbox;
public:
	/* Стационарная сфера */
	sphere(const point3& static_center, double radius, shared_ptr<material> mat) 
		: center(static_center, vec3(0,0,0)), radius(fmax(0, radius)), mat(mat) 
	{
		// Ограничение стационарной сферы объемом 
		vec3 rvec = vec3(radius, radius, radius);
		bbox = aabb(static_center - rvec, static_center + rvec);
	}

	/* Движущаяся сфера */
	sphere(const point3& center_pos1, point3& center_pos2, double radius, shared_ptr<material> mat) 
		: center(center_pos1, center_pos2 - center_pos1), radius(fmax(0, radius)), mat(mat) 
	{
		vec3 rvec = vec3(radius, radius, radius);
		aabb box1(center.at(0) - rvec, center.at(0) + rvec);
		aabb box2(center.at(1) - rvec, center.at(1) + rvec);

		/**
	     * Объединение ограничивающих объемов момента времени time = 0 и ограничивающего 
		 * объема в момент временит time = 1.
		 */
		bbox = aabb(box1, box2); 
	}

	/**
	 * \brief Функция bounding_box() возвращает текущий ограничивающей объем сферы.
	 */
	aabb bounding_box() const override { return bbox; }

	/* hit() решает уравнение x^2 + y^2 + z^2 = r^2 как квадратное уравнение с
	 * использование формулы нахождения дискриминанта. Уравнение x^2 + y^2 + z^2 = r^2
	 * представимо в виде (C - P(t))(C - P(t)) = r^2, где P(t) некоторая точка, заданная
	 * уравнением a + tb. Записав уравнение в виде (C - a - tb)(C - a - tb) = r^2 и взяв
	 * С - а как временную переменную OC, уравнене можно решить как квадратное уравне-
	 * ние, раскрыв скобки и записав уравнение как t^2b^2 + 2tbOC - OC^2 - r^2 = 0, где
	 * a = t^2b^2, b = 2tbOC, c = OC^2 - r^2. Решив это уравнение через формулу дискре-
	 * минта, можно найти такие точки t, при который будет найдена точка P(t) на по-
	 * верхности сферы и нормаль N к P(t). 
	 *
	 * Если решения уравнения существуют Функция записывает в параметр rec типа
	 * hit_record&, значения P(t), N и параметр t, после чего возвращает true. В 
	 * проивном случае функция возвращет знчение false;
	 *
	 * Math: пусть есть некоторая точка P(t) на поверхности сферы, тогда можно сказать
	 * что a + tb = c + rp, где с - вектор к центру сферы (точке С), а rp - вектор от 
	 * центра сферы к точке P(t) (нормаль). a + tb = c + rp -> P(t) - c = rp -> c - 
	 * - P(t) = - rp. При возведениее правой и левой части уравнения в квадрат получа-
	 * ется (c - P(t))(c - P(t)) = (-rp^2) = r^2, что является уравнением выше.
	 * 
	 * Вычисление нормали: Теперь когда известна точка P(t) и вектор к этой точке P,
	 * можно вычилсть нормаль к этой точке - вектор N (раннее это был вектора rp, 
	 * квадрат длины которого равен r^2). Если a + tb = N + c -> a + tb - c = N, 
	 * то N = P(t) - c, где c - вектор к точке C (центр сферы).
	 * 
	 * Далее нормаль приводится к единичному вектору через свойство сферы: известно
	 * что длина нормали равна радиусу сферы, что позволяет не использовать вызов 
	 * функции vec.unitv()
	 * 
	 * Значения нормали находятся в диапазоне [-1,1], используя линейной масштабиро-
	 * ване можно преобразовать значения в диапазон [0,1]. Что соответствует соглаше-
	 * нию о цвете и позволяет использовать "затенение нормалями".
	 * 
	 * Специфика сферы позволяет представить компоненты a,b,c для нахождения корней в
	 * упрощенной форме (компоненты a,h,c):
	 * 
	 * > Во-первых, скалярное произведение вектора на себя равно квадрату длины этого
	 * вектора, что позволяет упростить вычисление компонента - a.
	 * 
	 * > Во-вторых, если b = -2d*oc, тогда можно представить точку h = b/-2 = d*oc, 
	 * что позволяет изабивьться от двух операций: -2.0*dot() дли b и 2.0*a как 
	 * конечная операция перед возвратом значения:
	 * 
	 * (-b +- sqrt(b^2-4ac))/2a
	 * (-(-2h) +- sqrt((-2h)^2 - 4ac))/2a = (2h +- sqrt(h^2 - ac))/2a =
	 * = (h +- sqrt(h^2 - ac))/a
	*/
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		point3 current_center = center.at(r.time());
		vec3 oc = current_center - r.origin();
		double a = r.direction().length_squared();
		double h = dot(r.direction(), oc);
		double c = oc.length_squared() - radius*radius;

		double discriminant = h*h - a*c;
		if (discriminant < 0) { return false; }
		
		double sqrtd = std::sqrt(discriminant);

		// Поиск ближайшего корня в допустимом диапазоне
		double root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) { 
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root)) { return false; }
		}

		rec.p = r.at(root);
		vec3 outward_normal = (rec.p - current_center) / radius; // Некоторе упрощения для объекта сферы, т.к. известно что длина 
																 //	нормали равна радиусу сферы. Что позволяет упростить вычисление
																 // единичного вектора нормали, как того требует функция set_face_normal().
		rec.set_face_normal(r, outward_normal);
		rec.t = root;
		rec.mat = mat;
		
		return true;
	}
};
#endif