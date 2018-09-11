#version 410 core
layout (location = 0) in vec3 bPos;
layout (location = 1) in vec2 uvPos;

out vec4 vPosition; 
out vec2 tcuvPosition;

void main()  {
	vPosition = vec4(bPos.xyz, 1.0);
	tcuvPosition = uvPos;
}