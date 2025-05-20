#pragma once
#include "pattern.hpp"

#include <functional>
#include <vector>

namespace memory
{
	class pattern_batch
	{
	public:
		explicit pattern_batch()  = default;
		~pattern_batch() noexcept = default;

		void add(std::string name, pattern pattern, std::function<void(memory::handle)> callback);
		void add(std::string name, pattern pattern, int min_version, int max_version, eGameBranch game_branch, std::function<void(memory::handle)> callback);
		void run(range region);

		struct entry
		{
			std::string m_name;
			pattern m_pattern;
			int m_min_version;
			int m_max_version;
			eGameBranch m_game_branch;
			std::function<void(memory::handle)> m_callback;

			explicit entry(std::string name, pattern pattern, std::function<void(memory::handle)> callback) :
			    m_name(std::move(name)),
			    m_pattern(std::move(pattern)),
			    m_callback(std::move(callback)),
				m_min_version(-1),
				m_max_version(-1),
				m_game_branch(eGameBranch::DontCare)
			{
			}

			explicit entry(std::string name, pattern pattern, int min_version, int max_version, eGameBranch game_branch, std::function<void(memory::handle)> callback) :
			    m_name(std::move(name)),
			    m_pattern(std::move(pattern)),
			    m_callback(std::move(callback)),
				m_min_version(min_version),
				m_max_version(max_version),
				m_game_branch(game_branch)
			{
			}
		};

	private:
		std::vector<entry> m_entries;
	};
}
