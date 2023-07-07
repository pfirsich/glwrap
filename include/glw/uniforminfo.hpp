#pragma once

#include <string>

#include "glad/glad.h"

namespace glw {
class UniformInfo {
public:
    enum class Type : GLenum {
        Invalid = 0,
        Float = GL_FLOAT,
        Vec2 = GL_FLOAT_VEC2,
        Vec3 = GL_FLOAT_VEC3,
        Vec4 = GL_FLOAT_VEC4,
        Double = GL_DOUBLE,
        Int = GL_INT,
        IVec2 = GL_INT_VEC2,
        IVec3 = GL_INT_VEC3,
        IVec4 = GL_INT_VEC4,
        UInt = GL_UNSIGNED_INT,
        UVec2 = GL_UNSIGNED_INT_VEC2,
        UVec3 = GL_UNSIGNED_INT_VEC3,
        UVec4 = GL_UNSIGNED_INT_VEC4,
        Bool = GL_BOOL,
        BVec2 = GL_BOOL_VEC2,
        BVec3 = GL_BOOL_VEC3,
        BVec4 = GL_BOOL_VEC4,
        Mat2 = GL_FLOAT_MAT2,
        Mat3 = GL_FLOAT_MAT3,
        Mat4 = GL_FLOAT_MAT4,
        Mat2x3 = GL_FLOAT_MAT2x3,
        Mat2x4 = GL_FLOAT_MAT2x4,
        Mat3x2 = GL_FLOAT_MAT3x2,
        Mat3x4 = GL_FLOAT_MAT3x4,
        Mat4x2 = GL_FLOAT_MAT4x2,
        Mat4x3 = GL_FLOAT_MAT4x3,
        Sampler1D = GL_SAMPLER_1D,
        Sampler2D = GL_SAMPLER_2D,
        Sampler3D = GL_SAMPLER_3D,
        SamplerVube = GL_SAMPLER_CUBE,
        Sampler1DShadow = GL_SAMPLER_1D_SHADOW,
        Sampler2DShadow = GL_SAMPLER_2D_SHADOW,
        Sampler1DArray = GL_SAMPLER_1D_ARRAY,
        Sampler2DArray = GL_SAMPLER_2D_ARRAY,
        Sampler1DArrayshadow = GL_SAMPLER_1D_ARRAY_SHADOW,
        Sampler2DArrayshadow = GL_SAMPLER_2D_ARRAY_SHADOW,
        Sampler2DMs = GL_SAMPLER_2D_MULTISAMPLE,
        Sampler2DMsArray = GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
        SamplerCubeShadow = GL_SAMPLER_CUBE_SHADOW,
        SamplerBuffer = GL_SAMPLER_BUFFER,
        Sampler2DRect = GL_SAMPLER_2D_RECT,
        Sampler2DRectShadow = GL_SAMPLER_2D_RECT_SHADOW,
        ISampler1D = GL_INT_SAMPLER_1D,
        ISampler2D = GL_INT_SAMPLER_2D,
        ISampler3D = GL_INT_SAMPLER_3D,
        ISamplercube = GL_INT_SAMPLER_CUBE,
        ISampler1DArray = GL_INT_SAMPLER_1D_ARRAY,
        ISampler2DArray = GL_INT_SAMPLER_2D_ARRAY,
        ISampler2DMs = GL_INT_SAMPLER_2D_MULTISAMPLE,
        ISampler2DMsArray = GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        ISamplerBuffer = GL_INT_SAMPLER_BUFFER,
        ISampler2DRect = GL_INT_SAMPLER_2D_RECT,
        USampler1D = GL_UNSIGNED_INT_SAMPLER_1D,
        USampler2D = GL_UNSIGNED_INT_SAMPLER_2D,
        USampler3D = GL_UNSIGNED_INT_SAMPLER_3D,
        USamplerCube = GL_UNSIGNED_INT_SAMPLER_CUBE,
        USampler1DArray = GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
        USampler2DArray = GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
        USampler2DMs = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
        USampler2DMsArray = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        USamplerBuffer = GL_UNSIGNED_INT_SAMPLER_BUFFER,
        USampler2DRect = GL_UNSIGNED_INT_SAMPLER_2D_RECT
    };

    std::string name = {};
    int index = -1;
    int size = 0;
    Type type = Type::Invalid;
};
}
