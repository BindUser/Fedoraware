#pragma once

#include "../Feature.h"

class CLogs
{
    void OutputInfo(int flags, std::string name, std::string string, std::string cstring);
    void TagsOnJoin(std::string name, uint32_t friendsID);
	bool bTagsOnJoin = false;

public:
    void TagsChanged(std::string name, std::string action, std::string color, std::string tag);
};

ADD_FEATURE(CLogs, Logs)