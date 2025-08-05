/***********************************************************************************
* ������ ��������� ������������ �������� ������� �������� � ���������� �� � ������� 
* P3, �.�. ����� ���� � ASCII - 24 ���� �� ������� (�� 8 ��� �� r,g,b). 
* 
* �������� ���� ((>) file.ppm) �������� �������� ��� ������� � ���������� 256 x 256, 
* �������� ������� �� ��������� 255.
*
* ���� .ppm �������� ������� �������� �������� �������� �����������. ����� ����� 
* ���� ����������� � �������� �������������� ������� ��� ����������� ��������� 
* �����������. 
* 
* ��������� �������������, �� ���������. r,g,b ����������� ������ ���������� � 
* ��������� [0,1]. ��� ����� ��������� ������ (���������������), ������� ����� 
* ���������� � ��������� [-1,1], ������������: 0.5 * (unit_direction.y() + 1.0) 
* (�������� ���������������).
* 
* �.�. ��� ������� ���������� ����������� ������� ������������ ������� 
* ������������� ����������� ���������� ����� (r,g,b). 
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