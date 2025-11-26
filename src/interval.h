#ifndef INTERVAL_H
#define INTERVAL_H

class interval
{
public:
	float min, max;
	static const interval empty, universe;

	interval() : min(+INF), max(-INF) {} // дефолтный интервал пустой!
	interval(float min, float max) : min(min), max(max) {}

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

	float size() const { return max - min; }
	
	bool contains(float x) const { return min <= x && x <= max; } // нестрогое неравенство

	bool surrounds(float x) const { return min < x && x < max; } // строгое неравенство

	float clamp(float x) const
	{
		if (x < min) { return min; }
		if (x > max) { return max;}
		return x;
	}

	interval expand(float delta) const
	{
		float padding = delta/2.0f;
		return interval(min - padding, max + padding);
	}
}; 

const interval interval::empty		= interval(+INF, -INF);
const interval interval::universe	= interval(-INF, +INF);

#endif