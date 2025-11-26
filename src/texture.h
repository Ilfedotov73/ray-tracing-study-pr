#ifndef TEXTURE_H
#define TEXTURE_H

typedef unsigned char uchar;

class texture
{
public:
	virtual ~texture() = default;
	virtual color value(float u, float v, const point3& p) const = 0;
};

class solid_color : public texture
{
private:
	color albedo;
public:
	solid_color(const color& albedo) : albedo(albedo) {}
	solid_color(const float red, float green, float blue) : solid_color(color(red,green,blue)) {}
	color value(float u, float v, const point3& p) const override { return albedo; }
};

class checker_texture : public texture
{
private:
	float inv_scale;			// Для влияния на четкость/нечеткость точки (размер клетчатого узора)
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
public:
	checker_texture(float scale, shared_ptr<texture> even, shared_ptr<texture> odd)
		:inv_scale(1.0f / scale), even(even), odd(odd) {}
	checker_texture(float scale, const color& c1, const color& c2)
		:checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

	color value(float u, float v, const point3& p) const override
	{
		int x = int(floor(inv_scale * p.x()));
		int y = int(floor(inv_scale * p.y()));
		int z = int(floor(inv_scale * p.z()));

		/** 
		 * Если сумма компонентов имеет нечетное значение, возваращется постоянный цвет 
		 * одной текстуры, в противном случае возвращается другой цвет.
		 */
		bool isEven = (x + y + z) % 2 == 0;
		return isEven ? even->value(u, v, p) : odd->value(u, v, p);
	}
};

class image_texture : public texture 
{
private:
	rt_image image;
public:
	image_texture(const char* filename) : image(filename) {}
	color value(float u, float v, const point3& p) const override
	{
		if (image.height() <= 0) { return color(0.0f,1.0f,1.0f); }

		u = interval(0.0f,1.0f).clamp(u);
		v = 1.0f - interval(0.0f,1.0f).clamp(v);

		int i = int(u*image.width());
		int j = int(v*image.height());
		const uchar* pixel = image.pixel_data(i,j);

		double color_scale = 1.0f/255.0f;
		return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
	}
};

#endif