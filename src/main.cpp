/***********************************************************************************
* Данная программа рассчитывает значения матрицы пискслей и записывает их в формате 
* P3, т.е. цвета даны в ASCII - 24 бита на пиксель (по 8 бит на r,g,b). 
* 
* Алгоритм вычисления цвета конкретного пикселя основан на методе трассировки лучей.
* Суть которого: для каждого пикселя viewport рассчитывается его проекция в трех-
* мерном пространстве, относительно точки P(0,0). После чего задается объект типа 
* ray(camera_center, ray_direction) и вызывается ray_color().
* 
* Выходной файл ((>) file.ppm) содержит значения для пикселя в количестве 256 x 256, 
* значения которых не превышают 255.
*
* Файл .ppm является простым форматом хранения цветного изображения. Также может 
* быть использован в качестве промежуточного формата при конвертации растровых 
* изображений. 
* 
* Внутренне представление, по соглашени. r,g,b компонентов должно находиться в 
* диапазоне [0,1]. Для этого единичный вектор (нормализованный), который имеют 
* компоненты в диапазоне [-1,1], адаптируются: 0.5 * (unit_direction.y() + 1.0) 
* (линейное масштабирование).
* 
* Т.е. для каждого компонента конкретного пикселя определяется степень 
  принадлежности конкретного компонента (r,g,b) к соответствующему цвету
*
* cmake --build . --config Release
***********************************************************************************/

#include "ray.h"
#include "vec3.h"
#include "color.h"
#include <iostream>

/* hit_sphere() решает уравнение x^2 + y^2 + z^2 = r^2 как квадратное уравнение с
использование формулы нахождения дискриминанта. Уравнение x^2 + y^2 + z^2 = r^2
представленое в виде (C - P(t))(C - P(t)) = r^2, где P(t) является лучом из 
P.camera_center к P.direction, P(t) = Q + td. Таким образом, решить уравнение,
значит найти t, при которых(-ой) выполняется (C-P(t))^2 = r^2, т.е. если 
discriminant >= 0.															      */
bool hit_sphere(const point3& sp_center, double radius, const ray& r)
{
	vec3 v = sp_center - r.origin(); //вектор от точки r.direction к sp_center
	double a = dot(r.direction(), r.direction());
	double b = -2.0*dot(r.direction(), v);
	double c = dot(v,v) - radius*radius;
	double discriminant = b*b - 4*a*c;
	return (discriminant>=0);
}

/* ray_color() линейно смешивает белыый и синий цвета в зависимости от высоты 
направления луча после нормализации, т.е. масштабирования до unit vector, и
преобразования к диапазону [0,1] (линейное масштабирование).				      */
color ray_color(const ray& r) {  
	if(hit_sphere(point3(0,0,-1), 0.5, r)) { return color(1,0,0); }
	vec3 unit_direction = unitv(r.direction());
	double a = 0.5 * (unit_direction.y() + 1.0); // [-1;1] -> [0;1], 0.0 <= a <= 1.0
	/* linear interpolation */
	return (1.0-a)*color(1.0,1.0,1.0) + a*color(0.5, 0.7, 1.0); // a = 1.0 -> синий, a = 0.0 -> белый
}

int main() 
{
	/* image settings */
	double ASPECT_RATIO = 16.0 / 9.0;
	int IMAGE_WIDTH = 400;
	int IMAGE_HEIGHT = int(IMAGE_WIDTH / ASPECT_RATIO);
	IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT; // Высота изображения не должна быть < 1.

	/* viewport settings */
	double VIEWPORT_HEIGHT = 2.0; // высота просмотра выбрана произвольно
	double VIEWPORT_WIDTH = VIEWPORT_HEIGHT * (double(IMAGE_WIDTH)/IMAGE_HEIGHT);
	vec3 VIEWPORT_U = vec3(VIEWPORT_WIDTH, 0, 0);
	vec3 VIEWPORT_V = vec3(0, -VIEWPORT_HEIGHT, 0);

	/* camera settings */
	double FOCAL_LENGTH = 1.0; 
	point3 CAMERA_CENTER(0,0,0);

	/* calculate delta vectotrs */
	vec3 PIXEL_DELTA_U = VIEWPORT_U / IMAGE_WIDTH;
	vec3 PIXEL_DELTA_V = VIEWPORT_V / IMAGE_HEIGHT;

	/* calculate location viewport upper left/pixel(0,0) */
	point3 VIEWPORT_UPPER_LEFT = CAMERA_CENTER - vec3(0, 0, FOCAL_LENGTH) - VIEWPORT_U/2 - VIEWPORT_V/2;
	point3 PIXEL_LOC_00 = VIEWPORT_UPPER_LEFT + 0.5 * (PIXEL_DELTA_U + PIXEL_DELTA_V);

	std::ios_base::sync_with_stdio(0);
	/* render */
	std::cout << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";
	for (int j = 0; j < IMAGE_HEIGHT; ++j) {
		std::clog << "\rScanlines remaining: " << (IMAGE_HEIGHT - j) << ' ' << std::flush;
		for (int i = 0; i < IMAGE_WIDTH; ++i) {
			point3 pixel_center = PIXEL_LOC_00 + (i * PIXEL_DELTA_U) + (j * PIXEL_DELTA_V);
			vec3 ray_direction = pixel_center - CAMERA_CENTER;
			ray r(CAMERA_CENTER,ray_direction);

			color pix_color = ray_color(r);
			write_color(std::cout, pix_color);
		}
	}
	std::clog << "\rDone.                 \n";
}