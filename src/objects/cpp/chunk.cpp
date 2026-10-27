//
// Created by cayde on 19/10/2026.
//

#include "../headers/chunk.h"

Chunk::Chunk(const TextureAtlasData &tad, glm::vec3 spawnPos) {
    worldPos = spawnPos;
    textureTileSize = object::calculate_texture_atlas_tile_size(tad.textureAtlasWidth, tad.textureAtlasHeight);

    voxelTextureTable = {
        {1, {
            object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
            object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
            object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
            object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
            object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight),
            object::calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight)}
        },
    };
}

bool Chunk::is_solid(const int x, const int y, const int z) const {
    // checks if there are any negative positions which are automatically invalid
    if (x < 0 || y < 0 || z < 0) {
        return false;
    }

    // checks if the block is outside this chunk
    if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
        return false;
    }

    // returns true if the block is not an air block
    return voxels[x][y][z] != 0;
}

void Chunk::build_chunk(Chunk &chunk) {
    std::vector<Vertex> vertices = {};

    // loops through every voxel in this chunk
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                // continues if the current voxel is an air block
                if (!chunk.is_solid(x, y, z)) continue;

                // gets the current block type
                const uint8_t blockType = voxels[x][y][z];

                // loops through every face side of the current voxel
                for (int f = 0; f < std::size(FACES); ++f) {
                    const FaceData currentFace = FACES[f];

                    // INDEX 0 = TOP FACE
                    // INDEX 1 = BOTTOM FACE
                    // INDEX 2 = RIGHT FACE
                    // INDEX 3 = LEFT FACE
                    // INDEX 4 = FRONT FACE
                    // INDEX 5 = BACK FACE

                    // DIRT
                    if (blockType == 0) {

                    }
                    // LOG
                    if (blockType == 1) {

                    }

                    // the position of the voxel being checked adjacent to the current voxel
                    const int pos_x = x + currentFace.neighbour[0];
                    const int pos_y = y + currentFace.neighbour[1];
                    const int pos_z = z + currentFace.neighbour[2];

                    // if the voxel adjacent to this current voxel being checked is solid then we can skip this face since it will not be seen
                    if (chunk.is_solid(pos_x, pos_y, pos_z)) continue;

                    // the order to draw the current face of the voxel
                    const int order[6] = {0, 1, 2, 0, 2, 3};
                    for (int i: order) {

                        // calculates the UV coords for this vertex
                        const float U = object::normalize_to_range(currentFace.uvs[i][0], voxelTextureTable[0].second[0].x, voxelTextureTable[0].second[0].x + textureTileSize.x);
                        const float V = object::normalize_to_range(currentFace.uvs[i][1], voxelTextureTable[0].second[0].y, voxelTextureTable[0].second[0].y + textureTileSize.y);

                        // const float uvMinX = object::normalize_to_range();
                        // const float uvMaxX = object::normalize_to_range();
                        // const float uvMinY = object::normalize_to_range();
                        // const float uvMaxY = object::normalize_to_range();
                        //
                        // const float uvMinX = object::normalize_to_range(voxelTextureTable[0].second[0], 0.0f, static_cast<float>(tad.textureAtlasWidth));
                        // const float uvMaxX = object::normalize_to_range(voxelTextureTable[0].second[0] + tileSize.x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
                        // const float uvMinY = object::normalize_to_range(voxelTextureTable[0].second[0], 0.0f, static_cast<float>(tad.textureAtlasWidth));
                        // const float uvMaxY = object::normalize_to_range(voxelTextureTable[0].second[0] + tileSize.y, 0.0f, static_cast<float>(tad.textureAtlasWidth));
                        //

                        Vertex vertexToAdd = {
                            .x = x + currentFace.corners[i][0],
                            .y = y + currentFace.corners[i][1],
                            .z = z + currentFace.corners[i][2],

                            .nx = static_cast<float>(currentFace.neighbour[0]),
                            .ny = static_cast<float>(currentFace.neighbour[1]),
                            .nz = static_cast<float>(currentFace.neighbour[2]),

                            .u = U,
                            .v = V
                        };
                        vertices.push_back(vertexToAdd);
                    }
                }
            }
        }
    }

    // uses the size of the vertices vector to set the vertex count of this chunk
    chunk.vertexCount = static_cast<int>(vertices.size());

    // if there is no VAO & VBO set for the chunk it will be generated
    if (chunk.chunkVAO == 0) {
        glGenVertexArrays(1, &chunk.chunkVAO);
        glGenBuffers(1, &chunk.chunkVBO);
    }

    glBindVertexArray(chunk.chunkVAO);
    glBindBuffer(GL_ARRAY_BUFFER, chunk.chunkVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void Chunk::draw_chunk() {

}

glm::vec2 Chunk::get_voxel_face_uv_coords() {
    return glm::vec2(1.0f);
}
