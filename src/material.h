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
	double fuzz;
public:
	metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz:1) {}

	bool scatter(const ray& r_in, const hit_record& rec,
		 color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(r_in.direction(), rec.normal);			// Вектор отражения луча по длине прекции луча к 
																		// нормали.
		
		reflected = unitv(reflected) + (fuzz * random_unit_vector());	// Для того, чтобы регулировать интенсивность размытия,
																		// вводится коэф. fuzz, который позволяет регулировать 
																		// размер единичной сферы (т.е. при необходимости делать
																		// ее меньше.

																		// Также необходимо нормализовать вектор отражения до 
																		// единичного вектора, чтобы проверить отраженный луч на 
																		// его направление и не допустить отражения обратно на 
																		// родную поверхность.


		scattered = ray(rec.p, reflected);								// Генерация рассеивающего луча.
		attenuation = albedo;											// Доля отраженного цвета от поверхности (затухание
																		// глобального освещения).
		
		return (dot(scattered.direction(), rec.normal) > 0);			// Нечеткое отражение должно не должно быть против 
																		// направления нормали.
	}
};

class dielectric : public material
{
private: 
	double refraction_index;
public:
	dielectric(double refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec,
		color& attenuation, ray& scattered) const override
	{
		attenuation = color(1.0, 1.0, 1.0);
		double ri = rec.front_face ? (1.0/refraction_index) : refraction_index; // Определение направления падающего луча.

																				// Если луч пересекат поверхность со стороны среды eta,
																				// то вычисляется отношение eta/eta_prime, т.к. мы переходим
																				// из одной среды (eta) в более плотную среду (eta_prime). 
																		 
																				// В противном случае, переход происходит в среду c меньшим 
																				// показателм плотности (1.0).
																			    
																				// Вчера 19.07 -- С днем рождения Комбербетч.
																				
		vec3 unit_direction = unitv(r_in.direction());							
		vec3 refracted = refract(unit_direction, rec.normal, ri);				// Вычисление вектора преломления.
		scattered = ray(rec.p, refracted);										// Генерация рассеивающего луча от точки преломления.
		return true;
	}
};

#endif