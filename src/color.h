/***********************************************************************************
* ������������ ���� color.h ���������� ����� ���� ��������� ������� ��� ������ 
* ����� ����������� ������� � ����������� ����� ������.
* 
* ��� color �������� ����������� ��� vec3.
***********************************************************************************/

#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

using color = vec3;

void write_color(std::ostream& out, const color& pix_color)
{
	double r = pix_color.x();
	double g = pix_color.y();
	double b = pix_color.z();

	/* �������������� �������� ���������� � ��������� [0,1] � �������� ������ [0,255] */
	static const interval intensity(0.000, 0.999); // clipping
	int rbyte = int(255.999 * intensity.clip(r));
	int gbyte = int(255.999 * intensity.clip(g));
	int bbyte = int(255.998 * intensity.clip(b));

	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
#endif