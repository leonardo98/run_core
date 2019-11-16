#include "engine.h"

#include "Render.h"
#include "InputSystem.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/controls.hpp>

Engine *Engine::_instance = nullptr;

Engine::~Engine()
{
    // Cleanup VBO and shader
    //glDeleteBuffers(1, &vertexbuffer);
    //glDeleteBuffers(1, &uvbuffer);
    glDeleteProgram(_texturedProgramID);
    glDeleteProgram(_flatProgramID);
    glDeleteTextures(1, &_textureID);
    //glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}

Engine::Engine()
{
    assert(_instance == nullptr);
    _instance = this;
}

Engine *Engine::Instance()
{
    return _instance;
}

void resizeWindowCallback(GLFWwindow *, int width, int height)
{
    SetScreenSize(width, height);
    glViewport(0, 0, width, height);
}

int cursorPosX = 0, cursorPosY = 0;

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    cursorPosX = xpos;
    cursorPosY = ypos;
    InputSystem::MouseMove(FPoint2D(cursorPosX, cursorPosY));
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        InputSystem::MouseDown(FPoint2D(cursorPosX, cursorPosY));
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        InputSystem::MouseUp();
    }
}

bool Engine::CreateWindow(const char *name)
{
    SetScreenSize(960, 640);
    Render::SetScreenSize(getScreenWidth(), getScreenHeight());

    // Initialise GLFW
    if( !glfwInit() )
    {
            fprintf( stderr, "Failed to initialize GLFW\n" );
            getchar();
            return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    _window = glfwCreateWindow( getScreenWidth(), getScreenHeight(), name, NULL, NULL);
    if( _window == NULL ){
            fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
            getchar();
            glfwTerminate();
            return -1;
    }
    glfwMakeContextCurrent(_window);
    glfwSetWindowSizeCallback(_window, resizeWindowCallback);
    glfwSetMouseButtonCallback(_window, mouseButtonCallback);
    glfwSetCursorPosCallback(_window, cursorPositionCallback);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to initialize GLEW\n");
            getchar();
            glfwTerminate();
            return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(_window, GLFW_CURSOR, 0);//GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(_window, getScreenWidth() / 2, getScreenHeight() / 2);

    // Dark blue background
    glClearColor(0.4f, 0.4f, 0.4f, 0.f);

    // Disable depth test
    glDisable(GL_DEPTH_TEST);

    // Disable Cull triangles which normal is not towards the camera
    glDisable(GL_CULL_FACE);

    // Turn blending on
    glEnable(GL_BLEND);

    // Set blending func
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create and compile our GLSL program from the shaders
    _texturedProgramID = LoadShaders( "game_data/TransformVertexShader.vertexshader", "game_data/TextureFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    _texturedMatrixID = glGetUniformLocation(_texturedProgramID, "MVP");

    // Get a handle for our "myTextureSampler" uniform
    _textureID  = glGetUniformLocation(_texturedProgramID, "myTextureSampler");

    // Create and compile our GLSL program from the shaders
    _flatProgramID = LoadShaders( "game_data/TransformVertexShader.vertexshader", "game_data/FlatFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    _flatMatrixID = glGetUniformLocation(_flatProgramID, "MVP");

    computeMatricesFromInputs();
}

void Engine::SetShader(ShaderType shaderType)
{
    if (shaderType == ShaderType::Textured)
    {
        // Use our shader
        glUseProgram(_texturedProgramID);

        glm::mat4 MVP = getProjectionMatrix();

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(_texturedMatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Bind our texture in GLTexture Unit 0
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, GLTexture);
        // Set our "myTextureSampler" sampler to use GLTexture Unit 0
        glUniform1i(_textureID, 0);
    }
    else if (shaderType == ShaderType::Flat)
    {
        // Use our shader
        glUseProgram(_flatProgramID);

        glm::mat4 MVP = getProjectionMatrix();

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(_flatMatrixID, 1, GL_FALSE, &MVP[0][0]);
    }
    else
    {
        assert(false);
    }
}

int Engine::Run(const std::function<int()> &frameFunc)
{
    int result = 0;

    do{
        SetShader(ShaderType::Textured);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        result = frameFunc();

        // Swap buffers
        glfwSwapBuffers(_window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( result != 0 && glfwGetKey(_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(_window) == 0 );

    return 0;
}

/*
int main( void )
{
	// Load the texture
	GLuint GLTexture = loadDDS("uvtemplate.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	// Two UV coordinatesfor each vertex. They were created with Blender.
	static const GLfloat g_uv_buffer_data[] = { 
		0.000059f, 0.000004f, 
		0.000103f, 0.336048f, 
		0.335973f, 0.335903f, 
		1.000023f, 0.000013f, 
		0.667979f, 0.335851f, 
		0.999958f, 0.336064f, 
		0.667979f, 0.335851f, 
		0.336024f, 0.671877f, 
		0.667969f, 0.671889f, 
		1.000023f, 0.000013f, 
		0.668104f, 0.000013f, 
		0.667979f, 0.335851f, 
		0.000059f, 0.000004f, 
		0.335973f, 0.335903f, 
		0.336098f, 0.000071f, 
		0.667979f, 0.335851f, 
		0.335973f, 0.335903f, 
		0.336024f, 0.671877f, 
		1.000004f, 0.671847f, 
		0.999958f, 0.336064f, 
		0.667979f, 0.335851f, 
		0.668104f, 0.000013f, 
		0.335973f, 0.335903f, 
		0.667979f, 0.335851f, 
		0.335973f, 0.335903f, 
		0.668104f, 0.000013f, 
		0.336098f, 0.000071f, 
		0.000103f, 0.336048f, 
		0.000004f, 0.671870f, 
		0.336024f, 0.671877f, 
		0.000103f, 0.336048f, 
		0.336024f, 0.671877f, 
		0.335973f, 0.335903f, 
		0.667969f, 0.671889f, 
		1.000004f, 0.671847f, 
		0.667979f, 0.335851f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in GLTexture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GLTexture);
		// Set our "myTextureSampler" sampler to use GLTexture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	return 0;
}
*/
