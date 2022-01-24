#include "renderer.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include <GL/GLU.h>
#include <gl/GL.h>

#include <windows.h>
#include <windowsx.h>

namespace toast
{
#ifdef TWIN32
	struct renderContext
	{
		HDC device;
		HGLRC render;
		GLuint vbo;
		u16 width, height;
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
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR), // size
			1,							// version number
			PFD_DRAW_TO_WINDOW |		// support window
			PFD_SUPPORT_OPENGL |		// support OpenGL
			PFD_DOUBLEBUFFER,			// double buffered
			PFD_TYPE_RGBA,				// RGBA type
			24,							// 24-bit color depth
			0, 0, 0, 0, 0, 0,			// color bits ignored
			0,							// no alpha buffer
			0,							// shift bit ignored
			0,							// no accumulation buffer
			0, 0, 0, 0,					// accum bits ignored
			32,							// 32-bit z-buffer
			0,							// no stencil buffer
			0,							// no auxiliary
			PFD_MAIN_PLANE,				// main layer
			0,							// reserved
			0, 0, 0						// layer masks ignored
		};

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
		//compileShader(fragment_shader, context, 1, GL_FRAGMENT_SHADER);

		context->program = glCreateProgram();
		glAttachShader(context->program, context->shaders[0]);


		// instead of max width being 0 -> 1 
		// its 0 -> whatever width is set to be
		glMatrixMode(GL_PROJECTION_MATRIX);
		glLoadIdentity();

		gluOrtho2D(0, width, 0, height);

		// setting width and height
		context->width = width;
		context->height = height;

		// creating a general usage vbo
		glGenBuffers(1, &context->vbo);

		return true;
	}

	void Renderer::drawFrame(renderPacket* packet)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		if (packet->vertexCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
			glBufferData(GL_ARRAY_BUFFER, packet->vertexCount * sizeof(f32) * 3,
				packet->vertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glDrawArrays(GL_POINTS, 0, packet->vertexCount * sizeof(f32) * 3);
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