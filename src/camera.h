/***********************************************************************************
* Алгоритм вычисления цвета конкретного пикселя основан на методе трассировки лучей.
* Суть которого: в каждый пиксель области просмотра испускается луч из камеры,
* который можно описать вектором b. Положение пикселя (точка P), в который попадет
* луч, будет описан как вектора a + b, где а вектор к виртуальной камере.
* Если задать некотрый параметр t для вектора направления b, то будет возможно
* сдвигать точку P вдоль направления луча. Таким образом, если при некоторых
* значениях t, точка P(t) = a + tb будет лежать на некоторой поверхности, то пере-
* сеченный пиксель области просмотра будет покрашен в соответствующий цвет.
*
* При камере заданной в произвольной точке, необходимо компенсировать расстояние
* от точка начала координат до точки A для точки задающей направление луча. Пусть
* имеется некоторая точка D (пиксель области просмотра) и есть неоторый вектор d
* к этой точке. Если P(t) = а + td, тогда получившийся луч будет параллелен точке
* D. Т.к. требуется именно пересечение точки D, выше вводится вектор b = (d - a).
***********************************************************************************/

#ifndef CAMERA_H
#define CANERA_H

#include "hittable.h"
class camera
{
private:
	int    IMAGE_HEIGHT;        // Ширина визуализируемого изображения в пикселях
	double PIXEL_SAMPLES_SCALE; // Коэффициент цветовой гаммы для суммы выборок пикслей для усреднения 
								// суммы значений интенсивности пикселей области вокруг (i,j) пикселя.
	point3 CAMERA_CENTER;       // Центр камеры 
	point3 PIXEL_LOC_00;        // Позиция пикселя в (0,0)
	vec3   PIXEL_DELTA_U;       // Смешение пикслея по горизонтали
	vec3   PIXEL_DELTA_V;       // Смещение пикселя по вертикали

	void initialize()
	{
		/* image settings */
		IMAGE_HEIGHT = int(IMAGE_WIDTH / ASPECT_RATIO);
		IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT; // Высота изображения не должна быть < 1.

		/* viewport settings */
		double VIEWPORT_HEIGHT = 2.0; // высота просмотра выбрана произвольно
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
	
	/*
	 * Функция get_ray() возвращает луч исходящий из камеры в произольно выбранную 
	 * точку вокруг местоположения (i,j) пикселя. Что представляет собой сглаживание
	 * методм единичной квадратной области.
	*/
	ray get_ray(int i, int j) const 
	{
		vec3 offset = sample_square(); // инициализация некоторой случайной точки из диапазона значений
									   // единичной квадратной области.
		point3 pixel_sample = PIXEL_LOC_00 + ((i + offset.x()) * PIXEL_DELTA_U) 
										   + ((j + offset.y()) * PIXEL_DELTA_V); // определение сэмплируемого пикселя (т.е. случайного 
																			     // пикселя вокруг (i,j) пикселя) из области просмотра.
		point3 ray_origin = CAMERA_CENTER; 
		point3 ray_direction = pixel_sample - ray_origin; // компенсируем расстояние от точки начала координа до камеры, 
														  // для корректного перечечения пикселя области просмотра.
		return ray(ray_origin, ray_direction);
	}

	/* Возращает вектор до случайной точки в единичной квадратной области, в диапазоне [-0.5,-0.5] - [0.5,0.5] */
	vec3 sample_square() const { return vec3(random_double() - 0.5, random_double() - 0.5, 0); }

public:
	/* image settings */
	double ASPECT_RATIO = 1.0;     // Отношение ширины изображения к высоте.
	int    IMAGE_WIDTH = 100;      // Ширина визуализируемого изображения в пикселях.
	int	   SAMPLES_PER_PIXEL = 10; // Количество случайных сэмплов (выборок) для каждого пикселя.

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
					ray r = get_ray(i,j);	// на каждый (i,j) пиксель генерируется samples_per_pixel пикслей (сэмплов)

					/* Antialiasing */
					pixel_color += ray_color(r, world); // суммирование значение интенсивности цветов пикслей 
														// вокруг (i.j) пикселя. 
				}
				write_color(std::cout, PIXEL_SAMPLES_SCALE * pixel_color); // записываем в выхондной поток > .ppm.
																		   // pixel_color * (1.0 / SAMPLES_PER_PIXEL): 
																		   // усредненное значение pixel_color позволяет
																		   // получить склаженное значение для (i,j) пикселя.
			}
		}
		std::clog << "\rDone.                 \n";
	}
};

#endif