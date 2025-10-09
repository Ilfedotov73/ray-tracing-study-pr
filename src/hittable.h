#ifndef HITTABLE_H
#define HITTABLE_H

class material;

class hit_record {
public:
	point3 p;
	vec3 normal;
	shared_ptr<material> mat; // указатель на реализацию абстрактного материала, для 
							  // вызова его функции (scatter()). Так ray_color() сможет
							  // определить, к какому материалу принадлежит поверхность
							  // и как от нее рассеивать свет.
	double t;
	bool front_face;

	/*
	 * Функция set_face_normal вычисляет направление нормали методом: нормаль всегда
	 * направлена против направления луча. Скалярное произведение dot(r.direction(),
	 * outward_normal) определяет положение луча относительно пересеченного объекта.
	 * 
	 * Если dot() возвращает значение < 0, тогда луч пересекает поверхность снаружи
	 * в таком случае front_face = true -> нормаль имеет свое "обычное" направление
	 * 
	 * Если dot() возвращает значение > 0, тогда луч пересекает поверхность изнутри,
	 * в таком случае front_face = false -> нормаль необходимо инвертировать (т.е.
	 * умножить на (-1)).
	 *
	 * > В противном случае нормаль будет иметь одно направление с лучом, что пред-
	 * ставляет собой другой метод определения нормали.
	*/
	void set_face_normal(const ray& r, const vec3& outward_normal)
	{
		// Предполагается что outward_normal имеет единичную длину.
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	virtual ~hittable() = default;
	virtual bool hit(const ray& r, interval ray_t,hit_record& rec) const = 0;
	virtual aabb bounding_box() const = 0;
};

#endif