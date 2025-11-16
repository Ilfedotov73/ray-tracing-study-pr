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
#include "time.h"

void simple_light()
{
	hittable_list WORLD;

	shared_ptr<texture> brick_texture = make_shared<image_texture>("brick.jpg");
	shared_ptr<material> surface = make_shared<lambertian>(brick_texture);
	WORLD.add(make_shared<quad>(point3(-10, 0, -10),vec3(50,0,0),vec3(0, 0, 50),surface));

	shared_ptr<material> diffcolor = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	WORLD.add(make_shared<sphere>(point3(0,2,0), 2, diffcolor));

	shared_ptr<diffuse_light> difflight = make_shared<diffuse_light>(color(4,4,4));
	WORLD.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
	WORLD.add(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));

	camera cam;
	cam.ASPECT_RATIO = 16.0 / 9.0;
	cam.IMAGE_WIDTH = 400;
	cam.SAMPLES_PER_PIXEL = 100;
	cam.MAX_DEPTH = 50;
	cam.background = color(0,0,0);
	cam.VFOV = 20;
	cam.LOOKFROM = point3(26, 3, 6);
	cam.LOOKAT = point3(0, 2, 0);
	cam.VUP = vec3(0, 1, 0);
	cam.FOCUS_ANGLE = 0;
	cam.FOCUS_DIST = 10.0;

	cam.render(WORLD);
}

void quads()
{
	hittable_list WORLD;

	shared_ptr<texture> brick_texture = make_shared<image_texture>("brick.jpg");
	shared_ptr<material> surface = make_shared<lambertian>(brick_texture);

	shared_ptr<material> green 	= make_shared<lambertian>(color(0.2, 1.0, 0.2));
	shared_ptr<material> blue 	= make_shared<lambertian>(color(0.2, 0.2, 1.0));
	shared_ptr<material> orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
	shared_ptr<material> teal 	= make_shared<lambertian>(color(0.2, 0.8, 0.8));

	WORLD.add(make_shared<quad>(point3(-3,-2,5),vec3(0,0,-4),vec3(0,4,0),surface));
	WORLD.add(make_shared<quad>(point3(-2,-2,0),vec3(4,0,0),vec3(0,4,0),green));
	WORLD.add(make_shared<quad>(point3(3,-2,1),vec3(0,0,4),vec3(0,4,0),blue));
	WORLD.add(make_shared<quad>(point3(-2,3,1),vec3(4,0,0),vec3(0,0,4),orange));
	WORLD.add(make_shared<quad>(point3(-2,-3,5),vec3(4,0,0),vec3(0,0,-4),teal));

	camera cam;
	cam.ASPECT_RATIO = 1.0;
	cam.IMAGE_WIDTH = 400;
	cam.SAMPLES_PER_PIXEL = 100;
	cam.MAX_DEPTH = 50;
	cam.VFOV = 80;
	cam.LOOKFROM = point3(0, 0, 9);
	cam.LOOKAT = point3(0, 0, 0);
	cam.VUP = vec3(0, 1, 0);
	cam.FOCUS_ANGLE = 0;
	cam.FOCUS_DIST = 10.0;
	cam.background = color(0.70, 0.80, 1.00);

	cam.render(WORLD);
}

void brick()
{
	shared_ptr<texture> brick_texture = make_shared<image_texture>("brick.jpg");
	shared_ptr<material> surface = make_shared<lambertian>(brick_texture);
	shared_ptr<sphere> obj = make_shared<sphere>(point3(0,0,0), 2, surface); 

	camera cam;
	cam.ASPECT_RATIO = 16.0 / 9.0;
	cam.IMAGE_WIDTH = 400;
	cam.SAMPLES_PER_PIXEL = 100;
	cam.MAX_DEPTH = 50;
	cam.VFOV = 20;
	cam.LOOKFROM = point3(13, 2, 3);
	cam.LOOKAT = point3(0, 0, 0);
	cam.VUP = vec3(0, 1, 0);
	cam.FOCUS_ANGLE = 0.6;
	cam.FOCUS_DIST = 10.0;
	cam.background = color(0.70, 0.80, 1.00);

	clock_t start, stop;

	start = clock();
	cam.render(hittable_list(obj));
	stop = clock();

	double timer = ((double)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer << " seconds.\n";
}

void bouncing_spheres()
{
	// settings
	std::ios_base::sync_with_stdio(0);

	hittable_list WORLD;
	shared_ptr<texture> checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
	//shared_ptr<material> ground_mat = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	WORLD.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));
	for (int a = -11; a < 11; ++a) {
		for (int b = -11; b < 11; ++b) {
			double choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_mat;

				if (choose_mat < 0.8) {
					/* diffuse */
					color albedo = color::random() * color::random();
					sphere_mat = make_shared<lambertian>(albedo);
					point3 center_pos2 = center + vec3(0, random_double(0, 0.5), 0);
					WORLD.add(make_shared<sphere>(center, center_pos2, 0.2, sphere_mat));
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

	shared_ptr<texture> cow = make_shared<image_texture>("images.jpeg");
	shared_ptr<material> mat2 = make_shared<lambertian>(cow);
	//shared_ptr<material> mat2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	WORLD.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, mat2));

	shared_ptr<material> mat3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	WORLD.add(make_shared<sphere>(point3(4, 1, 0), 1.0, mat3));

	WORLD = hittable_list(make_shared<bvh_node>(WORLD));

	camera cam;
	cam.ASPECT_RATIO = 16.0 / 9.0;
	cam.IMAGE_WIDTH = 400;
	cam.SAMPLES_PER_PIXEL = 100;
	cam.MAX_DEPTH = 50;
	cam.VFOV = 20;
	cam.LOOKFROM = point3(13, 2, 3);
	cam.LOOKAT = point3(0, 0, 0);
	cam.VUP = vec3(0, 1, 0);
	cam.FOCUS_ANGLE = 0.6;
	cam.FOCUS_DIST = 10.0;
	cam.background = color(0.70, 0.80, 1.00);
	clock_t start, stop;

	start = clock();
	cam.render(WORLD);
	stop = clock();

	double timer = ((double)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer << " seconds.\n";
}

void checkered_spheres()
{
	hittable_list WORLD;
	shared_ptr<texture> checker = make_shared<checker_texture>(0.6, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

	WORLD.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
	WORLD.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

	camera cam;
	cam.ASPECT_RATIO = 16.0 / 9.0;
	cam.IMAGE_WIDTH = 400;
	cam.SAMPLES_PER_PIXEL = 100;
	cam.MAX_DEPTH = 50;
	cam.VFOV = 20;
	cam.LOOKFROM = point3(13, 2, 3);
	cam.LOOKAT = point3(0, 0, 0);
	cam.VUP = vec3(0, 1, 0);
	cam.FOCUS_ANGLE = 0;
	cam.FOCUS_DIST = 10.0;
	cam.background = color(0.70, 0.80, 1.00);
	clock_t start, stop;

	start = clock();
	cam.render(WORLD);
	stop = clock();

	double timer = ((double)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer << " seconds.\n";
}

int main() 
{
	switch (5) {
	case 1: bouncing_spheres(); break;
	case 2: checkered_spheres(); break;
	case 3: brick(); break;
	case 4: quads(); break;
	case 5: simple_light(); break;
	}
}