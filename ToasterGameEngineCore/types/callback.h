#include "primitive.h"

namespace toast
{
	template<typename Rtype = void, typename Ptype = void>
	using func = Rtype(*)(Ptype);
}