/*
* Emily Wood
* CS 330
* 6/11/2023
* 6-6 Milestone
* Lighting Complex Objects
* meshes.h, meshes.cpp, and lighting used from
* Professor Battersby
*/

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "meshes.h"

using namespace std; // Uses the standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
	const char* const WINDOW_TITLE = "Emily Wood 6-6 Milestone: Lighting Complex Objects"; // Macro for window title

	// Variables for window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;     // Handle for the vertex array object
		GLuint vbos[2];     // Handles for the vertex buffer objects
		GLuint nVertices;   // Number of vertices of the mesh
		GLuint nIndices;
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;
	// Shader program
	GLuint gSurfaceProgramId;
	GLuint gLightProgramId;
	// Texture
	GLuint textureTableId;
	GLuint textureMugId;
	bool isTextureOn = true;
	GLuint textureMugLogoId;
	GLuint textureMugHandleId;
	GLuint textureCoffeeId;
	GLuint textureCoffeepotId;
	GLuint textureHandleId;
	GLuint textureSpoutId;
	glm::vec2 gUVScale(1.0f, 1.0f);

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

///////////////////////////////////////////////////////////////////////////////////////////////////////
/* Surface Vertex Shader Source Code*/
const GLchar* surfaceVertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 vertexPosition; // VAP position 0 for vertex position data
	layout(location = 1) in vec3 vertexNormal; // VAP position 1 for normals
	layout(location = 2) in vec2 textureCoordinate;

	out vec3 vertexFragmentNormal; // For outgoing normals to fragment shader
	out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
	out vec2 vertexTextureCoordinate;

	//Uniform / Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(vertexPosition, 1.0f); // Transforms vertices into clip coordinates

		vertexFragmentPos = vec3(model * vec4(vertexPosition, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

		vertexFragmentNormal = mat3(transpose(inverse(model))) * vertexNormal; // get normal vectors in world space only and exclude normal translation properties
		vertexTextureCoordinate = textureCoordinate;
	}
);
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Surface Fragment Shader Source Code*/
const GLchar* surfaceFragmentShaderSource = GLSL(440,

	in vec3 vertexFragmentNormal; // For incoming normals
	in vec3 vertexFragmentPos; // For incoming fragment position
	in vec2 vertexTextureCoordinate;

	out vec4 fragmentColor; // For outgoing cube color to the GPU



	// Uniform / Global variables for object color, light color, light position, and camera/view position
	uniform vec4 objectColor;
	uniform vec3 ambientColor;
	uniform vec3 light1Color;
	uniform vec3 light1Position;
	uniform vec3 light2Color;
	uniform vec3 light2Position;
	uniform vec3 light3Color;
	uniform vec3 light3Position;
	uniform vec3 light4Color;
	uniform vec3 light4Position;
	uniform vec3 light5Color;
	uniform vec3 light5Position;
	uniform vec3 viewPosition;
	uniform sampler2D uTexture; // Useful when working with multiple textures
	uniform sampler2D uTextureBase;
	uniform sampler2D uTextureExtra;
	uniform bool multipleTextures;
	uniform vec2 uvScale;
	uniform bool ubHasTexture;
	uniform float ambientStrength = 0.1f; // Set ambient or global lighting strength
	uniform float specularIntensity1 = 0.1f;
	uniform float highlightSize1 = 16.0f;
	uniform float specularIntensity2 = 0.1f;
	uniform float highlightSize2 = 16.0f;
	uniform float specularIntensity3 = 0.1f;
	uniform float highlightSize3 = 16.0f;
	uniform float specularIntensity4 = 0.1f;
	uniform float highlightSize4 = 16.0f;
	uniform float specularIntensity5 = 0.1f;
	uniform float highlightSize5 = 16.0f;

	void main()
	{
		/*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

		//Calculate Ambient lighting
		vec3 ambient = ambientStrength * ambientColor; // Generate ambient light color

		//**Calculate Diffuse lighting**
		vec3 norm = normalize(vertexFragmentNormal); // Normalize vectors to 1 unit
		vec3 light1Direction = normalize(light1Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
		float impact1 = max(dot(norm, light1Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
		vec3 diffuse1 = impact1 * light1Color; // Generate diffuse light color
		vec3 light2Direction = normalize(light2Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
		float impact2 = max(dot(norm, light2Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
		vec3 diffuse2 = impact2 * light2Color; // Generate diffuse light color
		vec3 light3Direction = normalize(light3Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
		float impact3 = max(dot(norm, light3Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
		vec3 diffuse3 = impact3 * light3Color; // Generate diffuse light color

		vec3 light4Direction = normalize(light4Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
		float impact4 = max(dot(norm, light4Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
		vec3 diffuse4 = impact4 * light4Color; // Generate diffuse light color

		vec3 light5Direction = normalize(light5Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
		float impact5 = max(dot(norm, light5Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
		vec3 diffuse5 = impact5 * light5Color; // Generate diffuse light color

		//**Calculate Specular lighting**
		vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
		vec3 reflectDir1 = reflect(-light1Direction, norm);// Calculate reflection vector
		//Calculate specular component
		float specularComponent1 = pow(max(dot(viewDir, reflectDir1), 0.0), highlightSize1);
		vec3 specular1 = specularIntensity1 * specularComponent1 * light1Color;
		vec3 reflectDir2 = reflect(-light2Direction, norm);// Calculate reflection vector
		//Calculate specular component
		float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
		vec3 specular2 = specularIntensity2 * specularComponent2 * light2Color;

		// 3rd light
		vec3 reflectDir3 = reflect(-light3Direction, norm);// Calculate reflection vector
		float specularComponent3 = pow(max(dot(viewDir, reflectDir3), 0.0), highlightSize3);
		vec3 specular3 = specularIntensity3 * specularComponent3 * light3Color;

		// 4th light
		vec3 reflectDir4 = reflect(-light4Direction, norm);// Calculate reflection vector
		float specularComponent4 = pow(max(dot(viewDir, reflectDir4), 0.0), highlightSize4);
		vec3 specular4 = specularIntensity4 * specularComponent4 * light4Color;

		// 5th light
		vec3 reflectDir5 = reflect(-light5Direction, norm);// Calculate reflection vector
		float specularComponent5 = pow(max(dot(viewDir, reflectDir5), 0.0), highlightSize5);
		vec3 specular5 = specularIntensity5 * specularComponent3 * light5Color;

		//**Calculate phong result**
		//Texture holds the color to be used for all three components
		vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);
		vec3 phong1;
		vec3 phong2;
		vec3 phong3;
		vec3 phong4;
		vec3 phong5;


		if (ubHasTexture == true)
		{
			phong1 = (ambient + diffuse1 + specular1) * textureColor.xyz;
			phong2 = (ambient + diffuse2 + specular2) * textureColor.xyz;
			phong3 = (ambient + diffuse3 + specular3) * textureColor.xyz;
			phong4 = (ambient + diffuse4 + specular4) * textureColor.xyz;
			phong5 = (ambient + diffuse5 + specular5) * textureColor.xyz;
		}
		else
		{
			phong1 = (ambient + diffuse1 + specular1) * objectColor.xyz;
			phong2 = (ambient + diffuse2 + specular2) * objectColor.xyz;
			phong3 = (ambient + diffuse3 + specular3) * objectColor.xyz;
			phong4 = (ambient + diffuse4 + specular4) * objectColor.xyz;
			phong5 = (ambient + diffuse5 + specular5) * objectColor.xyz;
		}

		if (multipleTextures)
		{
			vec4 extraTexture = texture(uTextureExtra, vertexTextureCoordinate * uvScale);
			if (extraTexture.a != 0.0)
				fragmentColor = extraTexture;
			else
				fragmentColor = vec4(phong1 + phong2 + phong3 + phong4 + phong5, 1.0); // Send lighting results to GPU
		}
		else
		{
			fragmentColor = vec4(phong1 + phong2 + phong3 + phong4 + phong5, 1.0); // Send lighting results to GPU
		}
	}
);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Light Object Shader Source Code*/
const GLchar* lightVertexShaderSource = GLSL(330,
	layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Light Object Shader Source Code*/
const GLchar* lightFragmentShaderSource = GLSL(330,
	out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0); // set all 4 vector values to 1.0
}
);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);

// main function. Entry point to the OpenGL program
int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	// Create the mesh
	meshes.CreateMeshes();

	// Create the shader program
	if (!UCreateShaderProgram(surfaceVertexShaderSource, surfaceFragmentShaderSource, gSurfaceProgramId))
		return EXIT_FAILURE;

	if (!UCreateShaderProgram(lightVertexShaderSource, lightFragmentShaderSource, gLightProgramId))
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
	texFilename = "resources/textures/Terrazzo001_1K_Color.jpg";
	if (!UCreateTexture(texFilename, textureMugHandleId))
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
	glUseProgram(gSurfaceProgramId);
	// We set the texture as texture unit 0
	glUniform1i(glGetUniformLocation(gSurfaceProgramId, "uTexture"), 0);
	glUniform1i(glGetUniformLocation(gSurfaceProgramId, "uTextureExtra"), 1);
	glUniform2f(glGetUniformLocation(gSurfaceProgramId, "uvScale"), gUVScale.x, gUVScale.y);

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

		URender();

		glfwPollEvents();
	}

	// Release mesh data
	meshes.DestroyMeshes();

	UDestroyShaderProgram(gSurfaceProgramId);
	UDestroyShaderProgram(gLightProgramId);

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

	float velocity = gCamera.MovementSpeed * gDeltaTime;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.Position -= gCamera.Up * velocity;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.Position += gCamera.Up * velocity;

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

void URender()
{
	GLint modelLoc;
	GLint viewLoc;
	GLint projLoc;
	GLint viewPosLoc;
	GLint ambStrLoc;
	GLint ambColLoc;
	GLint light1ColLoc;
	GLint light1PosLoc;
	GLint light2ColLoc;
	GLint light2PosLoc;
	GLint light3ColLoc;
	GLint light3PosLoc;
	GLint light4ColLoc;
	GLint light4PosLoc;
	GLint light5ColLoc;
	GLint light5PosLoc;
	GLint objColLoc;
	GLint specInt1Loc;
	GLint highlghtSz1Loc;
	GLint specInt2Loc;
	GLint highlghtSz2Loc;
	GLint specInt3Loc;
	GLint highlghtSz3Loc;
	GLint specInt4Loc;
	GLint highlghtSz4Loc;
	GLint specInt5Loc;
	GLint highlghtSz5Loc;
	GLint uHasTextureLoc;
	bool ubHasTextureVal;
	glm::mat4 scale;
	glm::mat4 rotation;
	glm::mat4 translation;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	GLuint multipleTexturesLoc = glGetUniformLocation(gSurfaceProgramId, "multipleTextures");

	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view = gCamera.GetViewMatrix();

	// toggle between perspective and ortho
	if (isPerspective)
	{
		projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}
	else {
		projection = glm::ortho(-(800.0f / 100), 800.0f / 100, -(600.0f / 100), 600.0f / 100, 0.1f, 100.0f);
	}

	// Set the shader to be used
	glUseProgram(gSurfaceProgramId);

	// Retrieves and passes transform matrices to the Shader program
	modelLoc = glGetUniformLocation(gSurfaceProgramId, "model");
	viewLoc = glGetUniformLocation(gSurfaceProgramId, "view");
	projLoc = glGetUniformLocation(gSurfaceProgramId, "projection");
	viewPosLoc = glGetUniformLocation(gSurfaceProgramId, "viewPosition");
	ambStrLoc = glGetUniformLocation(gSurfaceProgramId, "ambientStrength");
	ambColLoc = glGetUniformLocation(gSurfaceProgramId, "ambientColor");
	light1ColLoc = glGetUniformLocation(gSurfaceProgramId, "light1Color");
	light1PosLoc = glGetUniformLocation(gSurfaceProgramId, "light1Position");
	light2ColLoc = glGetUniformLocation(gSurfaceProgramId, "light2Color");
	light2PosLoc = glGetUniformLocation(gSurfaceProgramId, "light2Position");
	light3ColLoc = glGetUniformLocation(gSurfaceProgramId, "light3Color");
	light3PosLoc = glGetUniformLocation(gSurfaceProgramId, "light3Position");
	light4ColLoc = glGetUniformLocation(gSurfaceProgramId, "light4Color");
	light4PosLoc = glGetUniformLocation(gSurfaceProgramId, "light4Position");
	light5ColLoc = glGetUniformLocation(gSurfaceProgramId, "light5Color");
	light5PosLoc = glGetUniformLocation(gSurfaceProgramId, "light5Position");
	objColLoc = glGetUniformLocation(gSurfaceProgramId, "objectColor");
	specInt1Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity1");
	highlghtSz1Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize1");
	specInt2Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity2");
	highlghtSz2Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize2");
	specInt2Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity2");
	highlghtSz2Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize2");
	specInt3Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity3");
	highlghtSz3Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize3");
	specInt4Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity4");
	highlghtSz4Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize4");
	specInt5Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity5");
	highlghtSz5Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize5");
	uHasTextureLoc = glGetUniformLocation(gSurfaceProgramId, "ubHasTexture");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the camera view location
	glUniform3f(viewPosLoc, gCamera.Position.x, gCamera.Position.y, gCamera.Position.z);

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

	glProgramUniform4f(gSurfaceProgramId, objColLoc, 1.0f, 0.961f, 0.933f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// turn off second texture
	// so it doesn't apply to remaining objects
	isTextureOn = false;
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

	glProgramUniform4f(gSurfaceProgramId, objColLoc, 0.545f, 0.271f, 0.075f, 1.0f);

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
	glBindTexture(GL_TEXTURE_2D, textureMugHandleId);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.5f, 0.90f, 2.0f));
	// 2. Rotate the object
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(5.0f, 1.0f, 4.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glProgramUniform4f(gSurfaceProgramId, objColLoc, 1.0f, 0.961f, 0.933f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, meshes.gTorusMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	/*
	--------------------------------------end coffee mug object----------------------------------------------------------------------------
	*/

	/*
	* ------------------------------------begin coffee pot object-----------------------------------------------------------------------------
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

	glProgramUniform4f(gSurfaceProgramId, objColLoc, 0.0f, 1.0f, 1.0f, 1.0f);

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

	glProgramUniform4f(gSurfaceProgramId, objColLoc, 1.0f, 1.0f, 0.0f, 1.0f);

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

	glProgramUniform4f(gSurfaceProgramId, objColLoc, 0.0f, 1.0f, 0.0f, 1.0f);

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

	glProgramUniform4f(gSurfaceProgramId, objColLoc, 0.0f, 0.0f, 1.0f, 1.0f);

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

	glProgramUniform4f(gSurfaceProgramId, objColLoc, 0.0f, 0.5f, 0.5f, 1.0f);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid3Mesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	/*
	* ------------------------------------end coffee pot object------------------------------------------------------
	*/

	glBindVertexArray(0);

	glBindVertexArray(0);
/*
* --------------------------------------lighting---------------------------------------------------------------------------
*/
	glUniform1f(ambStrLoc, 0.1f);				//set ambient lighting strength
	glUniform3f(ambColLoc, 0.1f, 0.1f, 0.1f);	//set ambient color

	//glUniform3f(light1ColLoc, 1.0f, 1.0f, 1.0f);
	//glUniform3f(light1PosLoc, 20.0f, 4.0f, -20.0f);
	glUniform3f(light2ColLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light2PosLoc, 20.0f, 10.0f, 0.0f);
	glUniform3f(light3ColLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light3PosLoc, 0.0f, 3.0f, 20.0f);
	glUniform3f(light4ColLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light4PosLoc, -6.0f, 20.0f, 0.0f);
	//glUniform3f(light5ColLoc, 1.0f, 1.0f, 1.0f);
	//glUniform3f(light5PosLoc, 6.0f, 20.0f, 0.0f);

	//set specular intensity
	glUniform1f(specInt1Loc, .01f);
	glUniform1f(specInt2Loc, .01f);
	glUniform1f(specInt3Loc, .01f);
	glUniform1f(specInt4Loc, .01f);
	glUniform1f(specInt5Loc, .01f);
	//set specular highlight size
	glUniform1f(highlghtSz1Loc, .01f);
	glUniform1f(highlghtSz2Loc, .01f);
	glUniform1f(highlghtSz3Loc, .01f);
	glUniform1f(highlghtSz4Loc, .01f);
	glUniform1f(highlghtSz5Loc, .01f);

	ubHasTextureVal = true;
	glUniform1i(uHasTextureLoc, ubHasTextureVal);

	// Set the shader to be used
	glUseProgram(gLightProgramId);

	// Retrieves and passes transform matrices to the Shader program
	modelLoc = glGetUniformLocation(gLightProgramId, "model");
	viewLoc = glGetUniformLocation(gLightProgramId, "view");
	projLoc = glGetUniformLocation(gLightProgramId, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

/*
* -----------------------------light 1-------------------------------------------------------------------
*/

	//// Activate the VBOs contained within the mesh's VAO
	//glBindVertexArray(meshes.gPyramid4Mesh.vao);

	//// 1. Scales the object by 2
	//scale = glm::scale(glm::vec3(0.4f, 0.4f, 0.4f));
	//// 2. Rotates shape by 15 degrees in the x axis
	//rotation = glm::rotate(-0.2f, glm::vec3(1.0, 0.0f, 0.0f));
	//// 3. Place object at the origin
	//translation = glm::translate(glm::vec3(20.0f, 4.0f, -20.0f));
	//// Model matrix: transformations are applied right-to-left order
	//model = translation * rotation * scale;

	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid4Mesh.nVertices);
/*
* -----------------------------light 2-------------------------------------------------------------------
*/
	// 1. Scales the object by 2
	scale = glm::scale(glm::vec3(0.4f, 0.4f, 0.4f));
	// 2. Rotates shape by 15 degrees in the x axis
	rotation = glm::rotate(-0.2f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Place object at the origin
	translation = glm::translate(glm::vec3(20.0f, 10.0f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid4Mesh.nVertices);

/*
* -----------------------------light 3-------------------------------------------------------------------
*/

// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPyramid4Mesh.vao);

	// 1. Scales the object by 2
	scale = glm::scale(glm::vec3(0.4f, 0.4f, 0.4f));
	// 2. Rotates shape by 15 degrees in the x axis
	rotation = glm::rotate(-0.2f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Place object at the origin
	translation = glm::translate(glm::vec3(0.0f, 3.0f, 20.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid4Mesh.nVertices);

	/*
	* -----------------------------light 4-------------------------------------------------------------------
	*/
	// 1. Scales the object by 2
	scale = glm::scale(glm::vec3(0.4f, 0.4f, 0.4f));
	// 2. Rotates shape by 15 degrees in the x axis
	rotation = glm::rotate(-0.2f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Place object at the origin
	translation = glm::translate(glm::vec3(-6.0f, 20.0f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid4Mesh.nVertices);

/*
* -----------------------------light 5-------------------------------------------------------------------
*/
	//	// 1. Scales the object by 2
	//scale = glm::scale(glm::vec3(0.4f, 0.4f, 0.4f));
	//// 2. Rotates shape by 15 degrees in the x axis
	//rotation = glm::rotate(-0.2f, glm::vec3(1.0, 0.0f, 0.0f));
	//// 3. Place object at the origin
	//translation = glm::translate(glm::vec3(6.0f, 20.0f, 0.0f));
	//// Model matrix: transformations are applied right-to-left order
	//model = translation * rotation * scale;

	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid4Mesh.nVertices);

	glBindVertexArray(0);

	glUseProgram(0);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

void UDestroyMesh(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
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