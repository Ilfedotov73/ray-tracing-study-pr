#ifndef RT_OBJ_LOADER_H
#define RT_OBJ_LOADER_H

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

class obj_loader
{
private:
    shared_ptr<hittable_list> sides;
public:
    obj_loader() { sides = make_shared<hittable_list>(); }

    void load_models(const char* filename) 
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, "", false)) {
            std::cerr << warn << " " << err << ".\n";
        }

        shared_ptr<material> green = make_shared<lambertian>(color(.12f, .45f, .15f));

        // Цикл по объектам.
        for (size_t s = 0, s_size = shapes.size(); s < s_size; ++s) { 
            std::cerr << "Shape:" << s << "\n";
            // Цикл по полигонам.
            size_t index_offset = 0;
            for (size_t f = 0, size = shapes[s].mesh.num_face_vertices.size(); f < size; ++f) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
                std::vector<float> quad_vertices;
                // Цикл по вершинам полигона.
                for (size_t v = 0; v < fv; ++v) {
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                    tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                    tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                    quad_vertices.push_back(vx);
                    quad_vertices.push_back(vy);
                    quad_vertices.push_back(vz);
                }
                index_offset += fv;

                point3 v0 = point3(quad_vertices[0], quad_vertices[1], quad_vertices[2]);
                point3 v1 = point3(quad_vertices[3], quad_vertices[4], quad_vertices[5]);
                point3 v2 = point3(quad_vertices[6], quad_vertices[7], quad_vertices[8]);
                point3 v3 = point3(quad_vertices[9], quad_vertices[10], quad_vertices[11]);

                point3 Q = point3(quad_vertices[0], quad_vertices[1], quad_vertices[2]);
                point3 U = point3(quad_vertices[3], quad_vertices[4], quad_vertices[5]) - Q;
                point3 V = point3(quad_vertices[9], quad_vertices[10], quad_vertices[11]) - Q;

                sides->add(make_shared<quad>(Q, U, V, green));

                std:: cerr << "Quad vertices" << f << ": ";
                for (int i = 0; i < fv; ++i) {
                    std::cerr << "(" << quad_vertices[i*3] << quad_vertices[i*3 + 1] << quad_vertices[i*3 + 2] << ")" << " ";
                }
                std::cerr << '\n';
            }
        }
    }

    shared_ptr<hittable_list> get_sides() { return sides; }
};

#endif