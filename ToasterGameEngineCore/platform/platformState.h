namespace toast
{
#ifdef TWIN32
#include <windows.h>
#include <windowsx.h>
	struct platformState
	{
		HINSTANCE hinst;
		HWND hwnd;
	};
#elif defined(TLINUX)
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/Xlib-xcb.h>
	struct platformState
	{
		Display* display;
		xcb_connection_t* connection;
		xcb_window_t window;
		xcb_screen_t* screen;
		xcb_atom_t wm_protocols;
		xcb_atom_t wm_delete_win;
	};
#endif
}