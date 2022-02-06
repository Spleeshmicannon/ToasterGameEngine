#include "renderer.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include <GL/GLU.h>
#include <gl/GL.h>

#include <windows.h>
#include <windowsx.h>

#include "../platform/platform.h"

#include "../glm/glm.hpp"
#include "../glm/ext.hpp"
#include "../glm/common.hpp"
#include "../glm/fwd.hpp"
#include "../glm/geometric.hpp"

namespace toast
{
#ifdef TWIN32
	struct renderContext
	{
		HDC device;
		HGLRC render;
		GLuint vbo, vao;
		GLuint shaders[2];
		GLuint program;
	};

	b8 compileShader(const str<char>& shader, renderContext* context, int index, GLenum type);

	b8 compileShader(const str<char> &shader, renderContext * context, int index, GLenum type)
	{	
		context->shaders[index] = glCreateShader(type);
		
		const char* shaders[1] = { shader.c_str() };
		const int size = shader.size();

		glShaderSource(context->shaders[index],1, shaders, &size);

		glCompileShader(context->shaders[index]);

		GLint success = 0;
		glGetShaderiv(context->shaders[index], GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint logSize = 0;
			glGetShaderiv(context->shaders[index], GL_INFO_LOG_LENGTH, &logSize);
			
			char * errorLog = allocate<char>(logSize);
			glGetShaderInfoLog(context->shaders[index], logSize, &logSize, errorLog);

			const str<char> erroInfo = errorLog;
			Logger::staticLog<logLevel::TERROR>("Failed to compile shader: " + erroInfo);

			glDeleteShader(context->shaders[index]);

			return false;
		}
		
		return true;
	}

	b8 Renderer::initialise(const str<char>& name, platformState* state, u16 width, u16 height)
	{
		// setting up context
		context = allocate<renderContext>();
		
		context->device = GetDC(state->hwnd);
		
		// setting pixel format
		PIXELFORMATDESCRIPTOR pfd;
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cAlphaBits = 0;
		pfd.cAccumBits = 0;
		pfd.cDepthBits = 0;
		pfd.cStencilBits = 0;
		pfd.cAuxBuffers = 0;
		pfd.iLayerType = PFD_MAIN_PLANE;

		SetPixelFormat(context->device, ChoosePixelFormat(context->device, &pfd), &pfd);

		// creating context
		context->render = wglCreateContext(context->device);
		
		// using the acquired context
		if (!wglMakeCurrent(context->device, context->render))
		{
			char buffer[32];
			_ultoa(GetLastError(), buffer, 10);
			const str<char> msg = buffer;

			Logger::staticLog<logLevel::TFATAL>("Failed to make gl context current: " + msg);
			return false;
		}

		// wrangling OpenGL libraries
		const GLint GlewInitResult = glewInit();

		if (GLEW_OK != GlewInitResult)
		{
			const str<char> msg = (char*)glewGetErrorString(GlewInitResult);
			Logger::staticLog<logLevel::TFATAL>("Glew error, " + msg);
			return false;
		}

		// shaders
		const str<char> vertex_shader = readFile("./shaders/shader.vert");
		const str<char> fragment_shader = readFile("./shaders/shader.frag");

		compileShader(vertex_shader, context, 0, GL_VERTEX_SHADER);
		compileShader(fragment_shader, context, 1, GL_FRAGMENT_SHADER);

		// linking the program (collection of shaders)
		context->program = glCreateProgram();
		
		glAttachShader(context->program, context->shaders[0]);
		glAttachShader(context->program, context->shaders[1]);
		
		glLinkProgram(context->program);

		GLint isLinked = 0;
		glGetProgramiv(context->program, GL_LINK_STATUS, (int*)&isLinked);

		// error checking
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(context->program, GL_INFO_LOG_LENGTH, &maxLength);

			char* errorLog = allocate<char>(maxLength);
			glGetProgramInfoLog(context->program, maxLength, &maxLength, errorLog);

			const str<char> erroInfo = errorLog;
			Logger::staticLog<logLevel::TFATAL>("Failed to link shaders: " + erroInfo);

			glDeleteProgram(context->program);
			return false;
		}

		// Orth2D management
		glm::mat<4, 4, f32> matrix = glm::ortho<f32>(0, width, 0, height);

		glUseProgram(context->program);
		glUniformMatrix4fv(glGetUniformLocation(context->program, "model"), 1, 
			GL_FALSE, glm::value_ptr(matrix));

		// creating a general usage vbo
		glGenBuffers(1, &context->vbo);
		glGenVertexArrays(1, &context->vao);

		return true;
	}

	void Renderer::drawFrame(renderPacket* packet)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		if (packet->vertexCount > 0)
		{
			// bind VAO
			glBindVertexArray(context->vao);

			// binding VBO
			glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
			glBufferData(GL_ARRAY_BUFFER, packet->vertexCount * sizeof(f32) * 3,
				packet->vertices, GL_STATIC_DRAW);

			// vertex shader interpret info
			glEnableVertexAttribArray(0);
			
			// ortho2D
			glm::mat<4, 4, f32> matrix = glm::ortho<f32>(
				0, packet->winWidth, 
				0, packet->winHeight
			);

			// init shader program
			glUseProgram(context->program);

			// passing window size to the vertex shader
			glUniformMatrix4fv(glGetUniformLocation(context->program, "model"), 1,
				GL_FALSE, glm::value_ptr(matrix));

			// drawing 
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glDrawArrays(GL_POINTS, 0, packet->vertexCount * sizeof(f32) * 3);
			
			// ender shader usage
			glDisableVertexAttribArray(0);
		}

		// swapping front and back buffers
		SwapBuffers(context->device);
		return;
	}

	void Renderer::shutdown(platformState* state)
	{
		// deleting the current context
		wglMakeCurrent(context->device, NULL);
		ReleaseDC(state->hwnd, context->device);
		wglDeleteContext(context->render);

		// freeing up memory
		deallocate<renderContext>(context);
	}
#elif defined(TLINUX)

#endif
}