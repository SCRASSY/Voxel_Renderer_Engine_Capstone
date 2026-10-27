//
// Created by cayde on 5/09/2026.
//

#include "../headers/voxel.h"

voxel::voxel(const BlockType type, const TextureAtlasData &tad, glm::vec3 spawnPos) {
    blockType = type;
    position = spawnPos;

    switch (blockType) {
        case BlockType::DIRT: {
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 15, tad.textureAtlasWidth, tad.textureAtlasHeight));
        }
            break;
        case BlockType::OAK_LOG: {
                objTextureUVs.push_back(calculate_texture_uv_from_atlas(5, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
                objTextureUVs.push_back(calculate_texture_uv_from_atlas(5, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
                objTextureUVs.push_back(calculate_texture_uv_from_atlas(4, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
                objTextureUVs.push_back(calculate_texture_uv_from_atlas(4, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
                objTextureUVs.push_back(calculate_texture_uv_from_atlas(4, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
                objTextureUVs.push_back(calculate_texture_uv_from_atlas(4, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
        }
            break;
        case BlockType::SAND: {
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(2, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
        }
            break;
        case BlockType::COBBLESTONE: {
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(0, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(0, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(0, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(0, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(0, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(0, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
        }
            break;
        case BlockType::BLOCK_OF_IRON: {
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(6, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(6, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(6, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(6, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(6, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(6, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
        }
        case BlockType::BLOCK_OF_DIAMOND: {
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(8, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(8, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(8, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(8, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(8, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
            objTextureUVs.push_back(calculate_texture_uv_from_atlas(8, 14, tad.textureAtlasWidth, tad.textureAtlasHeight));
        }
    }

    const glm::vec2 tileSize = calculate_texture_atlas_tile_size(tad.textureAtlasWidth, tad.textureAtlasHeight);

    // top
    const float uvTopMin_x = normalize_to_range(objTextureUVs[0].x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvTopMax_x = normalize_to_range(objTextureUVs[0].x + tileSize.x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvTopMin_y = normalize_to_range(objTextureUVs[0].y, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvTopMax_y = normalize_to_range(objTextureUVs[0].y + tileSize.y, 0.0f, static_cast<float>(tad.textureAtlasWidth));

    // bottom
    const float uvBottomMin_x = normalize_to_range(objTextureUVs[1].x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvBottomMax_x = normalize_to_range(objTextureUVs[1].x + tileSize.x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvBottomMin_y = normalize_to_range(objTextureUVs[1].y, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvBottomMax_y = normalize_to_range(objTextureUVs[1].y + tileSize.y, 0.0f, static_cast<float>(tad.textureAtlasWidth));

    // front
    const float uvFrontMin_x = normalize_to_range(objTextureUVs[2].x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvFrontMax_x = normalize_to_range(objTextureUVs[2].x + tileSize.x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvFrontMin_y = normalize_to_range(objTextureUVs[2].y, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvFrontMax_y = normalize_to_range(objTextureUVs[2].y + tileSize.y, 0.0f, static_cast<float>(tad.textureAtlasWidth));

    // back
    const float uvBackMin_x = normalize_to_range(objTextureUVs[3].x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvBackMax_x = normalize_to_range(objTextureUVs[3].x + tileSize.x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvBackMin_y = normalize_to_range(objTextureUVs[3].y, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvBackMax_y = normalize_to_range(objTextureUVs[3].y + tileSize.y, 0.0f, static_cast<float>(tad.textureAtlasWidth));

    // left (from block pov)
    const float uvLeftMin_x = normalize_to_range(objTextureUVs[4].x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvLeftMax_x = normalize_to_range(objTextureUVs[4].x + tileSize.x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvLeftMin_y = normalize_to_range(objTextureUVs[4].y, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvLeftMax_y = normalize_to_range(objTextureUVs[4].y + tileSize.y, 0.0f, static_cast<float>(tad.textureAtlasWidth));

    // right (from block pov)
    const float uvRightMin_x = normalize_to_range(objTextureUVs[5].x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvRightMax_x = normalize_to_range(objTextureUVs[5].x + tileSize.x, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvRightMin_y = normalize_to_range(objTextureUVs[5].y, 0.0f, static_cast<float>(tad.textureAtlasWidth));
    const float uvRightMax_y = normalize_to_range(objTextureUVs[5].y + tileSize.y, 0.0f, static_cast<float>(tad.textureAtlasWidth));

    float vertices[] = {
    // back (z = -0.5, normal 0,0,-1)
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  uvBackMin_x, uvBackMin_y,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  uvBackMax_x, uvBackMin_y,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  uvBackMax_x, uvBackMax_y,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  uvBackMin_x, uvBackMax_y,

    // front (z = 0.5, normal 0,0,1)
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  uvFrontMin_x, uvFrontMin_y,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  uvFrontMax_x, uvFrontMin_y,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  uvFrontMax_x, uvFrontMax_y,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  uvFrontMin_x, uvFrontMax_y,

    // left (x = -0.5, normal -1,0,0)
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  uvLeftMax_x, uvLeftMax_y,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  uvLeftMin_x, uvLeftMax_y,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  uvLeftMin_x, uvLeftMin_y,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  uvLeftMax_x, uvLeftMin_y,

    // right (x = 0.5, normal 1,0,0)
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  uvRightMax_x, uvRightMax_y,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  uvRightMin_x, uvRightMax_y,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  uvRightMin_x, uvRightMin_y,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  uvRightMax_x, uvRightMin_y,

    // bottom (y = -0.5, normal 0,-1,0)
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  uvBottomMin_x, uvBottomMax_y,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  uvBottomMax_x, uvBottomMax_y,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  uvBottomMax_x, uvBottomMin_y,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  uvBottomMin_x, uvBottomMin_y,

    // top (y = 0.5, normal 0,1,0)
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  uvTopMin_x, uvTopMax_y,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  uvTopMax_x, uvTopMax_y,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  uvTopMax_x, uvTopMin_y,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  uvTopMin_x, uvTopMin_y
    };   // the actual raw data used to draw an obj to the screen
    uint32_t indices[] = {
        // back
        0, 1, 2,    2, 3, 0,

        // front
        4, 5, 6,    6, 7, 4,

        // right
        8, 9, 10,   10, 11, 8,

        // left
        12, 13, 14, 14, 15, 12,

        // bottom
        16, 17, 18, 18, 19, 16,

        // top
        20, 21, 22, 22, 23, 20
    };

    objData = {
        .verticesArr = vertices,
        .indicesArr = indices,
        .verticesArrSize = sizeof(vertices),
        .indicesArrSize = sizeof(indices),
    };

    create_obj_vao();
}

void voxel::create_obj_vao() {
    // -- VERTEX ARRAY OBJ --
    glGenVertexArrays(1, &objVAO);
    glBindVertexArray(objVAO);

    // -- VERTEX BUFFER OBJ --
    uint32_t VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, objData.verticesArrSize, objData.verticesArr, GL_STATIC_DRAW);

    // -- ELEMENT ARRAY BUFFER OBJ --
    uint32_t EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, objData.indicesArrSize, objData.indicesArr, GL_STATIC_DRAW);

    /* tells OpenGL how each attribute of a vertex should be formatted
     * Vertex = [
     * {attr=0, vec3(pos)},
     * {attr=1, vec2(UV coord)}
     * ]
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));

    // enables the target attribute index
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // unbinds the vao so the previous vao obj does not get modified
    glBindVertexArray(0);
}
