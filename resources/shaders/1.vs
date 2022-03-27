#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTx;
layout (location = 2) in vec3 aNormal;

out vec2 tc;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    normal = aNormal;
    fragPos = vec3(model * vec4(aPos,1.0));
    gl_Position = proj * view * model * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    tc = vec2(aTx.x,aTx.y);
}