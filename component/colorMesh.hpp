#pragma once
#include "global.hpp"
#include "vertex.hpp"
namespace Rx
{
namespace Component{

struct ColorMesh{
    std::vector<Vertex::Color> vertices;
    std::vector<uint32_t> indices;
};

}


}