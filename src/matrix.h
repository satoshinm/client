#ifndef _matrix_h_
#define _matrix_h_
#include <Eigen/Geometry>

namespace konstructs {
    using namespace Eigen;
    // Hash function for Eigen matrix and vector.
    // The code is from `hash_combine` function of the Boost library. See
    // http://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine .
    template<typename T>
    struct matrix_hash : std::unary_function<T, size_t> {
        std::size_t operator()(T const& matrix) const {
            // Note that it is oblivious to the storage order of Eigen matrix (column- or
            // row-major). It will give you the same hash value for two different matrices if they
            // are the transpose of each other in different storage order.
            size_t seed = 0;
            for (size_t i = 0; i < matrix.size(); ++i) {
                auto elem = *(matrix.data() + i);
                seed ^= std::hash<typename T::Scalar>()(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    namespace matrix {
        Matrix4f projection(int width, int height);
        Matrix4f projection_2d(const int width, const int height);
        Matrix4f projection_frustum(float left, float right, float bottom,
                                    float top, float znear, float zfar);
        Matrix4f projection_perspective(float fov, float aspect,
                                        float znear, float zfar);
    };
};

void normalize(float *x, float *y, float *z);
void mat_identity(float *matrix);
void mat_translate(float *matrix, float dx, float dy, float dz);
void mat_rotate(float *matrix, float x, float y, float z, float angle);
void mat_vec_multiply(float *vector, float *a, float *b);
void mat_multiply(float *matrix, float *a, float *b);
void mat_apply(float *data, float *matrix, int count, int offset, int stride);
void frustum_planes(float planes[6][4], int radius, float *matrix);
void mat_frustum(
    float *matrix, float left, float right, float bottom,
    float top, float znear, float zfar);
void mat_perspective(
    float *matrix, float fov, float aspect,
    float near, float far);
void mat_ortho(
    float *matrix,
    float left, float right, float bottom, float top, float near, float far);
void set_matrix_2d(float *matrix, int width, int height);
void set_matrix_3d(
    float *matrix, int width, int height,
    float x, float y, float z, float rx, float ry,
    float fov, int ortho, int radius);
void set_matrix_item(float *matrix, int width, int height, int scale);
void set_matrix_item_r(float *matrix, int width, int height, float scale, float xoffset, float yoffset, float rx, float ry, float rz);

#endif
