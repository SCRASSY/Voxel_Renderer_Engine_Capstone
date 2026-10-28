//
// Created by cayde on 9/09/2026.
//

#ifndef OPENGL_TUTORIAL_CAMERA_H
#define OPENGL_TUTORIAL_CAMERA_H
#include <glm/glm.hpp>

enum class MovementDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct CameraWindowData {
    int WINDOW_WIDTH = 0;
    int WINDOW_HEIGHT = 0;
};

class camera {
public:
    glm::mat4 cameraView = glm::mat4(1.0f);
    glm::mat4 cameraProjection = glm::mat4(1.0f);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float cameraSpeed = 5.0f;
    float cameraSensitivity = 0.1f;
    float cameraFov = 90.0f;
    float cameraYaw = -90.0f;
    float cameraPitch = 0.0f;

    CameraWindowData camWinData{};

    camera(CameraWindowData& cwd);

    void update_cam_fov(float newFov);
    void update_cam_projection();
    void update_cam_view();

    void move_cam(MovementDirection movementDir, float deltaTime);
    void rotate_cam(float yaw, float pitch);
};


#endif //OPENGL_TUTORIAL_CAMERA_H
