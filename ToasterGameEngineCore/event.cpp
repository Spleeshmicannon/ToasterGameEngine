#include "event.h"
#include "memory.h"
#include "containers.h"
#include "types/callback.h"


#define TOAST_MAX_MESSAGE_CODES 8000

namespace toast
{

	struct onEventData
	{
		u16 code;
		void* sender;
		void* listenerInst;
		eventContext evCont;
	};

	struct registeredEvent
	{
		void* listener;
		func<b8, onEventData> callback;
	};

	struct eventState
	{
		LiteVector<registeredEvent> events;
	};


}