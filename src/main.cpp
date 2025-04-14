/***********************************************************************************
* Данный файл записывает изображение в формате P3, т.е. цвета даны в ASCII - 24 бита 
* на пиксель (по 8 бит на r,g,b). 
* 
* Выходной файл ((>) file.ppm) содержит значения для пикселя в количестве 256 x 256, 
* значения которых не превышают 255.
*
* Файл .ppm является простым форматом хранения цветного изображения. Также может 
* быть использован в качестве промежуточного формата при конвертации растровых 
* изображений. 
* 
* Внутренне представление, по соглашени. r,g,b компонентов должно находиться в 
* диапазоне [0;1].
* 
* Т.е. для каждого компонента конкретного пикселя определяется степень 
  принадлежности конкретного компонента (r,g,b) к конкретному цвету.
*
* cmake --build . --config release
***********************************************************************************/

#include "ray.h"
#include "vec3.h"
#include "color.h"
#include <iostream>

color ray_color(const ray& ray) { 
	vec3 unit_direction = unitv(ray.direction());
	double a = 0.5 * (unit_direction.y() + 1.0);
	return (1.0-a)*color(1.0,1.0,1.0) + a*color(0.5, 0.7, 1.0);
}

int main() 
{
	/* image settings */
	double ASPECT_RATIO = 16.0 / 9.0;
	int IMAGE_WIDTH = 400;
	int IMAGE_HEIGHT = int(IMAGE_WIDTH / ASPECT_RATIO);
	IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT; // Высота изображения не должна быть < 1.

	/* viewport settings */
	double VIEWPORT_HEIGHT = 2.0; 
	double VIEWPORT_WIDTH = VIEWPORT_HEIGHT * (double(IMAGE_WIDTH)/IMAGE_HEIGHT);
	vec3 VIEWPORT_U = vec3(VIEWPORT_WIDTH, 0, 0);
	vec3 VIEWPORT_V = vec3(0, -VIEWPORT_HEIGHT, 0);

	/* camera settings */
	double FOCAL_LENGTH = 1.0; 
	point3 CAMERA_CENTER(0,0,0);

	
	/* calculate delta vectotrs */
	vec3 PIXEL_DELTA_U = VIEWPORT_U / IMAGE_WIDTH;
	vec3 PIXEL_DELTA_V = VIEWPORT_V / IMAGE_HEIGHT;

	/* calculate location upper left pixel */
	point3 VIEWPORT_UPPER_LEFT = CAMERA_CENTER - vec3(0, 0, FOCAL_LENGTH) - VIEWPORT_U/2 - VIEWPORT_V/2;
	point3 PIXEL100_LOC = VIEWPORT_UPPER_LEFT + 0.5 * (PIXEL_DELTA_U + PIXEL_DELTA_V);

	std::ios_base::sync_with_stdio(0);
	/* render */
	std::cout << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";
	for (int j = 0; j < IMAGE_HEIGHT; ++j) {
		std::clog << "\rScanlines remaining: " << (IMAGE_HEIGHT - j) << ' ' << std::flush;
		for (int i = 0; i < IMAGE_WIDTH; ++i) {
			point3 pixel_center = PIXEL100_LOC + (i * PIXEL_DELTA_U) + (j * PIXEL_DELTA_V);
			vec3 ray_direction = pixel_center - CAMERA_CENTER;
			ray ray_orig(CAMERA_CENTER,ray_direction);

			color pix_color = ray_color(ray_orig);
			write_color(std::cout, pix_color);
		}
	}
	std::clog << "\rDone.                 \n";
}