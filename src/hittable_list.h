#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>
#include <memory>
#include "hittable.h"

using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable {
public:
	std::vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void add(shared_ptr<hittable> object) { objects.push_back(object); }
	void clear() { objects.clear(); }

	/*
	 * Функция hit() перебирает указатели на объекты типа hittable вектора objects,
	 * где object есть указатель на константную ссылку объекта типа hittable. Далее
	 * через косвенно обращение через указатель к объекту, вызывается его функция 
	 * hit в которуб передеается луч, диапазон значение для параметра t и временная
	 * запись о пересечении.
	 * 
	 * Как луч пересекает объект, он помечается как hit_anything = true (он пересечен)
	 * и сохраняется параметр t в cloaest_so_far как парметр t ближайшего пересечения
	 * луча с поверхностью. Далее выходному параметру rec типа hit_record& присваива-
	 * ется запись о пересечении луча с объектом - rec_temp и возвращается флаг о пере-
	 * сечении объекта hit_anything = true;
	*/
	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const ovveride
	{
		hit_record temp_rec;
		bool hit_anything = false;
		double closest_so_far = ray_tmax;

		for (const *hittable& object : objects) {
			if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}
		return hit_anything;
	}
};

#endif