#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/string.h"
#include "game.h"
#include "memory.h"
#include "input.h"

namespace toast
{
	struct _internals;

	b8 isKeyDown(keys key);
	b8 isKeyUp(keys key);
	b8 wasKeyDown(keys key);
	b8 wasKeyUp(keys key);

	b8 isMseBtnDown(mseBtns btn);
	b8 isMseBtnUp(mseBtns btn);
	b8 wasMseBtnDown(mseBtns btn);
	b8 wasMseBtnUp(mseBtns btn);

	void getMsePos(point* pt);
	void getPrevMsePos(point* pt);

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