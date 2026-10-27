#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextCoord;

out vec3 vertexPos;
out vec3 normal;
out vec2 texCoord;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // sets the position of this vertex by converting it to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // passes the calculated normal vector position converted to view space to the fragment shader
    normal = mat3(transpose(inverse(view * model))) * aNormal;

    // passes the UV coords to the fragment shader
    texCoord = aTextCoord;

    // calculates the position for the current vertex in view space only,
    // this is then interpolated between all 3 vertices which make up a single triangle
    // and allows the fragment shader to calculate lighting using this interpolated value for each fragment
    // this is essentially giving each fragment a position in view space
    fragPos = vec3(view * model * vec4(aPos, 1.0));
}