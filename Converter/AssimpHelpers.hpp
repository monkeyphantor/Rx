#pragma once

#include "Global.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

template<typename T>
void writeToData(std::vector<char>& data, T* values, uint64_t multiplicity) {
    data.insert(data.end(), reinterpret_cast<char*>(values), reinterpret_cast<char*>(values) + sizeof(T) * multiplicity);
}

namespace AssimpGLMHelpers {

    inline glm::mat4 convertToMat4(const aiMatrix4x4& from) {
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    inline glm::vec3 convertToVec3(const aiVector3D& from) {
        return glm::vec3(from.x, from.y, from.z);
    }

    inline glm::quat convertToQuat(const aiQuaternion& from) {
        return glm::quat(from.w, from.x, from.y, from.z);
    }

}