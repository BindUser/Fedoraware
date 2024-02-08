#pragma once
#include "../Feature.h"

typedef int                i32;
typedef unsigned int       u32;

struct WeaponStorage
{
	float Bucket = 300.f;

	float BaseDamage = 0.f; // literally fucking unused, yet causes crash with some weapons if removed ??
	float Damage = 0.f;
	float BaseCost = 0.f;
	std::pair<int, int> ShotsCrits = { 1, 0 };
	float Cost = 0.f;

	int AvailableCrits = 0;
	int PotentialCrits = 0;

	int StreamWait = -1;
	int StreamEnd = -1;

	int DefIndex = -1;
};

class CCritHack
{
private:
	bool IsEnabled();
	bool AreRandomCritsEnabled();

	void Fill(CBaseCombatWeapon* pWeapon, const CUserCmd* pCmd, const bool bAttacking, int n = 10);
	int LastGoodCritTick(const CUserCmd* pCmd);
	int LastGoodSkipTick(const CUserCmd* pCmd);

	bool IsCritCommand(const i32 command_number, const bool crit = true);
	u32 DecryptOrEncryptSeed(CBaseCombatWeapon* pWeapon, const u32 seed);

	void GetTotalCrits(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	void CanFireCriticalShotHandler(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon);
	void GetDamageTilUnban(CBaseEntity* pLocal);
	void FixHeavyRevBug(CUserCmd* pCmd);
	bool WeaponCanCrit(CBaseCombatWeapon* weapon);

	void ResetWeapon(CBaseCombatWeapon* pWeapon);
	void Reset();

	std::deque<int> ForceCmds{};
	std::deque<int> SkipCmds{};

	float BucketDefault = 300.f;
	float BucketBottom = -250.f;
	float BucketCap = 1000.f;

	float CritDamage = 0;
	float AllDamage = 0;

	bool CritBanned = false;
	int DamageTilUnban = 0;
	float CritChance = 0.f;
	int WishRandomSeed = 0;

public:

	void Run(CUserCmd* pCmd);
	bool CalcIsAttackCriticalHandler(CBaseEntity* pLocal, CBaseCombatWeapon* weapon);
	void Event(CGameEvent* pEvent, FNV1A_t uNameHash);
	void Draw();

	std::unordered_map<int, WeaponStorage> Storage = {};
	bool ProtectData = false;
};

ADD_FEATURE(CCritHack, CritHack)