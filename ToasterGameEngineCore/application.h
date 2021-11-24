#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/string.h"
#include "game.h"
#include "memory.h"

namespace toast
{
	struct _internals;

	class TEXPORT Application
	{
	private:
		b8 initialised;
		b8 running;
		b8 suspended;

	private:
		_internals* internals;

	public:
		Application();
		~Application();
		b8 create(Game &usrGame);
		b8 runLoop(Game &usrGame);
	};
}