#include "primitive.h"

namespace toast
{
	template<typename Rtype = void, typename ... Ptype>
	using func = Rtype(*)(Ptype ...);
}