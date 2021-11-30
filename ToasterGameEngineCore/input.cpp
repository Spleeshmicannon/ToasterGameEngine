#include "input.h"
#include "logger.h"
#include "memory.h"

namespace toast
{

	inputState* Input::state;
	EventManager* Input::eventManager;

	struct keyboardState
	{
		b8 keys[256];
	};

	struct mouseState
	{
		point pt;
		u8 btns[static_cast<u8>(mseBtns::MAX_BUTTONS)];
	};

	struct inputState
	{
		keyboardState prevKb;
		keyboardState currKb;
		mouseState prevMse;
		mouseState currMse;
	};

	b8 Input::initialise(EventManager* _eventManager)
	{
		state = allocate<inputState>(1);
		eventManager = _eventManager;
		return true;
	}

	b8 Input::shutdown()
	{
		deallocate<inputState>(state);
		return true;
	}

	void Input::update(f32 deltaTime)
	{
		copyMem<keyboardState>(&state->prevKb, &state->currKb,1);
		copyMem<mouseState>(&state->prevMse, &state->currMse,1);
	}

	b8 Input::isKeyDown(keys key)
	{
		return state->currKb.keys[static_cast<u16>(key)];
	}

	b8 Input::isKeyUp(keys key)
	{
		return !state->currKb.keys[static_cast<u16>(key)];
	}

	b8 Input::wasKeyDown(keys key)
	{
		return state->prevKb.keys[static_cast<u16>(key)];
	}

	b8 Input::wasKeyUp(keys key)
	{
		return !state->prevKb.keys[static_cast<u16>(key)];
	}

	b8 Input::isMseBtnDown(mseBtns btn)
	{
		return state->currMse.btns[static_cast<u16>(btn)];
	}

	b8 Input::isMseBtnUp(mseBtns btn)
	{
		return !state->currMse.btns[static_cast<u16>(btn)];
	}

	b8 Input::wasMseBtnDown(mseBtns btn)
	{
		return state->prevMse.btns[static_cast<u16>(btn)];
	}

	b8 Input::wasMseBtnUp(mseBtns btn)
	{
		return !state->prevMse.btns[static_cast<u16>(btn)];
	}

	void Input::getMsePos(point* pt)
	{
		(*pt).x = state->currMse.pt.x;
		(*pt).y = state->currMse.pt.y;
	}

	void Input::getPrevMsePos(point* pt)
	{
		(*pt).x = state->prevMse.pt.x;
		(*pt).y = state->prevMse.pt.y;
	}

	void Input::processKey(keys key, b8 pressed)
	{
		if(state->currKb.keys[static_cast<u16>(key)] != pressed)
		{
			// update key
			state->currKb.keys[static_cast<u16>(key)] = pressed;

			//fire event
			eventContext context;
			context._u16[0] = static_cast<u16>(key);

			eventManager->fireEvent(pressed ? eventCode::KEY_PRESSED : 
				eventCode::KEY_RELEASED, 0, context);
		}
	}

	void Input::processBtn(mseBtns btn, b8 pressed)
	{
		if (state->currMse.btns[static_cast<u16>(btn)] != pressed)
		{
			// update key
			state->currMse.btns[static_cast<u16>(btn)] = pressed;

			//fire event
			eventContext context;
			context._u16[0] = static_cast<u16>(btn);
			eventManager->fireEvent(pressed ? eventCode::BUTTON_PRESSED :
				eventCode::BUTTON_RELEASED, 0, context);
		}
	}

	void Input::processMseMove(i16 x, i16 y)
	{
		if (state->currMse.pt.x != x || state->currMse.pt.y != y)
		{
			// update internals
			state->currMse.pt.x = x;
			state->currMse.pt.y = y;

			//fire event
			eventContext context;
			context._i16[0] = x;
			context._i16[1] = y;
			eventManager->fireEvent(eventCode::MOUSE_SHIFT, 0, context);
		}
	}

	void Input::processMseWheel(i32 zDelta)
	{
		eventContext context;
		context._u8[0] = zDelta;
		eventManager->fireEvent(eventCode::MOUSE_WHEEL, 0, context);
		return;
	}
}