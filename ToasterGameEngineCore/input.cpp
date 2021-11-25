#include "input.h"
#include "logger.h"
#include "memory.h"

namespace toast
{
	struct keyboardState
	{
		b8 keys[256];
	};

	struct mouseState
	{
		point pt;
		u8 btns[BUTTON_MAX_BUTTONS];
	};

	struct inputState
	{
		keyboardState prevKb;
		keyboardState currKb;
		mouseState prevMse;
		mouseState currMse;
	};
}