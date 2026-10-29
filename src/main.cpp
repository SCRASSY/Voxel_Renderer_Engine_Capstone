#include <iostream>
#include <ranges>
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
#include <tracy/Tracy.hpp>

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

// key holds the index position of the chunk in the world, the value is the actual chunk
static std::vector<std::pair<glm::ivec2, Chunk>> worldChunks = {};

static TextureAtlasData gameGlobalTextureAtlasData = {};

// FUNC DECLARATIONS
static void setup_glfw();
static void update_inputs();
static void update_render();
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void process_input();
static void link_callback_functions();
static bool create_engine_window();
static Chunk generate_chunk_data(glm::vec3 chunkPos);
static void build_chunks(std::vector<std::pair<glm::ivec2, Chunk>> &chunksList);
static void generate_terrain();

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
    playerCamObj->cameraPos = glm::vec3(0.0f, 2.0f, 0.0f);

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

    // sets the global texture atlas data to be the newly loaded game texture atlas
    gameGlobalTextureAtlasData = {
        .textureAtlasWidth = textureAtlasWidth,
        .textureAtlasHeight = textureAtlasHeight
    };

    // -- GENERATE THE WORLD/TERRAIN DATA --
    constexpr int terrainRadius = 16;
    for (int x = -terrainRadius / 2; x < terrainRadius / 2; ++x) {
        for (int z = -terrainRadius / 2; z < terrainRadius / 2; ++z) {
            const auto worldOrigin = glm::vec3(0.0f, -CHUNK_SIZE_Y / 2, 0.0f);
            const auto newChunkPos = worldOrigin + glm::vec3(CHUNK_SIZE_X * x, 0.0f, CHUNK_SIZE_Z * z);

            // generates a new game chunk & adds it to the game worlds chunk list
            const std::pair chunkPair = {glm::vec2(x + terrainRadius, z + terrainRadius), generate_chunk_data(newChunkPos)};
            worldChunks.emplace_back(chunkPair);
        }
    }

    // -- BUILD THE WORLD/TERRAIN VERTICES/FACES --
    build_chunks(worldChunks);

    double fpsTimer = 0.0;
    int fpsFrames = 0;
    float averageFps = 0.0f;

    // main render loop
    while (!glfwWindowShouldClose(window)) {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        update_inputs();
        update_render();

        // used to send the current fps to the tracy profiler
        FrameMark;

        fpsFrames++;
        fpsTimer += deltaTime;
        if (fpsTimer >= 0.5) {
            averageFps = static_cast<float>(fpsFrames / fpsTimer);
            TracyPlot("Avg FPS", averageFps);

            const std::string title = "Scrassy's Voxel Renderer | " + std::to_string(static_cast<int>(averageFps)) + " FPS";
            glfwSetWindowTitle(window, title.c_str());

            fpsTimer = 0.0;
            fpsFrames = 0;
        }
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
    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearColor(0.4706f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // holds a list of blocks to draw
    // std::vector<voxel> voxels{};

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

    // std::vector<light> lights = {
    //     light(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
    // };

    // -- CAMERA --
    playerCamObj->update_cam_view();
    playerCamObj->update_cam_projection();

    // changes the engines current shader program to use the solid color shader program, used when rendering objects that only require a solid color
    engineShaderProgram->change_shader_program("LightProgram");
    engineShaderProgram->use();

    // // renders each light source in the light list
    // for (int i = 0; i < lights.size(); ++i) {
    //     light *currentLight = &lights[i];
    //
    //     const float rotationDir = i % 2 == 0 ? -1 : 1;
    //
    //     glm::mat4 _model(1.0f);
    //     _model = glm::translate(_model, currentLight->position);
    //     _model = glm::rotate(_model, static_cast<float>(glfwGetTime() * 0.5f) * glm::radians(50.0f * rotationDir), glm::vec3(0.5f, 1.0f, 0.5f));
    //     _model = glm::scale(_model, glm::vec3(1.0f));
    //
    //     const float color_r = (sin(glfwGetTime()) * 1.0f + 1) / 2;
    //     const float color_g = (cos(glfwGetTime()) * 1.0f + 1) / 2;
    //     const float color_b = (sin(glfwGetTime()) * 1.0f + 1) / 2;
    //
    //     // currentLight->m_color = glm::vec4(
    //     //     glm::max(color_r, 0.2f),
    //     //     glm::max(color_b, 0.2f),
    //     //     glm::max(color_g, 0.2f),
    //     //     1.0f
    //     //     );
    //
    //     engineShaderProgram->set_mat4("model", _model);
    //     engineShaderProgram->set_mat4("view", playerCamObj->cameraView);
    //     engineShaderProgram->set_mat4("projection", playerCamObj->cameraProjection);
    //
    //     // set shader program uniform [LIGHT OBJ COLOR]
    //     const GLint _colorLoc = glGetUniformLocation(engineShaderProgram->m_currentShaderProgramID, "color");
    //     glUniform4fv(_colorLoc, 1, glm::value_ptr(currentLight->m_color));
    //
    //     // draws this object
    //     currentLight->draw_obj();
    // }

    // changes the engines current shader program to use the texture shader program, used when rendering objects with textures
    engineShaderProgram->change_shader_program("TextureProgram");
    engineShaderProgram->use();

    // // -- RENDER VOXELS --
    // for (auto &i : voxels) {
    //     voxel *currentVoxel = &i;
    //
    //     // sets the current cameras location uniform in the current fragment shader
    //     engineShaderProgram->set_vec3("viewPos", playerCamObj->cameraPos);
    //
    //     glm::mat4 _model(1.0f);
    //     _model = glm::translate(_model, currentVoxel->position);//glm::vec3(newPosX, newPosY, newPosZ));
    //     _model = glm::scale(_model, currentVoxel->scale);
    //
    //     auto lightColorVec = glm::vec3(lights[0].m_color.r, lights[0].m_color.g, lights[0].m_color.b);
    //     auto lightViewPos = glm::vec3(playerCamObj->cameraView * glm::vec4(lights[0].position, 1.0f)); // converts the lights position world space to the cameras view space
    //
    //     // sets the current engines shader uniforms
    //     engineShaderProgram->set_vec3("lightColor", lightColorVec);
    //     engineShaderProgram->set_vec3("lightPos", lightViewPos);
    //     engineShaderProgram->set_mat4("model", _model);
    //     engineShaderProgram->set_mat4("view", playerCamObj->cameraView);
    //     engineShaderProgram->set_mat4("projection", playerCamObj->cameraProjection);
    //     engineShaderProgram->set_float("material.ambient", currentVoxel->material.ambient);
    //     engineShaderProgram->set_float("material.diffuse", currentVoxel->material.diffuse);
    //     engineShaderProgram->set_float("material.specular", currentVoxel->material.specular);
    //     engineShaderProgram->set_float("material.shininess", currentVoxel->material.shininess);
    //
    //     // draws this object
    //     currentVoxel->draw_obj();
    // }

    // -- RENDER CHUNK --
    for (auto &currentChunkData: worldChunks) {
        const auto currentChunk = currentChunkData.second;

        auto model = glm::translate(glm::mat4(1.0f), currentChunk.worldPos);
        //auto lightColorVec = glm::vec3(lights[0].m_color.r, lights[0].m_color.g, lights[0].m_color.b);
        //auto lightViewPos = glm::vec3(playerCamObj->cameraView * glm::vec4(lights[0].position, 1.0f)); // converts the lights position world space to the cameras view space

        // sets the current engines shader uniforms
        engineShaderProgram->set_vec3("viewPos", playerCamObj->cameraPos);
        //engineShaderProgram->set_vec3("lightColor", lightColorVec);
        //engineShaderProgram->set_vec3("lightPos", lightViewPos);
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
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        playerCamObj->move_cam(MovementDirection::DOWN, deltaTime);
    }

}
void link_callback_functions() {
    // updates OpenGL's available render screen space when the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}
bool create_engine_window() {
    // creates the program window & checks if it was created successfully, otherwise terminate the program
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Scrassy's Voxel Renderer", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window.\n";
        glfwTerminate();
        return false;
    }

    // tells OpenGl which window to render its output in
    glfwMakeContextCurrent(window);

    return true;
}
Chunk generate_chunk_data(glm::vec3 chunkPos) {
    auto localChunk = Chunk(gameGlobalTextureAtlasData, chunkPos);
    localChunk.worldPos = chunkPos;
    localChunk.chunkID = worldChunks.size();

    // creates the voxel data in this chunk
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                if (y > CHUNK_SIZE_Y / 2) {
                    localChunk.voxels[x][y][z] = 0;
                    continue;
                }
                if (localChunk.chunkID % 2 == 0) {
                    localChunk.voxels[x][y][z] = 2;
                    continue;
                }
                if (y < CHUNK_SIZE_Y / 2) {
                    localChunk.voxels[x][y][z] = 1;
                    continue;
                }
                if (y == CHUNK_SIZE_Y / 2) {
                    localChunk.voxels[x][y][z] = 3;
                }
            }
        }
    }

    return localChunk;
}

void build_chunks(std::vector<std::pair<glm::ivec2, Chunk>> &chunksList) {
    for (auto &chunkData: chunksList) {
        auto chunkIndex = chunkData.first;
        auto &chunkObj = chunkData.second;

        std::vector<Vertex> vertices = {};

        // loops through every voxel in this chunk
        for (int x = 0; x < CHUNK_SIZE_X; ++x) {
            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                for (int z = 0; z < CHUNK_SIZE_Z; ++z) {

                    // continues if the current voxel is an air block
                    if (!chunkObj.is_solid(x, y, z)) continue;

                    // gets the current block type
                    const uint8_t blockType = chunkObj.voxels[x][y][z];

                    // loops through every face side of the current voxel
                    for (int f = 0; f < std::size(chunkObj.FACES); ++f) {
                        const FaceData currentFace = chunkObj.FACES[f];

                        // INDEX 0 = TOP FACE
                        // INDEX 1 = BOTTOM FACE
                        // INDEX 2 = RIGHT FACE
                        // INDEX 3 = LEFT FACE
                        // INDEX 4 = FRONT FACE
                        // INDEX 5 = BACK FACE

                        // the position of the voxel being checked adjacent to the current voxel
                        const int pos_x = x + currentFace.neighbour[0];
                        const int pos_y = y + currentFace.neighbour[1];
                        const int pos_z = z + currentFace.neighbour[2];

                        // if the voxel adjacent to this current voxel being checked is solid then we can skip this face since it will not be seen
                        if (chunkObj.is_solid(pos_x, pos_y, pos_z)) continue;

                        // -- NEED TO CHECK IF THIS FACE IS ON THE EDGE OF A CHUNK AND IF THERE IS ANOTHER CHUNK HIDING IT

                        // checks if the current voxel is at the start or end of the chunk on the x axis
                        if (pos_x == -1 || pos_x == CHUNK_SIZE_X) {
                            const int signValue = (pos_x > 0) - (pos_x < 0);
                            auto target = chunkIndex + glm::ivec2(signValue, 0.0f);

                            auto it = std::ranges::find_if(chunksList, [&](const auto &p) {
                                return p.first == target;
                            });

                            // checks if the iterator found a valid chunk
                            if (it != chunksList.end()) {
                                constexpr int size = CHUNK_SIZE_X - 1;
                                const int newPosX = (pos_x % size + size) % size;

                                // skips this face if the voxel next to it in the adjacent chunk is solid
                                if (it->second.is_solid(newPosX, y, z)) continue;
                            }
                        }

                        // checks if the current voxel is at the start or end of the chunk on the z axis
                        if (pos_z == -1 || pos_z == CHUNK_SIZE_Z) {
                            const int signValue = (pos_z > 0) - (pos_z < 0);
                            auto target = chunkIndex + glm::ivec2(0.0f, signValue);

                            auto it = std::ranges::find_if(chunksList, [&](const auto &p) {
                                return p.first == target;
                            });

                            // checks if the iterator found a valid chunk
                            if (it != chunksList.end()) {
                                constexpr int size = CHUNK_SIZE_Z - 1;
                                const int newPosZ = (pos_z % size + size) % size;

                                // skips this face if the voxel next to it in the adjacent chunk is solid
                                if (it->second.is_solid(x, y, newPosZ)) continue;
                            }
                        }

                        // the order to draw the current face of the voxel
                        const int order[6] = {0, 1, 2, 0, 2, 3};
                        for (int i: order) {
                            const float valueU = currentFace.uvs[i][0];
                            const float valueV = currentFace.uvs[i][1];

                            const float tilePosU = chunkObj.voxelTextureTable.at(blockType)[f].x;
                            const float tilePosV = chunkObj.voxelTextureTable.at(blockType)[f].y;

                            const float finalValueU = tilePosU + valueU * chunkObj.textureTileSize.x;
                            const float finalValueV = tilePosV + valueV * chunkObj.textureTileSize.y;

                            // calculates the UV coords for this vertex
                            const float U = object::normalize_to_range(finalValueU, 0.0f, static_cast<float>(gameGlobalTextureAtlasData.textureAtlasWidth));
                            const float V = object::normalize_to_range(finalValueV, 0.0f, static_cast<float>(gameGlobalTextureAtlasData.textureAtlasHeight));

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
        chunkObj.vertexCount = static_cast<int>(vertices.size());

        // if there is no VAO & VBO set for the chunk it will be generated
        if (chunkObj.chunkVAO == 0) {
            glGenVertexArrays(1, &chunkObj.chunkVAO);
            glGenBuffers(1, &chunkObj.chunkVBO);
        }

        glBindVertexArray(chunkObj.chunkVAO);
        glBindBuffer(GL_ARRAY_BUFFER, chunkObj.chunkVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(6 * sizeof(float)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
    }
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