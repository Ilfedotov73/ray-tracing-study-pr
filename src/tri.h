#ifndef TRI_H
#define TRI_H

class tri : public quad 
{
public:
    tri(const point3& o, const vec3& aa, const vec3& ab, shared_ptr<material> m) :
        quad(o, aa, ab, m) {}
    
    virtual bool is_interior(float a, float b, hit_record& rec) const override {
        if ((a < 0) || (b < 0) || (a + b > 1)) { return false; }

        rec.u = a;
        rec.v = b;
        return true;
    }
};

#endif