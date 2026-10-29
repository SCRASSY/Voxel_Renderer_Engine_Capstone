//
// Created by cayde on 19/10/2026.
//

#include "../headers/chunk.h"

Chunk::Chunk(const TextureAtlasData &tad, glm::vec3 spawnPos) {
    worldPos = spawnPos;
    textureTileSize = object::calculate_texture_atlas_tile_size(tad.textureAtlasWidth, tad.textureAtlasHeight);

    voxelTextureTable = {
        // DIRT
        {
            1, {
                object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight)
            }
        },
        // SAND
        {
            2, {
                object::calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight)
            }
        },
        // GRASS
        {
            3, {
                object::calculate_texture_uv_from_atlas(0, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(3, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(3, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(3, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(3, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
                object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
            }
        },
    };
}

bool Chunk::is_solid(const int x, const int y, const int z) const {
    // checks if there are any negative positions which are automatically invalid
    if (x < 0 || y < 0 || z < 0) {
        return false;
    }

    // checks if the block is outside this chunk
    if (x >= CHUNK_SIZE_X || y >= CHUNK_SIZE_Y || z >= CHUNK_SIZE_Z) {
        return false;
    }

    // returns true if the block is not an air block
    return voxels[x][y][z] != 0;
}

void Chunk::draw_chunk() {

}

glm::vec2 Chunk::get_voxel_face_uv_coords() {
    return glm::vec2(1.0f);
}
