#include "crossmap.hpp"
#include "hooking/hooking.hpp"
#include "invoker.hpp"
#include "native_hooks/native_hooks.hpp"

namespace big
{
	rage::scrNativeHash get_community_hash_from_game_hash(rage::scrNativeHash hash)
	{
		for (const rage::scrNativePair& mapping : g_crossmap)
		{
			if (mapping.second == hash)
			{
				return mapping.first;
			}
		}

		return hash;
	}

	void hooks::create_native(void* a1, rage::scrNativeHash native_hash, rage::scrNativeHandler native_handler)
	{
		rage::scrNativeHash community_hash = get_community_hash_from_game_hash(native_hash);

		g_native_invoker.add_native_handler(community_hash, native_handler);

		auto hooked_handler = g_native_hooks->get_hooked_handler(community_hash);
		if (hooked_handler.has_value())
		{
			native_handler = hooked_handler.value();
		}

		g_hooking->get_original<hooks::create_native>()(a1, native_hash, native_handler);
	}
}