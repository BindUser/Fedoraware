#pragma once
#include "../Feature.h"

class CChatInfo
{
	void OutputInfo(int flags, std::string name, std::string string, std::string cstring);
	
public:
	void Event(CGameEvent* pEvent, FNV1A_t uNameHash);
	void UserMessage(UserMessageType type, bf_read& msgData);
	void TagsChanged(std::string name, std::string action, std::string color, std::string tag);

	std::unordered_map<int, bool> KnownBots;
};

ADD_FEATURE(CChatInfo, ChatInfo)