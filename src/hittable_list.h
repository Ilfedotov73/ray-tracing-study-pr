#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>
#include "hittable.h"

class hittable_list : public hittable {
public:
	std::vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void add(shared_ptr<hittable> object) { objects.push_back(object); }
	void clear() { objects.clear(); }

	/*
	 * ������� hit() ���������� ��������� �� ������� ���� hittable ������� objects,
	 * ��� object ���� ��������� �� ����������� ������ ������� ���� hittable. �����
	 * ����� �������� ��������� ����� ��������� � �������, ���������� ��� ������� 
	 * hit � ������� ����������� ���, �������� �������� ��� ��������� t � ���������
	 * ������ � �����������.
	 * 
	 * ���� ��� ���������� ������, �� ���������� ��� hit_anything = true (�� ���������)
	 * � ����������� �������� t � closest_so_far ��� ������� t ���������� �����������
	 * ���� � ������������. ����� ��������� ��������� rec ���� hit_record& ���������-
	 * ���� ������ � ����������� ���� � �������� - rec_temp � ������������ ���� � ����-
	 * ������� ������� hit_anything = true;
	*/
	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool hit_anything = false;
		double closest_so_far = ray_tmax;

		for (const auto& object : objects) {
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