//
// Created by cayde on 22/09/2026.
//

#ifndef OPENGL_TUTORIAL_LIGHT_H
#define OPENGL_TUTORIAL_LIGHT_H
#include "object.h"


class light : public object {
public:
    light(glm::vec4 color, glm::vec3 spawnPos);

    void create_obj_vao() override;

    glm::vec4 m_color = glm::vec4(1.0f);
};


#endif //OPENGL_TUTORIAL_LIGHT_H
