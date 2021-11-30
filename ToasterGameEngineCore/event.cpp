#include "event.h"
#include "memory.h"
#include "containers.h"

#define TOAST_MAX_MESSAGE_CODES 8000

namespace toast
{
	struct registeredEvent
	{
		ptr listener;
		func<b8, eventCode, ptr, ptr, eventContext> callback;
	};

	struct eventCodeEntry
	{
		vector<registeredEvent> * events;
	};

	struct eventState
	{
		eventCodeEntry registered[TOAST_MAX_MESSAGE_CODES];
	};

	EventManager::EventManager() : state(allocate<eventState>(1))
	{}

	EventManager::~EventManager()
	{
		for (i32 i = 0; i < TOAST_MAX_MESSAGE_CODES; ++i)
		{
			if (state->registered[i].events != nullptr)
			{
				tdelete<std::vector<registeredEvent>>(state->registered[i].events);
			}
		}

		deallocate<eventState>(state);
	}

	b8 EventManager::registerEvent(eventCode code, ptr listener,
		func<b8, eventCode, ptr, ptr, eventContext> onEvent)
	{

		// allocating any necessary new memory
		if (state->registered[static_cast<u16>(code)].events == nullptr)
		{
			state->registered[static_cast<u16>(code)].events =
				tnew<std::vector<registeredEvent>>();
		}
		
		// checking for duplicate listener
		b8 duplicateFound = false;
		for (u32 i = 0; i < state->registered[static_cast<u16>(code)].events->size(); ++i)
		{
			if ((*state->registered[static_cast<u16>(code)].events)[i].callback == onEvent)
			{
				Logger::staticLog<logLevel::TWARN>("Can't register duplicate event!");
				return false;
			}
		}

		// register event if no duplicate found
		state->registered[static_cast<u16>(code)].events->push_back({ listener, onEvent });
		return true;
	}

	b8 EventManager::unregisterEvent(eventCode code, ptr listener,
		func<b8, eventCode, ptr, ptr, eventContext> onEvent)
	{
		if (state->registered[static_cast<u16>(code)].events == nullptr)
		{
			Logger::staticLog<logLevel::TWARN>("Can't unregister unregsitered event!");
			return false;
		}

		for (u32 i = 0; i < state->registered[static_cast<u16>(code)].events->size(); ++i)
		{


			if ((*state->registered[static_cast<u16>(code)].events)[i].callback == onEvent)
			{
				state->registered[static_cast<u16>(code)].events->erase(
					state->registered[static_cast<u16>(code)].events->begin() + i);
				return true;
			}
		}
		
		Logger::staticLog<logLevel::TWARN>("Can't unregister unregsitered event!");
		return false;
	}

	b8 EventManager::fireEvent(eventCode code, ptr sender, eventContext context)
	{
		if (state->registered[static_cast<u16>(code)].events == nullptr)
		{
			//Logger::staticLog<logLevel::TWARN>("Event doesn't exist");
			return false;
		}

		for (u32 i = 0; i < state->registered[static_cast<u16>(code)].events->size(); ++i)
		{
			const registeredEvent e = (*state->registered[static_cast<u16>(code)].events)[i];

			if (e.callback != nullptr)
			{
				if (e.callback(code, sender, e.listener, context))
				{
					// message has been handled, dont send to other listeners
					return true;
				}
			}
			else
			{
				Logger::staticLog<logLevel::TWARN>("Null callback");
			}
		}

		Logger::staticLog<logLevel::TWARN>("Event not handled");
		return false;
	}
}