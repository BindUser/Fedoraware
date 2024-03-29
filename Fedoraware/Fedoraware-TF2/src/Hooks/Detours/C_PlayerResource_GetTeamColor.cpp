#include "../../Hooks/Hooks.h"
#include "../../Features/Colour.h"
#include "../../Features/Menu/Playerlist/PlayerUtils.h"

// Credits: https://github.com/rei-kes

namespace S
{
    MAKE_SIGNATURE(CTFPlayer_Resource_Call, CLIENT_DLL, "33 C9 89 45 ? 89 4D ? 85 C0 75", 0x0);
}

int iCurPlayer;
unsigned char _color[4];

__inline Color_t GetScoreboardColor(int iIndex, bool enableOtherColors)
{
    Color_t out = { 0, 0, 0, 0 };

    PlayerInfo_t pi{}; bool bTagColor = false; Color_t plTagColor;
    if (I::EngineClient->GetPlayerInfo(iIndex, &pi))
    {
        std::string _; PriorityLabel plTag;
        if (bTagColor = F::PlayerUtils.GetSignificantTag(pi.friendsID, &_, &plTag))
            plTagColor = plTag.Color;
    }

    if (iIndex == I::EngineClient->GetLocalPlayer())
        out = Vars::Colours::Local.Value;
    else if (g_EntityCache.IsFriend(iIndex))
        out = F::PlayerUtils.vTags["Friend"].Color;
    else if (bTagColor)
        out = plTagColor;

    return out;
}

MAKE_HOOK(C_TFPlayer_Resource_GetPlayerConnectionState, S::CTFPlayer_Resource_GetPlayerConnectionState(), MM_PlayerConnectionState_t, __fastcall,
    void* ecx, void* edx, int iIndex)
{
    static auto dwCall = S::CTFPlayer_Resource_Call();
    const auto dwRetAddr = reinterpret_cast<DWORD>(_ReturnAddress());

    const auto result = Hook.Original<FN>()(ecx, edx, iIndex);

    if (result != MM_WAITING_FOR_PLAYER)
    {
        if (dwRetAddr != dwCall)
            return result;

        iCurPlayer = iIndex;
    }
    else
        iCurPlayer = 0;

    return result;
}

MAKE_HOOK(C_PlayerResource_GetTeamColor, S::CPlayerResource_GetTeamColor(), unsigned char*, __fastcall,
    void* ecx, void* edx, int index)
{
    if (index < 0 || index > I::EngineClient->GetMaxClients() || !iCurPlayer || !Vars::Visuals::ScoreboardColors.Value)
    {
    const Color_t cReturn = GetScoreboardColor(iCurPlayer,Vars::ESP::Main::EnableTeamEnemyColors.Value);
    if (!cReturn.a)
        return Hook.Original<FN>()(ecx, edx, index);

    _color[0] = cReturn.r; _color[1] = cReturn.g; _color[2] = cReturn.b; _color[3] = 255; // force alpha
    return _color;
    } // <---reikes doesnt have curly brace in his if statement but doesnt have a error like i do ?
}