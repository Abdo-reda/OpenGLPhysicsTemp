#version 330 core


in vec3 bPos;
in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 color;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixWeight;

void main(){
	
	FragColor = vec4(1.0f, 0.0, 0.0, 1.0f);
	FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord) , mixWeight) * vec4(color, 1.0f);
	//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord) , mixWeight) * vec4(color, 1.0f);
	//FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

}