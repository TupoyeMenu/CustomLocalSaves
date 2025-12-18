#pragma once
#include "file_manager.hpp"

#include <bitset>
#include <cstdint>
#include <string>

#ifdef ENABLE_GUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#endif

class CNetGamePlayer;

namespace big
{
	class menu_settings
	{
	public:
		void destroy();
		void init(const file& save_file);

		bool load();

	private:
		bool write_default_config();
		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false);
		bool save();

	private:
		bool m_running;

		file m_save_file;

		nlohmann::json m_default_options;
		nlohmann::json m_options;

	public:
		bool save_unmodified_stats = false;
		bool use_human_readable_stat_names = false;
		bool load_fsl_files = false;
		bool no_rgs = false;
		bool enable_debug_logs = false;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(menu_settings, save_unmodified_stats, use_human_readable_stat_names, load_fsl_files, no_rgs, enable_debug_logs)
	};

	inline auto g = menu_settings();
}
