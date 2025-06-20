#ifndef CAMERA_H
#define CANERA_H

#include "hittable.h"
class camera
{
private:
	int    IMAGE_HEIGHT;
	point3 CAMERA_CENTER;
	point3 PIXEL_LOC_00;
	vec3   PIXEL_DELTA_U;
	vec3   PIXEL_DELTA_V;

	void initialize()
	{
		/* image settings */
		IMAGE_HEIGHT = int(IMAGE_WIDTH / ASPECT_RATIO);
		IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT; // Высота изображения не должна быть < 1.

		/* viewport settings */
		double VIEWPORT_HEIGHT = 2.0; // высота просмотра выбрана произвольно
		double VIEWPORT_WIDTH = VIEWPORT_HEIGHT * (double(IMAGE_WIDTH) / IMAGE_HEIGHT);
		vec3 VIEWPORT_U = vec3(VIEWPORT_WIDTH, 0, 0);
		vec3 VIEWPORT_V = vec3(0, -VIEWPORT_HEIGHT, 0);

		/* camera settings */
		double FOCAL_LENGTH = 1.0;
		point3 CAMERA_CENTER(0, 0, 0);

		/* calculate delta vectotrs */
		PIXEL_DELTA_U = VIEWPORT_U / IMAGE_WIDTH;
		PIXEL_DELTA_V = VIEWPORT_V / IMAGE_HEIGHT;

		/* calculate location viewport upper left/pixel(0,0) */
		point3 VIEWPORT_UPPER_LEFT = CAMERA_CENTER - vec3(0, 0, FOCAL_LENGTH) - VIEWPORT_U / 2 - VIEWPORT_V / 2;
		PIXEL_LOC_00 = VIEWPORT_UPPER_LEFT + 0.5 * (PIXEL_DELTA_U + PIXEL_DELTA_V);
	}
	
	/*
	 * Функция ray_color() принимает луч и константную ссылку на объект реализующий
	 * абстрактный класс hittable - hittable_list, который содержит массив указателей
	 * на поверхности (также реализующие hittable).
	 *
	 * В теле функции создается объект класса hit_record rec, который регистрирует
	 * информацию о пересечении объекта лучом. Если луч пересекает один из поверхностей
	 * world.hit() вернет true и ray_color вернет цвет пикселя "затененного" нормалью.
	 *
	 * Затенение нормалями: цвет пикселя поверхности определяется положением нормали
	 * единичной длины, что позволяет получить значение пикселся в диапазоне [-1,1] для
	 * последуюущего линейного масштабирование в диапазон [0,1].
	 *
	 * Если пересечение с объектами не происходит,то рассчитывается градиент от белого
	 * к синему, в зависимости о высоты пикселя. Чем ближе значение a к единице, тем
	 * ближе значение к синему (0.5, 0.7, 1.0) - рассчет неба для сцены.
	*/
	color ray_color(const ray& r, const hittable& world) const
	{
		hit_record rec;
		if (world.hit(r, interval(0, INF), rec)) {
			return 0.5 * (rec.normal + color(1, 1, 1)); // 0.5*(normal + 1) -- линейное масштабирование до диапазона значений [0,1]
		}

		/* sky */
		vec3 unit_direction = unitv(r.direction());
		double a = 0.5 * (unit_direction.y() + 1.0); // [-1;1] -> [0;1], 0.0 <= a <= 1.0
		/* linear interpolation */
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0); // a = 1.0 -> синий, a = 0.0 -> белый
	}
	
public:
	/* image settings */
	double ASPECT_RATIO = 1.0;
	int    IMAGE_WIDTH = 100;

	void render(const hittable& world)
	{
		initialize();
		std::cout << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";
		for (int j = 0; j < IMAGE_HEIGHT; ++j) {
			std::clog << "\rScanlines remaining: " << (IMAGE_HEIGHT - j) << ' ' << std::flush;
			for (int i = 0; i < IMAGE_WIDTH; ++i) {
				point3 pixel_center = PIXEL_LOC_00 + (i * PIXEL_DELTA_U) + (j * PIXEL_DELTA_V); // определяем пиксь в области просмотра 
				point3 ray_direction = pixel_center - CAMERA_CENTER; // компенсируем расстояние от точки начала координа до камеры, 
				// для корректного перечечения пикселя области просмотра. 
				ray r(CAMERA_CENTER, ray_direction); // испускаем луч

				color pix_color = ray_color(r, world); // определяем цвет пересеченного пикслея области просмотра
				write_color(std::cout, pix_color); // записываем в выхондной поток > .ppm.
			}
		}
		std::clog << "\rDone.                 \n";
	}
};

#endif