#include "platform.h"

#include <cstdlib>

#ifdef TLINUX
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>
#include <iostream>
#include <cstring>
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else
#include <uinstd.h>
#endif
#endif

namespace toast
{
#ifdef TWIN32

	// time based variables
	f64 clockFreq;
	LARGE_INTEGER startTime;

	LRESULT CALLBACK WIN32_processMessage(HWND hwnd, u32 msg,
		WPARAM wparam, LPARAM lparam);

	Platform::Platform() : state(new platformState)
	{
		// clock setup
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		clockFreq = 1.0 / (f64)freq.QuadPart;
		QueryPerformanceCounter(&startTime);
	}

	err Platform::start(const str<cv>& name,
		i32 x, i32 y, i32 width, i32 height)
	{

		state->hinst = GetModuleHandleA(0);

		HICON icon = LoadIcon(state->hinst, IDI_APPLICATION);
		WNDCLASSA wc;
		memset(&wc, 0, sizeof(wc));

		wc.style = CS_DBLCLKS;
		wc.lpfnWndProc = WIN32_processMessage;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = state->hinst;
		wc.hIcon = icon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszClassName = "toast_window_class";

		if (!RegisterClassA(&wc))
		{
			MessageBoxA(0, "Window registration failed", "Error",
				MB_ICONEXCLAMATION | MB_OK);
			return 1;
		}

		u32 clientX = x;
		u32 clientY = y;
		u32 clientWidth = width;
		u32 clientHeight = height;

		u32 windowX = clientX;
		u32 windowY = clientY;
		u32 windowWidth = clientWidth;
		u32 windowHeight = clientHeight;

		u32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
		u32 windowExStyle = WS_EX_APPWINDOW;

		windowStyle |= WS_MAXIMIZEBOX;
		windowStyle |= WS_MINIMIZEBOX;
		windowStyle |= WS_THICKFRAME;

		RECT borderRect = { 0,0,0,0 };
		AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);

		windowX += borderRect.left;
		windowY += borderRect.top;

		windowWidth += borderRect.right - borderRect.left;
		windowWidth += borderRect.bottom - borderRect.top;

		HWND handle = CreateWindowExA(windowExStyle, "toast_window_class", (LPCSTR)name.c_str(),
			windowStyle, windowX, windowY, windowWidth, windowHeight, 0, 0, state->hinst, 0);

		if (handle == 0)
		{
			MessageBoxA(0, "Window creation failed", "Error: ",
				MB_ICONEXCLAMATION | MB_OK);
			return 2;
		}
		else
		{
			state->hwnd = handle;
		}

		b8 shouldActivate = true;
		const i32 showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;

		ShowWindow(state->hwnd, showWindowCommandFlags);
		return 0;
	}

	void Platform::shutdown()
	{
		if (state->hwnd)
		{
			DestroyWindow(state->hwnd);
			state->hwnd = 0;
		}
	}

	Platform::~Platform()
	{
		delete state;
	}

	b8 Platform::pumpMessages()
	{
		MSG message;
		while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}

		return true;
	}

	void* Platform::allocate(u64 size, b8 aligned)
	{
		return HeapAlloc(GetProcessHeap(), 
			HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, size);
	}

	void Platform::deallocate(void* block, b8 aligned)
	{
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, block);
	}

	void* Platform::zeroMem(void* block, u64 size)
	{
		return memset(block, 0, size);
	}

	void* Platform::copyMem(void* dest, const void* source, u64 size)
	{
		return memcpy(dest, source, size);
	}

	void* Platform::setMem(void* dest, i32 value, u64 size)
	{
		return memset(dest, value, size);
	}

	void Platform::consoleWrite(const str<cv> &message, const color color)
	{
		// setting colour
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		static const u8 levels[6] = { 192, 12, 6, 9, 2, 7 };
		SetConsoleTextAttribute(consoleHandle, levels[color]);

		// outputing message
		OutputDebugStringA(message.c_str());
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message.c_str(), message.length(), 0, 0);
	}

	void Platform::consoleWriteError(const str<cv> &message, const color color)
	{
		// setting colour
		HANDLE consoleHandle = GetStdHandle(STD_ERROR_HANDLE);
		static const u8 levels[6] = { 192, 12, 6, 9, 2, 7 };
		SetConsoleTextAttribute(consoleHandle, levels[color]);

		// outputing message
		OutputDebugStringA(message.c_str());
		WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message.c_str(), message.length(), 0, 0);
	}

	f64 Platform::getAbsTime()
	{
		LARGE_INTEGER now_time;
		QueryPerformanceCounter(&now_time);
		return (f64)now_time.QuadPart * clockFreq;
	}

	void Platform::sleep(u64 ms)
	{
		Sleep(ms);
	}

	LRESULT CALLBACK WIN32_processMessage(HWND hwnd, u32 msg,
		WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
			case WM_ERASEBKGND:
				return 1;
			case WM_CLOSE:
				// TODO: Fire quit event
				return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_SIZE:
			{
				/*RECT r;
				GetClientRect(hwnd, &r);
				u32 width = r.right - r.left;
				u32 height = r.bottom - r.top;*/

				// TODO: Fire a window resize event
			} break;
			case WM_KEYDOWN: break;
			case WM_KEYUP: break;
			case WM_SYSKEYDOWN: break;
			case WM_SYSKEYUP: break;
			case WM_MOUSEMOVE:
			{
				/*const i32 x = GET_X_LPARAM(lparam);
				const i32 y = GET_Y_LPARAM(lparam);*/
			} break;
			case WM_MOUSEWHEEL:
			{
				//i32 zDelta = GET_WHEEL_DELTA_WPARAM(wparam);
				//if (zDelta != 0)
				//{
				//	zDelta = (zDelta < 0) ? -1 : 1;
				//	// TODO: Input processing
				//}
			} break;
			case WM_LBUTTONDOWN: break;
			case WM_MBUTTONDOWN: break;
			case WM_RBUTTONDOWN: break;
			case WM_LBUTTONUP: break;
			case WM_MBUTTONUP: break;
			case WM_RBUTTONUP: break;
		}

		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}
#elif defined(TLINUX)
	struct platformState
	{
		Display* display;
		xcb_connection_t* connection;
		xcb_window_t window;
		xcb_screen_t* screen;
		xcb_atom_t wm_protocols;
		xcb_atom_t wm_delete_win;
	};

	Platform::Platform() : state(new platformState)
	{}

	err Platform::start(const str<cv>& name,
		i32 x, i32 y, i32 width, i32 height)
	{
		// connect to X server
		state->display = XOpenDisplay(NULL);

		if (state->display == NULL)
		{
			return 3;
		}

		// Turn off key repeats globally... its the only way
		XAutoRepeatOff(state->display);

		state->connection = XGetXCBConnection(state->display);

		if (xcb_connection_has_error(state->connection)) {
			return 2;
		}

		// get data from the X server
		const xcb_setup_t* setup = xcb_get_setup(state->connection);

		// loop through screens using iterator
		xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
		int screen_p = 0;
		for (i32 s = screen_p; s > 0; --s) {
			xcb_screen_next(&it);
		}

		// After screens have been looped through, assign it
		state->screen = it.data;

		// Allocate XID for the window to be created
		state->window = xcb_generate_id(state->connection);

		// Register event types
		u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

		// listen for keyboard and mouse buttons, and exposure for window resizing I think
		u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
			XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
			XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
			XCB_EVENT_MASK_STRUCTURE_NOTIFY;

		// Values to be sent over XCB (bg colour, events)
		u32 value_list[] = { state->screen->black_pixel, event_values };

		// create the window
		xcb_void_cookie_t cookie = xcb_create_window(
			state->connection,
			XCB_COPY_FROM_PARENT,			// depth
			state->window,
			state->screen->root,			// parent
			x, y, width, height,
			0,								// no border
			XCB_WINDOW_CLASS_INPUT_OUTPUT,	// class
			state->screen->root_visual,
			event_mask,
			value_list
		);

		// change title
		xcb_change_property(
			state->connection,
			XCB_PROP_MODE_REPLACE,
			state->window,
			XCB_ATOM_WM_NAME,
			XCB_ATOM_STRING,
			8,
			name.length(),
			name.c_str()
		);
 
		// Tell the server to notify when the window manager
		// attmempts to destroy the window
		xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(
			state->connection,
			0,
			strlen("WM_DELETE_WINDOW"),
			"WM_DELETE_WINDOW"
		);

		xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
			state->connection,
			0,
			strlen("WM_PROTOCOLS"),
			"WM_PROTOCOLS"
		);

		xcb_intern_atom_reply_t *wm_delete_reply = xcb_intern_atom_reply(
			state->connection,
			wm_delete_cookie,
			NULL
		);

		xcb_intern_atom_reply_t *wm_protocols_reply = xcb_intern_atom_reply(
			state->connection,
			wm_protocols_cookie,
			NULL
		);

		state->wm_delete_win = wm_delete_reply->atom;
		state->wm_protocols = wm_protocols_reply->atom;

		// Map the window to the screen
		xcb_map_window(state->connection, state->window);

		// flush the stream
		i32 stream_result = xcb_flush(state->connection);
		if (stream_result <= 0) 
		{
			return 1;
		}

		return 0;
	}

	void Platform::shutdown()
	{
		// Turn key repeats back on since this is global for the os
		XAutoRepeatOn(state->display);

		xcb_destroy_window(state->connection, state->window);
	}

	b8 Platform::pumpMessages()
	{
		xcb_generic_event_t* event;
		xcb_client_message_event_t* cm;

		b8 quit_flagged = false;

		while (event != 0) 
		{
			event = xcb_poll_for_event(state->connection);
			if (event == 0)
			{
				break;
			}

			// Input events
			switch (event->response_type & ~0x80)
			{
				case XCB_KEY_PRESS:
				case XCB_KEY_RELEASE: break;
				case XCB_BUTTON_PRESS:
				case XCB_BUTTON_RELEASE: break;
				case XCB_MOTION_NOTIFY: break;
				case XCB_CONFIGURE_NOTIFY: break;
				case XCB_CLIENT_MESSAGE: {
					cm = (xcb_client_message_event_t*)event;
					
					// Window close
					if (cm->data.data32[0] == state->wm_delete_win) {
						quit_flagged = true;
					}
				} break;
				default: break;
			}

			free(event); // event is allocated with malloc in API
		}

		return !quit_flagged;
	}


	void* Platform::allocate(u64 size, b8 aligned)
	{
		return malloc(size);
	}

	void Platform::deallocate(void* block, b8 aligned)
	{
		free(block);
	}

	void* Platform::zeroMem(void* block, u64 size)
	{
		return memset(block, 0, size);
	}

	void* Platform::copyMem(void* dest, const void* source, u64 size)
	{
		return memcpy(dest, source, size);
	}

	void* Platform::setMem(void* dest, i32 value, u64 size)
	{
		return memset(dest, value, size);
	}

	void Platform::consoleWrite(const str<cv> &message, const color color)
	{
		const str<cv> colour_strings[] = {"0;41", "1;31", "1;33", "1;34", "1;32", "1;30"};
		std::cout << "\033[" + colour_strings[color] + "m" + message + "\033[0m";
	}

	void Platform::consoleWriteError(const str<cv> &message, const color color)
	{
		const str<cv> colour_strings[] = { "0;41", "1;31", "1;33", "1;34", "1;32", "1;30" };
		std::cerr << "\033[" + colour_strings[color] + "m" + message + "\033[0m";
	}

	f64 Platform::getAbsTime()
	{
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		return now.tv_sec + now.tv_nsec * 0.000000001;
	}

	void Platform::sleep(u64 ms)
	{
#if _POSIX_C_SOURCE >= 199309L
		timespec ts;
		ts.tv_sec = ms / 1000;
		ts.tv_nsec = (ms % 1000) * 1000 * 1000;
		nanosleep(&ts, 0);
#else
		if (ms >= 1000)
		{
			sleep(ms / 1000);
		}
		usleep((ms % 1000) * 1000);
#endif
	}

	Platform::~Platform()
	{
		delete state;
	}

#elif defined(TAPPLE)
#error "no apple support, sorry :("
#else
#error "unkown/unsupported platform"
#endif
}