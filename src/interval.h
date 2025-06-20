#ifndef INTERVAL_H
#define INTERVAL_H

class interval
{
public:
	double min, max;
	static const interval empty, universe;

	interval() : min(+INF), max(-INF) {} // дефолтный интервал пустой!
	interval(double min, double max) : min(min), max(max) {}

	double size() const { return max - min; }
	
	bool contrains(double x) const { return min <= x && x <= max; }

	bool surrounds(double x) const { return min < x && x < max; }
}; 

const interval interval::empty		= interval(+INF, -INF);
const interval interval::universe	= interval(-INF, +INF);

#endif