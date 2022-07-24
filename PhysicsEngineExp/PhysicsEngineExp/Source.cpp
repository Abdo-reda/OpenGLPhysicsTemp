#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>
#include "Shader.h"
#include "PhysicsWorld.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "stb_image.h" //image loader


using namespace std;
using namespace glm;

//--------Global Variables
int WIDTH = 800;
int HEIGHT = 800;
float lastFrame = 0.0f;
float deltaTime = 0.0f;
float accumlatedDT = 0.0f;
float FPS = 60.0f;
float timeStep = 1.0f / FPS;
vec3 cameraPos = vec3(0.0, 0.0, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 camemraUp = vec3(0.0, 1.0, 0.0);
bool clickF = false;
PhysicsWorld myPhyWrld;
vec3 shadowDir = normalize(vec3(-1.0f, -1.0f, -1.0f));
float shadowDis = 0.07f;
float shadowColor = 210.0 / 256.0; // 220/256
float Repeat = 32.0f;


/*------------ - TODO
	- Add shadow objects.
	- get creative with collision response, change color or shape.
	- fix the chain stuff
	- add varied objects and their collisions.
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main() {

	
	

	//------ Intilize GLFW and OpenGL context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//------ Create window name and size
	GLFWwindow* myWindow = glfwCreateWindow(WIDTH, HEIGHT, "Physics Engine", NULL, NULL);
	if (myWindow == NULL) return -1;
	glfwMakeContextCurrent(myWindow);
	//---- GLAD stuff
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

	//------- Rendering viewport
	glViewport(0,0, WIDTH, HEIGHT);

	//------ Callback functions 
	glfwSetFramebufferSizeCallback(myWindow, framebuffer_size_callback);
	glfwSetMouseButtonCallback(myWindow, mouse_button_callback);

	//------ Variables
	vec3 myVec = vec3(1.0f, 2.0f, 3.0f);

	//------- Create Shaders
	Shader myShader("vShader.glsl", "fShader.glsl");

	//---------Physics stuff
	myPhyWrld.addConst(vec3(0.0f), 1.0f);


	//------- Background Textures & Texture Data
	float verticesB[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f,	1.0f, 0.0f, 0.0f, Repeat,
		1.0f,	1.0f, 0.0f, Repeat, Repeat,
		1.0f, -1.0f, 0.0f, Repeat, 0.0f,
	};

	unsigned int indeciesB[] = {
		0,1,3,
		1,2,3
	};
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glGenTextures(1, &texture2);
	//----------Texture Stuff
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load and generate the texture 
	int width, height, nrChannels;
	unsigned char* data = stbi_load("GridPattern1.png", &width, &height, &nrChannels, 0);
	// Attach texture and generate mipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);


	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load and generate the texture 
	data = stbi_load("GridPattern1.png", &width, &height, &nrChannels, 0);
	// Attach texture and generate mipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int VAO_B;
	glGenVertexArrays(1, &VAO_B); //refernce vertex array object

	unsigned int VBO_B;
	glGenBuffers(1, &VBO_B); //reference buffer object

	unsigned int EBO_B;
	glGenBuffers(1, &EBO_B);

	glBindVertexArray(VAO_B); //bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_B); //bind VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesB), verticesB, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_B);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeciesB), indeciesB, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0); //unbind the Vertex array

	Shader Shader_B("vShader.glsl", "ftShader.glsl");




	//-------------------------- Circle Vertex data
	int n = 30;
	//float * vertices = (float*) malloc(sizeof(float) * n * 3);
	//float vertices[153]; // (n+1) * 3
	float* vertices = (float*)malloc(sizeof(float)*(n+1)*3);

	for (int t = 0; t <= n; t++) {

		if (t == 0) {
			vertices[0] = 0.0;
			vertices[1] = 0.0;
			vertices[2] = 0.0;
			continue;
		}
		vertices[(t+1) * 3 - 3] = cos(t*1.0f/n * 2 * pi<float>()); //x coordinate
		vertices[(t+1) * 3 - 2] = sin(t*1.0f/n * 2 * pi<float>()); //y coordinate
		vertices[(t+1) * 3 - 1] = 0.0f; //z coordinate
	}


	//------ Indices Array
	unsigned int* indicies = (unsigned int*)malloc(sizeof(unsigned int) * n * 3);  //(n)*3
	for (int i = 1; i <= (n); i++) {
		
		if (i == n) {
			indicies[i * 3 - 3] = 0;
			indicies[i * 3 - 2] = i;
			indicies[i * 3 - 1] = 1;
			continue;
		}

		indicies[i * 3 - 3] = 0;
		indicies[i * 3 - 2] = i;
		indicies[i * 3 - 1] = i+1; 

	}

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (n + 1) * 3, (vertices), GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * n * 3, indicies, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	

	//---------------Square Vertex Data
	float verticesS[] = {
	-1.0f, -1.0f, 0.0f,
	-1.0f,	1.0f, 0.0f,
	1.0f,	1.0f, 0.0f,
	1.0f, -1.0f, 0.0f
	};

	unsigned int indeciesS[] = {
		0,1,3,
		1,2,3
	};

	unsigned int VAO_S;
	glGenVertexArrays(1, &VAO_S);
	unsigned int VBO_S;
	glGenBuffers(1, &VBO_S);
	unsigned int EBO_S;
	glGenBuffers(1, &EBO_S);

	glBindVertexArray(VAO_S);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_S);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesS), (verticesS), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_S);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeciesS) , indeciesS, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//-------- Camera stuff
	vec3 target = vec3(0.0f);
	mat4 view;
	mat4 projection;
	view = lookAt(cameraPos, target, camemraUp);
	projection = perspective(radians(60.0f), 1.0f*WIDTH/HEIGHT, 0.1f, 100.0f);

	//--------- Matrices
	mat4 model = mat4(1.0f);
	//model = translate(model, circleOne.curPosition);
	//model = scale(model, circleOne.scale * vec3(1.0f));


	//--------- Uniform values
	myShader.use();
	myShader.setMat4("model", model);
	myShader.setMat4("view", view);
	myShader.setMat4("projection", projection);
	myShader.setVec3("color", vec3(1.0));

	Shader_B.use();
	Shader_B.setMat4("model", model);
	Shader_B.setMat4("view", view);
	Shader_B.setMat4("projection", projection);
	Shader_B.setVec3("color", vec3(1.0));
	Shader_B.setInt("texture1", 0);
	Shader_B.setInt("texture2", 1);


	//--------- Different Modes
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//antialisasing?

	//---------Create a link
	/*for (int i = 0; i < 10; i++) {
		VerletObject circleOne(vec3(i/20.0, i/20.0, 0.0f), 0.1, vec3(0.0f));
		myPhyWrld.addObject(circleOne);
	}

	for (int i = 0; i < 9; i++) {
		myPhyWrld.addLink(i, i+1, 0.05);
	}*/

	
	//---------------Modes
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//----------------------------Render Loop
	while (!glfwWindowShouldClose(myWindow)) {

		//----Frame calcualtions
		float curFrame = glfwGetTime();
		deltaTime = curFrame - lastFrame;
		lastFrame = curFrame;

		//---Handle Physics
		if ((curFrame - accumlatedDT) > timeStep) {
			myPhyWrld.updatePos(timeStep);
			accumlatedDT = curFrame;
			//cout << myPhyWrld.objectList[0].curPosition.x << "," << myPhyWrld.objectList[0].curPosition.y << endl;
		}
		
		

		//----Handle Inputs


		//----Clearing buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//---Render (activate shader, set uniforms, bind VAO, Draw)
		myShader.use();
		for (int i = 0; i < myPhyWrld.objectList.size(); i++) {
			model = mat4(1.0f);
			model = translate(model, myPhyWrld.objectList[i].curPosition);
			model = scale(model, myPhyWrld.objectList[i].scale * vec3(1.0f));

			myShader.setMat4("model", model);
			myShader.setVec3("color", myPhyWrld.objectList[i].color); //random color get color from object
			if (myPhyWrld.objectList[i].type == circle) {
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, (n + 1) * 3, GL_UNSIGNED_INT, 0);
			} else {
				glBindVertexArray(VAO_S);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			

			//----Draw Shadow
			model = mat4(1.0f);
			model = translate(model, myPhyWrld.objectList[i].curPosition + shadowDir*shadowDis);
			model = scale(model, myPhyWrld.objectList[i].scale * vec3(1.0f));
			myShader.setMat4("model", model);
			myShader.setVec3("color", vec3(shadowColor, shadowColor, shadowColor)); 
			if (myPhyWrld.objectList[i].type == circle) {
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, (n + 1) * 3, GL_UNSIGNED_INT, 0);
			} else {
				glBindVertexArray(VAO_S);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}

		}
		//----------Draw constraint
		/*glBindVertexArray(VAO);
		model = mat4(1.0f);
		model = scale(model, vec3(1.0f));
		myShader.setMat4("model", model);
		myShader.setVec3("color", vec3(0.1f));
		glDrawElements(GL_TRIANGLES, (n + 1) * 3, GL_UNSIGNED_INT, 0);*/


		glBindVertexArray(0);

		//------------Draw background
		Shader_B.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO_B);
	
		model = mat4(1.0f);
		model = translate(model, vec3(0.0f, 0.0f, -1.0f));
		model = scale(model, vec3(2.5f));
		Shader_B.setMat4("model", model);
		Shader_B.setVec3("color", vec3(1.0f));
		Shader_B.setFloat("mixWeight", 0.0f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		
		//----------Draw Ground
		/*model = mat4(1.0f);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		model = translate(model, vec3(0.0f, -3.4f, -0.99f));
		model = scale(model, vec3(2.5f));
		Shader_B.setMat4("model", model);
		Shader_B.setVec3("color", vec3(1.0f, 0.0f, 0.0f));
		Shader_B.setFloat("mixWeight", 1.0f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/
		
		
		
		//glDrawArrays(GL_TRIANGLES, 0, (n+1)*3);

		//--- Update buffers
		glfwSwapBuffers(myWindow);
		glfwPollEvents(); //handle callback functions

	}


	//---Cleaning things up
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	myShader.remove();
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!clickF) {
			clickF = true;
			//spawn a circle; //add an object maybe;
			//the position should be the position of the right clk mouse!
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			//	cout << xpos << ", "  << ypos << endl;
			
			//unneaccassry compolex mapping to world coordinates.
			/*
			vec2 coor = vec2(xpos, ypos);
			mat2 tran = mat2(2.0/WIDTH);
			tran[1][1] = -2.0 / WIDTH;
			coor = coor - vec2(400, 400);
			coor = tran * coor;
			//cout << coor.x << ", " << coor.y << endl;
			*/

			//----- Basic mapping to world coordinates.
			xpos = (xpos - 400) / 400.0;
			ypos = -1.0*((ypos - 400) / 400.0);
			//cout << xpos << ", " << ypos << endl;

			int temp = 0;
			//int temp = rand()%2;
			if (!temp) {
				//circle
				VerletObject circleOne(vec3(xpos, ypos, 0.0f), 0.1, vec3(0.0f));
				myPhyWrld.addObject(circleOne);
			} else {
				VerletObject squareOne(vec3(xpos, ypos, 0.0f), 0.1, 0.1, vec3(0.0f));
				myPhyWrld.addObject(squareOne);
			}


		}
	}
	clickF = false;
}