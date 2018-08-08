#version 330 core
out vec4 FragColor;

void main() {
	float depth = pow(gl_FragCoord.z, 30.0f);
	FragColor = vec4(vec3(depth), 1.0f);
}