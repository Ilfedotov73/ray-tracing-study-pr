#ifndef BVH_H
#define BVH_H

class bvh_node : public hittable
{
private:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb bbox;

	/* Операторы сравнения ограничивающих объемов */

	/**
	 * \brief Функция box_compare() сравнивает ограничивающие объемы по заданной оси axis_index и 
	 * возвращает значение true, если первый ограничивающий объем меньше второго.  
	 */
	static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index)
	{
		interval a_axis_interval = a->bounding_box().axis_interval(axis_index);
		interval b_axis_interval = b->bounding_box().axis_interval(axis_index);
		return a_axis_interval.min < b_axis_interval.min;
	}
	static bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 0); }
	static bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 1); }
	static bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 2); }

public:
	/**
	 * \brief Конструктор создает неявную копию списка hittable_list, которая в последующем
	 * будет изменяться. Срок службы скопированного спика длиться до заврешения работы конструк-
	 * тора, однако, это нормально, т.к. требуется сохранить исключительно результирующую иерархию.
	 */
	bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}
	bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) 
	{
		int axis = random_int(0,2);
		bool (*comparator)(const shared_ptr<hittable> a, const shared_ptr<hittable> b) 
			= (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;
		
		// Кол-во объектов в ограничивающей области 
		size_t object_span = end - start; 

		if (object_span == 1) { left = right = objects[start]; }
		else if (object_span == 2) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			/*
			 * Примитивы ограничивающего объема сортируются по минимальным заданным плоскостям,
			 * выбирается середина и происходит разделение исходного ограничивающего объема на 
			 * дочерние узлы.
			 */
			std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);
			size_t mid = start + object_span/2;
			left  = make_shared<bvh_node>(objects, start, mid);
			right = make_shared<bvh_node>(objects, mid, end);
		}
		bbox = aabb(left->bounding_box(), right->bounding_box());
	}

	/**
	 * \brief Функция hit() пытается пересечь луч с ограничивающим объемом. Если луч 
	 * попдает в узел (внешнюю ограничивающую область), то необходимо проверить дочерние
	 * узлы.
	 */
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		if (!bbox.hit(r, ray_t)) { return false; }
		bool hit_left = left->hit(r, ray_t, rec);
		bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);
		return hit_left || hit_right;
	}

	aabb bounding_box() const override { return bbox; }
};

#endif