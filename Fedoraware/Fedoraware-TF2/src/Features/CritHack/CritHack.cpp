#include "CritHack.h"
#include "../TickHandler/TickHandler.h"

#define TF_DAMAGE_CRIT_MULTIPLIER		3.0f
#define TF_DAMAGE_CRIT_CHANCE			0.02f
#define TF_DAMAGE_CRIT_CHANCE_RAPID		0.02f
#define TF_DAMAGE_CRIT_DURATION_RAPID	2.0f
#define TF_DAMAGE_CRIT_CHANCE_MELEE		0.15f

bool CCritHack::IsEnabled()
{
	if (!Vars::CritHack::Active.Value)
		return false;
	if (!AreRandomCritsEnabled())
		return false;
	if (!I::EngineClient->IsInGame())
		return false;

	return true;
}

bool CCritHack::AreRandomCritsEnabled()
{
	if (static auto tf_weapon_criticals = g_ConVars.FindVar("tf_weapon_criticals"); tf_weapon_criticals)
		return tf_weapon_criticals->GetBool();
	return true;
}



void CCritHack::Fill(CBaseCombatWeapon* pWeapon, const CUserCmd* pCmd, const bool bAttacking, int n)
{
	static int prev_weapon = 0;
	static int previousCrit = 0;
	static int starting_num = pCmd->command_number;

	if (bAttacking && G::WeaponCanAttack/* || pCmd->buttons & IN_ATTACK*/)
		return;

	if (prev_weapon != pWeapon->GetIndex())
	{
		starting_num = pCmd->command_number;
		prev_weapon = pWeapon->GetIndex();
		ForceCmds.clear();
		SkipCmds.clear();
	}

	//ProtectData = true;
	//const int seed_backup = MD5_PseudoRandom(pCmd->command_number) & 0x7FFFFFFF;
	for (int i = 0; i < n; i++)
	{
		if (ForceCmds.size() >= 15)
			break;

		const int cmd_num = starting_num + i;

		if (IsCritCommand(cmd_num))
			ForceCmds.push_back(cmd_num);
	}
	for (int i = 0; i < n; i++)
	{
		if (SkipCmds.size() >= 15)
			break;

		const int cmd_num = starting_num + i;

		if (IsCritCommand(cmd_num, false))
			SkipCmds.push_back(cmd_num);
	}
	starting_num += n;
	//ProtectData = false;
}

int CCritHack::LastGoodCritTick(const CUserCmd* pCmd)
{
	int retVal = -1;
	bool popBack = false;

	for (const auto& cmd : ForceCmds)
	{
		if (cmd >= pCmd->command_number)
			retVal = cmd;
		else
			popBack = true;
	}

	if (popBack)
		ForceCmds.pop_front();

	return retVal;
}

int CCritHack::LastGoodSkipTick(const CUserCmd* pCmd)
{
	int retVal = -1;
	bool popBack = false;

	for (const auto& cmd : SkipCmds)
	{
		if (cmd >= pCmd->command_number)
			retVal = cmd;
		else
			popBack = true;
	}

	if (popBack)
		SkipCmds.pop_front();

	return retVal;
}



bool CCritHack::IsCritCommand(const i32 command_number, const bool crit)
{
	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pWeapon)
		return false;

	const auto random_seed = MD5_PseudoRandom(command_number) & 0x7FFFFFFF;
	Utils::RandomSeed(DecryptOrEncryptSeed(pWeapon, random_seed));
	const auto random_int = Utils::RandomInt(0, 9999);

	return crit ? random_int < 100 : random_int > 6000;
}

u32 CCritHack::DecryptOrEncryptSeed(CBaseCombatWeapon* pWeapon, u32 seed)
{
	if (!pWeapon)
		return 0;

	unsigned int iMask = pWeapon->GetIndex() << 8 | I::EngineClient->GetLocalPlayer();

	if (pWeapon->GetSlot() == SLOT_MELEE)
		iMask <<= 8;

	return iMask ^ seed;
}



void CCritHack::GetTotalCrits(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon) // this is all pretty garbo
{
	if (!pLocal || !pWeapon || pLocal->deadflag())
		return;

	if (!Vars::CritHack::Active.Value)
		return;

	const auto slot = pWeapon->GetSlot();
	auto tfWeaponInfo = pWeapon->GetTFWeaponInfo();
	if (!tfWeaponInfo)
		return;
	auto& weaponData = tfWeaponInfo->GetWeaponData(0);

	float flDamage = weaponData.m_nDamage;
	flDamage = Utils::ATTRIB_HOOK_FLOAT(flDamage, "mult_dmg", pWeapon, 0, true);
	int nProjectilesPerShot = weaponData.m_nBulletsPerShot;
	if (nProjectilesPerShot >= 1)
		nProjectilesPerShot = Utils::ATTRIB_HOOK_FLOAT(nProjectilesPerShot, "mult_bullets_per_shot", pWeapon, 0, true);
	else
		nProjectilesPerShot = 1;
	flDamage *= nProjectilesPerShot;
	Storage[slot].Damage = flDamage;

	if (pWeapon->IsStreamingWeapon())
	{
		Storage[slot].Damage *= TF_DAMAGE_CRIT_DURATION_RAPID / weaponData.m_flTimeFireDelay;

		if (Storage[slot].Damage * TF_DAMAGE_CRIT_MULTIPLIER > BucketCap)
			Storage[slot].Damage = BucketCap / TF_DAMAGE_CRIT_MULTIPLIER;
	}
	if (slot == SLOT_MELEE)
		Storage[slot].Damage *= 35.f / 32.5f;

	float flMult = 1.f;
	if (slot == SLOT_MELEE)
		flMult = 0.5f;
	else if (Storage[slot].ShotsCrits.second > 0 && Storage[slot].ShotsCrits.first > 0)
		flMult = Math::RemapValClamped((float)Storage[slot].ShotsCrits.second / (float)Storage[slot].ShotsCrits.first, 0.1f, 1.f, 1.f, 3.f);

	if (slot == SLOT_MELEE)
	{
		const float cost = Storage[slot].Damage * flMult;
		Storage[slot].BaseCost = cost;
		Storage[slot].Cost = cost;
	}
	else
	{
		Storage[slot].BaseCost = Storage[slot].Damage * TF_DAMAGE_CRIT_MULTIPLIER;
		Storage[slot].Cost = Storage[slot].BaseCost * flMult;
	}

	auto Bucket = Storage[slot].Bucket;

	if (BucketCap)
		Storage[slot].PotentialCrits = static_cast<unsigned int>((BucketCap/* - std::max(BucketBottom, -Storage[slot].Cost)*/) / Storage[slot].BaseCost);

	if (slot == SLOT_MELEE)
		Storage[slot].AvailableCrits = std::floor((Bucket/* - std::max(BucketBottom, -Storage[slot].Cost)*/) / Storage[slot].Cost);
	else
	{
		int shots = Storage[slot].ShotsCrits.first, crits = Storage[slot].ShotsCrits.second;
		if (shots > 0 && crits > 0)
		{
			int iCrits = 0;

			float bucket = Bucket/* - std::max(BucketBottom, -Storage[slot].Cost)*/, flCost = Storage[slot].BaseCost;
			const int iAttempts = std::min(Storage[slot].PotentialCrits + 1, 100); // just in case
			for (int i = 0; i < iAttempts; i++)
			{
				flMult = Math::RemapValClamped((float)crits / (float)shots, 0.1f, 1.f, 1.f, 3.f);
				bucket -= flCost * flMult;
				if (bucket < 0.f)
					break;

				shots++; crits++;
				iCrits++;
			}

			Storage[slot].AvailableCrits = iCrits;
		}
		else
			Storage[slot].AvailableCrits = std::floor((Bucket/* - std::max(BucketBottom, -Storage[slot].Cost)*/) / Storage[slot].Cost);
	}
}

void CCritHack::CanFireCriticalShotHandler(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	//pWeapon->SetObservedCritChance(0.f);
	CritBanned = false;
	CritChance = 0.02f * pLocal->GetCritMult() + 0.1f;
	if (pWeapon->IsStreamingWeapon())
	{
		float flNonCritDuration = (TF_DAMAGE_CRIT_DURATION_RAPID / CritChance) - TF_DAMAGE_CRIT_DURATION_RAPID;
		CritChance = 1.f / flNonCritDuration;
	}

	if (CritDamage == 0.f || pWeapon->GetSlot() == SLOT_MELEE)
		return;

	const auto divCritDamage = CritDamage / TF_DAMAGE_CRIT_MULTIPLIER;
	const auto ratio = divCritDamage / (AllDamage - 2 * divCritDamage);
	//pWeapon->SetObservedCritChance(ratio);

	CritBanned = ratio >= CritChance;
}

void CCritHack::GetDamageTilUnban(CBaseEntity* pLocal)
{
	DamageTilUnban = 0;

	if (!CritBanned)
		return;

	const auto divCritDamage = CritDamage / TF_DAMAGE_CRIT_MULTIPLIER;

	DamageTilUnban = divCritDamage / CritChance + 2 * divCritDamage - AllDamage;
}

void CCritHack::FixHeavyRevBug(CUserCmd* pCmd)
{
	const auto& pLocal = g_EntityCache.GetLocal();
	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pLocal || !pWeapon || pLocal->deadflag())
		return;

	if (!pLocal->IsClass(CLASS_HEAVY) || pWeapon->GetWeaponID() != TF_WEAPON_MINIGUN)
		return;

	if (pCmd->buttons & IN_ATTACK)
		pCmd->buttons &= ~IN_ATTACK2;
}

bool CCritHack::WeaponCanCrit(CBaseCombatWeapon* pWeapon)
{
	if (Utils::ATTRIB_HOOK_FLOAT(1.f, "mult_crit_chance", pWeapon, 0, true)) //<= 0.f)
		return false;

	switch (pWeapon->GetWeaponID())
	{
	case TF_WEAPON_LUNCHBOX:
	case TF_WEAPON_JAR_MILK:
	case TF_WEAPON_BUFF_ITEM:
	case TF_WEAPON_FLAME_BALL:
	case TF_WEAPON_JAR_GAS:
	case TF_WEAPON_ROCKETPACK:
	case TF_WEAPON_LASER_POINTER:
	case TF_WEAPON_MEDIGUN:
	case TF_WEAPON_SNIPERRIFLE:
	case TF_WEAPON_SNIPERRIFLE_DECAP:
	case TF_WEAPON_SNIPERRIFLE_CLASSIC:
	case TF_WEAPON_COMPOUND_BOW:
	case TF_WEAPON_JAR:
	case TF_WEAPON_KNIFE:
	case TF_WEAPON_PDA_SPY:
	case TF_WEAPON_PDA_SPY_BUILD:
	case TF_WEAPON_PDA:
	case TF_WEAPON_PDA_ENGINEER_BUILD:
	case TF_WEAPON_PDA_ENGINEER_DESTROY:
	case TF_WEAPON_BUILDER:
		return false;
	}

	return true;
}



void CCritHack::ResetWeapon(CBaseCombatWeapon* pWeapon)
{
	const auto slot = pWeapon->GetSlot();
	const auto index = pWeapon->GetItemDefIndex();
	if (Storage[slot].DefIndex == index)
		return;

	Utils::ConLog("Crithack", "Resetting weapon", { 0, 255, 255, 255 }, Vars::Debug::Logging.Value);

	Storage[slot] = {};
	Storage[slot].Bucket = BucketDefault;
	Storage[slot].DefIndex = index;
}

void CCritHack::Reset()
{
	Utils::ConLog("Crithack", "Resetting", { 0, 255, 255, 255 }, Vars::Debug::Logging.Value);

	ForceCmds = {};
	SkipCmds = {};

	Storage = {};

	CritDamage = 0;
	AllDamage = 0;

	CritBanned = false;
	DamageTilUnban = 0;
	CritChance = 0.f;
	ProtectData = false;
}



void CCritHack::Run(CUserCmd* pCmd)
{
	FixHeavyRevBug(pCmd);

	auto bucketDefault = g_ConVars.FindVar("tf_weapon_criticals_bucket_default");
	auto bucketBottom = g_ConVars.FindVar("tf_weapon_criticals_bucket_bottom");
	auto bucketCap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");

	if (!bucketDefault || !bucketBottom || !bucketCap)
		return;

	BucketDefault = bucketDefault->GetFloat();
	BucketBottom = bucketBottom->GetFloat();
	BucketCap = bucketCap->GetFloat();

	const auto& pLocal = g_EntityCache.GetLocal();
	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pLocal || !pWeapon)
		return;

	bool bAttacking = Utils::IsAttacking(pCmd, pWeapon);
	if (G::CurWeaponType == EWeaponType::MELEE)
	{
		bAttacking = G::WeaponCanAttack && pCmd->buttons & IN_ATTACK;
		if (!bAttacking && pWeapon->GetWeaponID() == TF_WEAPON_FISTS)
			bAttacking = G::WeaponCanAttack && pCmd->buttons & IN_ATTACK2;
	}
	if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN && !(G::LastUserCmd->buttons & IN_ATTACK)) // silly
		bAttacking = false;

	if (Storage[pWeapon->GetSlot()].StreamWait <= I::GlobalVars->tickcount - 1)
		Storage[pWeapon->GetSlot()].StreamWait = -1;
	if (Storage[pWeapon->GetSlot()].StreamEnd <= I::GlobalVars->tickcount - 1)
		Storage[pWeapon->GetSlot()].StreamEnd = -1;

	const bool bRapidFire = pWeapon->IsStreamingWeapon();
	const bool bStreamWait = Storage[pWeapon->GetSlot()].StreamWait > 0;
	const bool bStreamEnd = Storage[pWeapon->GetSlot()].StreamEnd > 0;

	ResetWeapon(pWeapon);
	if (pLocal->deadflag() || !WeaponCanCrit(pWeapon) || pLocal->InCond(TF_COND_CRITBOOSTED)/* || !pWeapon->CanFireCriticalShot(false)*/)
		return;

	CanFireCriticalShotHandler(pLocal, pWeapon);
	GetDamageTilUnban(pLocal);
	Fill(pWeapon, pCmd, bAttacking, 15);

	const int closestCrit = LastGoodCritTick(pCmd);
	const int closestSkip = LastGoodSkipTick(pCmd);

    static KeyHelper critKey{ &Vars::CritHack::CritKey.Value };
	bool pressed = critKey.Down(); //(Vars::CritHack::CritKey.Value)
	if (!pressed && Vars::CritHack::AlwaysMelee.Value && pWeapon->GetSlot() == SLOT_MELEE)
		pressed = true;
	if (bAttacking && !pWeapon->IsInReload()) // is it valid & should we even use it
	{
		//ProtectData = true;

		if (IsEnabled())
		{
			if (pressed && Storage[pWeapon->GetSlot()].AvailableCrits > 0 && (!CritBanned || pWeapon->GetSlot() == SLOT_MELEE) && closestCrit >= 0 && !bStreamWait && !bStreamEnd)
				pCmd->command_number = closestCrit;
			else if (Vars::CritHack::AvoidRandom.Value && closestSkip >= 0)
				pCmd->command_number = closestSkip;
		}

		if (bRapidFire && !bStreamWait)
			Storage[pWeapon->GetSlot()].StreamWait = I::GlobalVars->tickcount + 1 / I::GlobalVars->interval_per_tick;

		if (bRapidFire && (bStreamEnd || bStreamWait))
			goto out;

		Storage[pWeapon->GetSlot()].ShotsCrits.first += 1;

		if (IsCritCommand(pCmd->command_number))
		{
			if (bRapidFire)
				Storage[pWeapon->GetSlot()].StreamEnd = I::GlobalVars->tickcount + 2 / I::GlobalVars->interval_per_tick;
			Storage[pWeapon->GetSlot()].ShotsCrits.second += 1;
			Storage[pWeapon->GetSlot()].Bucket = std::clamp(Storage[pWeapon->GetSlot()].Bucket - Storage[pWeapon->GetSlot()].Cost, std::max(BucketBottom, 0.f), BucketCap);
		}
		else
			Storage[pWeapon->GetSlot()].Bucket = std::clamp(Storage[pWeapon->GetSlot()].Bucket + Storage[pWeapon->GetSlot()].Damage, std::max(BucketBottom, 0.f), BucketCap);
	}
	/*
	else if (IsEnabled() && closest_skip >= 0)
		pCmd->command_number = closest_skip;
	*/

out:
	//ProtectData = false;

	GetTotalCrits(pLocal, pWeapon);

	if (pCmd->command_number == closestCrit)
		ForceCmds.pop_front();
	else if (pCmd->command_number == closestSkip)
		SkipCmds.pop_front();
}

bool CCritHack::CalcIsAttackCriticalHandler(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	if (I::Prediction->m_bFirstTimePredicted)
		return false;

	if (!pWeapon || pLocal)
		return false;

	{
		static int s_nPreviousTickcount = 0;
		if (s_nPreviousTickcount == I::GlobalVars->tickcount)
			return false;
		s_nPreviousTickcount = I::GlobalVars->tickcount;
	}

	if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN ||
		pWeapon->GetWeaponID() == TF_WEAPON_FLAMETHROWER)
	{
		auto nPreviousAmmoCount = pLocal->GetAmmoCount(pWeapon->m_iPrimaryAmmoType());
		static auto nNewAmmoCount = nPreviousAmmoCount;

		const auto bHasFiredBullet = nNewAmmoCount != nPreviousAmmoCount;

		if (!bHasFiredBullet)
			return false;
	}

	return true;
}

void CCritHack::Event(CGameEvent* pEvent, FNV1A_t uNameHash)
{
	if (uNameHash == FNV1A::HashConst("player_hurt"))
	{
		const auto& pLocal = g_EntityCache.GetLocal();
		if (!pLocal)
			return;

		const auto attacked = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"));
		const auto attacker = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker"));
		auto	   crit = pEvent->GetInt("crit");
		auto       damage = pEvent->GetInt("damageamount");
		const auto health = pEvent->GetInt("health");
		const auto weapon_id = pEvent->GetInt("weaponid");

		if (attacker == pLocal->GetIndex() && attacked != attacker)
		{
			CBaseCombatWeapon* pWeapon = reinterpret_cast<CBaseCombatWeapon*>(I::ClientEntityList->GetClientEntityFromHandle(pEvent->GetInt("weaponid")));
			if (!pWeapon)
				pWeapon = g_EntityCache.GetWeapon();
			if (!pWeapon)
				return;

			if (pWeapon->GetSlot() == SLOT_MELEE || pLocal->InCond(TF_COND_CRITBOOSTED))
				return;

			AllDamage += damage;
			if (crit)
			{
				CritDamage += damage;

				Utils::ConLog("Crithack", "Adding crit", { 0, 255, 255, 255 }, Vars::Debug::Logging.Value);
			}
		}
	}
	else if (uNameHash == FNV1A::HashConst("teamplay_round_start"))
	{
		CritDamage = 0.f;
		AllDamage = 0.f;
	}
	else if (uNameHash == FNV1A::HashConst("client_beginconnect") || uNameHash == FNV1A::HashConst("client_disconnect") ||
		uNameHash == FNV1A::HashConst("game_newmap")) // i believe newmap is practically rejoining ?
	{
		Utils::ConLog("Crithack", "Resetting", { 0, 255, 255, 255 }, Vars::Debug::Logging.Value);
		Reset();
	}
	else if (uNameHash == FNV1A::HashConst("player_changeclass"))
	{
		const bool bLocal = I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid")) == I::EngineClient->GetLocalPlayer();
		if (bLocal)
			Storage = {};
	}
}

void CCritHack::Draw()
{
	if (!Vars::CritHack::Active.Value || !(Vars::CritHack::Indicators.Value & (1 << 1)))
		return;
	if (!IsEnabled() || !G::CurrentUserCmd)
		return;

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive() || pLocal->IsAGhost())
		return;

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon)
		return;

	int x = Vars::CritHack::IndicatorPos.Value.c;
	int y = Vars::CritHack::IndicatorPos.Value.y + 8;

	const auto& Font = g_Draw.GetFont(FONT_INDICATORS); //fFont

	EStringAlign align = ALIGN_TOP;
	if (x <= (100 + 50 * Vars::Menu::DPI.Value))
	{
		x -= 42 * Vars::Menu::DPI.Value;
		align = ALIGN_TOPLEFT;
	}
	else if (x >= g_ScreenSize.w - (100 + 50 * Vars::Menu::DPI.Value))
	{
		x += 42 * Vars::Menu::DPI.Value;
		align = ALIGN_TOPRIGHT;
	}

	if (WeaponCanCrit(pWeapon))
	{
		const auto slot = pWeapon->GetSlot();
		const auto bRapidFire = pWeapon->IsStreamingWeapon();

		if (Storage[slot].Damage > 0)
		{
			if (pLocal->IsCritBoosted())
				g_Draw.String(Font, x, y, { 100, 255, 255, 255 }, align, "Crit Boosted");
			else if (bRapidFire && Storage[slot].StreamEnd > 0)
			{
				const float time = std::max(TICKS_TO_TIME(Storage[slot].StreamEnd - pLocal->m_nTickBase()), 0.f);
				g_Draw.String(Font, x, y, { 100, 255, 255, 255 }, align, std::format("Streaming crits {:.1f}s", time).c_str());
			}
			else if (!CritBanned)
			{
				if (Storage[slot].AvailableCrits > 0)
				{
					if (bRapidFire && Storage[slot].StreamWait > 0)
					{
						const float time = std::max((TICKS_TO_TIME(Storage[slot].StreamWait - pLocal->m_nTickBase())), 0.f);
						g_Draw.String(Font, x, y, { 181, 181, 181, 255 }, align, std::format("Wait {:.1f}s", time).c_str());
						//how the fuck does the previous old one works on rei's fork without having any errors??
 					}
					else
						g_Draw.String(Font, x, y, { 150, 255, 150, 255 }, align, "Crit Ready");
				}
				else
				{
					const float damage = Storage[slot].Damage;
					const int shots = Storage[slot].Cost / damage - (Storage[slot].Bucket/* - std::max(BucketBottom, -Storage[pWeapon->GetSlot()].Cost)*/) / damage + 1;
					g_Draw.String(Font, x, y, { 255, 150, 150, 255 }, align, shots == 1 ? std::format("Crit in {} shot", shots).c_str() : std::format("Crit in {} shots", shots).c_str());
				}
			}
			else
			g_Draw.String(Font, x, y, { 255, 150, 150, 255 }, align, std::format("Deal {} damage", DamageTilUnban).c_str());

			g_Draw.String(Font, x, y + Vars::Fonts::FONT_INDICATORS::nTall.Value + 2, { 181, 181, 181, 255 }, align, std::format("{} / {} potential Crits", Storage[slot].AvailableCrits, Storage[slot].PotentialCrits).c_str());
		}
		else
		    g_Draw.String(Font, x, y, { 181, 181, 181, 255 }, align, "Calculating");

		if (Vars::Debug::DebugInfo.Value)
		{
			g_Draw.String(Font, x, y + Font.nTall * 3, { 181, 181, 181, 255 }, align, std::format("AllDamage: {}, CritDamage: {}", AllDamage, CritDamage).c_str());
			g_Draw.String(Font, x, y + Font.nTall * 4, { 181, 181, 181, 255 }, align, std::format("Bucket: {}", Storage[slot].Bucket).c_str());
			g_Draw.String(Font, x, y + Font.nTall * 5, { 181, 181, 181, 255 }, align, std::format("Damage: {}, Cost: {}", Storage[slot].Damage, Storage[slot].Cost).c_str());
			g_Draw.String(Font, x, y + Font.nTall * 6, { 181, 181, 181, 255 }, align, std::format("Shots: {}, Crits: {}", Storage[slot].ShotsCrits.first, Storage[slot].ShotsCrits.second).c_str());
			g_Draw.String(Font, x, y + Font.nTall * 7, { 181, 181, 181, 255 }, align, std::format("CritBanned: {}, DamageTilUnban: {}", CritBanned, DamageTilUnban).c_str());
			g_Draw.String(Font, x, y + Font.nTall * 8, { 181, 181, 181, 255 }, align, std::format("CritChance: {:.2f}", CritChance).c_str());
			g_Draw.String(Font, x, y + Font.nTall * 9, { 181, 181, 181, 255 }, align, std::format("Force: {}, Skip: {}", ForceCmds.size(), SkipCmds.size()).c_str());
		}
	}
}
