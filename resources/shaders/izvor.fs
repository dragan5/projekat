#version 330 core
out vec4 FragColor;

uniform vec3 lcol;

void main()
{
    FragColor = vec4(lcol,1.0); // set alle 4 vector values to 1.0
}