#include "stats_service.hpp"

#include "file_manager.hpp"
#include "gta/joaat.hpp"
#include "gta/stat.hpp"
#include "pointers.hpp"

#include <cstddef>

namespace big
{

	template<class Fnc>
	void script_save_json_metadata::visit(const Fnc& fnc)
	{
		do_visit(stats_service::script_json::json_pointer{}, fnc);
	}

	template<class Ptr, class Fnc>
	void script_save_json_metadata::do_visit(const Ptr& ptr, const Fnc& fnc)
	{
		using value_t                 = nlohmann::detail::value_t;
		stats_service::script_json& j = *static_cast<stats_service::script_json*>(this);
		switch (j.type())
		{
		case value_t::object:
			fnc(ptr, j);
			for (const auto& entry : j.items())
			{
				entry.value().do_visit(ptr / entry.key(), fnc);
			}
			break;
		case value_t::array:
			fnc(ptr, j);
			for (std::size_t i = 0; i < j.size(); ++i)
			{
				j.at(i).do_visit(ptr / std::to_string(i), fnc);
			}
			break;
		case value_t::null:
		case value_t::string:
		case value_t::boolean:
		case value_t::number_integer:
		case value_t::number_unsigned:
		case value_t::number_float:
		case value_t::binary: fnc(ptr, j); break;
		case value_t::discarded:
		default: break;
		}
	}

	stats_service::stats_service()
	{
		g_stats_service     = this;
		m_save_file_default = g_file_manager.get_project_file("./save_default0000.json");
		m_save_file_char1   = g_file_manager.get_project_file("./save_char0001.json");
		m_save_file_char2   = g_file_manager.get_project_file("./save_char0002.json");
		m_save_file_script  = g_file_manager.get_project_file("./save_script.json");
		m_save_overwrite    = g_file_manager.get_project_file("./save_overwrite.json");
		if (g.load_fsl_files)
		{
			m_save_file_default_pso = g_file_manager.get_project_file("./save_default0000.pso");
			m_save_file_char1_pso   = g_file_manager.get_project_file("./save_char0001.pso");
			m_save_file_char2_pso   = g_file_manager.get_project_file("./save_char0002.pso");
		}
	}
	stats_service::~stats_service()
	{
		g_stats_service = nullptr;
	}

	void stats_service::register_stat(const char* name)
	{
		if (g.use_human_readable_stat_names)
		{
			m_stat_hash_to_string[rage::joaat(name)] = name;
		}
	}

	sStatData* stats_service::get_stat_by_hash(Hash stat_to_find)
	{
		const auto& stats = *g_pointers->m_stats;
		for (const auto& stat : stats)
		{
			if (stat.m_hash == stat_to_find)
			{
				return stat.m_stat;
			}
		}
		return nullptr;
	}

	size_t stats_service::get_pso_file_size(uint8_t char_index)
	{
		std::ifstream pso_file;
		if (char_index == 0)
		{
			if (!m_save_file_default_pso.exists())
				return -1;
			pso_file.open(m_save_file_default_pso.get_path(), std::ios::binary);
		}
		else if (char_index == 1)
		{
			if (!m_save_file_char1_pso.exists())
				return -1;
			pso_file.open(m_save_file_char1_pso.get_path(), std::ios::binary);
		}
		else if (char_index == 2)
		{
			if (!m_save_file_char2_pso.exists())
				return -1;
			pso_file.open(m_save_file_char2_pso.get_path(), std::ios::binary);
		}
		pso_file.seekg(0, std::ios::end);
		auto size = pso_file.tellg();

		return size;
	}
	void stats_service::read_pso_file(uint8_t char_index, char* buf, size_t size)
	{
		std::ifstream pso_file;
		if (char_index == 0)
		{
			if (!m_save_file_default_pso.exists())
				return;
			pso_file.open(m_save_file_default_pso.get_path(), std::ios::binary);
		}
		else if (char_index == 1)
		{
			if (!m_save_file_char1_pso.exists())
				return;
			pso_file.open(m_save_file_char1_pso.get_path(), std::ios::binary);
		}
		else if (char_index == 2)
		{
			if (!m_save_file_char2_pso.exists())
				return;
			pso_file.open(m_save_file_char2_pso.get_path(), std::ios::binary);
		}

		pso_file.read(buf, size);
	}

	inline uint8_t get_char_index_from_stat(sStatData* stat)
	{
		uint8_t index = stat->m_flags >> 0x14 & 7;
		return index == 6 || index == 7 ? 0 : index;
	}

	template<typename T>
	void stats_service::save_stat_map_to_json(nlohmann::json& json, T& map, bool use_stat_names, uint8_t char_index)
	{
		auto stats = nlohmann::json::object();
		for (auto stat : map)
		{
			if (get_char_index_from_stat(get_stat_by_hash(stat.first)) != char_index)
				continue;

			if (use_stat_names)
			{
				stats[m_stat_hash_to_string[stat.first]] = stat.second;
			}
			else
			{
				stats[std::to_string(stat.first)] = stat.second;
			}
		}
		json = stats;
	}

	template<typename T>
	void stats_service::load_stat_map_from_json(const nlohmann::json& json, T& map, bool use_stat_names)
	{
		if (json.is_array())
		{
			load_stat_map_from_json_legacy(json, map, use_stat_names);
		}
		else
		{
			load_stat_map_from_json_modern(json, map, use_stat_names);
		}
	}
	template<typename T>
	void stats_service::load_stat_map_from_json_modern(const nlohmann::json& json, T& map, bool use_stat_names)
	{
		if (use_stat_names)
		{
			for (const auto& stat : json.items())
			{
				map[rage::joaat(stat.key())] = stat.value();
			}
		}
		else
		{
			for (const auto& stat : json.items())
			{
				map[std::stoul(stat.key())] = stat.value();
			}
		}
	}
	template<typename T>
	void stats_service::load_stat_map_from_json_legacy(const nlohmann::json& json, T& map, bool use_stat_names)
	{
		if (use_stat_names)
		{
			for (const auto& stat : json)
			{
				map[rage::joaat(stat[0])] = stat[1];
			}
		}
		else
		{
			for (const auto& stat : json)
			{
				map[stat[0]] = stat[1];
			}
		}
	}

	void stats_service::update_script_data_json(script_json& json)
	{
		for (auto& data : json)
		{
			if (data.save_var.data_ptr != 0)
			{
				data = data.save_var;
			}
			else
			{
				update_script_data_json(data);
			}
		}
	}

	static void update_script_var_from_json(script_save_var& save_var, const stats_service::script_json& json)
	{
		switch (save_var.type)
		{
		case eScriptSaveType::INT:
		{
			*reinterpret_cast<int*>(save_var.data_ptr) = json.template get<int>();
			break;
		}
		case eScriptSaveType::INT64:
		{
			*reinterpret_cast<int64_t*>(save_var.data_ptr) = json.template get<int64_t>();
			break;
		}
		case eScriptSaveType::ENUM:
		{
			*reinterpret_cast<int32_t*>(save_var.data_ptr) = json.template get<int32_t>();
			break;
		}
		case eScriptSaveType::FLOAT:
		{
			*reinterpret_cast<float*>(save_var.data_ptr) = json.template get<float>();
			break;
		}
		case eScriptSaveType::BOOL_:
		{
			*reinterpret_cast<BOOL*>(save_var.data_ptr) = json.template get<BOOL>();
			break;
		}
		case eScriptSaveType::TEXT_LABEL_15_:
		{
			strncpy(reinterpret_cast<char*>(save_var.data_ptr), json.template get<std::string>().c_str(), 15);
			break;
		}
		case eScriptSaveType::TEXT_LABEL_23_:
		{
			strncpy(reinterpret_cast<char*>(save_var.data_ptr), json.template get<std::string>().c_str(), 23);
			break;
		}
		case eScriptSaveType::TEXT_LABEL_31_:
		{
			strncpy(reinterpret_cast<char*>(save_var.data_ptr), json.template get<std::string>().c_str(), 31);
			break;
		}
		case eScriptSaveType::TEXT_LABEL_:
		case eScriptSaveType::TEXT_LABEL_63_:
		{
			strncpy(reinterpret_cast<char*>(save_var.data_ptr), json.template get<std::string>().c_str(), 63);
			break;
		}
		}
	}

	void stats_service::load_script_data_from_json(const nlohmann::json& json)
	{
		m_script_save_data.visit([&](const script_json::json_pointer& p, script_json& j) {
			const auto& var = json[p];
			if (!var.is_null() && j.is_primitive() && j.save_var.data_ptr != 0)
			{
				update_script_var_from_json(j.save_var, var);
			}
		});
	}

	void stats_service::save_script_data_to_json()
	{
		try
		{
			update_script_data_json(m_script_save_data);
		}
		catch (const std::exception& ex)
		{
			LOG(FATAL) << "Script data failed to save: " << ex.what();
			return;
		}

		std::ofstream script_save(m_save_file_script.get_path(), std::ios::out | std::ios::trunc);

		script_save << m_script_save_data.dump(1, '	');
		script_save.close();
	}

	void stats_service::save_internal_stats_to_json(uint8_t char_index)
	{
		nlohmann::json json;

		bool string_names                      = g.use_human_readable_stat_names;
		json["uses_human_readable_stat_names"] = string_names;

		try
		{
			save_stat_map_to_json(json["INT"], m_int_stats, string_names, char_index);
			save_stat_map_to_json(json["FLOAT"], m_float_stats, string_names, char_index);
			save_stat_map_to_json(json["STRING"], m_string_stats, string_names, char_index);
			save_stat_map_to_json(json["BOOL"], m_bool_stats, string_names, char_index);
			save_stat_map_to_json(json["UINT8"], m_uint8_stats, string_names, char_index);
			save_stat_map_to_json(json["UINT16"], m_uint16_stats, string_names, char_index);
			save_stat_map_to_json(json["UINT32"], m_uint32_stats, string_names, char_index);
			save_stat_map_to_json(json["UINT64"], m_uint64_stats, string_names, char_index);
			save_stat_map_to_json(json["INT64"], m_int64_stats, string_names, char_index);
			save_stat_map_to_json(json["DATE"], m_date_stats, string_names, char_index);
			save_stat_map_to_json(json["POS"], m_pos_stats, string_names, char_index);
			save_stat_map_to_json(json["TEXTLABEL"], m_textlabel_stats, string_names, char_index);
			save_stat_map_to_json(json["PACKED"], m_packed_stats, string_names, char_index);
			save_stat_map_to_json(json["USERID"], m_userid_stats, string_names, char_index);
		}
		catch (const std::exception& ex)
		{
			LOG(FATAL) << "Stats failed to save: " << ex.what();
			return;
		}
		std::ofstream file;
		if (char_index == 0)
		{
			file.open(m_save_file_default.get_path(), std::ios::out | std::ios::trunc);
		}
		else if (char_index == 1)
		{
			file.open(m_save_file_char1.get_path(), std::ios::out | std::ios::trunc);
		}
		else if (char_index == 2)
		{
			file.open(m_save_file_char2.get_path(), std::ios::out | std::ios::trunc);
		}

		file << json.dump(1, '	');
		file.close();
	}

	bool stats_service::load_internal_stats_from_json(uint8_t char_index)
	{
		std::ifstream file;
		if (char_index == 0)
		{
			if (!m_save_file_default.exists())
				return false;

			LOG(VERBOSE) << "Loading save_default0000.json";
			file.open(m_save_file_default.get_path());
		}
		else if (char_index == 1)
		{
			if (!m_save_file_char1.exists())
				return false;

			LOG(VERBOSE) << "Loading save_char0001.json";
			file.open(m_save_file_char1.get_path());
		}
		else if (char_index == 2)
		{
			if (!m_save_file_char2.exists())
				return false;

			LOG(VERBOSE) << "Loading save_char0002.json";
			file.open(m_save_file_char2.get_path());
		}
		else if (char_index == SAVE_OVERWRITE_INDEX)
		{
			if (!m_save_overwrite.exists())
				return false;

			LOG(VERBOSE) << "Loading save_overwrite.json";
			file.open(m_save_overwrite.get_path());
		}

		try
		{
			// Ignore comments for save_overwrite.json
			const nlohmann::json& json          = nlohmann::json::parse(file, nullptr, true, /*ignore_comments*/ true);
			bool uses_human_readable_stat_names = json["uses_human_readable_stat_names"];
			load_stat_map_from_json(json["INT"], m_int_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["FLOAT"], m_float_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["STRING"], m_string_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["BOOL"], m_bool_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["UINT8"], m_uint8_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["UINT16"], m_uint16_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["UINT32"], m_uint32_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["UINT64"], m_uint64_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["INT64"], m_int64_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["DATE"], m_date_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["POS"], m_pos_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["TEXTLABEL"], m_textlabel_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["PACKED"], m_packed_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["USERID"], m_userid_stats, uses_human_readable_stat_names);

			return true;
		}
		catch (const std::exception& ex)
		{
			LOG(WARNING) << "Detected corrupt save file: " << ex.what();
		}

		return false;
	}

	bool stats_service::load_internal_script_data_from_json()
	{
		if (!m_save_file_script.exists())
		{
			return false;
		}

		try
		{
			std::ifstream file(m_save_file_script.get_path());
			LOG(VERBOSE) << "Loading save_script.json";

			const nlohmann::json& json = nlohmann::json::parse(file);
			load_script_data_from_json(json);

			return true;
		}
		catch (const std::exception& ex)
		{
			LOG(WARNING) << "Detected corrupt script save file: " << ex.what();
		}

		return false;
	}

	void stats_service::save_stats()
	{
		const auto& stats = *g_pointers->m_stats;
		for (const auto& stat : stats)
		{
			if (!g.save_unmodified_stats && stat.m_stat->IsZero())
				continue;

			// Skip stats not marked "online"
			if (!(stat.m_stat->m_flags & (1 << 3)))
				continue;

			switch (stat.m_stat->GetTypeId())
			{
			case eStatType::INT:
			{
				m_int_stats[stat.m_hash] = stat.m_stat->GetIntData();
				break;
			}
			case eStatType::FLOAT:
			{
				m_float_stats[stat.m_hash] = stat.m_stat->GetFloatData();
				break;
			}
			case eStatType::STRING:
			{
				m_string_stats[stat.m_hash] = std::string(stat.m_stat->GetStringData());
				break;
			}
			case eStatType::BOOL_:
			{
				m_bool_stats[stat.m_hash] = stat.m_stat->GetBoolData();
				break;
			}
			case eStatType::UINT8:
			{
				m_uint8_stats[stat.m_hash] = stat.m_stat->GetUint8Data();
				break;
			}
			case eStatType::UINT16:
			{
				m_uint16_stats[stat.m_hash] = stat.m_stat->GetUint16Data();
				break;
			}
			case eStatType::UINT32:
			{
				m_uint32_stats[stat.m_hash] = stat.m_stat->GetUint32Data();
				break;
			}
			case eStatType::UINT64:
			{
				m_uint64_stats[stat.m_hash] = stat.m_stat->GetUint64Data();
				break;
			}
			case eStatType::INT64:
			{
				m_int64_stats[stat.m_hash] = stat.m_stat->GetInt64Data();
				break;
			}
			case eStatType::DATE:
			{
				m_date_stats[stat.m_hash] = stat.m_stat->GetUint64Data();
				break;
			}
			case eStatType::POS:
			{
				m_pos_stats[stat.m_hash] = stat.m_stat->GetUint64Data();
				break;
			}
			case eStatType::TEXTLABEL:
			{
				m_textlabel_stats[stat.m_hash] = stat.m_stat->GetIntData();
				break;
			}
			case eStatType::PACKED:
			{
				m_packed_stats[stat.m_hash] = stat.m_stat->GetUint64Data();
				break;
			}
			case eStatType::USERID:
			{
				m_userid_stats[stat.m_hash] = stat.m_stat->GetUint64Data();
				break;
			}
			case eStatType::PROFILE_SETTING: break;
			default:
			{
				LOG(WARNING) << "Unknown stat type: " << (int)stat.m_stat->GetTypeId() << "In stat: " << stat.m_hash;
				break;
			}
			}
		}

		int last_character = g_stats_service->get_stat_by_hash(RAGE_JOAAT("MPPLY_LAST_MP_CHAR"))->GetIntData();
		save_internal_stats_to_json(0);
		save_internal_stats_to_json(last_character + 1);

		save_script_data_to_json();
	}

	bool stats_service::load_stats()
	{
		if (!load_internal_stats_from_json(0))
		{
			return false;
		}

		// Load character stats if they exist.
		load_internal_stats_from_json(1);
		load_internal_stats_from_json(2);

		// Load stat overrides last.
		load_internal_stats_from_json(SAVE_OVERWRITE_INDEX);

		load_internal_script_data_from_json();

		const auto& stats = *g_pointers->m_stats;
		for (const auto& stat : stats)
		{
			switch (stat.m_stat->GetTypeId())
			{
			case eStatType::INT:
			{
				if (m_int_stats.contains(stat.m_hash))
					stat.m_stat->SetIntData(m_int_stats[stat.m_hash]);
				break;
			}
			case eStatType::FLOAT:
			{
				if (m_float_stats.contains(stat.m_hash))
					stat.m_stat->SetFloatData(m_float_stats[stat.m_hash]);
				break;
			}
			case eStatType::STRING:
			{
				if (m_string_stats.contains(stat.m_hash))
					strncpy(((sSubStatData<char[32]>*)stat.m_stat)->m_data, m_string_stats[stat.m_hash].data(), 32);
				break;
			}
			case eStatType::BOOL_:
			{
				if (m_bool_stats.contains(stat.m_hash))
					stat.m_stat->SetBoolData(m_bool_stats[stat.m_hash]);
				break;
			}
			case eStatType::UINT8:
			{
				if (m_uint8_stats.contains(stat.m_hash))
					stat.m_stat->SetUint8Data(m_uint8_stats[stat.m_hash]);
				break;
			}
			case eStatType::UINT16:
			{
				if (m_uint16_stats.contains(stat.m_hash))
					stat.m_stat->SetUint16Data(m_uint16_stats[stat.m_hash]);
				break;
			}
			case eStatType::UINT32:
			{
				if (m_uint32_stats.contains(stat.m_hash))
					stat.m_stat->SetUint32Data(m_uint32_stats[stat.m_hash]);
				break;
			}
			case eStatType::UINT64:
			{
				if (m_uint64_stats.contains(stat.m_hash))
					stat.m_stat->SetUint64Data(m_uint64_stats[stat.m_hash]);
				break;
			}
			case eStatType::INT64:
			{
				if (m_int64_stats.contains(stat.m_hash))
					stat.m_stat->SetInt64Data(m_int64_stats[stat.m_hash]);
				break;
			}
			case eStatType::DATE:
			{
				if (m_date_stats.contains(stat.m_hash))
					stat.m_stat->SetUint64Data(m_date_stats[stat.m_hash]);
				break;
			}
			case eStatType::POS:
			{
				if (m_pos_stats.contains(stat.m_hash))
					stat.m_stat->SetUint64Data(m_pos_stats[stat.m_hash]);
				break;
			}
			case eStatType::TEXTLABEL:
			{
				if (m_textlabel_stats.contains(stat.m_hash))
					stat.m_stat->SetIntData(m_textlabel_stats[stat.m_hash]);
				break;
			}
			case eStatType::PACKED:
			{
				if (m_packed_stats.contains(stat.m_hash))
					stat.m_stat->SetUint64Data(m_packed_stats[stat.m_hash]);
				break;
			}
			case eStatType::USERID:
			{
				if (m_userid_stats.contains(stat.m_hash))
					stat.m_stat->SetUint64Data(m_userid_stats[stat.m_hash]);
				break;
			}
			case eStatType::PROFILE_SETTING: break;
			default:
			{
				LOGF(WARNING, "Unknown stat type: {} In stat: {}", (int)stat.m_stat->GetTypeId(), stat.m_hash);
				break;
			}
			}

			//  Hack to fix the character creator always activating.
			if (stat.m_hash == RAGE_JOAAT("MP0_CHAR_IS_NGPC_VERSION") || stat.m_hash == RAGE_JOAAT("MP1_CHAR_IS_NGPC_VERSION"))
			{
				stat.m_stat->SetIntData(1);
			}
		}
		return true;
	}
}