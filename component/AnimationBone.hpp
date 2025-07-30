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
	
    std::tuple<glm::vec3, glm::quat, glm::vec3> getLocalTransform(float animationTime);
    std::string getBoneName() const { return name; }	

    int getPositionIndex(float animationTime);

    int getRotationIndex(float animationTime);

    int getScaleIndex(float animationTime);

private:

    float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

    glm::vec3 interpolatePosition(float animationTime);

    glm::quat interpolateRotation(float animationTime);

    glm::vec3 interpolateScaling(float animationTime);
};
    }
}