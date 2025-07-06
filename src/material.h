#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material
{
public:
	virtual ~material() = default;
	
	virtual bool scatter(const ray& r_in, const hit_record& rec,
		color& attenuation, ray& scattered) const
	{
		return false;
	}
};

class lambertian : public material
{
private:
	color albedo;
public:
	lambertian(const color& albedo) : albedo(albedo) {}

	bool scatter(const ray& r_in, const hit_record& rec,
		 color& attenuation, ray& scattered) const override
	{
		vec3 scatter_dir = (rec.normal + random_unit_vector()); // Направление рассеивания, согласно распределению
															    // ламберта.

		/* Если сгенерированное вектор рассеивания будет противоположно вектору нормали */
		if (scatter_dir.near_zero()) { scatter_dir = rec.normal; }

		scattered = ray(rec.p, scatter_dir);				  // Генерация рассеивающего луча
		attenuation = albedo;								  // Доля отраженного цвета от поверхности (затухание
															  // глобального освещения).
		return true;
	}
};

class metal : public material
{
private:
	color albedo;
public:
	metal(const color& albedo) : albedo(albedo) {}

	bool scatter(const ray& r_in, const hit_record& rec,
		 color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(r_in.direction(), rec.normal); // Вектор отражения луча по длине прекции луча к 
																// нормали.
		scattered = ray(rec.p, reflected);						// Генерация рассеивающего луча
		attenuation = albedo;									// Доля отраженного цвета от поверхности (затухание
																// глобального освещения.
		return true;
	}
};

#endif