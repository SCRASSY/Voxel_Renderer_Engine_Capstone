//
// Created by cayde on 19/10/2026.
//

#ifndef OPENGL_TUTORIAL_CHUNK_H
#define OPENGL_TUTORIAL_CHUNK_H

#include "object.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

constexpr int CHUNK_SIZE = 8;

struct FaceData {
    int neighbour[3];
    float corners[4][3];
    float uvs[4][2];
};

struct Vertex {
    // pos
    float x = 0;
    float y = 0;
    float z = 0;

    // normals
    float nx = 0;
    float ny = 0;
    float nz = 0;

    // uv coords
    float u = 0;
    float v = 0;
};

class Chunk {
public:
    uint8_t voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {}; // all indices = 0 / air
    glm::vec3 worldPos{0.0f};

    GLuint chunkVAO = 0;
    GLuint chunkVBO = 0;

    int vertexCount = 0;

    // TEXTURE DATA
    glm::vec2 textureTileSize = glm::vec2(0.0f);

    const FaceData FACES[6] = {
        // +Y (top)
        {{ 0, 1, 0}, {{0,1,0},{0,1,1},{1,1,1},{1,1,0}}, {{0,0},{0,1},{1,1},{1,0}}},
        // -Y (bottom)
        {{ 0,-1, 0}, {{0,0,0},{1,0,0},{1,0,1},{0,0,1}}, {{0,0},{1,0},{1,1},{0,1}}},
        // +X (right)
        {{ 1, 0, 0}, {{1,0,0},{1,1,0},{1,1,1},{1,0,1}}, {{1,0},{1,1},{0,1},{0,0}}},
        // -X (left)
        {{-1, 0, 0}, {{0,0,0},{0,0,1},{0,1,1},{0,1,0}}, {{0,0},{1,0},{1,1},{0,1}}},
        // +Z (front)
        {{ 0, 0, 1}, {{0,0,1},{1,0,1},{1,1,1},{0,1,1}}, {{0,0},{1,0},{1,1},{0,1}}},
        // -Z (back)
        {{ 0, 0,-1}, {{0,0,0},{0,1,0},{1,1,0},{1,0,0}}, {{1,0},{1,1},{0,1},{0,0}}},
    };

    std::vector<std::pair<uint8_t, std::vector<glm::vec2>>> voxelTextureTable = {};

    Chunk(const TextureAtlasData &tad, glm::vec3 spawnPos);

    bool is_solid(int x, int y, int z) const;
    void build_chunk(Chunk& chunk);
    void draw_chunk();
    glm::vec2 get_voxel_face_uv_coords();
};


#endif //OPENGL_TUTORIAL_CHUNK_H
