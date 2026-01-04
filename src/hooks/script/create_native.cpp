#include "crossmap.hpp"
#include "hooking/hooking.hpp"
#include "invoker.hpp"
#include "logger/logger.hpp"
#include "native_hooks/native_hooks.hpp"

namespace big
{
	void hooks::create_native(void* a1, rage::scrNativeHash native_hash, rage::scrNativeHandler native_handler)
	{
		rage::scrNativeHash community_hash = native_hash;
		for (const rage::scrNativePair& mapping : g_crossmap)
		{
			if (mapping.second == native_hash)
			{
				community_hash = mapping.first;
				break;
			}
		}
		g_native_invoker.add_native_handler(community_hash, native_handler);

		//LOGF(VERBOSE, "{{{:#018X}, {:#018X}}},", community_hash, native_hash);

		g_hooking->get_original<hooks::create_native>()(a1, native_hash, native_handler);
	}
}