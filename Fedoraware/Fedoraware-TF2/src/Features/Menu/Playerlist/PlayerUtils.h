#pragma once
#include "../../Feature.h"

#include <mutex>

struct ListPlayer
{
	const char* Name{};
	uint32_t FriendsID{};
	int UserID{};
	int Team{};
	int Class{};
	bool Alive{};
	bool Local{};
	bool Friend{};
	bool Fake{};
};

struct PriorityLabel
{
	Color_t Color = { 255, 255, 255, 255 };
	int Priority = 0;

	bool Label = false;
	bool Assignable = true;
	bool Locked = false; // don't allow it to be removed
};

class CPlayerlistUtils
{
public:
	bool GetTag(std::string sTag, PriorityLabel* plTag);

	void AddTag(uint32_t friendsID, std::string sTag, bool bSave = true, std::string sName = "");
	void AddTag(int iIndex, std::string sTag, bool bSave = true, std::string sName = "");
	void RemoveTag(uint32_t friendsID, std::string sTag, bool bSave = true, std::string sName = "");
	void RemoveTag(int iIndex, std::string sTag, bool bSave = true, std::string sName = "");
	bool HasTags(uint32_t friendsID);
	bool HasTags(int iIndex);
	bool HasTag(uint32_t friendsID, std::string sTag);
	bool HasTag(int iIndex, std::string sTag);

	int GetPriority(uint32_t friendsID);
	int GetPriority(int iIndex);
	bool GetSignificantTag(uint32_t friendsID, std::string* sTag, PriorityLabel* plTag, int iMode = 1); // iMode: 0 - Priorities & Labels, 1 - Priorities, 2 - Labels
	bool GetSignificantTag(int iIndex, std::string* sTag, PriorityLabel* plTag, int iMode = 1); // iMode: 0 - Priorities & Labels, 1 - Priorities, 2 - Labels
	bool IsIgnored(uint32_t friendsID);
	bool IsIgnored(int iIndex);
	bool IsFriend(int iIndex);

	std::mutex mutex;

	std::unordered_map<std::string, PriorityLabel> vTags = {
		{ "Default", { { 200, 200, 200, 255 }, 0, false, false, true } },
		{ "Ignored", { { 200, 200, 200, 255 }, -1, false, true, true } },
		{ "Cheater", { { 255, 100, 100, 255 }, 1, false, true, true } },
		{ "Friend", { { 100, 255, 100, 255 }, 0, true, false, true } }
	};

	std::vector<ListPlayer> vPlayerCache = {};
	std::unordered_map<uint32_t, ListPlayer> mPriorityCache = {};

	bool bLoadPlayers = true;
	bool bSavePlayers = false;
	bool bLoadTags = true;
	bool bSaveTags = false;
};

ADD_FEATURE(CPlayerlistUtils, PlayerUtils)