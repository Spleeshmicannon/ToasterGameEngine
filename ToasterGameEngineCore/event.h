#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/callback.h"

namespace toast
{
	namespace eventCode
	{
		enum sysEventCode
		{
			TAPPLICATION_QUIT	= 0x01,
			TKEY_PRESSED		= 0x02,
			TKEY_RELEASED		= 0x03,
			TBUTTON_PRESSED		= 0x04,
			TBUTTON_RELEASED	= 0x05,
			TMOUSE_MOVED		= 0x06,
			TMOUSE_WHEEL		= 0x07,
			TCODE_RESIZED		= 0x08,

			TMAX_EVENT_CODE		= 0xFF
		};
	}

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

	struct onEventData
	{
		u16 code;
		ptr sender;
		ptr listenerInst;
		eventContext evCont;
	};

	struct eventState;

	class EventManager
	{
	private:
		eventState * state;

	public:
		EventManager();
		~EventManager();

		b8 registerEvent(eventCode::sysEventCode code, ptr listener,
			func<b8, onEventData> onEvent);
		b8 unregisterEvent(eventCode::sysEventCode code, ptr listener,
			func<b8, onEventData> onEvent);

		b8 fireEvent(eventCode::sysEventCode code, ptr listener,
			func<b8, onEventData> onEvent);

	};
}