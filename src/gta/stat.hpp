#pragma once

enum class eStatType
{
	INT             = 1,
	FLOAT           = 2,
	STRING          = 3,
	BOOL_           = 4,
	UINT8           = 5,
	UINT16          = 6,
	UINT32          = 7,
	UINT64          = 8,
	INT64           = 26,
	DATE            = 20,
	POS             = 21,
	TEXTLABEL       = 22,
	PACKED          = 23,
	USERID          = 24,
	PROFILE_SETTING = 25,
};

class sStatData
{
public:
	virtual void destructor()                    = 0;
	virtual void _1()                            = 0;
	virtual void SetIntData(int data)            = 0;
	virtual void SetInt64Data(int64_t data)      = 0;
	virtual void SetFloatData(float data)        = 0;
	virtual void SetBoolData(bool data)          = 0;
	virtual void SetUint8Data(uint8_t data)      = 0;
	virtual void SetUint16Data(uint16_t data)    = 0;
	virtual void SetUint32Data(uint32_t data)    = 0;
	virtual void SetUint64Data(uint64_t data)    = 0;
	virtual void SetStringData(const char* data) = 0;
	virtual void _11()                           = 0;
	virtual int GetIntData()                     = 0;
	virtual int64_t GetInt64Data()               = 0;
	virtual float GetFloatData()                 = 0;
	virtual bool GetBoolData()                   = 0;
	virtual uint8_t GetUint8Data()               = 0;
	virtual uint16_t GetUint16Data()             = 0;
	virtual uint32_t GetUint32Data()             = 0;
	virtual uint64_t GetUint64Data()             = 0;
	virtual const char* GetStringData()          = 0;
	virtual void _21()                           = 0;
	virtual void _22()                           = 0;
	virtual void _23()                           = 0;
	virtual eStatType GetTypeId()                = 0;
	virtual const char* GetTypeString()          = 0;
	virtual void _26()                           = 0;
	virtual bool IsObfuscated()                  = 0;
	virtual void _28()                           = 0;
	virtual void _29()                           = 0;
	virtual bool IsZero()                        = 0;

	uint8_t GetCharIndex()
	{
		uint8_t index = m_flags >> 0x14 & 7;
		return index == 6 || index == 7 ? 0 : index;
	}

	uint64_t m_flags;
};

// Not a r* class
template<typename T>
class sSubStatData : public sStatData
{
public:
	T m_data;
};

struct sStatArrayEntry
{
	Hash m_hash;
	sStatData* m_stat;
};


class CSavegameQueuedOperation_MPStats_Load
{
public:
	virtual void _00();
	virtual bool process_load(); // hooks::mp_save_download
	virtual bool _02();
	virtual bool return_false();

public:
	uint32_t m_result;
	char m_pad1[4];
	uint32_t m_char_slot;
	uint32_t m_download_state;
	uint32_t m_unk1;
	char m_pad2[4];
	uint32_t m_download_status; // Probably a sub class
	int32_t m_http_error;
	char m_pad3[8];
	char* m_save_buffer; // Also a sub class
	char m_pad4[4];
	uint32_t m_save_buffer_size;
	char m_pad5[672];
	bool m_download_successful;
	bool m_unk_failed;
};
