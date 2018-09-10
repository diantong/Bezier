#version 410 core
layout (location = 0) in vec3 bPos;

uniform mat4 mvp;

out vec4 vPosition; 

void main()  {
	vPosition = mvp * vec4(bPos.xyz, 1.0);
}