#ifndef TRI_H
#define TRI_H

class tri : public quad 
{
private:
    std::vector<std::pair<float, float>> texcoords;
public:
    tri(const point3& o, const vec3& aa, const vec3& ab, shared_ptr<material> m, std::vector<std::pair<float, float>> texcoords) :
        quad(o, aa, ab, m), texcoords(texcoords) {}
    
    virtual bool is_interior(float a, float b, hit_record& rec) const override {
        if ((a < 0) || (b < 0) || (a + b > 1)) { return false; }

        rec.u = a;
        rec.v = b;

        const float w = 1.0f - rec.u - rec.v;
        std::tie (rec.u, rec.v) = std::make_pair(w*texcoords[0].first + rec.u*texcoords[1].first + rec.v*texcoords[2].first,
        w*texcoords[0].second + rec.u*texcoords[1].second + rec.v*texcoords[2].second);

        return true;
    }
};
#endif