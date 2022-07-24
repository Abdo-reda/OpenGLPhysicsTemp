#version 330 core


in vec3 bPos;
out vec4 FragColor;

uniform vec3 color;
//uniform sampler2D ourTexture;

void main(){

	//FragColor =  texture(ourTexture, TexCoord) * vec4(color, 1.0f);
	FragColor = vec4(color, 1.0f);

}