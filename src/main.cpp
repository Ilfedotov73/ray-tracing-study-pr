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

void load_scene()
{
	hittable_list WORLD; 

	obj_loader loader; 
	loader.load_models("test.obj");

	WORLD.add(loader.get_sides());

	camera cam;

	cam.ASPECT_RATIO = 1.0f;
	cam.IMAGE_WIDTH = 300.0f;
	cam.SAMPLES_PER_PIXEL = 100.0f;
	cam.MAX_DEPTH = 50.0f;
	cam.background = color(0.0f,0.0f,0.0f);
	cam.VFOV = 40.0f;
	cam.LOOKFROM = point3(2.0f, 2.0f, 2.0f);
	cam.LOOKAT = point3(0.0f,0.0f,0.f);
	cam.VUP = vec3(0.0f, 1.0f, 0.0f);
	cam.FOCUS_ANGLE = 0.0f;
	cam.FOCUS_DIST = 10.0f;
	cam.background = color(0.70f, 0.80f, 1.00f);

	cam.render(WORLD);
}

void cornell_box()
{
	hittable_list WORLD;

	shared_ptr<material> red = make_shared<lambertian>(color(0.65f, 0.05f, 0.05f));
	shared_ptr<material> white = make_shared<lambertian>(color(0.73f, 0.73f, 0.73f));
	shared_ptr<material> green = make_shared<lambertian>(color(0.12f, 0.45f, 0.15f));
	shared_ptr<diffuse_light> light = make_shared<diffuse_light>(color(15.0f, 15.0f, 15.0f));
	
	WORLD.add(make_shared<quad>(point3(555.0f,0.0f,0.0f), vec3(0.0f,555.0f,0.0f), vec3(0.0f,0.0f,555.0f), green));
	WORLD.add(make_shared<quad>(point3(0.0f,0.0f,0.0f), vec3(0.0f,555.0f,0.0f), vec3(0.0f,0.0f,555.0f), red));
	WORLD.add(make_shared<quad>(point3(343.0f,554.0f,332.0f), vec3(-130.0f,0.0f,0.0f), vec3(0.0f,0.0f,-105.0f), light));
	WORLD.add(make_shared<quad>(point3(0.0f,0.0f,0.0f), vec3(555.0f,0.0f,0.0f), vec3(0.0f,0.0f,555.0f), white));
	WORLD.add(make_shared<quad>(point3(555.0f,555.0f,555.0f), vec3(-555.0f,0.0f,0.0f), vec3(0.0f,0.0f,-555.0f), white));
	WORLD.add(make_shared<quad>(point3(0.0f,0.0f,555.0f), vec3(555.0f,0.0f,0.0f), vec3(0.0f,555.0f,0.0f), white));

	camera cam;

	cam.ASPECT_RATIO = 1.0f;
	cam.IMAGE_WIDTH = 600.0f;
	cam.SAMPLES_PER_PIXEL = 200.0f;
	cam.MAX_DEPTH = 50.0f;
	cam.background = color(0.0f,0.0f,0.0f);
	cam.VFOV = 40.0f;
	cam.LOOKFROM = point3(278.0f, 278.0f, -800.0f);
	cam.LOOKAT = point3(278.0f, 278.0f, 0.0f);
	cam.VUP = vec3(0.0f, 1.0f, 0.0f);
	cam.FOCUS_ANGLE = 0.0f;
	cam.FOCUS_DIST = 10.0f;

	cam.render(WORLD);
}

void simple_light()
{
	hittable_list WORLD;

	shared_ptr<texture> brick_texture = make_shared<image_texture>("brick.jpg");
	shared_ptr<material> surface = make_shared<lambertian>(brick_texture);
	WORLD.add(make_shared<quad>(point3(-10.0f, 0.0f, -10.0f),vec3(50.0f,0.0f,0.0f),vec3(0.0f, 0.0f, 50.0f),surface));

	shared_ptr<material> diffcolor = make_shared<lambertian>(color(0.40f, 0.2f, 0.1f));
	WORLD.add(make_shared<sphere>(point3(0.0f,2.0f,0.0f), 2.0f, diffcolor));

	shared_ptr<diffuse_light> difflight = make_shared<diffuse_light>(color(4.0f,4.0f,4.0f));
	WORLD.add(make_shared<sphere>(point3(0.0f,7.0f,0.0f), 2.0f, difflight));
	WORLD.add(make_shared<quad>(point3(3.0f,1.0f,-2.0f), vec3(2.0f,0.0f,0.0f), vec3(0.0f,2.0f,0.0f), difflight));

	camera cam;
	cam.ASPECT_RATIO = 16.0f / 9.0f;
	cam.IMAGE_WIDTH = 400.0f;
	cam.SAMPLES_PER_PIXEL = 100.0f;
	cam.MAX_DEPTH = 50.0f;
	cam.background = color(0.0f,0.0f,0.0f);
	cam.VFOV = 20.0f;
	cam.LOOKFROM = point3(26.0f, 3.0f, 6.0f);
	cam.LOOKAT = point3(0.0f, 2.0f, 0.0f);
	cam.VUP = vec3(0.0f, 1.0f, 0.0f);
	cam.FOCUS_ANGLE = 0.0f;
	cam.FOCUS_DIST = 10.0f;

	cam.render(WORLD);
}

void quads()
{
	hittable_list WORLD;

	shared_ptr<texture> brick_texture = make_shared<image_texture>("brick.jpg");
	shared_ptr<material> surface = make_shared<lambertian>(brick_texture);

	shared_ptr<material> green 	= make_shared<lambertian>(color(0.2f, 1.0f, 0.2f));
	shared_ptr<material> blue 	= make_shared<lambertian>(color(0.2f, 0.2, 1.0f));
	shared_ptr<material> orange = make_shared<lambertian>(color(1.0f, 0.5f, 0.0f));
	shared_ptr<material> teal 	= make_shared<lambertian>(color(0.2f, 0.8f, 0.8f));

	WORLD.add(make_shared<quad>(point3(-3.0f,-2.0f,5.0f),vec3(0.0f,0.0f,-4.0f),vec3(0.0f,4.0f,0.0f),surface));
	WORLD.add(make_shared<quad>(point3(-2.0f,-2.0f,0.0f),vec3(4.0f,0.0f,0.0f),vec3(0.0f,4.0f,0.0f),green));
	WORLD.add(make_shared<quad>(point3(3.0f,-2.0f,1.0f),vec3(0.0f,0.0f,4.0f),vec3(0.0f,4.0f,0.0f),blue));
	WORLD.add(make_shared<quad>(point3(-2.0f,3.0f,1.0f),vec3(4.0f,0.0f,0.0f),vec3(0.0f,0.0f,4.0f),orange));
	WORLD.add(make_shared<quad>(point3(-2.0f,-3.0f,5.0f),vec3(4.0f,0.0f,0.0f),vec3(0.0f,0.0f,-4.0f),teal));

	camera cam;
	cam.ASPECT_RATIO = 1.0f;
	cam.IMAGE_WIDTH = 400.0f;
	cam.SAMPLES_PER_PIXEL = 100.0f;
	cam.MAX_DEPTH = 500.0f;
	cam.VFOV = 80.0f;
	cam.LOOKFROM = point3(0.0f, 0.0f, 9.0f);
	cam.LOOKAT = point3(0.0f, 0.0f, 0.0f);
	cam.VUP = vec3(0.0f, 1.0f, 0.0f);
	cam.FOCUS_ANGLE = 0.0f;
	cam.FOCUS_DIST = 10.0f;
	cam.background = color(0.70f, 0.80f, 1.00f);

	cam.render(WORLD);
}

void brick()
{
	shared_ptr<texture> brick_texture = make_shared<image_texture>("brick.jpg");
	shared_ptr<material> surface = make_shared<lambertian>(brick_texture);
	shared_ptr<sphere> obj = make_shared<sphere>(point3(0.0f,0.0f,0.0f), 2.0f, surface); 

	camera cam;
	cam.ASPECT_RATIO = 16.0f / 9.0f;
	cam.IMAGE_WIDTH = 400.0f;
	cam.SAMPLES_PER_PIXEL = 100.0f;
	cam.MAX_DEPTH = 50.0f;
	cam.VFOV = 20.0f;
	cam.LOOKFROM = point3(13.0f, 2.0f, 3.0f);
	cam.LOOKAT = point3(0.0f, 0.0f, 0.0f);
	cam.VUP = vec3(0.0f, 1.0f, 0.0f);
	cam.FOCUS_ANGLE = 0.6f;
	cam.FOCUS_DIST = 10.0f;
	cam.background = color(0.70f, 0.80f, 1.00f);

	clock_t start, stop;

	start = clock();
	cam.render(hittable_list(obj));
	stop = clock();

	float timer = ((float)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer << " seconds.\n";
}

void bouncing_spheres()
{
	// settings
	std::ios_base::sync_with_stdio(0);

	hittable_list WORLD;
	shared_ptr<texture> checker = make_shared<checker_texture>(0.32f, color(.2f, .3f, .1f), color(.9f, .9f, .9f));
	//shared_ptr<material> ground_mat = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	WORLD.add(make_shared<sphere>(point3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<lambertian>(checker)));
	for (int a = -11; a < 11; ++a) {
		for (int b = -11; b < 11; ++b) {
			float choose_mat = random_float();
			point3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());
			if ((center - point3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
				shared_ptr<material> sphere_mat;

				if (choose_mat < 0.8f) {
					/* diffuse */
					color albedo = color::random() * color::random();
					sphere_mat = make_shared<lambertian>(albedo);
					point3 center_pos2 = center + vec3(0.0f, random_float(0, 0.5f), 0.0f);
					WORLD.add(make_shared<sphere>(center, center_pos2, 0.2f, sphere_mat));
				}
				else if (choose_mat < 0.95f) {
					/* metal */
					color albedo = color::random(0.5f, 1.0f);
					float fuzz = random_float(0.0f, 0.5f);
					sphere_mat = make_shared<metal>(albedo, fuzz);
				}
				else {
					/* glass */
					sphere_mat = make_shared<dielectric>(1.5f);
				}
				WORLD.add(make_shared<sphere>(center, 0.2f, sphere_mat));
			}
		}
	}
	shared_ptr<material> mat1 = make_shared<dielectric>(1.5f);
	WORLD.add(make_shared<sphere>(point3(0.0f, 1.0f, 0.0f), 1.0f, mat1));

	shared_ptr<texture> cow = make_shared<image_texture>("images.jpeg");
	shared_ptr<material> mat2 = make_shared<lambertian>(cow);
	//shared_ptr<material> mat2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	WORLD.add(make_shared<sphere>(point3(-4.0f, 1.0f, 0.0f), 1.0f, mat2));

	shared_ptr<material> mat3 = make_shared<metal>(color(0.7f, 0.6f, 0.5f), 0.0f);
	WORLD.add(make_shared<sphere>(point3(4.0f, 1.0f, 0.0f), 1.0f, mat3));

	WORLD = hittable_list(make_shared<bvh_node>(WORLD));

	camera cam;
	cam.ASPECT_RATIO = 16.0f / 9.0f;
	cam.IMAGE_WIDTH = 400.0f;
	cam.SAMPLES_PER_PIXEL = 100.0f;
	cam.MAX_DEPTH = 50.0f;
	cam.VFOV = 20.0f;
	cam.LOOKFROM = point3(13.0f, 2.0f, 3.0f);
	cam.LOOKAT = point3(0.0f, 0.0f, 0.0f);
	cam.VUP = vec3(0.0f, 1.0f, 0.0f);
	cam.FOCUS_ANGLE = 0.6f;
	cam.FOCUS_DIST = 10.0f;
	cam.background = color(0.70f, 0.80f, 1.00f);
	clock_t start, stop;

	start = clock();
	cam.render(WORLD);
	stop = clock();

	float timer = ((float)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer << " seconds.\n";
}

void checkered_spheres()
{
	hittable_list WORLD;
	shared_ptr<texture> checker = make_shared<checker_texture>(0.6f, color(0.2f, 0.3f, 0.1f), color(0.9f, 0.9f, 0.9f));

	WORLD.add(make_shared<sphere>(point3(0.0f, -10.0f, 0.0f), 10.0f, make_shared<lambertian>(checker)));
	WORLD.add(make_shared<sphere>(point3(0.0f, 10.0f, 0.0f), 10.0f, make_shared<lambertian>(checker)));

	camera cam;
	cam.ASPECT_RATIO = 16.0f / 9.0f;
	cam.IMAGE_WIDTH = 400.0f;
	cam.SAMPLES_PER_PIXEL = 100.0f;
	cam.MAX_DEPTH = 50.0f;
	cam.VFOV = 20.0f;
	cam.LOOKFROM = point3(13.0f, 2.0f, 3.0f);
	cam.LOOKAT = point3(0.0f, 0.0f, 0.0f);
	cam.VUP = vec3(0.0f, 1.0f, 0.0f);
	cam.FOCUS_ANGLE = 0.0f;
	cam.FOCUS_DIST = 10.0f;
	cam.background = color(0.70f, 0.80f, 1.00f);
	clock_t start, stop;

	start = clock();
	cam.render(WORLD);
	stop = clock();

	float timer = ((float)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer << " seconds.\n";
}

int main() 
{
	switch (7) {
	case 1: bouncing_spheres(); break;
	case 2: checkered_spheres(); break;
	case 3: brick(); break;
	case 4: quads(); break;
	case 5: simple_light(); break;
	case 6: cornell_box(); break;
	case 7: load_scene(); break;
	}
}