#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/structs.h"

#define TDEFINE_KEY(name, code) TKEY_##name = code

namespace toast
{

    enum mseBtns
    {
        BUTTON_LEFT,
        BUTTON_RIGHT,
        BUTTON_MIDDLE,
        BUTTON_MAX_BUTTONS
    };

    enum keys
    {
        TDEFINE_KEY(BACKSPACE, 0x08),
        TDEFINE_KEY(ENTER, 0x0D),
        TDEFINE_KEY(TAB, 0x09),
        TDEFINE_KEY(SHIFT, 0x10),
        TDEFINE_KEY(CONTROL, 0x11),

        TDEFINE_KEY(PAUSE, 0x13),
        TDEFINE_KEY(CAPITAL, 0x14),

        TDEFINE_KEY(ESCAPE, 0x1B),

        TDEFINE_KEY(CONVERT, 0x1C),
        TDEFINE_KEY(NONCONVERT, 0x1D),
        TDEFINE_KEY(ACCEPT, 0x1E),
        TDEFINE_KEY(MODECHANGE, 0x1F),

        TDEFINE_KEY(SPACE, 0x20),
        TDEFINE_KEY(PRIOR, 0x21),
        TDEFINE_KEY(NEXT, 0x22),
        TDEFINE_KEY(END, 0x23),
        TDEFINE_KEY(HOME, 0x24),
        TDEFINE_KEY(LEFT, 0x25),
        TDEFINE_KEY(UP, 0x26),
        TDEFINE_KEY(RIGHT, 0x27),
        TDEFINE_KEY(DOWN, 0x28),
        TDEFINE_KEY(SELECT, 0x29),
        TDEFINE_KEY(PRINT, 0x2A),
        TDEFINE_KEY(EXECUTE, 0x2B),
        TDEFINE_KEY(SNAPSHOT, 0x2C),
        TDEFINE_KEY(INSERT, 0x2D),
        TDEFINE_KEY(DELETE, 0x2E),
        TDEFINE_KEY(HELP, 0x2F),

        TDEFINE_KEY(A, 0x41),
        TDEFINE_KEY(B, 0x42),
        TDEFINE_KEY(C, 0x43),
        TDEFINE_KEY(D, 0x44),
        TDEFINE_KEY(E, 0x45),
        TDEFINE_KEY(F, 0x46),
        TDEFINE_KEY(G, 0x47),
        TDEFINE_KEY(H, 0x48),
        TDEFINE_KEY(I, 0x49),
        TDEFINE_KEY(J, 0x4A),
        TDEFINE_KEY(K, 0x4B),
        TDEFINE_KEY(L, 0x4C),
        TDEFINE_KEY(M, 0x4D),
        TDEFINE_KEY(N, 0x4E),
        TDEFINE_KEY(O, 0x4F),
        TDEFINE_KEY(P, 0x50),
        TDEFINE_KEY(Q, 0x51),
        TDEFINE_KEY(R, 0x52),
        TDEFINE_KEY(S, 0x53),
        TDEFINE_KEY(T, 0x54),
        TDEFINE_KEY(U, 0x55),
        TDEFINE_KEY(V, 0x56),
        TDEFINE_KEY(W, 0x57),
        TDEFINE_KEY(X, 0x58),
        TDEFINE_KEY(Y, 0x59),
        TDEFINE_KEY(Z, 0x5A),

        TDEFINE_KEY(LWIN, 0x5B),
        TDEFINE_KEY(RWIN, 0x5C),
        TDEFINE_KEY(APPS, 0x5D),

        TDEFINE_KEY(SLEEP, 0x5F),

        TDEFINE_KEY(NUMPAD0, 0x60),
        TDEFINE_KEY(NUMPAD1, 0x61),
        TDEFINE_KEY(NUMPAD2, 0x62),
        TDEFINE_KEY(NUMPAD3, 0x63),
        TDEFINE_KEY(NUMPAD4, 0x64),
        TDEFINE_KEY(NUMPAD5, 0x65),
        TDEFINE_KEY(NUMPAD6, 0x66),
        TDEFINE_KEY(NUMPAD7, 0x67),
        TDEFINE_KEY(NUMPAD8, 0x68),
        TDEFINE_KEY(NUMPAD9, 0x69),
        TDEFINE_KEY(MULTIPLY, 0x6A),
        TDEFINE_KEY(ADD, 0x6B),
        TDEFINE_KEY(SEPARATOR, 0x6C),
        TDEFINE_KEY(SUBTRACT, 0x6D),
        TDEFINE_KEY(DECIMAL, 0x6E),
        TDEFINE_KEY(DIVIDE, 0x6F),
        TDEFINE_KEY(F1, 0x70),
        TDEFINE_KEY(F2, 0x71),
        TDEFINE_KEY(F3, 0x72),
        TDEFINE_KEY(F4, 0x73),
        TDEFINE_KEY(F5, 0x74),
        TDEFINE_KEY(F6, 0x75),
        TDEFINE_KEY(F7, 0x76),
        TDEFINE_KEY(F8, 0x77),
        TDEFINE_KEY(F9, 0x78),
        TDEFINE_KEY(F10, 0x79),
        TDEFINE_KEY(F11, 0x7A),
        TDEFINE_KEY(F12, 0x7B),
        TDEFINE_KEY(F13, 0x7C),
        TDEFINE_KEY(F14, 0x7D),
        TDEFINE_KEY(F15, 0x7E),
        TDEFINE_KEY(F16, 0x7F),
        TDEFINE_KEY(F17, 0x80),
        TDEFINE_KEY(F18, 0x81),
        TDEFINE_KEY(F19, 0x82),
        TDEFINE_KEY(F20, 0x83),
        TDEFINE_KEY(F21, 0x84),
        TDEFINE_KEY(F22, 0x85),
        TDEFINE_KEY(F23, 0x86),
        TDEFINE_KEY(F24, 0x87),

        TDEFINE_KEY(NUMLOCK, 0x90),
        TDEFINE_KEY(SCROLL, 0x91),

        TDEFINE_KEY(NUMPAD_EQUAL, 0x92),

        TDEFINE_KEY(LSHIFT, 0xA0),
        TDEFINE_KEY(RSHIFT, 0xA1),
        TDEFINE_KEY(LCONTROL, 0xA2),
        TDEFINE_KEY(RCONTROL, 0xA3),
        TDEFINE_KEY(LALT, 0xA4),
        TDEFINE_KEY(RALT, 0xA5),

        TDEFINE_KEY(SEMICOLON, 0xBA),
        TDEFINE_KEY(PLUS, 0xBB),
        TDEFINE_KEY(COMMA, 0xBC),
        TDEFINE_KEY(MINUS, 0xBD),
        TDEFINE_KEY(PERIOD, 0xBE),
        TDEFINE_KEY(SLASH, 0xBF),
        TDEFINE_KEY(GRAVE, 0xC0),

        KEYS_MAX_KEYS
    };

    struct inputState;
	class InputManager
	{
    private:
        inputState* kbstate;

    public:
        InputManager();
        ~InputManager();
        void processKey(keys key, b8 pressed);
        void processBtn(mseBtns btn, b8 pressed);
    };
}