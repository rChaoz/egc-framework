#pragma once

#include "utils/glm_utils.h"


namespace transform3D
{
    // Translate matrix
    inline glm::mat4 Translate(float translateX, float translateY, float translateZ)
    {
        return glm::transpose(glm::mat4(
            1, 0, 0, translateX,
            0, 1, 0, translateY,
            0, 0, 1, translateZ,
            0, 0, 0, 1
        ));
    }

    inline glm::mat4 Translate(const glm::vec3& translate) {
        return Translate(translate.x, translate.y, translate.z);
    }

    // Scale matrix
    inline glm::mat4 Scale(float scaleX, float scaleY, float scaleZ)
    {
        return glm::transpose(glm::mat4(
            scaleX, 0, 0, 0,
            0, scaleY, 0, 0,
            0, 0, scaleZ, 0,
            0, 0, 0, 1
        ));
    }

    inline glm::mat4 Scale(float scale) {
        return Scale(scale, scale, scale);
    }

    // Rotate matrix relative to the OZ axis
    inline glm::mat4 RotateOZ(float radians)
    {
        return glm::transpose(glm::mat4(
            1, 0, 0, 0,
            0, cosf(radians), -sinf(radians), 0,
            0, sinf(radians), cosf(radians), 0,
            0, 0, 0, 1
        ));
    }

    // Rotate matrix relative to the OY axis
    inline glm::mat4 RotateOY(float radians)
    {
        return glm::transpose(glm::mat4(
            cosf(radians), 0, -sinf(radians), 0,
            0, 1, 0, 0,
            sinf(radians), 0, cosf(radians), 0,
            0, 0, 0, 1
        ));
    }

    // Rotate matrix relative to the OX axis
    inline glm::mat4 RotateOX(float radians)
    {
        return glm::transpose(glm::mat4(
            cosf(radians), -sinf(radians), 0, 0,
            sinf(radians), cosf(radians), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ));
    }

    // Rotate matrix relative to OX, then OY, then OZ
    inline glm::mat4 Rotate(float ox, float oy, float oz) {
        return RotateOZ(oz) * RotateOY(oy) * RotateOX(ox);
    }
}   // namespace transform3D
