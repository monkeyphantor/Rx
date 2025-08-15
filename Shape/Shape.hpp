#pragma once

#include "Global.hpp"
#include "Vertex.hpp"
#define PI 3.14159265359

namespace Rx{
    namespace Shape{

        struct ColorCube{

                ColorCube(glm::vec3 halfSize, glm::vec4 color, glm::vec3 offset = glm::vec3(0,0,0));

            private:
                std::vector<Vertex::Color> vertices;
            
                std::vector<uint32_t> indices = {
                    // Front face
                    0, 1, 2, 2, 3, 0,
                    // Back face
                    4, 7, 6, 6, 5, 4,
                    // Left face
                    8, 10, 11, 9, 10, 8,
                    // Right face
                    12, 14, 13, 15, 14, 12,
                    // Top face
                    16, 17, 18, 18, 19, 16,
                    // Bottom face
                    20, 23, 22, 22, 21, 20
                };

            public:
                const std::vector<Vertex::Color>& getVertices(){
                    return vertices;
                }

                const std::vector<uint32_t>& getIndices(){
                    return indices;
                }
        };


    struct ColorSphere{
        private:
        std::vector<Vertex::Color> vertices;
        std::vector<uint32_t> indices;

        public:
        ColorSphere(int radius, int latitudes, int longitudes, glm::vec3 color, glm::vec3 offset = glm::vec3(0,0,0));
        private:
        void GenerateSphereSmooth(int radius, int latitudes, int longitudes, glm::vec3 color, glm::vec3 offset);

    public:

        const std::vector<Vertex::Color>& getVertices(){
            return vertices;
        }

        const std::vector<uint32_t>& getIndices(){
            return indices;
        }

    };

    struct TextureCube{

                TextureCube(glm::vec3 halfSize, glm::vec3 offset = glm::vec3(0,0,0));

            private:
                std::vector<Vertex::Texture> vertices;
            
                std::vector<uint32_t> indices = {
                    // Front face
                    0, 1, 2, 2, 3, 0,
                    // Back face
                    4, 7, 6, 6, 5, 4,
                    // Left face
                    8, 10, 11, 9, 10, 8,
                    // Right face
                    12, 14, 13, 15, 14, 12,
                    // Top face
                    16, 17, 18, 18, 19, 16,
                    // Bottom face
                    20, 23, 22, 22, 21, 20
                };

            public:
                const std::vector<Vertex::Texture>& getVertices(){
                    return vertices;
                }

                const std::vector<uint32_t>& getIndices(){
                    return indices;
                }
    };


    struct TextureSphere{
        private:
        std::vector<Vertex::Texture> vertices;
        std::vector<uint32_t> indices;

        public:
        TextureSphere(int radius, int latitudes, int longitudes, glm::vec3 offset = glm::vec3(0,0,0));
        private:
        void GenerateSphereSmooth(int radius, int latitudes, int longitudes, glm::vec3 offset);

    public:

        const std::vector<Vertex::Texture>& getVertices(){
            return vertices;
        }

        const std::vector<uint32_t>& getIndices(){
            return indices;
        }

    };
    
    }
}