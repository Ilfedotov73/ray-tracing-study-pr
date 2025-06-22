/***********************************************************************************
* �������� ���������� ����� ����������� ������� ������� �� ������ ����������� �����.
* ���� ��������: � ������ ������� ������� ��������� ����������� ��� �� ������,
* ������� ����� ������� �������� b. ��������� ������� (����� P), � ������� �������
* ���, ����� ������ ��� ������� a + b, ��� � ������ � ����������� ������.
* ���� ������ �������� �������� t ��� ������� ����������� b, �� ����� ��������
* �������� ����� P ����� ����������� ����. ����� �������, ���� ��� ���������
* ��������� t, ����� P(t) = a + tb ����� ������ �� ��������� �����������, �� ����-
* �������� ������� ������� ��������� ����� �������� � ��������������� ����.
*
* ��� ������ �������� � ������������ �����, ���������� �������������� ����������
* �� ����� ������ ��������� �� ����� A ��� ����� �������� ����������� ����. �����
* ������� ��������� ����� D (������� ������� ���������) � ���� �������� ������ d
* � ���� �����. ���� P(t) = � + td, ����� ������������ ��� ����� ���������� �����
* D. �.�. ��������� ������ ����������� ����� D, ���� �������� ������ b = (d - a).
***********************************************************************************/

#ifndef CAMERA_H
#define CANERA_H

#include "hittable.h"
class camera
{
private:
	int    IMAGE_HEIGHT;        // ������ ���������������� ����������� � ��������
	double PIXEL_SAMPLES_SCALE; // ����������� �������� ����� ��� ����� ������� ������� ��� ���������� 
								// ����� �������� ������������� �������� ������� ������ (i,j) �������.
	point3 CAMERA_CENTER;       // ����� ������ 
	point3 PIXEL_LOC_00;        // ������� ������� � (0,0)
	vec3   PIXEL_DELTA_U;       // �������� ������� �� �����������
	vec3   PIXEL_DELTA_V;       // �������� ������� �� ���������

	void initialize()
	{
		/* image settings */
		IMAGE_HEIGHT = int(IMAGE_WIDTH / ASPECT_RATIO);
		IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT; // ������ ����������� �� ������ ���� < 1.

		/* viewport settings */
		double VIEWPORT_HEIGHT = 2.0; // ������ ��������� ������� �����������
		double VIEWPORT_WIDTH = VIEWPORT_HEIGHT * (double(IMAGE_WIDTH) / IMAGE_HEIGHT);
		vec3   VIEWPORT_U = vec3(VIEWPORT_WIDTH, 0, 0);
		vec3   VIEWPORT_V = vec3(0, -VIEWPORT_HEIGHT, 0);

		/* camera settings */
		double FOCAL_LENGTH = 1.0;
		point3 CAMERA_CENTER(0, 0, 0);

		/* calculate delta vectotrs */
		PIXEL_DELTA_U = VIEWPORT_U / IMAGE_WIDTH;
		PIXEL_DELTA_V = VIEWPORT_V / IMAGE_HEIGHT;

		/* calculate location viewport upper left/pixel(0,0) */
		point3 VIEWPORT_UPPER_LEFT = CAMERA_CENTER - vec3(0, 0, FOCAL_LENGTH) - VIEWPORT_U / 2 - VIEWPORT_V / 2;
		PIXEL_LOC_00 = VIEWPORT_UPPER_LEFT + 0.5 * (PIXEL_DELTA_U + PIXEL_DELTA_V);
		
		PIXEL_SAMPLES_SCALE = 1.0 / SAMPLES_PER_PIXEL;
	}
	
	/*
	 * ������� ray_color() ��������� ��� � ����������� ������ �� ������ �����������
	 * ����������� ����� hittable - hittable_list, ������� �������� ������ ����������
	 * �� ����������� (����� ����������� hittable).
	 *
	 * � ���� ������� ��������� ������ ������ hit_record rec, ������� ������������
	 * ���������� � ����������� ������� �����. ���� ��� ���������� ���� �� ������������
	 * world.hit() ������ true � ray_color ������ ���� ������� "�����������" ��������.
	 *
	 * ��������� ���������: ���� ������� ����������� ������������ ���������� �������
	 * ��������� �����, ��� ��������� �������� �������� �������� � ��������� [-1,1] ���
	 * ������������� ��������� ��������������� � �������� [0,1].
	 *
	 * ���� ����������� � ��������� �� ����������,�� �������������� �������� �� ������
	 * � ������, � ����������� � ������ �������. ��� ����� �������� a � �������, ���
	 * ����� �������� � ������ (0.5, 0.7, 1.0) - ������� ���� ��� �����.
	*/
	color ray_color(const ray& r, const hittable& world) const
	{
		hit_record rec;
		if (world.hit(r, interval(0, INF), rec)) {
			return 0.5 * (rec.normal + color(1, 1, 1)); // 0.5*(normal + 1) -- �������� ��������������� �� ��������� �������� [0,1]
		}

		/* sky */
		vec3 unit_direction = unitv(r.direction());
		double a = 0.5 * (unit_direction.y() + 1.0); // [-1;1] -> [0;1], 0.0 <= a <= 1.0
		/* linear interpolation */
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0); // a = 1.0 -> �����, a = 0.0 -> �����
	}
	
	/*
	 * ������� get_ray() ���������� ��� ��������� �� ������ � ���������� ��������� 
	 * ����� ������ �������������� (i,j) �������. ��� ������������ ����� �����������
	 * ������ ��������� ���������� �������.
	*/
	ray get_ray(int i, int j) const 
	{
		vec3 offset = sample_square(); // ������������� ��������� ��������� ����� �� ��������� ��������
									   // ��������� ���������� �������.
		point3 pixel_sample = PIXEL_LOC_00 + ((i + offset.x()) * PIXEL_DELTA_U) 
										   + ((j + offset.y()) * PIXEL_DELTA_V); // ����������� ������������� ������� (�.�. ���������� 
																			     // ������� ������ (i,j) �������) �� ������� ���������.
		point3 ray_origin = CAMERA_CENTER; 
		point3 ray_direction = pixel_sample - ray_origin; // ������������ ���������� �� ����� ������ �������� �� ������, 
														  // ��� ����������� ����������� ������� ������� ���������.
		return ray(ray_origin, ray_direction);
	}

	/* ��������� ������ �� ��������� ����� � ��������� ���������� �������, � ��������� [-0.5,-0.5] - [0.5,0.5] */
	vec3 sample_square() const { return vec3(random_double() - 0.5, random_double() - 0.5, 0); }

public:
	/* image settings */
	double ASPECT_RATIO = 1.0;     // ��������� ������ ����������� � ������.
	int    IMAGE_WIDTH = 100;      // ������ ���������������� ����������� � ��������.
	int	   SAMPLES_PER_PIXEL = 10; // ���������� ��������� ������� (�������) ��� ������� �������.

	void render(const hittable& world)
	{
		initialize();
		std::cout << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";
		for (int j = 0; j < IMAGE_HEIGHT; ++j) {
			std::clog << "\rScanlines remaining: " << (IMAGE_HEIGHT - j) << ' ' << std::flush;
			for (int i = 0; i < IMAGE_WIDTH; ++i) {
				color pixel_color(0,0,0);
				/* sampling */
				for (int sample = 0; sample < SAMPLES_PER_PIXEL; ++sample) { 
					ray r = get_ray(i,j);	// �� ������ (i,j) ������� ������������ samples_per_pixel ������� (�������)

					/* Antialiasing */
					pixel_color += ray_color(r, world); // ������������ �������� ������������� ������ ������� 
														// ������ (i.j) �������. 
				}
				write_color(std::cout, PIXEL_SAMPLES_SCALE * pixel_color); // ���������� � ��������� ����� > .ppm.
																		   // pixel_color * (1.0 / SAMPLES_PER_PIXEL): 
																		   // ����������� �������� pixel_color ���������
																		   // �������� ���������� �������� ��� (i,j) �������.
			}
		}
		std::clog << "\rDone.                 \n";
	}
};

#endif