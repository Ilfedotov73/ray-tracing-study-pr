#ifndef RT_STB_IMAGE_H
#define RT_STB_IMAGE_H

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

typedef unsigned char uchar;

class rt_image 
{
private:
const int   bytes_per_pixel = 3;
float*      fdata = nullptr;
uchar*      bdata = nullptr;
int         image_width = 0;
int         image_height = 0;
int         bytes_per_scanline = 0;

static uchar float_to_byte(float value) 
{
    if (value <= 0.0) { return 0; }
    if (1.0 <= value) { return 255; }
    return static_cast<uchar>(256.0 * value);
}

void convert_to_bytes()
{
    int total_bytes = image_width * image_height * bytes_per_pixel;
    bdata = new uchar[total_bytes];

    uchar* bptr = bdata;
    float* fptr = fdata;
    for (int i = 0; i < total_bytes; ++i, ++fptr, ++bptr) { *bptr = float_to_byte(*fptr); }
}

static int clamp(int x, int low, int high) 
{
    if (x < low) { return low; }
    if (x < high) { return x; }
    return high - 1;
}
public:
    rt_image() {}
    rt_image(const char* image_filename) 
    {
        std::string filename = std::string(image_filename);
        char* imgdir = getenv("RTW_IMAGES");

        if (imgdir && load(std::string(imgdir) + "/" + image_filename)) { return; }
        if (load(filename)) { return; }
        if (load("images/" + filename)) { return; }
        if (load("../images/" + filename)) { return; }
        if (load("../../images/" + filename)) { return; }
        if (load("../../../images/" + filename)) { return; }
        if (load("../../../../images/" + filename)) { return; }
        if (load("../../../../../images/" + filename)) { return; }
        if (load("../../../../../../images/" + filename)) { return; }

        std::cerr << "ERROR: Could no load image file '" << image_filename << "'.\n";
    }
    ~rt_image() {
        delete[] bdata;
        STBI_FREE(fdata);
    }

    bool load(const std::string& filename) 
    {
        int n = bytes_per_pixel;
        fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
        if (fdata == nullptr) { return false; }

        bytes_per_scanline = image_width * bytes_per_pixel;
        convert_to_bytes();
        return true;
    }

    int width() const { return (fdata == nullptr) ? 0 : image_width; }
    int height() const { return (fdata == nullptr) ? 0 : image_height; }

    const uchar* pixel_data(int x, int y) const 
    {
        static uchar magenta[] = { 255, 0, 255 };
        if (bdata == nullptr) { return magenta; }

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);
        
        return bdata + y*bytes_per_scanline + x*bytes_per_pixel;
    }
};

#endif