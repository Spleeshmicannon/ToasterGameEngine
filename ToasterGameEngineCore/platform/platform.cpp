#include "platform.h"
#include "platformState.h"

#include "../input.h"

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
	f64 Platform::clockFreq;
	LARGE_INTEGER Platform::startTime;

	LRESULT CALLBACK WIN32_processMessage(HWND hwnd, u32 msg,
		WPARAM wparam, LPARAM lparam);

	Platform::Platform() : state(new platformState)
	{}

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

		// clock setup
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		clockFreq = 1.0 / (f64)freq.QuadPart;
		QueryPerformanceCounter(&startTime);

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

	void* Platform::copyMem(void* dest, void* source, u64 size)
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
		static const u8 levels[6] = { 192, 12, 14, 9, 10, 7 };
		SetConsoleTextAttribute(consoleHandle, levels[static_cast<u8>(color)]);

		// outputing message
		OutputDebugStringA(message.c_str());
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message.c_str(), message.length(), 0, 0);
	}

	void Platform::consoleWriteError(const str<cv> &message, const color color)
	{
		// setting colour
		HANDLE consoleHandle = GetStdHandle(STD_ERROR_HANDLE);
		static const u8 levels[6] = { 192, 12, 14, 9, 2, 7 };
		SetConsoleTextAttribute(consoleHandle, levels[static_cast<u8>(color)]);

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
			case WM_KEYDOWN:
				Input::processKey(static_cast<keys>(static_cast<u16>(wparam)),
					true);
				break;
			case WM_SYSKEYDOWN:
				Input::processKey(static_cast<keys>(static_cast<u16>(wparam)),
					true);
				break;
			case WM_KEYUP:
				Input::processKey(static_cast<keys>(static_cast<u16>(wparam)),
					false);
				break;
			case WM_SYSKEYUP:
				Input::processKey(static_cast<keys>(static_cast<u16>(wparam)),
					false);
				break;
			case WM_MOUSEMOVE:
			{
				Input::processMseMove(GET_X_LPARAM(lparam),
					GET_Y_LPARAM(lparam));

			} break;
			case WM_MOUSEWHEEL:
			{
				i32 zDelta = GET_WHEEL_DELTA_WPARAM(wparam);
				if (zDelta != 0)
				{
					// flatten to be OS independent
					zDelta = (zDelta < 0) ? -1 : 1;
					
					// send data away for processing
					Input::processMseWheel(zDelta);
				}
			} break;
			case WM_LBUTTONDOWN: 
				Input::processBtn(mseBtns::LEFT, true);
				break;
			case WM_MBUTTONDOWN:
				Input::processBtn(mseBtns::MIDDLE, true);
				break;
			case WM_RBUTTONDOWN:
				Input::processBtn(mseBtns::RIGHT, true);
				break;
			case WM_LBUTTONUP:
				Input::processBtn(mseBtns::LEFT, false);
				break;
			case WM_MBUTTONUP:
				Input::processBtn(mseBtns::MIDDLE, false);
				break;
			case WM_RBUTTONUP:
				Input::processBtn(mseBtns::RIGHT, false);
				break;
		}

		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}
#elif defined(TLINUX)
	keys keysymToKey(KeySym ksym);

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
				{
					// cleaner than a cast but eh
					const xcb_key_press_event_t* kbEvent = (xcb_key_press_event_t*)event;
					Input::processKey(
						keysymToKey( // just some conversions to the keys enum
							XKeycodeToKeysym(
								state->display,
								(KeyCode)kbEvent->detail,
								((xcb_keycode_t)kbEvent->detail) & ShiftMask ? 1 : 0
							)
						),
						true // key is being pressed
					);
				} break;
				case XCB_KEY_RELEASE: 
				{
					// cleaner than a cast but eh
					const xcb_key_press_event_t* kbEvent = (xcb_key_press_event_t*)event;
					Input::processKey(
						keysymToKey( // just some conversions to the keys enum
							XKeycodeToKeysym(
								state->display,
								(KeyCode)kbEvent->detail,
								((xcb_keycode_t)kbEvent->detail) & ShiftMask ? 1 : 0
							)
						),
						true // key is being pressed
					);
				} break;
				case XCB_BUTTON_PRESS: 
				{
					switch (((xcb_button_press_event_t*)event)->detail)
					{
						case XCB_BUTTON_INDEX_1: 
							Input::processBtn(mseBtns::LEFT, true);
							break;
						case XCB_BUTTON_INDEX_2:
							Input::processBtn(mseBtns::MIDDLE, true);
							break;
						case XCB_BUTTON_INDEX_3:
							Input::processBtn(mseBtns::RIGHT, true);
							break;
						default: break;
					}
					
				}break;
				case XCB_BUTTON_RELEASE: 
				{
					switch (((xcb_button_press_event_t*)event)->detail)
					{
						case XCB_BUTTON_INDEX_1:
							Input::processBtn(mseBtns::LEFT, false);
							break;
						case XCB_BUTTON_INDEX_2:
							Input::processBtn(mseBtns::MIDDLE, false);
							break;
						case XCB_BUTTON_INDEX_3:
							Input::processBtn(mseBtns::RIGHT, false);
							break;
						default: break;
					}
				} break;
				case XCB_MOTION_NOTIFY: 
					Input::processMseMove(((xcb_motion_notify_event_t*)event)->event_x, 
						((xcb_motion_notify_event_t*)event)->event_y);
					break;
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

	void* Platform::copyMem(void* dest, void* source, u64 size)
	{
		return memcpy(dest, source, size);
	}

	void* Platform::setMem(void* dest, i32 value, u64 size)
	{
		return memset(dest, value, size);
	}

	void Platform::consoleWrite(const str<cv> &message, const color color)
	{
		const str<cv> colour_strings[] = {"30;101", "1;91", "1;93", "1;94", "1;92", "1;37"};
		std::cout << "\033[" + colour_strings[static_cast<u8>(color)] + "m" + message + "\033[0m";
	}

	void Platform::consoleWriteError(const str<cv> &message, const color color)
	{
		const str<cv> colour_strings[] = { "30;101", "1;91", "1;93", "1;94", "1;92", "1;37" };
		std::cerr << "\033[" + colour_strings[static_cast<u8>(color)] + "m" + message + "\033[0m";
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

	keys keysymToKey(KeySym ksym)
	{
		switch (ksym) {
			case XK_BackSpace: return keys::BACKSPACE;
			case XK_Return: return keys::ENTER;
			case XK_Tab:
				return keys::TAB;
				//case XK_Shift: return keys::SHIFT;
				//case XK_Control: return keys::CONTROL;

			case XK_Pause: return keys::PAUSE;
			case XK_Caps_Lock: return keys::CAPITAL;

			case XK_Escape: return keys::ESCAPE;

				// Not supported
				// case : return keys::CONVERT;
				// case : return keys::NONCONVERT;
				// case : return keys::ACCEPT;

			case XK_Mode_switch: return keys::MODECHANGE;

			case XK_space: return keys::SPACE;
			case XK_Prior: return keys::PRIOR;
			case XK_Next: return keys::NEXT;
			case XK_End: return keys::END;
			case XK_Home: return keys::HOME;
			case XK_Left: return keys::LEFT;
			case XK_Up: return keys::UP;
			case XK_Right: return keys::RIGHT;
			case XK_Down: return keys::DOWN;
			case XK_Select: return keys::SELECT;
			case XK_Print: return keys::PRINT;
			case XK_Execute:
				return keys::EXECUTE;
				// case XK_snapshot: return keys::SNAPSHOT; // not supported
			case XK_Insert: return keys::INSERT;
			case XK_Delete: return keys::TDELETE;
			case XK_Help:
				return keys::HELP;

			case XK_Meta_L: return keys::LWIN; // not sure this is right
			case XK_Meta_R:
				return keys::RWIN;
				// case XK_apps: return keys::APPS; // not supported

				// case XK_sleep: return keys::SLEEP; //not supported

			case XK_KP_0: return keys::NUMPAD0;
			case XK_KP_1: return keys::NUMPAD1;
			case XK_KP_2: return keys::NUMPAD2;
			case XK_KP_3: return keys::NUMPAD3;
			case XK_KP_4: return keys::NUMPAD4;
			case XK_KP_5: return keys::NUMPAD5;
			case XK_KP_6: return keys::NUMPAD6;
			case XK_KP_7: return keys::NUMPAD7;
			case XK_KP_8: return keys::NUMPAD8;
			case XK_KP_9: return keys::NUMPAD9;
			case XK_multiply: return keys::MULTIPLY;
			case XK_KP_Add: return keys::ADD;
			case XK_KP_Separator: return keys::SEPARATOR;
			case XK_KP_Subtract: return keys::SUBTRACT;
			case XK_KP_Decimal: return keys::DECIMAL;
			case XK_KP_Divide: return keys::DIVIDE;
			case XK_F1: return keys::F1;
			case XK_F2: return keys::F2;
			case XK_F3: return keys::F3;
			case XK_F4: return keys::F4;
			case XK_F5: return keys::F5;
			case XK_F6: return keys::F6;
			case XK_F7: return keys::F7;
			case XK_F8: return keys::F8;
			case XK_F9: return keys::F9;
			case XK_F10: return keys::F10;
			case XK_F11: return keys::F11;
			case XK_F12: return keys::F12;
			case XK_F13: return keys::F13;
			case XK_F14: return keys::F14;
			case XK_F15: return keys::F15;
			case XK_F16: return keys::F16;
			case XK_F17: return keys::F17;
			case XK_F18: return keys::F18;
			case XK_F19: return keys::F19;
			case XK_F20: return keys::F20;
			case XK_F21: return keys::F21;
			case XK_F22: return keys::F22;
			case XK_F23: return keys::F23;
			case XK_F24: return keys::F24;

			case XK_Num_Lock: return keys::NUMLOCK;
			case XK_Scroll_Lock: return keys::SCROLL;

			case XK_KP_Equal: return keys::NUMPAD_EQUAL;

			case XK_Shift_L: return keys::LSHIFT;
			case XK_Shift_R: return keys::RSHIFT;
			case XK_Control_L: return keys::LCONTROL;
			case XK_Control_R: return keys::RCONTROL;
			case XK_Alt_L: return keys::LALT;
			case XK_Alt_R: return keys::RALT;

			case XK_semicolon: return keys::SEMICOLON;
			case XK_plus: return keys::PLUS;
			case XK_comma: return keys::COMMA;
			case XK_minus: return keys::MINUS;
			case XK_period: return keys::PERIOD;
			case XK_slash: return keys::SLASH;
			case XK_grave: return keys::GRAVE;

			case XK_a:
			case XK_A: return keys::A;
			case XK_b:
			case XK_B: return keys::B;
			case XK_c:
			case XK_C: return keys::C;
			case XK_d:
			case XK_D: return keys::D;
			case XK_e:
			case XK_E: return keys::E;
			case XK_f:
			case XK_F: return keys::F;
			case XK_g:
			case XK_G: return keys::G;
			case XK_h:
			case XK_H: return keys::H;
			case XK_i:
			case XK_I: return keys::I;
			case XK_j:
			case XK_J: return keys::J;
			case XK_k:
			case XK_K: return keys::K;
			case XK_l:
			case XK_L: return keys::L;
			case XK_m:
			case XK_M: return keys::M;
			case XK_n:
			case XK_N: return keys::N;
			case XK_o:
			case XK_O: return keys::O;
			case XK_p:
			case XK_P: return keys::P;
			case XK_q:
			case XK_Q: return keys::Q;
			case XK_r:
			case XK_R: return keys::R;
			case XK_s:
			case XK_S: return keys::S;
			case XK_t:
			case XK_T: return keys::T;
			case XK_u:
			case XK_U: return keys::U;
			case XK_v:
			case XK_V: return keys::V;
			case XK_w:
			case XK_W: return keys::W;
			case XK_x:
			case XK_X: return keys::X;
			case XK_y:
			case XK_Y: return keys::Y;
			case XK_z:
			case XK_Z: return keys::Z;
			default: return keys::MAX_KEYS;
		}
	}

#elif defined(TAPPLE)
#error "no apple support, sorry :("
#else
#error "unkown/unsupported platform"
#endif
}