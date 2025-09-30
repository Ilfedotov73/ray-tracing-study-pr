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
#include "material.h"
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
	vec3   U, W, V;				// Ортонормированный базис
	vec3   FOCUS_DISK_U, 		// Ортонормированный базис линзы
		   FOCUS_DISK_V;		

	void initialize()
	{
		/* image settings */
		IMAGE_HEIGHT = int(IMAGE_WIDTH / ASPECT_RATIO);
		IMAGE_HEIGHT = (IMAGE_HEIGHT < 1) ? 1 : IMAGE_HEIGHT; // Высота изображения не должна быть < 1.
		
		/* camera settings */
		CAMERA_CENTER = LOOKFROM;
		
		/* viewport settings */
		double theta = degrees_to_radians(VFOV);
		double h = std::tan(theta/2);
		
		double VIEWPORT_HEIGHT = 2.0 * h * FOCUS_DIST; 
		double VIEWPORT_WIDTH = VIEWPORT_HEIGHT * (double(IMAGE_WIDTH) / IMAGE_HEIGHT);

		W = unitv(LOOKFROM - LOOKAT);
		U = unitv(cross(VUP, W));
		V = cross(W, U);

		vec3   VIEWPORT_U = VIEWPORT_WIDTH * U;		// Горизонтальный вектор области просмотра.
		vec3   VIEWPORT_V = -VIEWPORT_HEIGHT * V;	// Вертикальный вектор области просмотра.

		/* calculate delta vectotrs */
		PIXEL_DELTA_U = VIEWPORT_U / IMAGE_WIDTH;
		PIXEL_DELTA_V = VIEWPORT_V / IMAGE_HEIGHT;

		/* calculate location viewport upper left/pixel(0,0) */
		point3 VIEWPORT_UPPER_LEFT = CAMERA_CENTER - (FOCUS_DIST * W) - VIEWPORT_U / 2 - VIEWPORT_V / 2;
		PIXEL_LOC_00 = VIEWPORT_UPPER_LEFT + 0.5 * (PIXEL_DELTA_U + PIXEL_DELTA_V);
		
		/* calculate the camera focus lens basis vectorcs */
		double focus_radius = FOCUS_DIST * std::tan(degrees_to_radians(FOCUS_ANGLE/2)); // Наклон радиуса линзы.
		FOCUS_DISK_U = U * focus_radius;
		FOCUS_DISK_V = V * focus_radius;

		PIXEL_SAMPLES_SCALE = 1.0 / SAMPLES_PER_PIXEL;
	}
	
	/*
	 * Функция ray_color() принимает луч, максимальнуб глубину отражений луча и 
	 * константную ссылку на объект реализующий абстрактный класс hittable - 
	 * hittable_list, который содержит массив указателей на поверхности 
	 * (также реализующие hittable).
	 *
	 * В теле функции создается объект класса hit_record rec, который регистрирует
	 * информацию о пересечении объекта лучом. Если луч пересекает один из поверхностей
	 * world.hit() вернет true и ray_color вернет цвет пикселя "затененного" нормалью.
	 *
	 * Функция ray_color() является рекурсивной, испускаемые лучи рекурсивно отражаются
	 * до тех пор, пока перестанут встречать объекты для пересечения. Что является 
	 * потенциальной ошибкой переполнения стека или проблема занятости (в случа много-
	 * поточности). Чтобы избежать возникнования ошибки, каждый луч ограничен заданным
	 * числом отражений.
	 * 
	 * Отражение каждого луча специфично для материала пересеченной поверхности, оно 
	 * либо задается распределением ламберта, либо зеракально отражается по длине 
	 * проекции луча к нормали. Логика отражения инкапсулирована материалу.
	 * 
	 * Если пересечение с объектами не происходит,то рассчитывается градиент от белого
	 * к синему, в зависимости о высоты пикселя. Чем ближе значение a к единице, тем
	 * ближе значение к синему (0.5, 0.7, 1.0) - рассчет неба для сцены.
	 * 
	 * Из-за округления чисел с плавающей точкой точка P(t) может быть смещена так, что
	 * может быть немного выше или немного ниже поверхности. Из чего следует возмож-
	 * ность возникновения ошибки, что когда P(t) смещена вниз от поверхности, то она
	 * можнт внось пересечься с ней. Чтобы решить эту проблемы, можно попытаться игно-
	 * рировать точки близкие к нулю через задание минимального значения интервала 
	 * допустимых знчений, значением 0.001.
	*/
	color ray_color(const ray& r, int max_depth, const hittable& world) const
	{
		if (max_depth <= 0) { return color(0,0,0); }

		hit_record rec;
		if (world.hit(r, interval(0.001, INF), rec)) {                 
			ray   scattered;	// Рассеивающий луч
			color attenuation;	// Цвет затухания интенсивности глобального освещения (цвета градиента sky).
			
			/* Вычисления отражения на основе материала поверхности */
			if (rec.mat->scatter(r, rec, attenuation, scattered))
				return attenuation * ray_color(scattered, max_depth-1, world);
			return color(0,0,0);
		}

		/* sky */
		vec3 unit_direction = unitv(r.direction());
		double a = 0.5 * (unit_direction.y() + 1.0); // [-1;1] -> [0;1], 0.0 <= a <= 1.0
		/* linear interpolation */
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0); // a = 1.0 -> синий, a = 0.0 -> белый
	}
	
	/*
	 * Функция get_ray() возвращает луч исходящий из фокусирующей линзы камеры в произольно 
	 * выбранную точку вокруг местоположения (i,j) пикселя (область фокусировки камеры). 
	 * Что представляет собой сглаживание методом единичной квадратной области.
	*/
	ray get_ray(int i, int j) const 
	{
		vec3 offset = sample_square(); // инициализация некоторой случайной точки из диапазона значений
									   // единичной квадратной области.

		// Определение сэмплируемого пикселя (т.е. случайного пикселя вокруг (i,j) пикселя) из области просмотра.							   
		point3 pixel_sample = PIXEL_LOC_00 + ((i + offset.x()) * PIXEL_DELTA_U) 
										   + ((j + offset.y()) * PIXEL_DELTA_V); 
		/*
		 * Фокусирующая линза позволяет создать размытие изображения вне области фокусировки камеры. 
		 * Для этого требуется испускать луч не из центра камеры, а из случайной точки на линзе в 
		 * диапазоне [-1, 1] + учет размера линзы, размер которой пропорционален расстоянию от 
		 * камеры до фокусирующей плоскости (области просмотра).
		*/
		point3 ray_origin = (FOCUS_ANGLE <= 0) ? CAMERA_CENTER : focus_disk_sample(); 
		point3 ray_direction = pixel_sample - ray_origin; // компенсируем расстояние от точки начала координа до камеры, 
														  // для корректного перечечения пикселя области просмотра.
		double ray_time = random_double();
		return ray(ray_origin, ray_direction, ray_time);
	}

	/* Возращает вектор до случайной точки в единичной квадратной области, в диапазоне [-0.5,-0.5] - [0.5,0.5] */
	vec3 sample_square() const { return vec3(random_double() - 0.5, random_double() - 0.5, 0); }

	/* Возвращает случайную точку на линзе */
	point3 focus_disk_sample() const 
	{
		vec3 lens = random_in_unit_disk();
		return CAMERA_CENTER + (lens[0] * FOCUS_DISK_U) + (lens[1] * FOCUS_DISK_V);
	}

public:
	/* image settings */
	double ASPECT_RATIO      = 1.0;     		// Отношение ширины изображения к высоте.
	int    IMAGE_WIDTH       = 100;     		// Ширина визуализируемого изображения в пикселях.
	int	   SAMPLES_PER_PIXEL = 10;      		// Количество случайных сэмплов (выборок) для каждого пикселя.
	int	   MAX_DEPTH         = 10;				// Максимальное число отражений лучей в сцене.
	double VFOV				 = 90;				// Вертикальный угол обзора (поле зрения).
	point3 LOOKFROM 		 = point3(0,0,0);	// Точка положения камеры.
	point3 LOOKAT  			 = point3(0,0,-1);  // Точка направления камеры.  
	vec3   VUP               = vec3(0,1,0);		// Вектор, направленный вверх относительно обзора.

	double FOCUS_ANGLE	     = 0;				// Угол наклона линзы
	double FOCUS_DIST		 = 10;			    // Расстояние от камеры до плоскти идеальной фокусировки

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
					pixel_color += ray_color(r, MAX_DEPTH, world); // суммирование значение интенсивности цветов пикслей 
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