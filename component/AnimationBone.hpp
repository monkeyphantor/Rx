#pragma once
#include "global.hpp"

namespace Rx{
namespace Component{
   struct PositionKey{
        float time;
        glm::vec3 position;
    };

    struct RotationKey{
        float time;
        glm::quat rotation;
    };

    struct ScalingKey{
        float time;
        glm::vec3 scaling;
    };

class AnimationBone
{
public:
    std::string name;    	
    std::vector<Component::PositionKey> positions;
    std::vector<Component::RotationKey> rotations;
    std::vector<Component::ScalingKey> scalings;
	
    std::tuple<glm::vec3, glm::quat, glm::vec3> getLocalTransform(float animationTime) const;

    std::vector<float> getKeyFrameTimes() const;

    std::string getBoneName() const { return name; }	

    int getPositionIndex(float animationTime) const;

    int getRotationIndex(float animationTime) const;

    int getScaleIndex(float animationTime) const;

private:

    float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)  const;

    glm::vec3 interpolatePosition(float animationTime) const;
    glm::quat interpolateRotation(float animationTime) const;
    glm::vec3 interpolateScaling(float animationTime) const;
};
    }
}