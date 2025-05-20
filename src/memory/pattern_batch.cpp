#include "pattern_batch.hpp"

#include "common.hpp"
#include "range.hpp"
#include "util/is_enhanced.hpp"

namespace memory
{
	void pattern_batch::add(std::string name, pattern pattern, std::function<void(handle)> callback)
	{
		m_entries.emplace_back(std::move(name), std::move(pattern), std::move(callback));
	}
	void pattern_batch::add(std::string name, pattern pattern, int min_version, int max_version, eGameBranch game_branch, std::function<void(memory::handle)> callback)
	{
		m_entries.emplace_back(std::move(name), std::move(pattern), min_version, max_version, game_branch, std::move(callback));
	}

	void pattern_batch::run(range region)
	{
		bool all_found = true;
		for (auto& entry : m_entries)
		{
			if(entry.m_game_branch != eGameBranch::DontCare && entry.m_game_branch != big::get_game_branch())
			{
				continue;
			}
			if(entry.m_min_version != -1 && entry.m_min_version > big::g_game_version) // g_game_version is not implemented
			{
				continue;
			}
			if(entry.m_max_version != -1 && entry.m_max_version < big::g_game_version)
			{
				continue;
			}

			if (auto result = region.scan(entry.m_pattern))
			{
				if (entry.m_callback)
				{
					std::invoke(std::move(entry.m_callback), result);
					LOG(INFO) << "Found '" << entry.m_name << "' GTA5.exe+"
					                       << HEX_TO_UPPER(result.as<DWORD64>() - region.begin().as<DWORD64>());
				}
				else
				{
					all_found = false;
					LOG(WARNING) << "Failed to find '" << entry.m_name << "'.";
				}
			}
			else
			{
				all_found = false;
				LOG(WARNING) << "Failed to find '" << entry.m_name << "'.";
			}
		}

		m_entries.clear();
		if (!all_found)
		{
			throw std::runtime_error("Failed to find some patterns.");
		}
	}
}
