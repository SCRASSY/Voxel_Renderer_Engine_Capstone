//
// Created by cayde on 4/09/2026.
//

#ifndef OPENGL_TUTORIAL_OBJECT_H
#define OPENGL_TUTORIAL_OBJECT_H

#include <cstdint>
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"

struct TextureData2D {
    const char *filePath;
    GLenum textureSlot;
};
struct TextureAtlasData {
    int textureAtlasWidth = 0;
    int textureAtlasHeight = 0;
};
struct objectData {
    float *verticesArr = nullptr;
    uint32_t *indicesArr = nullptr;
    size_t verticesArrSize = 0;
    size_t indicesArrSize = 0;
};
struct Material {
    float ambient = 0.0f;
    float diffuse = 0.0f;
    float specular = 0.0f;
    float shininess = 0.0f;
};

// holds the total amount of tiles in the x and y axes in the currently used texture atlas
static constexpr int textureAtlasTileCount_x = 16;
static constexpr int textureAtlasTileCount_y = 16;

class object {
public:
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    Material material{};

    uint32_t objVAO = 0;
    objectData objData{};

    // this holds the individual uv coords of as pixel vector 2D positions from a texture atlas for each face of this object
    std::vector<glm::vec2> objTextureUVs{};

    // takes in an object data struct and creates a VAO for this object
    virtual void create_obj_vao();

    // draws this object on the screen
    void draw_obj();

    // -- HELPER FUNCTIONS --
    static glm::vec2 calculate_texture_uv_from_atlas(int x, int y, int textureWidth, int textureHeight); // finds a texture in a texture atlas based on x and y coords and returns the UV coords of the texture in the atlas
    static glm::vec2 calculate_texture_atlas_tile_size(int textureWidth, int textureHeight); // calculates the x and y pixel size for a single tile in a given texture atlas
    static float normalize_to_range(float value, float min, float max);
};


#endif //OPENGL_TUTORIAL_OBJECT_H
