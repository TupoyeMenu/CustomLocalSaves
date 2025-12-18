#include "hooking/hooking.hpp"
#include "util/profile_stats.hpp"

namespace big
{
	bool allow_profile_stats = false;
	bool hooks::profile_stats_download(int p0, void* p1, void* p2)
	{
		LOG(VERBOSE) << "Profile stat load attempted.";
		skip_profile_stats_patch::apply();
		return g_hooking->get_original<hooks::profile_stats_download>()(p0, p1, p2);
	}
	bool hooks::profile_stats_save(void* p0, void* p1, uint32_t* p2, uint32_t p3)
	{
		*p2 = 3;
		return true;
	}
}
