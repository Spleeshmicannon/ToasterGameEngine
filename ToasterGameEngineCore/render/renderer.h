#include "../macros.h"
#include "../types/types.h"
#include "../platform/platformState.h"
#include "../logger.h"
#include "../memory.h"

namespace toast
{
	struct renderPacket
	{
		f32 deltaTime;
		
		u64 vertexCount;
		vertex3D* vertices;
	};

	struct renderContext;

	class Renderer
	{
	private:
		renderContext* context;

	public:
		b8 initialise(str<char> name, platformState* state, u16 width, u16 height);
		void drawFrame(renderPacket* packet);
		void shutdown(platformState* state);
	};
}