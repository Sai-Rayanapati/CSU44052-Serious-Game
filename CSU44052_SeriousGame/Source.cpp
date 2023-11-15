#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <random>

#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>

#include "Text.h"
#include "Texture.h"
#include "Mesh.h"
#include "Skybox.h"
#include "Source.h"
#include "Camera.h"
#include "Shader.h"

// Window Dimensions
#define WIDTH 1000
#define HEIGHT 800

// Gameplay settings
#define GAMEPLAY_TIME 60.0f
#define NO_OF_TREES 500
#define NO_OF_GARBAGEBAGS 10
#define NO_OF_POWERUPS 5
#define NO_OF_BIRDS 5

std::vector<Mesh*> MeshList;
std::vector<StarProps> starPropsList;
std::vector<GarbageBagProps> garbageBagPropsList;
std::vector<BirdProps> birdPropsList;

// Classes instances
Texture groundTexture;
Mesh* groundPlane = new Mesh();
Skybox skybox;
Text gameText;

// Set number of trees
Model tree (NO_OF_TREES);

// Set number of garbage bags 
Model garbageBags(NO_OF_GARBAGEBAGS);

// Set number of Power ups
Model powerUps(NO_OF_POWERUPS);

// Set number of Birds 
Model leftWing(NO_OF_BIRDS);
Model birdBody(NO_OF_BIRDS);
Model rightWing(NO_OF_BIRDS);

Camera camera(glm::vec3(0.0f, 0.5f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // position and up vectors

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouseMove = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float timeElapsed = 0.0f;
float powerUpTimer = 0.0f;

int score = 0;

unsigned int instancedShaderProgram;
unsigned int shaderProgram;

std::vector<glm::mat4> tree_matrices, birdBody_matrices, leftWing_matrices, rightWing_matrices;
std::vector<float> starRotationAngles(NO_OF_POWERUPS, 0.0f);

int num_indices;
unsigned int vao, vbo, ibo, texture_id;

glm::mat4 projection_matrix;

// check game end
bool isGameFrozen = false;

// specular lighting materials
float woodShininessValue = 30.0f;
float woodSpecularIntensity = 0.3f;

float plasticShininessValue = 100.0f;
float plasticSpecularIntensity = 0.8f;

float grassShininessValue = 70.0f;
float grassSpecularIntensity = 0.8f;

// Check collision with bags
void checkBagPickup()
{
	// bag radius
	float radius = 0.2f;

	for (auto i = garbageBagPropsList.begin(); i != garbageBagPropsList.end(); i++)
	{
		glm::vec2 pos(i->translation.x - 1.078, i->translation.z+ 0.048);

		glm::vec2 camPos(camera.Position.x, camera.Position.z);

		float distance = glm::distance(camPos, pos);

		// Collision detected
		if (distance < radius)
		{
			// remove bag
			i = garbageBagPropsList.erase(i);
			score++;
			
			if (score >= NO_OF_GARBAGEBAGS)
			{
				isGameFrozen = true;
				return;
			}
			break;
		}
	}
}

// check collison with power ups
void checkPowerUpPickup()
{
	// collison radius
	float radius = 0.1f;

	for (auto i = starPropsList.begin(); i != starPropsList.end(); i++)
	{
		glm::vec2 pos(i->translation.x, i->translation.z);
		glm::vec2 camPos(camera.Position.x, camera.Position.z);
		float distance = glm::distance(camPos, pos);

		// Collision detected
		if (distance < radius)
		{
			// remove power up
			i = starPropsList.erase(i);
			powerUpTimer = 10.0f;

			if (camera.MoveSpeed == 2.5f)
			{
				camera.MoveSpeed = camera.MoveSpeed * 2.0f;
			}
			break;
		}
	}
}

// process key inputs
void processMovement(GLFWwindow* window, float deltaTime) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		isGameFrozen = true;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.keyControl(FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.keyControl(BACKWARD, deltaTime);
	}
		
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.keyControl(LEFT, deltaTime);
	}
		
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.keyControl(RIGHT, deltaTime);
	}
		
	//check collision with garbage bags
	checkBagPickup();

	// check collision with powerups 
	checkPowerUpPickup();
}

// ground Plane
void generatePlane()
{
	GLfloat groundVertices[] = {
		//	   x ,   y ,     z,     normals                 texture coordinates 
			-1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
			-1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		100.0f, 0.0f,
			1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		100.0f, 100.0f,
			1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 100.0f,
	};

	unsigned int groundIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	groundPlane->CreateMesh(groundVertices, groundIndices, 32, 6);
	MeshList.push_back(groundPlane);
}

// render skybox into the scene
void RenderSkyBox()
{
	glm::mat4 view_matrix = glm::mat4(glm::mat3(camera.calculateViewMatrix())); // remove translation from the view matrix
	skybox.DrawSkybox(view_matrix, projection_matrix);
}

// initilise the models by loading them from the obj files
void Init() 
{
	// ------------------------------------     TREES     ------------------------------------------------------------
	tree.LoadModelInstanced("models/tree/Tree.obj", "models/tree");
	
	for (int i = 0; i < NO_OF_TREES; i++) {
		glm::vec3 translation = glm::vec3(glm::linearRand(-20.0f, 20.0f), 0.0f, glm::linearRand(-20.0f, 20.0f));

		// Create object boundings for collision detection 
		ObjectBounding boundingArea;
		boundingArea.position = translation;
		boundingArea.radius = 0.5f;

		camera.objectBoundings.push_back(boundingArea);

		// Construct the transformation matrix from the translation vector
		glm::mat4 transformation_matrix = glm::translate(glm::mat4(1.0f), translation);
		tree_matrices.push_back(transformation_matrix);
	}

	// ------------------------------------     BIRDS     ------------------------------------------------------------
	birdBody.LoadModelInstanced("models/bird/body.obj", "models/bird");
	leftWing.LoadModelInstanced("models/bird/wingleft.obj", "models/bird");
	rightWing.LoadModelInstanced("models/bird/wingright.obj", "models/bird");

	for (size_t i = 0; i < NO_OF_BIRDS; i++)
	{
		glm::mat4 transformation_matrix = glm::mat4(1.0);
		birdBody_matrices.push_back(transformation_matrix);

		transformation_matrix = glm::mat4(1.0f);
		leftWing_matrices.push_back(transformation_matrix);

		transformation_matrix = glm::mat4(1.0f);
		rightWing_matrices.push_back(transformation_matrix);

		BirdProps birdProps;

		birdProps.birdPosition = glm::vec3(0.0f, 1.3f, 0.0f);
		birdProps.birdVelocity = glm::vec3(0.0f, 0.0f, 0.0f);

		auto randomDegrees = glm::linearRand(0.0f, 360.0f);

		float yawRad = glm::radians(randomDegrees);

		birdProps.birdVelocity = glm::vec3(glm::cos(yawRad), 0.0f, glm::sin(yawRad));

		birdPropsList.push_back(birdProps);
	}

	// ------------------------------------     GARBAGE BAGS     ------------------------------------------------------------
	garbageBags.LoadModelInstanced("models/bag/Garbage_Bag.obj", "models/bag");
	
	for (int i = 0; i < NO_OF_GARBAGEBAGS; i++) {
		glm::vec3 translation = glm::vec3(glm::linearRand(-20.0f, 20.0f), 0.0f, glm::linearRand(-20.0f, 20.0f));

		glm::mat4 transformation_matrix(1.0f);

		transformation_matrix = glm::translate(transformation_matrix, translation);

		transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.005f));

		GarbageBagProps garbageBagProps;
		garbageBagProps.translation = glm::vec3(translation);
		garbageBagProps.matrix = transformation_matrix;

		garbageBagPropsList.push_back(garbageBagProps);
	}

	// ------------------------------------     POWERUPS     ------------------------------------------------------------
	powerUps.LoadModelInstanced("models/star/Star_round.obj", "models/star");

	for (size_t i = 0; i < NO_OF_POWERUPS; i++)
	{
		glm::vec3 translation = glm::vec3(glm::linearRand(-20.0f, 20.0f), 0.3f, glm::linearRand(-20.0f, 20.0f));

		glm::mat4 transformation_matrix(1.0f);

		transformation_matrix = glm::translate(transformation_matrix, translation);
		transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.2f));
		transformation_matrix = glm::rotate(transformation_matrix, glm::radians(90.0f), glm::vec3(0.5f, 0.0f, 1.0f));

		starRotationAngles[i] = 0.0f;

		StarProps starProps;
		starProps.translation = translation;
		starProps.matrix = transformation_matrix;

		starPropsList.push_back(starProps);
	}

}

// creating the shader programs
void InitShaders()
{
	// for the objects loaded from obj files
	instancedShaderProgram = Shader::GetInstance()->CreateProgram("shaders/shader_instanced.vert", "shaders/shader_instanced.frag");

	// for the ground
	shaderProgram = Shader::GetInstance()->CreateProgram("shaders/shader.vert", "shaders/shader.frag");
}

// variables for the bird wings flapping
float wingAngle = 0.0f;              
float maxWingRoatation = 90.0f;      
float wingRotationSpeed = 180.0f;    
bool increasingAngle = true;     
float angleIncrement = 0.0f;

// renders the initilised models into the scene
void RenderModels (glm::mat4& view) 
{
	glUseProgram(instancedShaderProgram);

	// Set to use texture
	unsigned int hasTexture_location = glGetUniformLocation(instancedShaderProgram, "hasTexture");
	glUniform1i(hasTexture_location, 1);

	int shininessLocation = glGetUniformLocation(instancedShaderProgram, "shininess");
	int specularIntensityLocation = glGetUniformLocation(instancedShaderProgram, "specularIntensity");

	glm::mat4 model_matrix = glm::mat4(1.0f);

	// ------------------------------------     TREES     ------------------------------------------------------------

	glUniform1f(shininessLocation, woodShininessValue);
	glUniform1f(specularIntensityLocation, woodSpecularIntensity);

	tree.DrawInstanced(instancedShaderProgram, model_matrix, view, projection_matrix, tree_matrices);

	// ------------------------------------     Birds     ------------------------------------------------------------
	// logic for the wings to flap
	angleIncrement = wingRotationSpeed * deltaTime;

	if (increasingAngle)
	{
		wingAngle += angleIncrement;

		if (wingAngle >= maxWingRoatation)
		{
			wingAngle = maxWingRoatation;
			increasingAngle = false;
		}
	}
	else
	{
		wingAngle = std::max(wingAngle - angleIncrement, 0.0f);

		if (wingAngle <= 0.0f)
		{
			wingAngle = 0.0f;
			increasingAngle = true;
		}
	}

	for (size_t i = 0; i < NO_OF_BIRDS; i++)
	{
		// Update the bird's position using bird's velocity 
		birdPropsList[i].birdPosition += birdPropsList[i].birdVelocity * deltaTime;

		model_matrix = glm::mat4(1.0f);
		model_matrix = glm::translate(model_matrix, birdPropsList[i].birdPosition);
		model_matrix = glm::scale(model_matrix, glm::vec3(0.05f));

		// To make the bird face the direction of its velocity 
		float yaw = glm::atan(birdPropsList[i].birdVelocity.z, birdPropsList[i].birdVelocity.x) - glm::radians(90.0f);

		// Rotate the bird model around its local X-axis (to make it face forward)
		model_matrix = glm::rotate(model_matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model_matrix = glm::rotate(model_matrix, yaw, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 transformation_matrix(1.0f);

		// Left wing transformation
		transformation_matrix = glm::rotate(transformation_matrix, glm::radians(wingAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		transformation_matrix = glm::translate(transformation_matrix, glm::vec3(0.5f, 0.0f, 0.0f));
		leftWing_matrices[i] = model_matrix * transformation_matrix;

		transformation_matrix = glm::mat4(1.0f);

		// Right wing transformation
		transformation_matrix = glm::rotate(transformation_matrix, glm::radians(-wingAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		transformation_matrix = glm::translate(transformation_matrix, glm::vec3(-1.0f, 0.0f, 0.0f));
		rightWing_matrices[i] = model_matrix * transformation_matrix;

		birdBody_matrices[i] = model_matrix;
	}

	model_matrix = glm::mat4(1.0f);

	birdBody.DrawInstanced(instancedShaderProgram, model_matrix, view, projection_matrix, birdBody_matrices);
	leftWing.DrawInstanced(instancedShaderProgram, model_matrix, view, projection_matrix, leftWing_matrices);
	rightWing.DrawInstanced(instancedShaderProgram, model_matrix, view, projection_matrix, rightWing_matrices);

	glUseProgram(instancedShaderProgram);
	glUniform1i(shininessLocation, 0);
	glUniform1i(specularIntensityLocation, 0);

	// ------------------------------------     GARBAGE BAGS     ------------------------------------------------------------
	model_matrix = glm::mat4(1.0f);

	glUseProgram(instancedShaderProgram);
	glUniform1i(shininessLocation, plasticShininessValue);
	glUniform1i(specularIntensityLocation, plasticSpecularIntensity);

	std::vector<glm::mat4> matrices;

	// Used while removing bags
	for (size_t i = 0; i < garbageBagPropsList.size(); i++)
	{
		matrices.push_back(garbageBagPropsList.at(i).matrix);
	}

	garbageBags.DrawInstanced(instancedShaderProgram, model_matrix, view, projection_matrix, matrices);

	matrices.clear();

	// ------------------------------------     POWERUPS     ------------------------------------------------------------
	model_matrix = glm::mat4(1.0f);

	for (size_t i = 0; i < starPropsList.size(); i++)
	{
		// used while removing power ups 
		matrices.push_back(starPropsList.at(i).matrix);

		starRotationAngles[i] = 1.0f;
		glm::mat4 transformation_matrix = starPropsList[i].matrix;
		transformation_matrix = glm::rotate(transformation_matrix, glm::radians(starRotationAngles[i]), glm::vec3(1.0f, 0.0f, 0.0f));
		starPropsList[i].matrix = transformation_matrix;
	}

	powerUps.DrawInstanced(instancedShaderProgram, model_matrix, view, projection_matrix, matrices);

	glUseProgram(0);
}

// Calculates mouse inputs to rotate the camera 
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = (float)(xposIn);
	float ypos = (float)(yposIn);

	if (firstMouseMove) {
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.mouseControl(xoffset, yoffset);
}

// Use shader for the ground (shaderProgram)
void useShaderProgram(glm::mat4 view)
{
	glUseProgram(shaderProgram);
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::scale(model_matrix, glm::vec3(100.0f));

	// Set the model, view, and projection matrices in the shader
	GLint modelLoc = glGetUniformLocation(shaderProgram, "modelMatrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));

	GLint viewLoc1 = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewLoc1, 1, GL_FALSE, glm::value_ptr(view));

	GLint projLoc = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	int shininessLocation = glGetUniformLocation(shaderProgram, "shininess");
	int specularIntensityLocation = glGetUniformLocation(shaderProgram, "specularIntensity");
	glUniform1f(shininessLocation, grassShininessValue);
	glUniform1f(specularIntensityLocation, grassSpecularIntensity);
}

int main()
{
	// opengl set up
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CSU44052 Serious Game", NULL, NULL);
	if (window == NULL)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, MouseCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD");
		return 1;
	}

	// Initilising text rendering
	glm::mat4 orthoProjection = glm::ortho(0.0f, (float)(WIDTH), 0.0f, (float)(HEIGHT));
	gameText.intShader(orthoProjection);
	gameText.InitTextRendering();

	// Initilising a skybox 
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("skybox/right.jpg");
	skyboxFaces.push_back("skybox/left.jpg");
	skyboxFaces.push_back("skybox/top.jpg");
	skyboxFaces.push_back("skybox/bottom.jpg");
	skyboxFaces.push_back("skybox/front.jpg");
	skyboxFaces.push_back("skybox/back.jpg");

	skybox = Skybox(skyboxFaces);

	InitShaders();
	
	Init();
	
	// Initilise ground 
	generatePlane();

	// load texture
	groundTexture = Texture((char*)"textures/grass.jpg");
	groundTexture.LoadTexture();

	lastFrame = (float)(glfwGetTime());

	while (!glfwWindowShouldClose(window))
	{
		if (!isGameFrozen)
		{
			float currentFrame = (float)(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			timeElapsed += deltaTime;

			// Gameplay timer
			if (timeElapsed >= GAMEPLAY_TIME)
			{
				isGameFrozen = true;
			}

			if (powerUpTimer > 0.0f)
			{
				powerUpTimer -= deltaTime;

				if (powerUpTimer <= 0.0f)
				{
					powerUpTimer = 0.0f;
					camera.MoveSpeed = 2.5f;
				}
			}

			processMovement(window, deltaTime);

			glViewport(0, 0, WIDTH, HEIGHT);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_CULL_FACE);

			glm::mat4 view = camera.calculateViewMatrix();

			projection_matrix = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

			// Use grass texture
			groundTexture.UseTexture();

			useShaderProgram(view);

			// Render the plane
			MeshList[0]->RenderMesh();

			// Render Skybox 
			glDepthFunc(GL_LEQUAL);
			RenderSkyBox();
			glDepthFunc(GL_LESS);

			if(!isGameFrozen)
			RenderModels(view);

			// Render score and time left onto screen
			std::string scoreStr = "Score: " + std::to_string(score) + "/10";
			gameText.RenderText(scoreStr, 25.0f, HEIGHT - 25.0f, 0.5f, glm::vec3(1.0f));
			gameText.RenderText("Time: " + std::to_string((int)GAMEPLAY_TIME - (int)timeElapsed), 25.0f, HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f));
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (isGameFrozen)
		{
			if (score == NO_OF_GARBAGEBAGS)
			{
				printf("Your final score is: %d \n", score);
				printf("Congrats you have collected all bags" );
			}
			else
			{
				printf("Your final score is: %d \n", score);
				printf("Better luck next time!!");
			}

			glfwSetWindowShouldClose(window, true);
		}
	}

	//release
	glfwTerminate();
	return 0;
}