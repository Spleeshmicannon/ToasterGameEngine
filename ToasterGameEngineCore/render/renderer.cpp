#include "renderer.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include <gl/GLU.h>

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
	};

	b8 Renderer::initialise(str<char> name, platformState* state)
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
			ultoa(GetLastError(), buffer, 10);
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
		
		// creating a general usage vbo
		glGenBuffers(1, &context->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, context->vbo);

		return true;
	}

	void Renderer::drawFrame(renderPacket* packet)
	{
		// clear screen
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();

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