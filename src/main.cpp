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
	std::ios_base::sync_with_stdio(0);

	/* world */
	hittable_list WORLD;

	shared_ptr<material> mat1 = make_shared<lambertian>(color(0.8,0.8,0.0));
	shared_ptr<material> mat2 = make_shared<lambertian>(color(0.1,0.2,0.5));
	shared_ptr<material> mat3 = make_shared<dielectric>(1.50);           
	shared_ptr<material> mat31 = make_shared<dielectric>(1.00 / 1.50);
	shared_ptr<material> mat4 = make_shared<metal>(color(0.8,0.6,0.8), 0.1);

	double R = std::cos(PI/4);

	WORLD.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, mat1));
	WORLD.add(make_shared<sphere>(point3(0.0,  0.0,   -1.2), 0.5, mat2));
	WORLD.add(make_shared<sphere>(point3(-1.0, 0.0,   -1.0), 0.5, mat3));
	WORLD.add(make_shared<sphere>(point3(-1.0, 0.0,   -1.0), 0.4, mat31));
	WORLD.add(make_shared<sphere>(point3(1.0,  0.0,   -1.0), 0.5, mat4));

	camera cam;
	cam.ASPECT_RATIO = 16.0 / 9.0;
	cam.IMAGE_WIDTH  = 400;
	cam.SAMPLES_PER_PIXEL = 100;
	cam.MAX_DEPTH = 50;

	cam.VFOV = 20;
	cam.LOOKFROM = point3(-2,2,1);
	cam.LOOKAT = point3(0,0,-1);
	cam.VUP = vec3(0,1,0);

	cam.render(WORLD);
}