/***********************************************************************************
* Данная программа рассчитывает значения матрицы пискслей и записывает их в формате 
* P3, т.е. цвета даны в ASCII - 24 бита на пиксель (по 8 бит на r,g,b). 
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
* интенсивности конкретного компонента цвета (r,g,b). 
*
* cmake ..
* cmake --build . --config Release
***********************************************************************************/

#include "rt_settings.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

int main() 
{
	// settings
	std::ios_base::sync_with_stdio(0);

	hittable_list WORLD;
	shared_ptr<material> ground_mat = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	WORLD.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_mat));
	for (int a = -11; a < 11; ++a) {
		for (int b = -11; b < 11; ++b) {
			double choose_mat = random_double();
			point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
			if ((center - point3(4, 0.2, 0)).length() > 0.9) { 
				shared_ptr<material> sphere_mat; 

				if (choose_mat < 0.8) {
					/* diffuse */
					color albedo = color::random() * color::random();
					sphere_mat = make_shared<lambertian>(albedo);
				}
				else if (choose_mat < 0.95) {
					/* metal */
					color albedo = color::random(0.5, 1);
					double fuzz = random_double(0, 0.5);
					sphere_mat = make_shared<metal>(albedo, fuzz);
				}
				else {
					/* glass */
					sphere_mat = make_shared<dielectric>(1.5);
				}
				WORLD.add(make_shared<sphere>(center, 0.2, sphere_mat));
			}
		}
	}
	shared_ptr<material> mat1 = make_shared<dielectric>(1.5);
	WORLD.add(make_shared<sphere>(point3(0, 1, 0), 1.0, mat1));

	shared_ptr<material> mat2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	WORLD.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, mat2));

	shared_ptr<material> mat3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	WORLD.add(make_shared<sphere>(point3(4, 1, 0), 1.0, mat3));
	
	camera cam;
	cam.ASPECT_RATIO = 16.0 / 9.0;
	cam.IMAGE_WIDTH  = 1920;
	cam.SAMPLES_PER_PIXEL = 500;
	cam.MAX_DEPTH = 50;

	cam.VFOV = 20;
	cam.LOOKFROM = point3(13,2,3);
	cam.LOOKAT = point3(0,0,0);
	cam.VUP = vec3(0,1,0);
	cam.FOCUS_ANGLE = 0.6;
	cam.FOCUS_DIST = 10.0;

	cam.render(WORLD);
}