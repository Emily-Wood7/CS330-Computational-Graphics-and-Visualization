/*
* Emily Wood
* CS 330
* 5/10/2023
*/

#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

namespace
{
	const char* const WINDOW_TITLE = "Emily Wood Assigment 2-3";	// macro for the window title

	// constants for the window
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;


	struct GLMesh	// stores the GL data relative to a given mesh
	{
		GLuint vao;		// handle for the vertex array object
		GLuint vbos[2];		// handle for the vertex buffer object
		GLuint nIndices;	// number of indices of the mesh
	};

	GLFWwindow* gWindow = nullptr;	// main GLFW window
	GLMesh gMesh;	// triangle mesh data
	GLuint gProgramId;	// shader program

}

// function prototypes
bool UInitialize(int, char*[], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh &mesh);
void UDestroyMesh(GLMesh &mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId);
void UDestroyShaderProgram(GLuint programId);

// vertex shader program source code
const char* vertexShaderSource = "#version 440 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec4 colorFromVBO;\n"
	"out vec4 colorFromVS;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"   colorFromVS = colorFromVBO;\n"
	"}\n\0";


// fragment shader program source code
const char* fragmentShaderSource = "#version 440 core\n"
	"in vec4 colorFromVS;\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"   FragColor = colorFromVS;\n"
	"}\n\0";

int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	UCreateMesh(gMesh);	// create the vertex buffer object

	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))	// create the shader program
		return EXIT_FAILURE;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// sets background to black

	while (!glfwWindowShouldClose(gWindow))	// render loop
	{
		UProcessInput(gWindow);	// input

		URender(); // render this frame

		glfwPollEvents();
	}

	UDestroyMesh(gMesh);		// release mesh data

	UDestroyShaderProgram(gProgramId);	//release shader program

	exit(EXIT_SUCCESS);		// terminates the program
}

bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// initialize and configure GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__	
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// if user has a mac
#endif

	// GLFW window creation
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);

	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// render a frame
void URender()
{
	// clear the background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(gProgramId);	// set the shader to be used
	glBindVertexArray(gMesh.vao);	// activate the vbos contained within the mesh's vao
	glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);	// draws the triangle
	glBindVertexArray(0);	// deactivate the vao
	glfwSwapBuffers(gWindow);	// flips the back buffer with the front buffer every frame
}

// implements the UCreateMesh function
void UCreateMesh(GLMesh &mesh)
{
	// specifies normalized device coordinates for triangle verices
	GLfloat verts[] =
	{
		-1.0f, 1.0f, 0.0f,      // top-first_third of the screen
		1.0f, 0.0f, 0.0f, 1.0f, // red

		-1.0f, 0.0f, 0.0f,     // bottom-left of the screen
		0.0f, 0.0f, 1.0f, 1.0f, // blue

		-0.5f, 0.0f, 0.0f,      // bottom-center of the screen
		0.0f, 1.0f, 0.0f, 1.0f, // green

		0.0f, 0.0f, 0.0f,       // top-second_third of the screen
		1.0f, 0.0f, 0.0f, 1.0f, // red

		0.0f, -1.0f, 0.0f,      // bottom-right of the screen
		0.0f, 1.0f, 0.0f, 1.0f  // green
	};

	glGenVertexArrays(1, &mesh.vao);	// generate multiple vaos or buffers at the same time
	glBindVertexArray(mesh.vao);

	glGenBuffers(2, mesh.vbos);		// creates two buffers
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);	// activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // sends vertex or coordinate data to the GPU

	// create a buffer object for the indices
	GLushort indices[] = { 0, 1, 2, 3, 2, 4 };	// using index 2 twice
	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// creates the vertex attribute pointer
	const GLuint floatsPerVertex = 3;	// number of coordinates per vertex
	const GLuint floatsPerColor = 4;	// (red, green, blue, alpha)

	// strides between vertex coordinates is 6 (x, y, r, g, b, a). a tightly packed stride is 0.
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);	// the number of floats before each

	// creates the vertix attribute pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
}

void UDestroyMesh(GLMesh &mesh)
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
}

bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint &programId)
{
	// compilation and linkage error reporting
	int success = 0;
	char infoLog[512];
	
	programId = glCreateProgram();	// create a shader program object

	// create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programId);    // Uses the shader program

	return true;
}

void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}