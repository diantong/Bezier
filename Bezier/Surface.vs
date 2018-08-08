#version 330 core
layout (location = 0) in vec3 bPos;

uniform mat4 mvp;

void main()  {
	gl_Position = mvp * vec4(bPos.x, bPos.y, bPos.z, 1.0);
}