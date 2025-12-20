#include "gta/joaat.hpp"
#include "natives.hpp"
#include "services/stats/stats_service.hpp"

namespace big
{
	namespace all_scripts
	{
		static int indent = 0;
		static bool in_sp_save;
		static std::vector<stats_service::script_json*> last_key;

		inline void START_SAVE_DATA(rage::scrNativeCallContext* src)
		{
			in_sp_save = src->get_arg<BOOL>(2);
			if (!in_sp_save)
			{
				std::cout << "Started save" << "\n";
				last_key.emplace_back(&g_stats_service->get_script_save_data());
				return;
			}

			MISC::START_SAVE_DATA(src->get_arg<Any*>(0), src->get_arg<Any>(1), src->get_arg<BOOL>(2));
		}
		inline void STOP_SAVE_DATA(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				std::cout << "Ended save" << "\n";
				return;
			}
			MISC::STOP_SAVE_DATA();
		}
		inline void START_SAVE_STRUCT_WITH_SIZE(rage::scrNativeCallContext* src)
		{
			Any* struct_ptr         = src->get_arg<Any*>(0);
			int struct_size         = src->get_arg<int>(1);
			const char* struct_name = src->get_arg<const char*>(2);

			if (!in_sp_save)
			{
				(*last_key.back())[struct_name] = {};
				last_key.emplace_back(&(*last_key.back())[struct_name]);

				std::cout << std::string(last_key.size(), '	') << "Started save struct " << struct_name << " " << HEX_TO_UPPER(rage::literal_joaat(struct_name)) << "\n";
				return;
			}

			MISC::START_SAVE_STRUCT_WITH_SIZE(struct_ptr, struct_size, struct_name);
		}
		inline void STOP_SAVE_STRUCT(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				std::cout << std::string(last_key.size(), '	') << "Ended save struct" << "\n";

				if (last_key.size() > 1)
					last_key.pop_back();
				return;
			}

			MISC::STOP_SAVE_STRUCT();
		}
		inline void START_SAVE_ARRAY_WITH_SIZE(rage::scrNativeCallContext* src)
		{
			Any* array_ptr         = src->get_arg<Any*>(0);
			int array_size         = src->get_arg<int>(1);
			const char* array_name = src->get_arg<const char*>(2);

			if (!in_sp_save)
			{
				(*last_key.back())[array_name] = {};
				last_key.emplace_back(&(*last_key.back())[array_name]);

				std::cout << std::string(last_key.size(), '	') << "Started save array " << array_name << " " << HEX_TO_UPPER(rage::literal_joaat(array_name)) << "\n";
				return;
			}

			MISC::START_SAVE_ARRAY_WITH_SIZE(array_ptr, array_size, array_name);
		}
		inline void STOP_SAVE_ARRAY(rage::scrNativeCallContext* src)
		{
			if (!in_sp_save)
			{
				std::cout << std::string(last_key.size(), '	') << "Ended save array" << "\n";

				if (last_key.size() > 1)
					last_key.pop_back();
				return;
			}

			MISC::STOP_SAVE_ARRAY();
		}

		inline void REGISTER_INT_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::INT);
				return;
			}

			MISC::REGISTER_INT_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_INT64_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::INT64);
				return;
			}

			MISC::REGISTER_INT64_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_ENUM_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::ENUM);
				return;
			}

			MISC::REGISTER_ENUM_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_FLOAT_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::FLOAT);
				return;
			}

			MISC::REGISTER_FLOAT_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_BOOL_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::BOOL_);
				return;
			}

			MISC::REGISTER_BOOL_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_TEXT_LABEL_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::TEXT_LABEL_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_TEXT_LABEL_15_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::TEXT_LABEL_15_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_15_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_TEXT_LABEL_23_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::TEXT_LABEL_23_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_23_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_TEXT_LABEL_31_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::TEXT_LABEL_31_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_31_TO_SAVE(data_ptr, name);
		}
		inline void REGISTER_TEXT_LABEL_63_TO_SAVE(rage::scrNativeCallContext* src)
		{
			Any* data_ptr    = src->get_arg<Any*>(0);
			const char* name = src->get_arg<const char*>(1);

			if (!in_sp_save)
			{
				(*last_key.back())[name] = script_save_var((intptr_t)data_ptr, eScriptSaveType::TEXT_LABEL_63_);
				return;
			}

			MISC::REGISTER_TEXT_LABEL_63_TO_SAVE(data_ptr, name);
		}
	}
}