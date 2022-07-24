#version 330 core


in vec3 bPos;
out vec4 FragColor;

void main(){

	FragColor = vec4(bPos,1.0f);

}