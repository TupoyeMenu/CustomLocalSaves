#pragma once

#include "gta/stat.hpp"
namespace big
{
	enum class eScriptSaveType
	{
		INT,
		INT64,
		ENUM,
		FLOAT,
		BOOL_,
		TEXT_LABEL_,
		TEXT_LABEL_15_,
		TEXT_LABEL_23_,
		TEXT_LABEL_31_,
		TEXT_LABEL_63_
	};

	struct script_save_var
	{
		template<typename BasicJsonType>
		friend void update_json_data(BasicJsonType& nlohmann_json_j, const script_save_var& save_var)
		{
			switch (save_var.type)
			{
			case eScriptSaveType::INT:
			{
				nlohmann_json_j[0] = *reinterpret_cast<int*>(save_var.data_ptr);
				break;
			}
			case eScriptSaveType::INT64:
			{
				nlohmann_json_j[0] = *reinterpret_cast<int64_t*>(save_var.data_ptr);
				break;
			}
			case eScriptSaveType::ENUM:
			{
				nlohmann_json_j[0] = *reinterpret_cast<int32_t*>(save_var.data_ptr);
				break;
			}
			case eScriptSaveType::FLOAT:
			{
				nlohmann_json_j[0] = *reinterpret_cast<float*>(save_var.data_ptr);
				break;
			}
			case eScriptSaveType::BOOL_:
			{
				nlohmann_json_j[0] = *reinterpret_cast<BOOL*>(save_var.data_ptr);
				break;
			}
			case eScriptSaveType::TEXT_LABEL_:
			case eScriptSaveType::TEXT_LABEL_15_:
			case eScriptSaveType::TEXT_LABEL_23_:
			case eScriptSaveType::TEXT_LABEL_31_:
			case eScriptSaveType::TEXT_LABEL_63_:
			{
				nlohmann_json_j[0] = reinterpret_cast<const char*>(save_var.data_ptr);
				break;
			}
			}
			nlohmann_json_j[1] = save_var.type;
		}

		template<typename BasicJsonType>
		friend void to_json(BasicJsonType& nlohmann_json_j, const script_save_var& save_var)
		{
			update_json_data(nlohmann_json_j, save_var);

			nlohmann_json_j.save_var.data_ptr = save_var.data_ptr;
			nlohmann_json_j.save_var.type     = save_var.type;
		}

		intptr_t data_ptr;
		eScriptSaveType type;
	};

	class script_save_json_metadata
	{
	public:
		template<class Fnc>
		void visit(const Fnc& fnc);

		script_save_var save_var{};

	private:
		template<class Ptr, class Fnc>
		void do_visit(const Ptr& ptr, const Fnc& fnc);
	};

	class sCustomStat
	{
	public:
		sStatData* m_stat;
		bool m_is_character;
		uint8_t m_character_index;
		bool m_is_online;
	};

	class stats_service
	{
	public:
		stats_service();
		~stats_service();

		void save_stats();
		bool load_stats();

		void register_stat(const char* name);

		sStatData* get_stat_by_hash(Hash stat);

		size_t get_pso_file_size(uint8_t char_index);
		void read_pso_file(uint8_t char_index, char* buf, size_t size);

		using script_json = nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, double, std::allocator, nlohmann::adl_serializer, std::vector<std::uint8_t>, script_save_json_metadata>;

		script_json& get_script_save_data()
		{
			return m_script_save_data;
		}

	private:
		const uint8_t SAVE_OVERWRITE_INDEX = 111;
		void save_internal_stats_to_json(uint8_t char_index = 0);
		bool load_internal_stats_from_json(uint8_t char_index = 0);

		bool load_internal_script_data_from_json();

		template<typename T>
		void save_stat_map_to_json(nlohmann::json& json, T& map, bool use_stat_names, uint8_t char_index);
		template<typename T>
		void load_stat_map_from_json(const nlohmann::json& json, T& map, bool use_stat_names);

		void update_script_data_json(script_json& json);
		void load_script_data_from_json(const nlohmann::json& json);
		void save_script_data_to_json();

		std::unordered_map<Hash, int> m_int_stats;
		std::unordered_map<Hash, float> m_float_stats;
		std::unordered_map<Hash, std::string> m_string_stats;
		std::unordered_map<Hash, bool> m_bool_stats;
		std::unordered_map<Hash, uint8_t> m_uint8_stats;
		std::unordered_map<Hash, uint16_t> m_uint16_stats;
		std::unordered_map<Hash, uint32_t> m_uint32_stats;
		std::unordered_map<Hash, uint64_t> m_uint64_stats;
		std::unordered_map<Hash, int64_t> m_int64_stats;
		std::unordered_map<Hash, uint64_t> m_date_stats;
		std::unordered_map<Hash, uint64_t> m_pos_stats;
		std::unordered_map<Hash, int> m_textlabel_stats;
		std::unordered_map<Hash, int64_t> m_packed_stats;
		std::unordered_map<Hash, int64_t> m_userid_stats;

		std::unordered_map<Hash, std::string> m_stat_hash_to_string;

		script_json m_script_save_data; // It is pretty stupid to use the json structure for this, but it works and I don't care.

		file m_save_file_default;
		file m_save_file_char1;
		file m_save_file_char2;
		file m_save_file_script;
		file m_save_overwrite;
		file m_save_file_default_pso;
		file m_save_file_char1_pso;
		file m_save_file_char2_pso;
	};

	inline stats_service* g_stats_service;
}