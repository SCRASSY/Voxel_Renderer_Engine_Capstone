#include <iostream>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../shaders/headers/Shader.h"
#include "../dependencies/texture-loader/stb_image.h"
#include "objects/headers/voxel.h"
#include "objects/headers/camera.h"
#include "objects/headers/chunk.h"
#include "objects/headers/light.h"

constexpr int WINDOW_WIDTH = 2000;
constexpr int WINDOW_HEIGHT = 1200;

static GLFWwindow* window = nullptr;
static Shader* engineShaderProgram = nullptr;

static int textureAtlasWidth = 0;
static int textureAtlasHeight = 0;

static float lastMouseX = WINDOW_WIDTH / 2;
static float lastMouseY = WINDOW_HEIGHT / 2;

static float deltaTime = 0.0f; // Time between current frame and last frame
static float lastFrame = 0.0f; // Time of last frame

static bool isFirstMouseCall = true;

static camera *playerCamObj = nullptr;

static std::vector<Chunk> worldChunks = {};

// FUNC DECLARATIONS
static void setup_glfw();
static void update_inputs();
static void update_render();
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void process_input();
static void link_callback_functions();
static bool create_engine_window();
static void create_chunk(int chunkSize);
static void create_terrain();

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

static void load_texture_atlas(const TextureData2D &textureData);

int main() {
    setup_glfw();

    // creates the engine window used to render OpenGL results
    if (!create_engine_window()) return -1;

    /* this will use GLAD to get function ptrs to all of OpenGL's functions in the GPU's driver
     * this allows the program to use OpenGL functions as if they were statically-linked (normal C/C++ library) functions
     */
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to init GLAD.\n";
        return -1;
    }

    // prints a list of details of the graphics processor being used by OpenGL to render the scene
    std::cout << "Vendor:   " << glGetString(GL_VENDOR)   << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "Version:  " << glGetString(GL_VERSION)  << "\n";

    /* this will tell OpenGL how much of the current context window can be used to render
     * [set to be the whole window]
     */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    link_callback_functions();

    // allows the program to render objects as a wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glEnable(GL_CULL_FACE);

    // creates a new shader program that will be used to manage the current shaders & program in use
    engineShaderProgram = new Shader();
    engineShaderProgram->change_shader_program("TextureProgram");

    // set fps to uncapped
    glfwSwapInterval(0);

    // creates a player camera object
    CameraWindowData cwd {
        .WINDOW_WIDTH = WINDOW_WIDTH,
        .WINDOW_HEIGHT = WINDOW_HEIGHT
    };
    playerCamObj = new camera(cwd);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    engineShaderProgram->use(); // use the current shader program
    engineShaderProgram->set_int("texture1", 0);

    constexpr TextureData2D textureData1 = {
        .filePath = PROJECT_ROOT "/textures/minecraft_texture_atlas_02.png",
        .textureSlot = GL_TEXTURE0,
    };

    // generates a new texture object from the target texture file path
    load_texture_atlas(textureData1);

    /* enables the ability for the GPU to test which pixels are meant to be in front of others
     * this allows every 3D model to be drawn correctly rather than triangles being drawn in any order
     * which will result in the 3D model looking very wrong or inverted
     */
    glEnable(GL_DEPTH_TEST);

    const TextureAtlasData tad = {
        .textureAtlasWidth = textureAtlasWidth,
        .textureAtlasHeight = textureAtlasHeight
    };

    // -- CHUNK TESTING --
    Chunk testChunk(tad, glm::vec3(1.0f));
    testChunk.worldPos = glm::vec3(10.0f, 0.0f, 0.0f);

    // creates the voxels in the chunk
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                testChunk.voxels[x][y][z] = y < 8 ? 1 : 0;
            }
        }
    }

    testChunk.build_chunk(testChunk);
    worldChunks.push_back(testChunk);
    // -- END OF CHUNK TESTING --

    // main render loop
    while (!glfwWindowShouldClose(window)) {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        update_inputs();
        update_render();
    }

    glfwTerminate();
    return 0;
}

// FUNC DEFINITIONS
void setup_glfw() {
    glfwInit();

    // sets the open gl version & profile (core is the modern standard for open gl)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
void update_inputs() {
    // input
    process_input();

    // check for input before the next frame
    glfwPollEvents();
}
void update_render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //glClearColor(0.4706f, 0.6549f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // holds a list of blocks to draw
    std::vector<voxel> voxels{};

    // this is the list of blocks currently in the scene
    //  voxel currentVoxel(BlockType::DIRT, tad, chunckPos + glm::vec3(x, y, z));
    //
    //  currentVoxel.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    //  currentVoxel.material = {
    //      .ambient = 0.2f,
    //      .diffuse = 1.0f,
    //      .specular = 0.5f,
    //      .shininess = 2.0f
    //  };
    //
    // voxels.push_back(currentVoxel);

    std::vector<light> lights = {
        light(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
    };

    // -- CAMERA --
    playerCamObj->update_cam_view();
    playerCamObj->update_cam_projection();

    // changes the engines current shader program to use the solid color shader program, used when rendering objects that only require a solid color
    engineShaderProgram->change_shader_program("LightProgram");
    engineShaderProgram->use();

    // renders each light source in the light list
    for (int i = 0; i < lights.size(); ++i) {
        light *currentLight = &lights[i];

        const float rotationDir = i % 2 == 0 ? -1 : 1;

        glm::mat4 _model(1.0f);
        _model = glm::translate(_model, currentLight->position);
        _model = glm::rotate(_model, static_cast<float>(glfwGetTime() * 0.5f) * glm::radians(50.0f * rotationDir), glm::vec3(0.5f, 1.0f, 0.5f));
        _model = glm::scale(_model, glm::vec3(1.0f));

        const float color_r = (sin(glfwGetTime()) * 1.0f + 1) / 2;
        const float color_g = (cos(glfwGetTime()) * 1.0f + 1) / 2;
        const float color_b = (sin(glfwGetTime()) * 1.0f + 1) / 2;

        // currentLight->m_color = glm::vec4(
        //     glm::max(color_r, 0.2f),
        //     glm::max(color_b, 0.2f),
        //     glm::max(color_g, 0.2f),
        //     1.0f
        //     );

        engineShaderProgram->set_mat4("model", _model);
        engineShaderProgram->set_mat4("view", playerCamObj->cameraView);
        engineShaderProgram->set_mat4("projection", playerCamObj->cameraProjection);

        // set shader program uniform [LIGHT OBJ COLOR]
        const GLint _colorLoc = glGetUniformLocation(engineShaderProgram->m_currentShaderProgramID, "color");
        glUniform4fv(_colorLoc, 1, glm::value_ptr(currentLight->m_color));

        // draws this object
        currentLight->draw_obj();
    }

    // changes the engines current shader program to use the texture shader program, used when rendering objects with textures
    engineShaderProgram->change_shader_program("TextureProgram");
    engineShaderProgram->use();

    // -- RENDER VOXELS --
    for (auto &i : voxels) {
        voxel *currentVoxel = &i;

        // sets the current cameras location uniform in the current fragment shader
        engineShaderProgram->set_vec3("viewPos", playerCamObj->cameraPos);

        glm::mat4 _model(1.0f);
        _model = glm::translate(_model, currentVoxel->position);//glm::vec3(newPosX, newPosY, newPosZ));
        _model = glm::scale(_model, currentVoxel->scale);

        auto lightColorVec = glm::vec3(lights[0].m_color.r, lights[0].m_color.g, lights[0].m_color.b);
        auto lightViewPos = glm::vec3(playerCamObj->cameraView * glm::vec4(lights[0].position, 1.0f)); // converts the lights position world space to the cameras view space

        // sets the current engines shader uniforms
        engineShaderProgram->set_vec3("lightColor", lightColorVec);
        engineShaderProgram->set_vec3("lightPos", lightViewPos);
        engineShaderProgram->set_mat4("model", _model);
        engineShaderProgram->set_mat4("view", playerCamObj->cameraView);
        engineShaderProgram->set_mat4("projection", playerCamObj->cameraProjection);
        engineShaderProgram->set_float("material.ambient", currentVoxel->material.ambient);
        engineShaderProgram->set_float("material.diffuse", currentVoxel->material.diffuse);
        engineShaderProgram->set_float("material.specular", currentVoxel->material.specular);
        engineShaderProgram->set_float("material.shininess", currentVoxel->material.shininess);

        // draws this object
        currentVoxel->draw_obj();
    }

    // -- RENDER CHUNK --
    for (auto &currentChunk: worldChunks) {
        auto model = glm::translate(glm::mat4(1.0f), currentChunk.worldPos);
        auto lightColorVec = glm::vec3(lights[0].m_color.r, lights[0].m_color.g, lights[0].m_color.b);
        auto lightViewPos = glm::vec3(playerCamObj->cameraView * glm::vec4(lights[0].position, 1.0f)); // converts the lights position world space to the cameras view space

        // sets the current engines shader uniforms
        engineShaderProgram->set_vec3("viewPos", playerCamObj->cameraPos);
        engineShaderProgram->set_vec3("lightColor", lightColorVec);
        engineShaderProgram->set_vec3("lightPos", lightViewPos);
        engineShaderProgram->set_mat4("model", model);
        engineShaderProgram->set_mat4("view", playerCamObj->cameraView);
        engineShaderProgram->set_mat4("projection", playerCamObj->cameraProjection);

        engineShaderProgram->set_float("material.ambient", 0.1f);
        engineShaderProgram->set_float("material.diffuse", 0.2f);
        engineShaderProgram->set_float("material.specular", 0.2f);
        engineShaderProgram->set_float("material.shininess", 0.2f);

        glBindVertexArray(currentChunk.chunkVAO);
        glDrawArrays(GL_TRIANGLES, 0, currentChunk.vertexCount);
    }





    // finally swap the buffers to show the new final render
    glfwSwapBuffers(window);

    // resets the current bound VAO to null
    glBindVertexArray(0);
}
void framebuffer_size_callback(GLFWwindow */*_window*/, int width, int height) {
    glViewport(0, 0, width, height);
}
void process_input() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // -- CAMERA --
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        playerCamObj->move_cam(MovementDirection::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        playerCamObj->move_cam(MovementDirection::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        playerCamObj->move_cam(MovementDirection::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        playerCamObj->move_cam(MovementDirection::RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        playerCamObj->move_cam(MovementDirection::UP, deltaTime);
    }
}  
void link_callback_functions() {
    // updates OpenGL's available render screen space when the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}
bool create_engine_window() {
    // creates the program window & checks if it was created successfully, otherwise terminate the program
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window.\n";
        glfwTerminate();
        return false;
    }

    // tells OpenGl which window to render its output in
    glfwMakeContextCurrent(window);

    return true;
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (isFirstMouseCall) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        isFirstMouseCall = false;
    }

    const float xOffset = (xpos - lastMouseX) * playerCamObj->cameraSensitivity;
    const float yOffset = (lastMouseY - ypos) * playerCamObj->cameraSensitivity;

    lastMouseX = xpos;
    lastMouseY = ypos;

    const float _yaw = playerCamObj->cameraYaw + xOffset;
    const float _pitch = glm::clamp(playerCamObj->cameraPitch + yOffset, -89.0f, 89.0f);

    // rotate the player camera
    playerCamObj->rotate_cam(_yaw, _pitch);
}
void scroll_callback(GLFWwindow */*window*/, double xoffset, double yoffset) {
    playerCamObj->update_cam_fov(playerCamObj->cameraFov - yoffset);
}
void load_texture_atlas(const TextureData2D &textureData) {
    // stores the width, height & channels of the target texture 2D
    int _width;
    int _height;
    int _nrChannels;

    // flips any loaded textures on the y axis as most textures without this will be drawn upside down
    stbi_set_flip_vertically_on_load(true);

    // loads the target texture into memory
    unsigned char *_data = stbi_load(textureData.filePath, &_width, &_height, &_nrChannels, 0);
    if (!_data) {
        std::cout << "Failed to load texture\n";
    }

    // stores the pixel width and height globally to be accessed later
    textureAtlasWidth = _width;
    textureAtlasHeight = _height;

    // gets the format type of the image file from the _nrChannel variable
    GLenum _format;
    switch (_nrChannels) {
        case 1: _format = GL_RED;
            break;
        case 3: _format = GL_RGB;
            break;
        case 4: _format = GL_RGBA;
            break;
        default:
            std::cout << "Unexpected channel count: " << _nrChannels << "\n";
            stbi_image_free(_data);
            return;
    }

    // creates a texture 2D object & sets it as the currently bound texture
    uint32_t _texture;
    glGenTextures(1, &_texture);
    glActiveTexture(textureData.textureSlot);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // sets texture wrapping param for x(u) axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // sets texture wrapping param for y(v) axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // sets texture filtering param when downscaling texture 2D
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // sets texture filtering param when upscaling texture 2D

    // generates the actual texture 2D that will be used by OpenGL from the loaded image data into the currently bound texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, _format, GL_UNSIGNED_BYTE, _data);

    // generates all the required mipmap levels for the currently bound texture object
    glGenerateMipmap(GL_TEXTURE_2D);

    // now that the currently bound texture 2D is generated we can free the loaded texture in memory
    stbi_image_free(_data);
}