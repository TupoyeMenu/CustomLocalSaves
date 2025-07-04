#include "gta/stat.hpp"

#include "file_manager.hpp"
#include "hooking/hooking.hpp"
#include "logger/logger.hpp"
#include "pointers.hpp"
#include "services/stats/stats_service.hpp"
#include "util/decrypt_save.hpp"
#include "util/profile_stats.hpp"

namespace big
{
	// const char** name is actually a structure.
	void hooks::create_stat(void* p1, const char** name)
	{
		g_hooking->get_original<hooks::create_stat>()(p1, name);
		g_stats_service->register_stat(*name);
	}

	bool hooks::mp_stats_save(void* _this, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6)
	{
		LOGF(VERBOSE, "mp_stats_save({}, {}, {}, {}, {}, {})", _this, p2, p3, p4, p5, p6);

		g_stats_service->save_stats();

		return true;
	}

	std::string pso_buffer;
	char* og_buffer_ptr;
	uint64_t og_buffer_size;
	bool json_loaded   = false;
	bool pso_loaded    = false;
	bool tried_loading = false;
	bool hooks::mp_save_download(CSavegameQueuedOperation_MPStats_Load* _this)
	{
		LOG(VERBOSE) << _this->m_download_state;
		LOG(VERBOSE) << _this->m_download_status;
		LOG(VERBOSE) << _this->m_http_error;
		switch (_this->m_download_state)
		{
		case 1:
		{
			if (tried_loading)
				break;

			tried_loading = true;
			json_loaded   = g_stats_service->load_stats();

			if (json_loaded || !g.load_fsl_files)
			{
				// Make sure cloud load fails since we already have the stats.
				_this->m_download_status = 2;
				break;
			}

			auto size = g_stats_service->get_pso_file_size(_this->m_char_slot);
			if (size == -1)
			{
				break;
			}

			pso_buffer.resize(size, '\0');
			g_stats_service->read_pso_file(_this->m_char_slot, pso_buffer.data(), size);
			pso_loaded = true;
			LOGF(VERBOSE, "Loaded PSO file for char slot {}", _this->m_char_slot);

			og_buffer_ptr             = _this->m_save_buffer;
			og_buffer_size            = _this->m_save_buffer_size;
			_this->m_save_buffer      = pso_buffer.data();
			_this->m_save_buffer_size = size;

			_this->m_download_status = 3;
			decrypt_save_patch::apply();
			break;
		}
		case 3:
		{
			if (!json_loaded && !pso_loaded && !g.no_rgs)
			{
				// Allow profile stats to sync here. Reapplied at hooks::profile_stats_download
				skip_profile_stats_patch::restore();
				LOG(VERBOSE) << _this->m_save_buffer_size;
				if(_this->m_save_buffer_size)
				{
					LOG(VERBOSE) << _this->m_save_buffer;
				}
			}

			if (pso_loaded)
			{
				_this->m_save_buffer      = og_buffer_ptr;
				_this->m_save_buffer_size = og_buffer_size;

				pso_buffer.clear();
				decrypt_save_patch::restore();
			}
			if (json_loaded || pso_loaded)
			{
				// Set the load as successful
				LOGF(VERBOSE, "Setting load as successful for slot: {} json_loaded == {}, pso_loaded == {}", _this->m_char_slot, json_loaded, pso_loaded);
				json_loaded   = false;
				pso_loaded    = false;
				tried_loading = false;

				*g_pointers->m_mp_save_download_error = 0;
				_this->m_download_successful          = true;
				_this->m_unk_failed                   = false;
			}
			break;
		}
		}

		bool ret = g_hooking->get_original<hooks::mp_save_download>()(_this);
		return ret;
	}
}