/*
* Emily Wood
* CS 330
* 5/28/2023
* 5-5 Milestone
* Texturing Objects in a 3D Scene
* meshes.h and meshes.cpp used from
* Professor Battersby
*/

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"    // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "meshes.h"

#include "camera.h"

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
	const char* const WINDOW_TITLE = "Emily Wood 5-5 Milestone: Texturing Objects in a 3D Scene"; // Macro for window title

	// Variables for window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;         // Handle for the vertex array object
		GLuint vbos[2];     // Handles for the vertex buffer objects
		GLuint nIndices;    // Number of indices of the mesh
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;
	// Texture ids
	GLuint textureTableId;
	GLuint textureMugId;
	bool isTextureOn = true;
	GLuint textureMugLogoId;
	GLuint textureCoffeeId;
	GLuint textureCoffeepotId;
	GLuint textureHandleId;
	GLuint textureSpoutId;
	// Shader program
	GLuint gProgramId;

	//Shape Meshes from Professor Brian
	Meshes meshes;

	// camera
	Camera gCamera(glm::vec3(0.0f, 5.0f, 20.0f));
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	glm::mat4 gProjection;
	bool gFirstMouse = true;

	// timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;

	// on/off variable for perspective mode
	bool isPerspective = true;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void SetProjection();


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1
	layout(location = 2) in vec2 textureCoordinate;  // texture data from Vertex Attrib Pointer 2

	out vec2 vertexTextureCoordinate; // variable to transfer texture data to the fragment shader

	//Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
	vertexTextureCoordinate = textureCoordinate;	// references incoming texture data
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
	in vec2 vertexTextureCoordinate;

	out vec4 fragmentColor;

	uniform sampler2D uTextureBase;
	uniform sampler2D uTextureExtra;
	uniform bool multipleTextures;

	void main()
	{
		fragmentColor = texture(uTextureBase, vertexTextureCoordinate);
		if (multipleTextures)
		{
			vec4 extraTexture = texture(uTextureExtra, vertexTextureCoordinate);
			if (extraTexture.a != 0.0)
				fragmentColor = extraTexture;
		}
		
	}
);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
	for (int j = 0; j < height / 2; ++j)
	{
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i)
		{
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}


int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	// Create the mesh
	meshes.CreateMeshes();

	// Create the shader program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
		return EXIT_FAILURE;

	// load textures
	const char* texFilename = "resources/textures/Concrete042A_1K_Color.jpg";
	if (!UCreateTexture(texFilename, textureTableId))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "resources/textures/Terrazzo001_1K_Color.jpg";
	if (!UCreateTexture(texFilename, textureMugId))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "resources/textures/bulleitbourbon.png";
	if (!UCreateTexture(texFilename, textureMugLogoId))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "resources/textures/Wood028_1K_Color.jpg";
	if (!UCreateTexture(texFilename, textureCoffeeId))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "resources/textures/coffeepot.jpg";
	if (!UCreateTexture(texFilename, textureCoffeepotId))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "resources/textures/Metal028_1K_Color.jpg";
	if (!UCreateTexture(texFilename, textureHandleId))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "resources/textures/white.jpg";
	if (!UCreateTexture(texFilename, textureSpoutId))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	// TODO: Add more textures here

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	glUseProgram(gProgramId);
	// We set the texture as texture unit 0
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);
	// We set the texture as texture unit 1.
	glUniform1i(glGetUniformLocation(gProgramId, "uTextureExtra"), 1);

	// Sets the background color of the window to black (it will be implicitely used by glClear)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(gWindow))
	{
		// per-frame timing
		// --------------------
		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		// input
		// -----
		UProcessInput(gWindow);

		// Render this frame
		URender();

		glfwPollEvents();
	}

	// Release mesh data
	meshes.DestroyMeshes();

	// Release textures
	UDestroyTexture(textureTableId);
	UDestroyTexture(textureMugId);
	UDestroyTexture(textureCoffeeId);
	UDestroyTexture(textureMugLogoId);

	// Release shader program
	UDestroyShaderProgram(gProgramId);

	exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// GLFW: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW: window creation
	// ---------------------
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

	// GLEW: initialize
	// ----------------
	// Note: if using GLEW version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
	static const float cameraSpeed = 2.5f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);

	// changes the projection view from perspective to ortho
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		isPerspective = true;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		isPerspective = false;
	}
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: Whenever the mouse moves, this callback is called.
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: Whenever the mouse scroll wheel scrolls, this callback is called.
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gCamera.ProcessMouseScroll(yoffset);
}

// glfw: Handle mouse button events.
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			cout << "Left mouse button pressed" << endl;
		else
			cout << "Left mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			cout << "Middle mouse button pressed" << endl;
		else
			cout << "Middle mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			cout << "Right mouse button pressed" << endl;
		else
			cout << "Right mouse button released" << endl;
	}
	break;

	default:
		cout << "Unhandled mouse button event" << endl;
		break;
	}
}

// Function called to change projection
void SetProjection()
{
	if (isPerspective)
	{
		glm::mat4 view = gCamera.GetViewMatrix();
		gProjection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}
	else {
		glm::mat4 view = gCamera.GetViewMatrix();
		gProjection = glm::ortho(-(800.0f / 100), 800.0f / 100, -(600.0f / 100), 600.0f / 100, 0.1f, 100.0f);
	}
}

// Functioned called to render a frame
void URender()
{
	glm::mat4 scale;
	glm::mat4 rotation;
	glm::mat4 translation;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	GLint modelLoc;
	GLint viewLoc;
	GLint projLoc;
	GLint objectColorLoc;

	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera/view transformation
	view = gCamera.GetViewMatrix();

	// Creates a projection
	SetProjection();

	// Set the shader to be used
	glUseProgram(gProgramId);

	// Retrieves and passes transform matrices to the Shader program
	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objectColorLoc = glGetUniformLocation(gProgramId, "uObjectColor");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(gProjection));

	/*
	--------------------------------------begin plane the scene sits on------------------------------------------------------
	*/

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPlaneMesh.vao);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTableId);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(6.0f, 1.0f, 6.0f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gPlaneMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*
	--------------------------------------end plane the scene sits on--------------------------------------------------
	*/
	/*
	--------------------------------------begin coffee mug object------------------------------------------------------
	*/

	// coffee mug
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// use for second logo texture
	isTextureOn = true;
	GLuint multipleTexturesLoc = glGetUniformLocation(gProgramId, "multipleTextures");
	glUniform1i(multipleTexturesLoc, isTextureOn);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMugId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureMugLogoId);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.0f, 2.0f, 1.0f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(4.0f, 0.0f, 4.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gProgramId, objectColorLoc, 1.0f, 0.961f, 0.933f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// turn off second texture
	// so it doesn't apply to remaining objects
	isTextureOn = false;
	multipleTexturesLoc = glGetUniformLocation(gProgramId, "multipleTextures");
	glUniform1i(multipleTexturesLoc, isTextureOn);

	// cylinder for the coffee
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureCoffeeId);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.80f, -1.5f, 0.80f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(4.0f, 2.05f, 4.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gProgramId, objectColorLoc, 0.545f, 0.271f, 0.075f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gTorusMesh.vao);

	// coffee mug handle
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gTorusMesh.vao);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMugId);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.5f, 0.90f, 2.0f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(5.0f, 1.0f, 4.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gProgramId, objectColorLoc, 1.0f, 0.961f, 0.933f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, meshes.gTorusMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	
	/*
	--------------------------------------end coffee mug object------------------------------------------------------
	*/

	/*
	* ------------------------------------begin coffee pot object------------------------------------------------------
	*/
	/*
	* Coffee Pot
	*/
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gTaperedCylinderMesh.vao);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureCoffeepotId);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(2.5f, 3.75f, 2.5f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(2.5f, 0.0f, -1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gProgramId, objectColorLoc, 0.0f, 1.0f, 1.0f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*
	* Coffee Pot Lid
	*/
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandleId);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.5f, 0.125f, 1.5f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(2.5f, 3.75f, -1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gProgramId, objectColorLoc, 1.0f, 1.0f, 0.0f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*
	* Ball on Lid
	*/
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gSphereMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(2.5f, 3.75f, -1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gProgramId, objectColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*
	* Coffee Pot Handle
	*/
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gTorusMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(1.0f, 2.375f, 2.75f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(4.0f, 1.25f, -0.75f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gProgramId, objectColorLoc, 0.0f, 0.0f, 1.0f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, meshes.gTorusMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*
	* Spout
	*/
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPyramid3Mesh.vao);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureSpoutId);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.875f, 0.875f, 0.875f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(90.0f), glm::vec3(1.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(1.125f, 3.5f, -1.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gProgramId, objectColorLoc, 0.0f, 0.5f, 0.5f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid3Mesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	/*
	* ------------------------------------end coffee pot object------------------------------------------------------
	*/

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{
		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}

void UDestroyTexture(GLuint textureId)
{
	glGenTextures(1, &textureId);
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
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