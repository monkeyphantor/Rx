#include "AnimationBone.hpp"

namespace Rx {
namespace Component {

    std::tuple<glm::vec3, glm::quat, glm::vec3> AnimationBone::getLocalTransform(float animationTime)
    {
        glm::vec3 translation = interpolatePosition(animationTime);
        glm::quat rotation = interpolateRotation(animationTime);
        glm::vec3 scale = interpolateScaling(animationTime);

        return {translation, rotation, scale};
    }

    int AnimationBone::getPositionIndex(float animationTime)
    {
        for (int index = 0; index < positions.size() - 1; ++index)
        {
            if (positions[index].time <= animationTime && animationTime < positions[index + 1].time)
            {
                return index;
            }
        }
        return static_cast<int>(positions.size()) - 2;
    }

    int AnimationBone::getRotationIndex(float animationTime)
    {
        for (int index = 0; index < rotations.size() - 1; ++index)
        {
            if (rotations[index].time <= animationTime && animationTime < rotations[index + 1].time)
            {
                return index;
            }
        }
        return static_cast<int>(rotations.size()) - 2;
    }

    int AnimationBone::getScaleIndex(float animationTime)
    {
        for (int index = 0; index < scalings.size() - 1; ++index)
        {
            if (scalings[index].time <= animationTime && animationTime < scalings[index + 1].time)
            {
                return index;
            }
        }
        return static_cast<int>(scalings.size()) - 2;
    }


    float AnimationBone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    glm::vec3 AnimationBone::interpolatePosition(float animationTime)
    {
        if (1 == positions.size()){
            return positions[0].position;
        }

        int p0Index = getPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = getScaleFactor(positions[p0Index].time,
            positions[p1Index].time, animationTime);
        glm::vec3 finalPosition = glm::mix(positions[p0Index].position,
            positions[p1Index].position, scaleFactor);
        return finalPosition;
    }

    glm::quat AnimationBone::interpolateRotation(float animationTime)
    {
        if (1 == rotations.size())
        {
            auto rotation = glm::normalize(rotations[0].rotation);
            return rotation;
        }

        int p0Index = getRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = getScaleFactor(rotations[p0Index].time,
            rotations[p1Index].time, animationTime);
        glm::quat finalRotation = glm::slerp(rotations[p0Index].rotation,
            rotations[p1Index].rotation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return finalRotation;
    }

    glm::vec3 AnimationBone::interpolateScaling(float animationTime)
    {
        if (1 == scalings.size()){
            return scalings[0].scaling;
        }

        int p0Index = getScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = getScaleFactor(scalings[p0Index].time,
            scalings[p1Index].time, animationTime);
        glm::vec3 finalScale = glm::mix(scalings[p0Index].scaling, scalings[p1Index].scaling, 
        scaleFactor);
        return finalScale;
    }
}
}