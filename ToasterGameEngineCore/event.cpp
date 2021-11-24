#include "event.h"
#include "memory.h"
#include "containers.h"


#define TOAST_MAX_MESSAGE_CODES 8000

namespace toast
{
	struct registeredEvent
	{
		ptr listener;
		func<b8, onEventData> callback;
	};

	struct eventCodeEntry
	{
		LiteVector<registeredEvent> * events;
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
				tdelete<LiteVector<registeredEvent>>(state->registered[i].events);
			}
		}

		deallocate<eventState>(state);
	}

	b8 EventManager::registerEvent(eventCode::sysEventCode code, ptr listener,
		func<b8, onEventData> onEvent)
	{

		// allocating any necessary new memory
		if (state->registered[code].events == nullptr)
		{
			state->registered[code].events =
				tnew<LiteVector<registeredEvent>>();
		}
		
		// checking for duplicate listener
		b8 duplicateListnerFound = false;
		for (u32 i = 0; (i < state->registered[code].events->length) ||
			(!duplicateListnerFound); ++i)
		{
			duplicateListnerFound = state->registered[code].events
				->data[i].listener == listener;
		}

		if (duplicateListnerFound)
		{
			Logger::staticLog<logLevel::TWARN>("Can't register duplicate listener!");
			return false;
		}

		// register event if no duplicate found
		state->registered[code].events->push({listener, onEvent});
		return true;
	}

	b8 EventManager::unregisterEvent(eventCode::sysEventCode code, ptr listener,
		func<b8, onEventData> onEvent)
	{
		if (state->registered[code].events == nullptr)
		{
			Logger::staticLog<logLevel::TWARN>("Can't unregister unregsitered event!");
			return false;
		}

		for (u32 i = 0; i < state->registered[code].events->length; ++i)
		{
			if (state->registered[code].events->data[i].listener == listener)
			{
				state->registered[code].events->unsafePopAt(i);
				return true;
			}
		}
		
		Logger::staticLog<logLevel::TWARN>("Can't unregister unregsitered event!");
		return false;
	}

	b8 EventManager::fireEvent(eventCode::sysEventCode code, ptr listener,
		func<b8, onEventData> onEvent)
	{
		if (state->registered[code].events == nullptr)
		{
			return false;
		}

		for (u32 i = 0; i < state->registered[code].events->length; ++i)
		{
			if (state->registered[code].events->data[i].listener == listener)
			{
				// message has been handled, dont send to other listeners
				return true;
			}
		}

		Logger::staticLog<logLevel::TWARN>("Event not found");
		return false;
	}
}