#ifndef INTERVAL_H
#define INTERVAL_H

class interval
{
public:
	double min, max;
	static const interval empty, universe;

	interval() : min(+INF), max(-INF) {} // дефолтный интервал пустой!
	interval(double min, double max) : min(min), max(max) {}

	/* 
	 * \brief Опередение ограничивающего объема протяженностью от границы ограничивающего
	 * объема в один момент времени до ограницы того же ограничивающего объема в другой 
	 * момент времени.
	 */
	interval(const interval& a, const interval& b)
	{
		min = a.min <= b.min ? a.min : b.min;
		max = a.max >= b.max ? a.max : b.max;
	}

	double size() const { return max - min; }
	
	bool contrains(double x) const { return min <= x && x <= max; } // нестрогое неравенство

	bool surrounds(double x) const { return min < x && x < max; } // строгое неравенство

	double clamp(double x) const
	{
		if (x < min) { return min; }
		if (x > max) { return max;}
		return x;
	}

	interval expand(double delta) const
	{
		double padding = delta/2;
		return interval(min - padding, max + padding);
	}
}; 

const interval interval::empty		= interval(+INF, -INF);
const interval interval::universe	= interval(-INF, +INF);

#endif