#version 330 core
layout (location = 0) in vec3 bPos;

void main()  {
	gl_Position = vec4(bPos.x, bPos.y, bPos.z, 1.0);
}