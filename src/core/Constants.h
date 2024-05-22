// MIT License, Copyright (c) 2019 Malik Allen

#ifndef NEBULA_CONSTANTS_H
#define NEBULA_CONSTANTS_H

#include <cstdint>

namespace Nebula 
{
	typedef uint64_t EntityId;

	typedef uint64_t ComponentId;

	static constexpr size_t MAX_ENTITIES	{ 100000 };

	static constexpr size_t MAX_COMPONENTS_PER_ENTITY	{ 1000 };

	static constexpr size_t MAX_SYSTEMS	{ 1000 };

	static constexpr size_t MAX_COMPONENTS	{ MAX_ENTITIES * MAX_COMPONENTS_PER_ENTITY };
}

#endif // !NEBULA_CONSTANTS_H

