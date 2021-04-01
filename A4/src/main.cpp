#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"

using namespace std;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from

shared_ptr<Camera> camera;
shared_ptr<Program> program;

glm::vec3 move_camera;

double scale_time = 1;

glm::vec3 translate_time(0, 0, 0);

bool keyToggles[256] = {false}; // only for English keyboards!

struct object {
	shared_ptr<Shape> shape;
	string obj_file = "";
	glm::vec3 color;
	glm::vec3 translate;
	glm::vec3 scale;

} objects[26];

struct material {
	glm::vec3 ka, kd, ks;
	float s;
} materials[3];

struct light {
	glm::vec3 position;
	float intensity;
} lights[2];

string objfile;

// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	
}

// This function is called when the mouse is clicked
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = (mods & GLFW_MOD_SHIFT) != 0;
		bool ctrl  = (mods & GLFW_MOD_CONTROL) != 0;
		bool alt   = (mods & GLFW_MOD_ALT) != 0;
		camera->mouseClicked((float)xmouse, (float)ymouse, shift, ctrl, alt);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	camera->mouseMoved((float)xmouse, (float)ymouse);

}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
	switch (key) {
	case 'w' :
		move_camera[2]++;
		//move
		//control material
		break;
	case 's':
		move_camera[2]--;
		break;
	case 'a':
		move_camera[0]++;
		break;
	case 'd':
		move_camera[0]--;
		break;

	default:
		break;

	}
}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	//make materials
	materials[0].ka = glm::vec3(0.05f, 0.05f, 1.0f);
	materials[0].kd = glm::vec3(0.3f, 0.4f, 0.5f);
	materials[0].ks = glm::vec3(0.0f, 1.0f, 0.0f);
	materials[0].s = 100.0f;

	//make lights
	lights[0].position = glm::vec3(1.0f, 1.0f, 1.0f);
	lights[0].intensity = 0.8f;

	//make programs
	program = make_shared<Program>();


	string vertName = "vert0.glsl";
	string fragName = "frag0.glsl";

	program->setShaderNames(RESOURCE_DIR + vertName, RESOURCE_DIR + fragName);
	program->setVerbose(true);
	program->init();
	program->addAttribute("aPos");
	program->addAttribute("aNor");
	program->addUniform("MV");
	program->addUniform("ITMV");
	program->addUniform("P");
	program->addUniform("light0_position");
	program->addUniform("light0_intensity");
	program->addUniform("lights[1]");
	program->addUniform("ka");
	program->addUniform("kd");
	program->addUniform("ks");
	program->addUniform("s");
	program->setVerbose(false);
	
	
	camera = make_shared<Camera>();
	camera->setInitDistance(5.0f);
	for (int i = 0; i < size(objects); i++) {
		objects[i].shape = make_shared<Shape>();


		//sun
		if (i == 0) {
			objects[i].shape->loadMesh(RESOURCE_DIR + "sphere.obj");
			objects[i].scale = glm::vec3(5, 5, 5);
			objects[i].color = glm::vec3(1, 1, .3);
			objects[i].translate = glm::vec3(20, 20, 20);
		}
		//ground
		else if (i == size(objects) - 1) {
			objects[i].shape->loadMesh(RESOURCE_DIR + "cube.obj");
			objects[i].scale = glm::vec3(2000, .1, 2000);
			objects[i].color = glm::vec3(.2, 1, .3);
			objects[i].translate = glm::vec3(0, 0, 0);

		}
		else if (i % 2 == 0) {
			objects[i].shape->loadMesh(RESOURCE_DIR + "bunny.obj");
			objects[i].color[0] = i * 5;
			objects[i].color[1] = 2.0 / i;
			objects[i].scale = glm::vec3(1, 1, 1);
			objects[i].translate = glm::vec3((rand()%30)-15, .54, (rand()%30)-15);
		}
		else {
			objects[i].shape->loadMesh(RESOURCE_DIR + "teapot.obj");
			objects[i].color[0] = i * 5;
			objects[i].color[1] = 2.0 / i;
			objects[i].scale = glm::vec3(1, 1, 1);
			objects[i].translate = glm::vec3((rand() % 30)- 15, .3, (rand() % 30)-15);
		}

		objects[i].shape->fitToUnitBox();
		objects[i].shape->init();
	}
	GLSL::checkError(GET_FILE_LINE);
}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	if(keyToggles[(unsigned)'z']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	// Matrix stacks
	
	if (glfwGetTime() < 5) {
		scale_time += .005;
		translate_time[1] += .002;
	}
	else if(glfwGetTime() < 10){
		scale_time -= .005;
		translate_time[1] -= .002;
	}
	else {
		glfwSetTime(0.0);
	}

	// Apply camera transforms
	for (int i = 0; i < size(objects); i++) {
		auto P = make_shared<MatrixStack>();
		auto MV = make_shared<MatrixStack>();

		P->pushMatrix();
		camera->applyProjectionMatrix(P);
		MV->pushMatrix();
		MV->translate(move_camera);
		camera->applyViewMatrix(MV);

		MV->translate(objects[i].translate);

		if (i != 0 && i != size(objects) - 1) {
			MV->translate(translate_time);
		}

		MV->scale(objects[i].scale);
		if (i != 0 && i != size(objects) - 1) {
			MV->scale(scale_time);
		}

		program->bind();
		glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(program->getUniform("ITMV"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(MV->topMatrix()))));
		glUniform3f(program->getUniform("light0_position"), lights[0].position[0], lights[0].position[1], lights[0].position[2]);
		glUniform1f(program->getUniform("light0_intensity"), lights[0].intensity);
		glUniform3f(program->getUniform("ka"), objects[i].color[0], objects[i].color[1], objects[i].color[2]);
		glUniform3f(program->getUniform("kd"), materials[0].kd[0], materials[0].kd[1], materials[0].kd[2]);
		glUniform3f(program->getUniform("ks"), materials[0].ks[0], materials[0].ks[1], materials[0].ks[2]);
		glUniform1f(program->getUniform("s"), materials[0].s);
		objects[i].shape->draw(program);
		program->unbind();

		MV->popMatrix();
		P->popMatrix();
	}
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	//command line isnt working in Visual Studio 2019

	RESOURCE_DIR = "../resources/";
	objfile = "bunny.obj";


	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Chris Wray", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
