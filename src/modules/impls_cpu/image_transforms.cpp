#include "image_transforms.h"
#include <cstring>
#include <cblas.h>
#include <cmath>
#include <algorithm>
using namespace std;

void reflect(matrix& img, bool horizontal, bool vertical)
{
    if (!horizontal && !vertical) return;

    unsigned char* newArr = new unsigned char[img.width * img.height * img.components_num];

    for (unsigned y = 0; y < img.height; ++y) {
        for (unsigned x = 0; x < img.width; ++x) {
            unsigned target_x = vertical ? (img.width - 1 - x) : x;
            unsigned target_y = horizontal ? (img.height - 1 - y) : y;

            unsigned char* src = img.get(x, y);
            unsigned char* dst = &newArr[(target_y * img.width + target_x) * img.components_num];
            memcpy(dst, src, img.components_num);
        }
    }

    delete[] img.arr;
    img.arr = newArr;
}

void crop(matrix& img, unsigned crop_left, unsigned crop_top, unsigned crop_right, unsigned crop_bottom)
{
    unsigned new_width = img.width - crop_left - crop_right;
    unsigned new_height = img.height - crop_top - crop_bottom;


    if (new_width <= 0 || new_height <= 0)
    {
        return;
    }

    if (crop_left + crop_right > img.width || crop_top + crop_bottom > img.height)
    {
        return;
    }

    unsigned char* newArr = new unsigned char[new_width * new_height * img.components_num];
    unsigned char* src = img.arr + (crop_top * img.width + crop_left) * img.components_num;
    unsigned char* dst = newArr;

    for (unsigned i = 0; i < new_height; ++i)
    {
        memcpy(dst, src, new_width * img.components_num);
        src += img.width * img.components_num;
        dst += new_width * img.components_num;
    }


    delete[] img.arr;

    img.set_arr_interlaced(newArr, new_width, new_height);

}

void shear(matrix& img, float shx, float shy) {
    // Рассчитываем новые размеры
    unsigned new_width = static_cast<unsigned>(img.width + std::abs(shy)*img.height);
    unsigned new_height = static_cast<unsigned>(img.height + std::abs(shx)*img.width);

    unsigned char* newArr = new unsigned char[new_width * new_height * img.components_num]();
    std::fill(newArr, newArr + new_width*new_height*img.components_num, 255);

    // Корректируем смещения для разных направлений сдвига
    float offset_x = (shy > 0) ? 0 : std::abs(shy)*img.height;
    float offset_y = (shx > 0) ? 0 : std::abs(shx)*img.width;

    for (unsigned y = 0; y < new_height; ++y) {
        for (unsigned x = 0; x < new_width; ++x) {
            // Правильное обратное преобразование координат
            float src_x = (x - offset_x) - shy*(y - offset_y);
            float src_y = (y - offset_y) - shx*(x - offset_x);

            if (src_x >= 0 && src_x < img.width && src_y >= 0 && src_y < img.height) {
                bilinear_interpolate(img, src_x, src_y, &newArr[(y*new_width + x)*img.components_num]);
            }
        }
    }

    delete[] img.arr;
    img.set_arr_interlaced(newArr, new_width, new_height);
}

void rotate(matrix& img, float angle) {
    float radians = angle * M_PI / 180.0f;
    float cos_theta = cos(radians);
    float sin_theta = sin(radians);

    float R[4] = {
        cos_theta, -sin_theta,
        sin_theta,  cos_theta
    };

    float half_height = img.height * 0.5f;
    float half_width = img.width * 0.5f;

    float corners[4][2] = {
        {-half_width, -half_height},
        { half_width, -half_height},
        {-half_width,  half_height},
        { half_width,  half_height}
    };

    float rotated_corners[8];
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                4, 2, 2, 1.0f,
                &corners[0][0], 2,
                R, 2,
                0.0f, rotated_corners, 2);

    float min_x = rotated_corners[0], max_x = rotated_corners[0];
    float min_y = rotated_corners[1], max_y = rotated_corners[1];

    for (int i = 1; i < 4; ++i) {
        min_x = std::min(min_x, rotated_corners[i*2]);
        max_x = std::max(max_x, rotated_corners[i*2]);
        min_y = std::min(min_y, rotated_corners[i*2+1]);
        max_y = std::max(max_y, rotated_corners[i*2+1]);
    }

    unsigned new_width = static_cast<unsigned>(round(max_x - min_x));
    unsigned new_height = static_cast<unsigned>(round(max_y - min_y));

    unsigned char* newArr = new unsigned char[new_width * new_height * img.components_num];

    float new_half_width = new_width * 0.5f;
    float new_half_height = new_height * 0.5f;

    float R_inv[4] = {
        cos_theta,  sin_theta,
        -sin_theta, cos_theta
    };

    for (unsigned y = 0; y < new_height; ++y) {
        for (unsigned x = 0; x < new_width; ++x) {
            float coords[2] = {
                x - new_half_width,
                y - new_half_height
            };

            float src_coords[2];
            cblas_sgemv(CblasRowMajor, CblasNoTrans,
                       2, 2, 1.0f,
                       R_inv, 2,
                       coords, 1,
                       0.0f, src_coords, 1);

            float src_x = src_coords[0] + half_width;
            float src_y = src_coords[1] + half_height;

            if (src_x >= 0 && src_x <= img.width-1 &&
                src_y >= 0 && src_y <= img.height-1) {
                unsigned char* pixel = &newArr[(y * new_width + x) * img.components_num];
                bilinear_interpolate(img, src_x, src_y, pixel);
            } else {
                unsigned char* pixel = &newArr[(y * new_width + x) * img.components_num];
                memset(pixel, 255, img.components_num);
            }
        }
    }
    delete[] img.arr;
    img.set_arr_interlaced(newArr, new_width, new_height);
}
