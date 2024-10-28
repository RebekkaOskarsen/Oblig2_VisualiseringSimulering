#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "shaderClass.h"
#include "Camera.h"
#include "Box.h"


using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int SCR_WIDTH, int SCR_HEIGHT); 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window); 

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(10.0f, 10.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

struct Vertex 
{
	glm::vec3 position;
};

vector<Vertex> loadAndCenterPoints(const string& filename) 
{
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Error: Unable to open file " << filename << endl;
		return {};  // Return an empty vector if the file can't be opened
	}

	vector<Vertex> vertices;
	glm::vec3 min(FLT_MAX), max(-FLT_MAX);
	string line;

	while (getline(file, line))
	{
		Vertex vertex;
		stringstream ss(line);
		ss >> vertex.position.x >> vertex.position.z >> vertex.position.y;

		if (ss.fail()) {
			cout << "Error: Failed to read coordinates on line: " << line << endl;
			continue;
		}

		min = glm::min(min, vertex.position);
		max = glm::max(max, vertex.position);
		vertices.push_back(vertex);
	}

	file.close();

	// Check if any points were loaded
	if (vertices.empty()) {
		cout << "Error: No points loaded from file " << filename << endl;
		return vertices;  // Return if no points were loaded
	}

	// Calculate the center of the bounding box
	glm::vec3 center = (min + max) / 2.0f;

	// Shift all vertices so the center of the cloud is at (0, 0, 0)
	for (auto& vertex : vertices) {
		vertex.position -= center;
	}

	cout << "Loaded " << vertices.size() << " points centered around " << center.x << ", " << center.y << ", " << center.z << endl;
	return vertices;
}

int main()
{
	glfwInit(); //Initialize GLFW

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Oblig 1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, mouse_callback);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // tell GLFW to capture our mouse

	//Initialize GLAD
	gladLoadGL();

	//Set the viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	Shader shaderProgram("default.vert", "default.frag");

	Box box;

	// Load and center points
	vector<Vertex> points = loadAndCenterPoints("vsim_las.txt");

	// Create VAO, VBO for points
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// Set point size
	glPointSize(5.0f); // Increase point size for better visibility

	glEnable(GL_DEPTH_TEST);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) // Check if the window should close
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		//Render
		glClearColor(0.5f, 0.3f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.Activate();

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); // Adjust far clipping plane if needed
		shaderProgram.setMat4("projection", projection);// pass the projection matrix to the shader

		glm::mat4 view = camera.GetViewMatrix();
		shaderProgram.setMat4("view", view); // pass the view matrix to the shader

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f)); // Adjust scaling for larger coordinates
		shaderProgram.setMat4("model", model);
		
	

		// Draw the point cloud
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, points.size());

		// Draw box
		//box.DrawBox();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int SCR_WIDTH, int SCR_HEIGHT)
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	std::cout << "Window resized with " << SCR_WIDTH << "Height" << SCR_HEIGHT << std::endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouse)
	{
		lastX = static_cast<float>(xpos);   // Update lastX with xpos
		lastY = static_cast<float>(ypos);   // Update lastY with ypos
		firstMouse = false;
	}

	float xoffset = static_cast<float>(xpos) - lastX;
	float yoffset = lastY - static_cast<float>(ypos); // reversed since y-coordinates go from bottom to top

	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	camera.ProcessMouseMovement(xoffset, yoffset);
}