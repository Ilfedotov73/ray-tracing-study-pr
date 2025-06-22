#ifndef INTERVAL_H
#define INTERVAL_H

class interval
{
public:
	double min, max;
	static const interval empty, universe;

	interval() : min(+INF), max(-INF) {} // ��������� �������� ������!
	interval(double min, double max) : min(min), max(max) {}

	double size() const { return max - min; }
	
	bool contrains(double x) const { return min <= x && x <= max; } // ��������� �����������

	bool surrounds(double x) const { return min < x && x < max; } // ������� �����������

	double clamp(double x) const
	{
		if (x < min) { return min; }
		if (x > max) { return max;}
		return x;
	}
}; 

const interval interval::empty		= interval(+INF, -INF);
const interval interval::universe	= interval(-INF, +INF);

#endif