#version 460 core

out vec4 FragColor;

uniform sampler2D texture1; // container
uniform vec4 color;

void main()
{
    FragColor = color;
}