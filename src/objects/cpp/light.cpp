//
// Created by cayde on 22/09/2026.
//

#include "../headers/light.h"

light::light(const glm::vec4 color, const glm::vec3 spawnPos) {
    m_color = color;
    position = spawnPos;

    float vertices[] = {
        // back
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        // front
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // right
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,

        // left
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,

        // bottom
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // top
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };     // the actual raw data used to draw an obj to the screen
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

void light::create_obj_vao() {
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
     * {attr=0, vec3(pos)}
     * ]
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));

    // enables the target attribute index
    glEnableVertexAttribArray(0);

    // unbinds the vao so the previous vao obj does not get modified
    glBindVertexArray(0);
}
