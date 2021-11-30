#pragma once
#include "macros.h"
#include "types/primitive.h"
#include "types/string.h"

namespace toast
{
	struct config
	{
		i16 posX;
		i16 posY;
		i16 width;
		i16 height;
		str<cv> name;
	};

	/// <summary>
	/// This is class for deriving from
	/// and this will definitely have overhead
	/// so if you want to limit overhead you need to make
	/// direct calls to application.h
	/// </summary>
	class TEXPORT Game
	{
	public:

	public:
		virtual config init() = 0;
		virtual bool start() = 0;
		virtual bool update(float deltaTime) = 0;
		virtual TINLINE ~Game() = 0;
	};

	Game::~Game() {}
}