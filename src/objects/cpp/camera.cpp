//
// Created by cayde on 9/09/2026.
//

#include "../headers/camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

camera::camera(CameraWindowData &cwd) {
    camWinData = {
        .WINDOW_WIDTH = cwd.WINDOW_WIDTH,
        .WINDOW_HEIGHT = cwd.WINDOW_HEIGHT,
    };
}

void camera::update_cam_fov(const float newFov) {
    cameraFov = glm::clamp(newFov, 5.0f, 90.0f);
    update_cam_projection();
}

void camera::update_cam_projection() {
    cameraProjection = glm::perspective(glm::radians(cameraFov), static_cast<float>(camWinData.WINDOW_WIDTH)/static_cast<float>(camWinData.WINDOW_HEIGHT), 0.1f, 100.0f);
}

void camera::update_cam_view() {
    cameraView = glm::lookAt(cameraPos, cameraPos + cameraForward, cameraUp);
}

void camera::move_cam(const MovementDirection movementDir, const float deltaTime) {
    const float finalCameraSpeed = cameraSpeed * deltaTime;
    switch (movementDir) {
        case MovementDirection::FORWARD:
            cameraPos += finalCameraSpeed * cameraForward;
            break;
        case MovementDirection::BACKWARD:
            cameraPos -= finalCameraSpeed * cameraForward;
            break;
        case MovementDirection::LEFT:
            cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * finalCameraSpeed;
            break;
        case MovementDirection::RIGHT:
            cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * finalCameraSpeed;
            break;
        case MovementDirection::UP:
            cameraPos += finalCameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f); // moves the camera up in world space
            break;
        case MovementDirection::DOWN:
            cameraPos += finalCameraSpeed * glm::vec3(0.0f, -1.0f, 0.0f); // moves the camera down in world space
            break;
    }
}

void camera::rotate_cam(float yaw, float pitch) {
    cameraYaw = yaw;
    cameraPitch = pitch;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraForward = glm::normalize(direction);
}
