#define STB_IMAGE_IMPLEMENTATION

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include "Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;

Window mainWindow;
Mesh* floorPlane;
Camera camera;

Texture grassTexture;
Shader *mshader;

Model helicaptor;
Skybox skybox;


GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat helicaptorAngle = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateObjects() 
{
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	floorPlane = new Mesh();
	floorPlane->CreateMesh(floorVertices, floorIndices, 32, 6);
}

void CreateShaders()
{
	mshader = new Shader();
	mshader->CreateFromFiles(vShader, fShader);
}

void RenderScene()
{
	glm::mat4 model(1.0f);

	model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, -10.0f, -3.0f));
	model = glm::rotate(model, -11.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	grassTexture.UseTexture();

	floorPlane->RenderMesh();

	helicaptorAngle += 0.1f;
	if (helicaptorAngle > 360.0f)
	{
		helicaptorAngle = 0.1f;
	}

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -helicaptorAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-2.0f, 1.0f, 0.0f));
	model = glm::rotate(model, -80.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	helicaptor.RenderModel();
}

void RenderPass(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	mshader->UseShader();

	uniformModel = mshader->GetModelLocation();
	uniformProjection = mshader->GetProjectionLocation();
	uniformView = mshader->GetViewLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	mshader->SetTexture(1);
	mshader->Validate();

	RenderScene();
}

int main() 
{
	mainWindow = Window(1366, 768); 
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	grassTexture = Texture("Textures/grass.png");
	grassTexture.LoadTextureA();

	helicaptor = Model();
	helicaptor.LoadModel("Models/heli.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/posx.jpg");
	skyboxFaces.push_back("Textures/Skybox/negx.jpg");
	skyboxFaces.push_back("Textures/Skybox/posy.jpg");
	skyboxFaces.push_back("Textures/Skybox/negy.jpg");
	skyboxFaces.push_back("Textures/Skybox/posz.jpg");
	skyboxFaces.push_back("Textures/Skybox/negz.jpg");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		if (mainWindow.getsKeys()[GLFW_KEY_L])
		{
			mainWindow.getsKeys()[GLFW_KEY_L] = false;
		}

		glClear(GL_DEPTH_BUFFER_BIT);

		RenderScene();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		RenderPass(camera.calculateViewMatrix(), projection);
		mainWindow.swapBuffers();
	}

	return 0;
}