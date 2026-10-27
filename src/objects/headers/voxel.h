//
// Created by cayde on 5/09/2026.
//

#ifndef OPENGL_TUTORIAL_BLOCK_H
#define OPENGL_TUTORIAL_BLOCK_H
#include "object.h"

enum class BlockType {
    DIRT,
    OAK_LOG,
    SAND,
    COBBLESTONE,
    BLOCK_OF_IRON,
    BLOCK_OF_DIAMOND
};

class voxel : public object {
public:
    voxel(BlockType type, const TextureAtlasData& tad, glm::vec3 spawnPos);

    void create_obj_vao() override;

    BlockType blockType = BlockType::DIRT;
};


#endif //OPENGL_TUTORIAL_BLOCK_H
