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

		scattered = ray(rec.p, scatter_dir);				    // Генерация рассеивающего луча
		attenuation = albedo;								    // Доля отраженного цвета от поверхности (затухание
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

/*
 * Если материал применяется к сфере: для сферы с показателм преломления материала выше,
 * чем у окружающей среды, то не существует такого угла падения, при котором произошло
 * бы полное внутреннее отражение -- ни в точке входа луча в сферу, ни в точке выхода
 * луча из сферы. Это связано с геометрией сферы: падающий под скользящим углом луч 
 * всегда будет отклоняется на меньший угол, а зтем возвращаться к исходному углу при
 * выходе. Чтобы продемонстрировать работу диэлектрического материала, можно выполнить
 * полное внешнее отражение, где показатель преломления материала будет меньше,чем
 * показатель преломления окружающей среды.
*/
class dielectric : public material
{
private: 
	double refraction_index; // Отношение показателя преломления материала к показателю 
							 // преломдения окружающей среды.

	// Аппроксимация зеркального отражения по Шлику: вычисляет приближенное
	// значение фактора Френеля, который определяет долю зеркально отраженного
	// света. Чем больше угол падения, тем больше значения фактора Френеля.
	static double reflectance(double cosine, double refraction_index)
	{
		double r0 = (1 - refraction_index) / (1 + refraction_index);
		
		r0 *= r0;
		return r0 + (1-r0)*std::pow((1-cosine),5);
	}
public:
	dielectric(double refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec,
		color& attenuation, ray& scattered) const override
	{
		attenuation = color(1.0, 1.0, 1.0); // Затухание равно 1, т.к. стеклянная поверхность ничего не поглащает.

		double ri = rec.front_face ? (1.0/refraction_index) : refraction_index; // Определение направления падающего луча.

																				// Если луч пересекат поверхность со стороны среды eta,
																				// то вычисляется отношение eta/eta_prime, т.к. мы переходим
																				// из одной среды (eta) в более плотную среду (eta_prime). 
																		 
																				// В противном случае, переход происходит в среду c меньшим 
																				// показателм плотности (1.0).
																			    
																				// 19.07 -- С днем рождения Комбербетч.
																				
		vec3 unit_direction = unitv(r_in.direction());
		double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0); 
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
		
		bool cannot_refract = ri * sin_theta > 1.0; // sin(theta_prime)
		
		vec3 direction;

		// Если угол sin(theta_prime) больше критического угла, то необходимо 
		// выполнить полоное внутрнее (в случае сферы внешнее) отражение.
		if (cannot_refract || reflectance(cos_theta, ri) > random_double()) 
			direction = reflect(unit_direction, rec.normal);
		else { direction = refract(unit_direction, rec.normal, ri); }

		scattered = ray(rec.p, direction);										// Генерация преломленного или отраженного луча.
		return true;
	}
};

#endif