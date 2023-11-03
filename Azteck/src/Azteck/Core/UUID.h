#pragma once

namespace Azteck
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);

		operator uint64_t() const { return _uuid; }

	private:
		uint64_t _uuid;
	};
}

namespace std {

	template <typename T> struct hash;

	template<>
	struct hash<Azteck::UUID>
	{
		std::size_t operator()(const Azteck::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}
