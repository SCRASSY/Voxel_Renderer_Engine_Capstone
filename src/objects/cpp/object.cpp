//
// Created by cayde on 4/09/2026.
//

#include "../headers/object.h"
#include "glad/glad.h"

void object::create_obj_vao() {
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    // enables the target attribute index
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // unbinds the vao so the previous vao obj does not get modified
    glBindVertexArray(0);
}
glm::vec2 object::calculate_texture_uv_from_atlas(const int x, const int y, const int textureWidth, const int textureHeight) {
    const int tileWidth = textureWidth / textureAtlasTileCount_x;
    const int tileHeight = textureHeight / textureAtlasTileCount_y;

    return {tileWidth * x, tileHeight * y};
}
glm::vec2 object::calculate_texture_atlas_tile_size(const int textureWidth, const int textureHeight) {
    const int tileWidth = textureWidth / textureAtlasTileCount_x;
    const int tileHeight = textureHeight / textureAtlasTileCount_y;

    return {tileWidth, tileHeight};
}
float object::normalize_to_range(const float value, const float min, const float max) {
    return (value - min) / (max - min);
}
void object::draw_obj() {
    glBindVertexArray(objVAO); // specify which vertex array obj to render
    //glDrawArrays(GL_TRIANGLES, 0, objData.verticesArrSize / sizeof(int));
    glDrawElements(GL_TRIANGLES, objData.indicesArrSize / sizeof(uint32_t), GL_UNSIGNED_INT, nullptr); // render the current vao to the back buffer

    glBindVertexArray(0);
}
