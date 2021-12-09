#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/string.h"
#include "game.h"
#include "event.h"
#include "input.h"

namespace toast
{
	struct applicationInternals;

	class TEXPORT Application
	{
	private:
		static b8 running;
		static b8 suspended;

	private:
		static applicationInternals* internals;

	private:
		static b8 onEvent(eventCode code, ptr sender,
			ptr listener, eventContext context);

		static b8 onKey(eventCode code, ptr sender,
			ptr listener, eventContext context);

	public:

		static b8 create(Game &usrGame);
		static b8 runLoop(Game &usrGame);
		static void debugLog(const char* message);
	};
}