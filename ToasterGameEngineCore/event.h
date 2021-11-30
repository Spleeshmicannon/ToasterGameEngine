#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/callback.h"

namespace toast
{

	enum class eventCode
	{
		APPLICATION_QUIT	= 0x01,
		KEY_PRESSED			= 0x02,
		KEY_RELEASED		= 0x03,
		BUTTON_PRESSED		= 0x04,
		BUTTON_RELEASED		= 0x05,
		MOUSE_SHIFT			= 0x06, // mouse moved
		MOUSE_WHEEL			= 0x07,
		CODE_RESIZED		= 0x08,

		MAX_EVENT_CODE		= 0xFF
	};

	struct eventContext
	{
		union
		{
			i64 _i64[2];
			u64 _u64[2];
			f64 _f64[2];

			i32 _i32[4];
			u32 _u32[4];
			f32 _f32[4];

			i16 _i16[8];
			u16 _u16[8];

			i8 _i8[16];
			u8 _u8[16];
			cv _c[16];
		};
	};

	struct eventState;

	class EventManager
	{
	private:
		eventState * state;

	public:
		EventManager();
		~EventManager();

		b8 registerEvent(eventCode code, ptr listener,
			func<b8, eventCode, ptr, ptr, eventContext> onEvent);

		b8 unregisterEvent(eventCode code, ptr listener,
			func<b8, eventCode, ptr, ptr, eventContext> onEvent);

		b8 fireEvent(eventCode code, ptr sender, eventContext context);
	};
}