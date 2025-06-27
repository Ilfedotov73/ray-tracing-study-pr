/***********************************************************************************
* Заголовочный файд color.h определяет всего одну служебную функцию для вывода 
* цвета конкретного пикселя в стандартный поток вывода.
* 
* Тип color является псевдонимом для vec3.
***********************************************************************************/

#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

using color = vec3;

inline double liner_to_gamma(double linear_component)
{
	if (linear_component > 0) { return std::sqrt(linear_component); }
	return 0;
}

void write_color(std::ostream& out, const color& pix_color)
{
	double r = pix_color.x();
	double g = pix_color.y();
	double b = pix_color.z();

	r = liner_to_gamma(r);
	g = liner_to_gamma(g);
	b = liner_to_gamma(b);

	/* преобразование значений компонента в диапазоне [0,1] в диапазон байтов [0,255] */
	static const interval intensity(0.000, 0.999); // clipping
	int rbyte = int(255.999 * intensity.clip(r));
	int gbyte = int(255.999 * intensity.clip(g));
	int bbyte = int(255.998 * intensity.clip(b));

	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
#endif