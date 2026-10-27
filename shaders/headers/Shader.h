#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility>
#include <cstring>

class Shader {
public:
    uint32_t m_currentShaderProgramID = 0;

    // [shader program type, shader program ID]
    std::vector<std::pair<std::string, uint32_t>> m_shaderPrograms;

    // constructor reads & builds the shaders
    Shader() {
        gen_shader_programs();
    }

    // use the shader
    void use() const {
        glUseProgram(m_currentShaderProgramID);
    }

    // change to another shader program
    void change_shader_program(const std::string& programType) {

        // loops through every shader program in the list
        for (const auto& [type, ID]: m_shaderPrograms) {

            // if there is a match then the engines new shader program is the currently found shader program ID
            if (strcmp(type.c_str(), programType.c_str()) == 0) {
                m_currentShaderProgramID = ID;
            }
        }
    }

    // helper functions to create shaders & programs
    uint32_t gen_vertex_shader(const char *shaderSource) {
        uint32_t _vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(_vertexShader, 1, &shaderSource, nullptr);
        glCompileShader(_vertexShader);

        return _vertexShader;
    }
    uint32_t gen_fragment_shader(const char *shaderSource) {
        uint32_t _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(_fragmentShader, 1, &shaderSource, nullptr);
        glCompileShader(_fragmentShader);

        return _fragmentShader;
    }
    uint32_t gen_shader_program(const uint32_t vertexShader, const uint32_t fragmentShader) {
        uint32_t _shaderProgram = glCreateProgram();
        glAttachShader(_shaderProgram, vertexShader);
        glAttachShader(_shaderProgram, fragmentShader);
        glLinkProgram(_shaderProgram);

        // delete the shaders after linking them to the program
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return _shaderProgram;
    }
    void gen_shader_programs() {
        // -- GENERATES THE SHADER PROGRAM USED FOR 3D OBJECTS THAT USE TEXTURES --
        m_shaderPrograms.emplace_back("TextureProgram", create_shader_program_from_shader_files(
            PROJECT_ROOT "/shaders/glsl-files/texture-shaders/v_texture_shader.glsl",
            PROJECT_ROOT "/shaders/glsl-files/texture-shaders/f_texture_shader.glsl"
            ));

        // -- GENERATES THE SHADER PROGRAM USED FOR 3D OBJECTS THAT USE A SINGLE COLOUR --
        m_shaderPrograms.emplace_back("LightProgram", create_shader_program_from_shader_files(
            PROJECT_ROOT "/shaders/glsl-files/solid-color-shaders/v_light_source_shader.glsl",
            PROJECT_ROOT "/shaders/glsl-files/solid-color-shaders/f_light_source_shader.glsl"
            ));
    }

    uint32_t compile_shaders(const char* vertexShaderCode, const char* fragmentShaderCode) {
        // vertex shader
        const uint32_t _vertexShader = gen_vertex_shader(vertexShaderCode);
        check_vertex_shader_compilation(_vertexShader);

        // fragment shader
        const uint32_t _fragmentShader = gen_fragment_shader(fragmentShaderCode);
        check_fragment_shader_compilation(_fragmentShader);

        // shader program
        const uint32_t compiledShaderProgramID = gen_shader_program(_vertexShader, _fragmentShader);
        check_shader_program_link(compiledShaderProgramID);

        // delete the vertex & fragment shaders as they are now linked to a program
        glDeleteShader(_vertexShader);
        glDeleteShader(_fragmentShader);

        return  compiledShaderProgramID;
    }

    // util functions
    void set_bool(const std::string &name, const bool value) const {
        glUniform1i(glGetUniformLocation(m_currentShaderProgramID, name.c_str()), static_cast<int>(value));
    }
    void set_int(const std::string &name, const int value) const {
        glUniform1i(glGetUniformLocation(m_currentShaderProgramID, name.c_str()), value);
    }
    void set_float(const std::string &name, const float value) const {
        glUniform1f(glGetUniformLocation(m_currentShaderProgramID, name.c_str()), static_cast<float>(value));
    }
    void set_vec3(const std::string &name, const glm::vec3 vec) const {
        const GLint vec3Loc = glGetUniformLocation(m_currentShaderProgramID, name.c_str());
        glUniform3f(vec3Loc, vec.x, vec.y, vec.z);
    }
    void set_mat4(const std::string &name, const glm::mat4 &mat) {
        const GLint mat4Loc = glGetUniformLocation(m_currentShaderProgramID, name.c_str());
        glUniformMatrix4fv(mat4Loc, 1, GL_FALSE, glm::value_ptr(mat));
    }

    std::pair<std::string, std::string> get_shader_code_from_files(const char* vertexPath, const char* fragmentPath) {
        using namespace std;

        // -- RETRIEVE SHADER SOURCE CODE FROM FILE PATH --

        // this is the actual glsl code that will be stored from the shader .glsl files into a C++ string
        string _vertexCode;
        string _fragmentCode;

        // this is the .glsl file object that will be used to read from the target shader files
        ifstream _vShaderFile;
        ifstream _fShaderFile;

        // ensure ifstream objects can throw exceptions:
        _vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        _fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open shader files
            _vShaderFile.open(vertexPath);
            _fShaderFile.open(fragmentPath);

            stringstream _vShaderStream;
            stringstream _fShaderStream;

            // reads the shader files code into the target streams
            _vShaderStream << _vShaderFile.rdbuf();
            _fShaderStream << _fShaderFile.rdbuf();

            // closes the file handles
            _vShaderFile.close();
            _fShaderFile.close();

            // converts the string streams into plain C++ strings
            _vertexCode = _vShaderStream.str();
            _fragmentCode = _fShaderStream.str();

        } catch (ifstream::failure& error) {
            cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
        }

        return {_vertexCode, _fragmentCode};
    }

    uint32_t create_shader_program_from_shader_files(const char *vertexPath, const char *fragmentPath) {
        auto shaderPair = get_shader_code_from_files(vertexPath, fragmentPath);
        const char* _vShaderCode = shaderPair.first.c_str();
        const char* _fShaderCode = shaderPair.second.c_str();

        return compile_shaders(_vShaderCode, _fShaderCode);
    }

    // debug
    void check_vertex_shader_compilation(const uint32_t vertexShader) {
        int _success;
        char _infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &_success);

        if (!_success) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, _infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << _infoLog << "\n";
        }
    }
    void check_fragment_shader_compilation(const uint32_t fragmentShader) {
        int _success;
        char _infoLog[512];
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &_success);

        if (!_success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, _infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << _infoLog << "\n";
        }
    }
    void check_shader_program_link(const uint32_t shaderProgram) {
        int _success;
        char _infoLog[512];

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &_success);
        if (!_success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, _infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << _infoLog << "\n";
        }
    }
};
#endif