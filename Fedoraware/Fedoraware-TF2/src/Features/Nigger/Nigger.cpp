#include "Nigger.h"
#include "../Menu/Playerlist/PlayerUtils.h"

static std::string white({ '\x7', 'F', 'F', 'F', 'F', 'F', 'F' }); //FFFFFF
static std::string red({ '\x7', 'F', 'F', '3', 'A', '3', 'A' }); //FF3A3A
static std::string green({ '\x7', '3', 'A', 'F', 'F', '4', 'D' }); //3AFF4D
static std::string blue({ '\x7', '0', 'D', '9', '2', 'F', 'F' }); //0D92FF
static std::string yellow({ '\x7', 'C', '8', 'A', '9', '0', '0' }); //C8A900
static std::string gsred({ '\x7', '7', '5', '7', '5', '7', '5' }); //757575
static std::string gsgreen({ '\x7', 'B', '0', 'B', '0', 'B', '0' }); //b0b0b0
static std::string gsblue({ '\x7', '7', '6', '7', '6', '7', '6' }); //767676
static std::string gsyellow({ '\x7', 'C', 'A', 'C', 'A', 'C', 'A' }); //CACACA

// Tags
void CLogs::TagsOnJoin(std::string name, uint32_t friendsID)
{
	const auto& vTags = G::PlayerTags[friendsID];
	std::vector<std::pair<std::string, std::string>> vColorsTags = {};
	for (auto& sTag : vTags)
	{
		PriorityLabel lbTag;
		if (!F::PlayerUtils.GetTag(sTag, &lbTag))
			continue;
		vColorsTags.push_back({ lbTag.Color.to_hex_alpha(), sTag });
	}

	std::string tagtext, ctagtext;
	switch (vColorsTags.size())
	{
	case 0: return;
	case 1:
	{
		auto& pColorTag = *vColorsTags.begin();
		tagtext = pColorTag.second;
		ctagtext = std::format("{}{}", pColorTag.first, pColorTag.second);
		break;
	}
	case 2:
	{
		auto& pColorTag1 = *vColorsTags.begin(), &pColorTag2 = *(vColorsTags.begin() + 1);
		tagtext = std::format("{} and ", pColorTag1.second, pColorTag2.second);
		ctagtext = std::format("{}{}\x1 and {}{}", pColorTag1.first, pColorTag1.second, pColorTag2.first, pColorTag2.second);
		break;
	}
	default:
	{
		for (auto it = vColorsTags.begin(); it != vColorsTags.end(); it++)
		{
			auto& pColorTag = *it;
			if (it + 1 != vColorsTags.end())
			{
				tagtext += std::format("{}, ", pColorTag.second);
				ctagtext += std::format("{}{}\x1, ", pColorTag.first, pColorTag.second);
			}
			else
			{
				tagtext += std::format("and {}", pColorTag.second);
				ctagtext += std::format("and {}{}", pColorTag.first, pColorTag.second);
			}
		}
	}
	}

	std::string string = std::format("{} has the {} {}", (name), (vColorsTags.size() == 1 ? "tag" : "tags"), (tagtext));
	std::string cstring = std::format("{}{}\x1 has the {} {}", (yellow), (name), (vColorsTags.size() == 1 ? "tag" : "tags"), (ctagtext));
	OutputInfo(Vars::Logging::Tags::LogTo.Value, "Tags", string, cstring);
}

void CLogs::TagsChanged(std::string name, std::string action, std::string color, std::string tag)
{
	if (!(Vars::Logging::Logs.Value & 1 << 5))
		return;

	std::string string = std::format("{} tag {} {} {}", (action), (tag), (action == "Added" ? "to" : "from"), (name));
	std::string cstring = std::format("{} tag {}{}\x1 {} {}{}", (action), (color), (tag), (action == "Added" ? "to" : "from"), (yellow), (name));
	OutputInfo(Vars::Logging::Tags::LogTo.Value, "Tags", string, cstring);
}