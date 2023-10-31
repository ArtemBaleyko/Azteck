#include "azpch.h"
#include "UUID.h"

#include <random>

namespace Azteck
{
	static std::random_device device;
	static std::mt19937_64 engine(device());
	static std::uniform_int_distribution<uint64_t> uniformDistribution;

	UUID::UUID()
		: _uuid(uniformDistribution(engine))
	{
	}

	UUID::UUID(uint64_t uuid)
		: _uuid(uuid)
	{
	}
}
