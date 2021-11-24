#pragma once
#include "macros.h"
#include "types/primitive.h"

namespace toast
{
	namespace eventCode
	{
		enum sysEventCode
		{
			APPLICATION_QUIT	= 0x01,
			KEY_PRESSED			= 0x02,
			KEY_RELEASED		= 0x03,
			BUTTON_PRESSED		= 0x04,
			BUTTON_RELEASED		= 0x05,
			MOUSE_MOVED			= 0x06,
			MOUSE_WHEEL			= 0x07,
			CODE_RESIZED		= 0x08,

			MAX_EVENT_CODE		= 0xFF
		};
	}

	struct eventContext
	{
		union
		{
			i64 i64[2];
			u64 u64[2];
			f64 f64[2];

			i32 i32[4];
			u32 u32[4];
			f32 f32[4];

			i16 i16[8];
			u16 u16[8];

			i8 i8[16];
			u8 u8[16];
			cv c[16];
		};
	};

	struct eventState;

	class TEXPORT EventManager
	{
	private:
		eventContext evCont;
		eventState evState;
		b8 initialised;

	public:
		EventManager();
		~EventManager();

		b8 registerEvent();
		b8 unregisterEvent();

		b8 fireEvent();

	};
}