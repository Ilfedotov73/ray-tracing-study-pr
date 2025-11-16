#ifndef QUAD_H
#define QUAD_H

/**
 * @brief    
 * 
 */
class quad : public hittable
{
private:
    point3 Q;
    vec3 u, v;
    vec3 w;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
public:
    /**
     * @brief       Конструктор, создающий примитив -- четырехугольник (полигон).
     *              
     *              Четырехугольник задается неявной формулой, которая позволяет найти пересечение луча 
     *              с плоскостью: Ax + By + Cz + D = 0, где (A,B,C) -- компоненты нормали; (x,y,z) -- значения
     *              в любой точке на плоскости (в сторону который направлен луч от камеры); D -- коэффициент
     *              смещения нормали от начала координат (если четырехугольник берет свое начало не из точки
     *              начала координат). Также любая точка ортогональна нормали. 
     *              
     *              Если (A,B,C) = normal, a (x,y,z) -- совокупоность точек, которые удовлетворяют уравнению
     *              Ax + By + Cz = D, а также известно то, что точка Q точно лежит на плоскости, поэтому уравнение
     *              можно представить как NxQx + NyQy + NzQz = D или N * Q = D. 
     * 
     *              Конструктор определяет из уравнения занчение normal и параметр D из уравнения выше. Также 
     *              конструктор находят параметр w для упращения формулы пересечения. 
     *              
     * 
     * @param[in] Q начальный угол четырехугольника;        
     * @param[in] u вектор, представляющий первую сторону четырехугольника: Q + u, образующий смежный угол с Q;
     * @param[in] v вектор, представляющий вторую сторону четырехугольника: Q + v, образующий другой угол, смежный с Q.
     */
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
        : Q(Q), u(u), v(v), mat(mat) 
    {
        vec3 n = cross(u, v);
        normal = unitv(n);
        D = dot(normal, Q);
        w = n / dot(n,n);

        set_bounding_box();
    }

    /**
     * @brief               todo
     * 
     * @param[in] r         луч, пересекающий плоскость;   
     * @param[in] ray_t     интервал допустимых значений; 
     * @param[in] rec       запись о пересечении луча и плоскости.
     */
    bool hit(const ray& r, interval ray_t,hit_record& rec) const override
    {
        double denom = dot(normal, r.direction());
        
        if (std::fabs(denom) < 1e-8) { return false; }
        double t = (D - dot(normal, r.origin())) / denom;

        if (!ray_t.contains(t)) { return false; }

        point3 intersection = r.at(t);

        vec3 planar_hitpt_vec = intersection - Q;
        double alpha = dot(w, cross(planar_hitpt_vec, v));
        double beta = dot(w, cross(u, planar_hitpt_vec));

        if (!is_interior(alpha, beta, rec)) { return false; }

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    /**
     * @brief Функция set_bounding_box(...) создает ограничиваюший объем по двум диагоналям: Q + u и Q + v.
     *        Необходимо понимать, что четырехугольник (техничиски) является параллеллограммом и его стороны 
     *        не ортогональны.
     */
    virtual void set_bounding_box()
    {
        aabb bbox_diagonal1 = aabb(Q, Q + u + v);
        aabb bbox_diagonal2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

	aabb bounding_box() const override { return bbox; }  
    
    virtual bool is_interior(double a, double b, hit_record& rec) const {
        interval unit_interval = interval(0,1);

        if (!unit_interval.contains(a) || !unit_interval.contains(b)) { return false; }
        rec.u = a;
        rec.v = b;
        return true;
    }
};

#endif