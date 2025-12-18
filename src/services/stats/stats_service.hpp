#pragma once

#include "gta/stat.hpp"
namespace big
{
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
		
	private:
		const uint8_t SAVE_OVERWRITE_INDEX = 111;
		void save_internal_stats_to_json(uint8_t char_index = 0);
		bool load_internal_stats_from_json(uint8_t char_index = 0);

		template <typename T>
		void save_stat_map_to_json(nlohmann::json& json, T& map, bool use_stat_names, uint8_t char_index);
		template <typename T>
		void load_stat_map_from_json(const nlohmann::json& json, T& map, bool use_stat_names);

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

		file m_save_file_default;
		file m_save_file_char1;
		file m_save_file_char2;
		file m_save_overwrite;
		file m_save_file_default_pso;
		file m_save_file_char1_pso;
		file m_save_file_char2_pso;
	};

	inline stats_service* g_stats_service;
}