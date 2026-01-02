#include "hooking/hooking.hpp"

enum class eRosPrivilege : uint32_t
{
	CREATE_TICKET = 1,
	MULTIPLAYER = 3,
	LEADERBOARD_WRITE = 4,
	BANNED = 7,
	PRIVATE_MSG = 9,
	SOCIAL_CLUB = 10,
	CHEATER = 20,
	PLAYED_LAST_GEN = 24,
	SPECIAL_EDITION_CONTENT = 25,
	MP_TEXT_COMMUNICATION = 29,
	MP_VOICE_COMMUNICATION = 30,
	REPORTING = 31,
};

namespace big
{
	bool hooks::has_privilege(void* p1, uint32_t privilege)
	{
		switch ((eRosPrivilege)privilege)
		{
		case eRosPrivilege::CREATE_TICKET:
		case eRosPrivilege::MULTIPLAYER:
		case eRosPrivilege::PRIVATE_MSG:
		case eRosPrivilege::SOCIAL_CLUB:
		case eRosPrivilege::MP_TEXT_COMMUNICATION:
		case eRosPrivilege::MP_VOICE_COMMUNICATION:
		case eRosPrivilege::REPORTING:
			return true;

		case eRosPrivilege::BANNED:
		case eRosPrivilege::CHEATER:
			return false;

		default: break;
		};

		return g_hooking->get_original<hooks::has_privilege>()(p1, privilege);
	}
}
