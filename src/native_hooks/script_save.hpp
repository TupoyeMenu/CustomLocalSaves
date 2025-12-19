#include "gta/joaat.hpp"
#include "natives.hpp"
#include "services/stats/stats_service.hpp"

namespace big
{
	namespace all_scripts
	{
		static int indent;
		static bool in_sp_save;
		static std::vector<std::string> last_key;
		inline void LOG_SAVE_START(rage::scrNativeCallContext* src)
		{
			in_sp_save = src->get_arg<BOOL>(2);
			if (!in_sp_save)
			{
				indent++;
				std::cout << std::string(indent, '	') << "Started save" << "\n";
				return;
			}

			MISC::START_SAVE_DATA(src->get_arg<Any*>(0), src->get_arg<Any>(1), src->get_arg<BOOL>(2));
		}
		inline void LOG_SAVE_END(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				indent--;
				std::cout << std::string(indent, '	') << "Ended save" << "\n";
				return;
			}
			MISC::STOP_SAVE_DATA();
		}
		inline void LOG_SAVE_START_STRUCT(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				last_key.emplace_back(src->get_arg<const char*>(2));
				indent++;
				std::cout << std::string(indent, '	') << "Started save struct " << src->get_arg<const char*>(2) << " "
				          << HEX_TO_UPPER(rage::literal_joaat(src->get_arg<const char*>(2))) << "\n";
				return;
			}
			MISC::START_SAVE_STRUCT_WITH_SIZE(src->get_arg<Any*>(0), src->get_arg<int>(1), src->get_arg<const char*>(2));
		}
		inline void LOG_SAVE_END_STRUCT(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				indent--;
				if(!last_key.empty())
					last_key.pop_back();
				std::cout << std::string(indent, '	') << "Ended save struct" << "\n";
				return;
			}
			MISC::STOP_SAVE_STRUCT();
		}
		inline void LOG_SAVE_START_ARRAY(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				indent++;
				last_key.emplace_back(src->get_arg<const char*>(2));
				std::cout << std::string(indent, '	') << "Started save array " << src->get_arg<const char*>(2) << " "
				          << HEX_TO_UPPER(rage::literal_joaat(src->get_arg<const char*>(2))) << "\n";
				return;
			}
			MISC::START_SAVE_ARRAY_WITH_SIZE(src->get_arg<Any*>(0), src->get_arg<int>(1), src->get_arg<const char*>(2));
		}
		inline void LOG_SAVE_END_ARRAY(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				indent--;
				if(!last_key.empty())
					last_key.pop_back();
				std::cout << std::string(indent, '	') << "Ended save array" << "\n";
				return;
			}
			MISC::STOP_SAVE_ARRAY();
		}

		inline void REGISTER_INT_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::INT);
				return;
			}

			MISC::REGISTER_INT_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_INT64_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::INT64);
				return;
			}

			MISC::REGISTER_INT64_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_ENUM_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::ENUM);
				return;
			}

			MISC::REGISTER_ENUM_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_FLOAT_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::FLOAT);
				return;
			}

			MISC::REGISTER_FLOAT_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_BOOL_TO_SAVE(rage::scrNativeCallContext* src)
		{
			try
			{
				if (!in_sp_save)
				{
					g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::BOOL_);
					return;
				}
			}
			catch (const std::exception& ex)
			{
				LOG(FATAL) << "Stats failed to save: " << ex.what();
				return;
			}

			MISC::REGISTER_BOOL_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_TEXT_LABEL_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				LOG(VERBOSE) << src->get_arg<const char*>(0);
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::TEXT_LABEL_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_TEXT_LABEL_15_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::TEXT_LABEL_15_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_15_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_TEXT_LABEL_23_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::TEXT_LABEL_23_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_23_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_TEXT_LABEL_31_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::TEXT_LABEL_31_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_31_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
		inline void REGISTER_TEXT_LABEL_63_TO_SAVE(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				g_stats_service->get_script_save_data()[last_key.back()][src->get_arg<const char*>(1)] = script_save_var((intptr_t)src->get_arg<Any*>(0), eScriptSaveType::TEXT_LABEL_63_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_63_TO_SAVE(src->get_arg<Any*>(0), src->get_arg<const char*>(1));
		}
	}
}