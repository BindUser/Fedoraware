#include "Menu.h"

#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "../AttributeChanger/AttributeChanger.h"
#include "../Misc/Misc.h"

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_color_gradient.h"
#include "ImGui/imgui_stdlib.h"
#include "Components.hpp"

constexpr int MENU_KEY = VK_INSERT;
static ImGradient titleGradient;
static ImGradient mainGradient;

// Fonts
ImFont* SegoeLight = nullptr;	// 16px
ImFont* Segoe = nullptr;		// 16px
ImFont* SegoeBold = nullptr;	// 16px

ImFont* SectionFont = nullptr;	// 18px
ImFont* TabFont = nullptr;		// 22px
ImFont* TitleFont = nullptr;	// 26px

int unuPrimary = 0;
int unuSecondary = 0;

#pragma region Components
void SectionTitle(const char* title, float yOffset = 6)
{
	ImGui::Dummy({ 0, yOffset });
	ImGui::PushFont(SectionFont);
	const ImVec2 titleSize = ImGui::CalcTextSize(title);
	ImGui::SameLine((ImGui::GetColumnWidth()) / 2 - (titleSize.x / 2));
	ImGui::Text(title);
	ImGui::PopFont();

	const auto widgetPos = ImGui::GetCursorScreenPos();
	ImGui::GradientRect(&mainGradient, { widgetPos.x, widgetPos.y - 2 }, ImGui::GetColumnWidth(), 3);
}
#pragma endregion

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(800, 500);

	ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	if (ImGui::Begin("Fedoraware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		// Gradient line
		ImGui::GradientRect(&titleGradient, { windowPos.x, windowPos.y }, windowSize.x, 3.f);
		ImGui::Dummy({ 0, 2 });

		// Title Text
		{
			ImGui::PushFont(TitleFont);
			const auto titleWidth = ImGui::CalcTextSize("Fedoraware").x;
			drawList->AddText(TitleFont, TitleFont->FontSize, { windowPos.x + (windowSize.x / 2) - (titleWidth / 2), windowPos.y }, Accent, "Fedoraware");
			ImGui::PopFont();
		}

		// Tabbar
		ImGui::SetCursorPos({ 0, TitleHeight });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundLight.Value);
		if (TabHeight > 5.f)
		{
			if (ImGui::BeginChild("Tabbar", { windowSize.x + 5, TabHeight + SubTabHeight }))
			{
				DrawTabbar();
			}
			ImGui::EndChild();
		}
		ImGui::PopStyleColor();

		// Main content
		ImGui::SetCursorPos({ 0, TitleHeight + TabHeight + SubTabHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 15.f, 10.f });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundDark.Value);
		if (ImGui::BeginChild("Content", { windowSize.x, windowSize.y - (TitleHeight + TabHeight + SubTabHeight) }, false, ImGuiWindowFlags_AlwaysUseWindowPadding))
		{
			ImGui::PushFont(Segoe);
			switch (CurrentTab)
			{
			case MenuTab::Aimbot: { MenuAimbot(); break; }
			case MenuTab::Trigger: { MenuTrigger(); break; }
			case MenuTab::Visuals: { MenuVisuals(); break; }
			case MenuTab::HvH: { MenuHvH(); break; }
			case MenuTab::Misc: { MenuMisc(); break; }
			case MenuTab::Configs: { MenuConfigs(); break; }
			}

			ImGui::PopFont();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		// End
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void CMenu::DrawTabbar()
{
	ImGui::PushFont(TabFont);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	if (ImGui::BeginTable("TabbarTable", 5))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, BackgroundLight.Value);
		ImGui::PushStyleColor(ImGuiCol_Text, TextLight.Value);
		if (ImGui::TabButton("Aimbot", CurrentTab == MenuTab::Aimbot))
		{
			CurrentTab = MenuTab::Aimbot;
		}

		if (ImGui::TabButton("Trigger", CurrentTab == MenuTab::Trigger))
		{
			CurrentTab = MenuTab::Trigger;
		}

		if (ImGui::TabButton("Visuals", CurrentTab == MenuTab::Visuals))
		{
			CurrentTab = MenuTab::Visuals;
		}

		if (ImGui::TabButton("HvH", CurrentTab == MenuTab::HvH))
		{
			CurrentTab = MenuTab::HvH;
		}

		if (ImGui::TabButton("Misc", CurrentTab == MenuTab::Misc))
		{
			CurrentTab = MenuTab::Misc;
		}

		ImGui::PopStyleColor(2);
		ImGui::EndTable();
	}

	if (CurrentTab == MenuTab::Visuals)
	{
		SubTabHeight = 30.f;

		if (ImGui::BeginTable("SubbarTable", 6))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, BackgroundLight.Value);
			ImGui::PushStyleColor(ImGuiCol_Text, TextLight.Value);
			if (ImGui::TabButton("Players", CurrentVisualsTab == VisualsTab::Players))
			{
				CurrentVisualsTab = VisualsTab::Players;
			}

			if (ImGui::TabButton("Buildings", CurrentVisualsTab == VisualsTab::Buildings))
			{
				CurrentVisualsTab = VisualsTab::Buildings;
			}

			if (ImGui::TabButton("World", CurrentVisualsTab == VisualsTab::World))
			{
				CurrentVisualsTab = VisualsTab::World;
			}

			if (ImGui::TabButton("Font", CurrentVisualsTab == VisualsTab::Font))
			{
				CurrentVisualsTab = VisualsTab::Font;
			}

			if (ImGui::TabButton("Misc", CurrentVisualsTab == VisualsTab::Misc))
			{
				CurrentVisualsTab = VisualsTab::Misc;
			}

			if (ImGui::TabButton("Radar", CurrentVisualsTab == VisualsTab::Radar))
			{
				CurrentVisualsTab = VisualsTab::Radar;
			}

			ImGui::PopStyleColor(2);
			ImGui::EndTable();
		}
	} else
	{
		SubTabHeight = 0.f;
	}

	ImGui::PopStyleVar(3);
	ImGui::PopFont();
}

/* Tab: Aimbot */
void CMenu::MenuAimbot()
{
	using namespace ImGui;
	if (BeginTable("AimbotTable", 3))
	{
		/* Column 1 */
		TableNextColumn();
		{
			SectionTitle("Global");

			Checkbox("Aimbot", &Vars::Aimbot::Global::Active.m_Var); HelpMarker("Aimbot master switch");
			ColorPickerL("Target", Colors::Target);
			InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
			WSlider("Aimbot FoV####AimbotFoV", &Vars::Aimbot::Global::AimFOV.m_Var, 0.f, 180.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
			ColorPickerL("Aimbot FOV circle", Colors::FOVCircle);
			Checkbox("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var); HelpMarker("Automatically shoot when a target is found");
			MultiCombo({ "Players", "Buildings" }, { &Vars::Aimbot::Global::AimPlayers.m_Var, &Vars::Aimbot::Global::AimBuildings.m_Var }, "Choose which targets the Aimbot should aim at", "Aim targets");
			MultiCombo({ "Invulnerable", "Cloaked", "Friends", "Taunting" }, { &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var, &Vars::Aimbot::Global::IgnoreCloaked.m_Var, &Vars::Aimbot::Global::IgnoreFriends.m_Var, &Vars::Aimbot::Global::IgnoreTaunting.m_Var }, "Choose which targets should be ignored", "Ignored targets###HitscanIgnoredTargets");
			ColorPickerL("Invulnerable colour", Colors::Invuln);

			SectionTitle("Crits", 20);
			Checkbox("Crit hack", &Vars::CritHack::Active.m_Var);  HelpMarker("Enables the crit hack (BETA)");
			MultiCombo({ "Indicators", "Avoid Random" }, { &Vars::CritHack::indicators.m_Var, &Vars::CritHack::avoidrandom.m_Var }, "Misc options for crithack", "Misc###CrithackMiscOptions");
			InputKeybind("Crit key", Vars::CritHack::CritKey); HelpMarker("Will try to force crits when the key is held");

			SectionTitle("Backtrack", 20);
			Checkbox("Active", &Vars::Backtrack::Enabled.m_Var); HelpMarker("If you shoot at the backtrack manually it will attempt to hit it");
			Checkbox("Aimbot aims last tick", &Vars::Backtrack::Aim.m_Var); HelpMarker("Aimbot aims at the last tick if visible");
		}

		/* Column 2 */
		TableNextColumn();
		{
			SectionTitle("Hitscan");
			WCombo("Sort method###HitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.m_Var, { "FOV", "Distance" }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
			WCombo("Aim method###HitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.m_Var, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			WCombo("Hitbox###HitscanHitbox", &Vars::Aimbot::Hitscan::AimHitbox.m_Var, { "Head", "Body", "Auto" }); HelpMarker("Which hitbox the aimbot will target");
			WCombo("Tapfire###HitscanTapfire", &Vars::Aimbot::Hitscan::TapFire.m_Var, { "Off", "Distance", "Always" }); HelpMarker("How/If the aimbot chooses to tapfire enemies.");
			WSlider("Smooth factor###HitscanSmoothing", &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Changes how smooth the aimbot will aim at the target");
			MultiCombo({ "Body", "Head", "Buildings" }, { &Vars::Aimbot::Hitscan::ScanHitboxes.m_Var, &Vars::Aimbot::Hitscan::ScanHead.m_Var, &Vars::Aimbot::Hitscan::ScanBuildings.m_Var }, "Choose what the aimbot should multipoint", "Multipoint");
			Checkbox("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
			Checkbox("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.m_Var); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
			Checkbox("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var); HelpMarker("The aimbot will switch to the smooth method if being spectated");
			Checkbox("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.m_Var); HelpMarker("The aimbot will only shoot if scoped");
			Checkbox("Auto scope", &Vars::Aimbot::Hitscan::AutoScope.m_Var); HelpMarker("The aimbot will automatically scope in to shoot");
			Checkbox("Auto rev minigun", &Vars::Aimbot::Hitscan::AutoRev.m_Var); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
			Checkbox("Bodyaim if lethal", &Vars::Aimbot::Global::BAimLethal.m_Var); HelpMarker("The aimbot will aim for body when damage is lethal to it");
		}

		/* Column 3 */
		TableNextColumn();
		{
			SectionTitle("Projectile");
			Checkbox("Performance mode", &Vars::Aimbot::Projectile::PerformanceMode.m_Var); HelpMarker("Only target enemy closest to the crosshair");
			Checkbox("Movement simulation", &Vars::Aimbot::Projectile::MovementSimulation.m_Var); HelpMarker("Uses game functions to predict where the player will be");
			ColorPickerL("Prediction Line Color", Vars::Aimbot::Projectile::PredictionColor);
			if (Vars::Aimbot::Projectile::MovementSimulation.m_Var)
			{
				WSlider("Prediction Time", &Vars::Aimbot::Projectile::predTime.m_Var, 0.1f, 10.f, "%.1f");
			}
			{
				WCombo("Sort method###ProjectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.m_Var, { "FOV", "Distance" });
				WCombo("Aim method###ProjectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.m_Var, { "Plain", "Silent" });
				WCombo("Hitbox###ProjectileHitbox", &Vars::Aimbot::Projectile::AimPosition.m_Var, { "Body", "Feet", "Auto" });
			}
			Checkbox("Feet aim on ground (Demoman)", &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var); HelpMarker("Will aim at feet if target is on the ground");
			Checkbox("Custom huntsman Z-Adjust", &Vars::Aimbot::Projectile::ManualZAdjust.m_Var); HelpMarker("Enables the ability to adjust the Z-Position for huntsman");
			if (Vars::Aimbot::Projectile::ManualZAdjust.m_Var)
			{
				WSlider("Z-Value###ZAdjustValue", &Vars::Aimbot::Projectile::ZAdjustAmount.m_Var, 0.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Manual Z-Adjust for projectiles");
			}

			SectionTitle("Melee", 20);
			{
				WCombo("Sort method###MeleeSortMethod", &Vars::Aimbot::Melee::SortMethod.m_Var, { "FOV", "Distance", }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
				WCombo("Aim method###MeleeAimMethod", &Vars::Aimbot::Melee::AimMethod.m_Var, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			}
			WSlider("Smooth factor###MeleeSmoothing", &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How smooth the aimbot should be");
			Checkbox("Range check", &Vars::Aimbot::Melee::RangeCheck.m_Var); HelpMarker("Only aim at target if within melee range");
			Checkbox("Swing prediction", &Vars::Aimbot::Melee::PredictSwing.m_Var); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
			Checkbox("Whip teammates", &Vars::Aimbot::Melee::WhipTeam.m_Var); HelpMarker("Aimbot will target teammates if holding the Disciplinary Action");
		}

		/* End */
		EndTable();
	}
}

/* Tab: Trigger */
void CMenu::MenuTrigger()
{
	using namespace ImGui;
	if (BeginTable("TriggerTable", 3))
	{
		/* Column 1 */
		TableNextColumn();
		{
			SectionTitle("Global");
			Checkbox("Triggerbot", &Vars::Triggerbot::Global::Active.m_Var); HelpMarker("Global triggerbot master switch");
			InputKeybind("Trigger key", Vars::Triggerbot::Global::TriggerKey); HelpMarker("The key which activates the triggerbot");
			MultiCombo({ "Invulnerable", "Cloaked", "Friends" }, { &Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var, &Vars::Triggerbot::Global::IgnoreCloaked.m_Var, &Vars::Triggerbot::Global::IgnoreFriends.m_Var }, "Choose which targets should be ignored", "Ignored targets###TriggerIgnoredTargets");

			SectionTitle("Autoshoot", 20);
			Checkbox("Autoshoot###AutoshootTrigger", &Vars::Triggerbot::Shoot::Active.m_Var); HelpMarker("Shoots if mouse is over a target");
			MultiCombo({ "Players", "Buildings" }, { &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var, &Vars::Triggerbot::Shoot::TriggerBuildings.m_Var }, "Choose which target the triggerbot should shoot at", "Trigger targets");
			Checkbox("Head only###TriggerHeadOnly", &Vars::Triggerbot::Shoot::HeadOnly.m_Var); HelpMarker("Auto shoot will only shoot if you are aiming at the head");
			Checkbox("Wait for charge###TriggerbotWaitForCharge", &Vars::Triggerbot::Shoot::WaitForCharge.m_Var); HelpMarker("Auto shoot will only shoot if the sniper is charged enough to kill in one hit / is fully charged");
			WSlider("Head scale###TriggerHeadScale", &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The scale at which the auto shoot will try to shoot the targets head");
		}

		/* Column 2 */
		TableNextColumn();
		{
			SectionTitle("Autostab");
			Checkbox("Auto backstab###TriggerAutostab", &Vars::Triggerbot::Stab::Active.m_Var); HelpMarker("Auto backstab will attempt to backstab the target if possible");
			Checkbox("Rage mode", &Vars::Triggerbot::Stab::RageMode.m_Var); HelpMarker("Stabs whenever possible by aiming toward the back");
			Checkbox("Silent", &Vars::Triggerbot::Stab::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");
			Checkbox("Disguise on kill", &Vars::Triggerbot::Stab::Disguise.m_Var); HelpMarker("Will apply the previous disguise after stabbing");
			Checkbox("Ignore razorback", &Vars::Triggerbot::Stab::IgnRazor.m_Var); HelpMarker("Will not attempt to backstab snipers wearing the razorback");
			WSlider("Stab range###StabRange", &Vars::Triggerbot::Stab::Range.m_Var, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The range at which auto backstab will attempt to stab");

			SectionTitle("Auto Detonate", 20);
			Checkbox("Autodetonate###TriggerDet", &Vars::Triggerbot::Detonate::Active.m_Var);
			Checkbox("Explode stickies###TriggerSticky", &Vars::Triggerbot::Detonate::Stickies.m_Var); HelpMarker("Detonate sticky bombs when a player is in range");
			Checkbox("Detonate flares###TriggerFlares", &Vars::Triggerbot::Detonate::Flares.m_Var); HelpMarker("Detonate detonator flares when a player is in range");
			WSlider("Detonation radius###TriggerDetRadius", &Vars::Triggerbot::Detonate::RadiusScale.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The radius around the projectile that it will detonate if a player is in");
		}

		/* Column 3 */
		TableNextColumn();
		{
			SectionTitle("Autoblast");
			Checkbox("Autoblast###Triggreairblast", &Vars::Triggerbot::Blast::Active.m_Var); HelpMarker("Auto airblast master switch");
			Checkbox("Rage airblast###TriggerAirRage", &Vars::Triggerbot::Blast::Rage.m_Var); HelpMarker("Will airblast whenever possible, regardless of FoV");
			Checkbox("Silent###triggerblastsilent", &Vars::Triggerbot::Blast::Silent.m_Var); HelpMarker("Aim changes made by the rage mode setting aren't visible");

			SectionTitle("Autouber", 20);
			Checkbox("Autouber###Triggeruber", &Vars::Triggerbot::Uber::Active.m_Var); HelpMarker("Auto uber master switch");
			Checkbox("Only uber friends", &Vars::Triggerbot::Uber::OnlyFriends.m_Var); HelpMarker("Auto uber will only activate if healing steam friends");
			Checkbox("Preserve self", &Vars::Triggerbot::Uber::PopLocal.m_Var); HelpMarker("Auto uber will activate if local player's health falls below the percentage");
			Checkbox("Vaccinator resistances", &Vars::Triggerbot::Uber::AutoVacc.m_Var); HelpMarker("Auto uber will automatically find the best resistance and pop when needed (This doesn't work properly)");
			WSlider("Health left (%)###TriggerUberHealthLeft", &Vars::Triggerbot::Uber::HealthLeft.m_Var, 1.f, 99.f, "%.0f%%", 1.0f); HelpMarker("The amount of health the heal target must be below to actiavte");
		}

		EndTable();
	}
}

/* Tab: Visuals */
void CMenu::MenuVisuals()
{
	using namespace ImGui;

	switch(CurrentVisualsTab)
	{
		// Visuals: Players
		case VisualsTab::Players:
		{
			if (BeginTable("VisualsPlayersTable", 3))
			{
				/* Column 1 */
				TableNextColumn();
				{
					SectionTitle("ESP Main");
					Checkbox("ESP###EnableESP", &Vars::ESP::Main::Active.m_Var); HelpMarker("Global ESP master switch");
					Checkbox("Outlined health bars", &Vars::ESP::Main::Outlinedbar.m_Var); HelpMarker("Will outline the health bars");
					Checkbox("Relative colours", &Vars::ESP::Main::EnableTeamEnemyColors.m_Var); HelpMarker("Chooses colors relative to your team (team/enemy)");
					if (Vars::ESP::Main::EnableTeamEnemyColors.m_Var)
					{
						ColorPickerL("Enemy color", Colors::Enemy);
						ColorPickerL("Team color", Colors::rTeam, 1);
					}
					else
					{
						ColorPickerL("RED Team color", Colors::TeamRed);
						ColorPickerL("BLU Team color", Colors::TeamBlu, 1);
					}

					SectionTitle("Player ESP", 20);
					Checkbox("Player ESP###EnablePlayerESP", &Vars::ESP::Players::Active.m_Var); HelpMarker("Will draw useful information/indicators on players");
					Checkbox("Name ESP###PlayerNameESP", &Vars::ESP::Players::Name.m_Var); HelpMarker("Will draw the players name");
					Checkbox("Custom Name Color", &Vars::ESP::Players::NameC.m_Var); HelpMarker("Custom color for name esp");
					if (Vars::ESP::Players::NameC.m_Var)
					{
						ColorPickerL("Name ESP Color", Vars::ESP::Players::NameColor);
					}
					Checkbox("Name ESP box###PlayerNameESPBox", &Vars::ESP::Players::NameBox.m_Var); HelpMarker("Will draw a box around players name to make it stand out");
					Checkbox("Self ESP###SelfESP", &Vars::ESP::Players::ShowLocal.m_Var); HelpMarker("Will draw ESP on local player (thirdperson)");
					ColorPickerL("Local colour", Colors::Local);
					WCombo("Ignore team###IgnoreTeamESPp", &Vars::ESP::Players::IgnoreTeammates.m_Var, { "Off", "All", "Only friends" }); HelpMarker("Which teammates the ESP will ignore drawing on");
					ColorPickerL("Friend colour", Colors::Friend);
					WCombo("Ignore cloaked###IgnoreCloakESPp", &Vars::ESP::Players::IgnoreCloaked.m_Var, { "Off", "All", "Only enemies" }); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
					ColorPickerL("Cloaked colour", Colors::Cloak);
					WCombo("Ubercharge###PlayerUber", &Vars::ESP::Players::Uber.m_Var, { "Off", "Text", "Bar" }); HelpMarker("Will draw how much ubercharge a medic has");
					ColorPickerL("Ubercharge colour", Colors::UberColor);
					WCombo("Class###PlayerIconClass", &Vars::ESP::Players::Class.m_Var, { "Off", "Icon", "Text", "Both" }); HelpMarker("Will draw the class the player is");
					Checkbox("Weapon icons", &Vars::ESP::Players::WeaponIcon.m_Var); HelpMarker("Shows an icon for the weapon that the player has currently equipped");
					ColorPickerL("Invulnerable colour", Colors::WeaponIcon);
					Checkbox("Health bar###ESPPlayerHealthBar", &Vars::ESP::Players::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the player has");
					ColorPickerL("Health Bar Top", Colors::GradientHealthBar.startColour);
					ColorPickerL("Health Bar Bottom", Colors::GradientHealthBar.endColour, 1);
					Checkbox("Health text###ESPPlayerHealthText", &Vars::ESP::Players::Health.m_Var); HelpMarker("Will draw the players health, as well as their max health");
					Checkbox("Condition", &Vars::ESP::Players::Cond.m_Var); HelpMarker("Will draw what conditions the player is under");
					ColorPickerL("Condition colour", Colors::Cond);
					Checkbox("GUID", &Vars::ESP::Players::GUID.m_Var); HelpMarker("Show's the players Steam ID");
					Checkbox("Choked Packets", &Vars::ESP::Players::Choked.m_Var); HelpMarker("Shows how many packets the player has choked");
					ColorPickerL("Choked Bar Top", Colors::ChokedBar.startColour);
					ColorPickerL("Choked Bar Bottom", Colors::ChokedBar.endColour, 1);
					WCombo("Box###PlayerBoxESP", &Vars::ESP::Players::Box.m_Var, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on players");
					WCombo("Skeleton###PlayerSkellington", &Vars::ESP::Players::Bones.m_Var, { "Off", "Custom colour", "Health" }); HelpMarker("Will draw the bone structure of the player");
					ColorPickerL("Skellington colour", Colors::Bones);
					Checkbox("Lines###Playerlines", &Vars::ESP::Players::Lines.m_Var); HelpMarker("Draws lines from the local players position to enemies position");
					Checkbox("Dlights###PlayerDlights", &Vars::ESP::Players::Dlights.m_Var); HelpMarker("Will make players emit a dynamic light around them");
					WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Players::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
					WSlider("ESP alpha###PlayerESPAlpha", &Vars::ESP::Players::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				}

				/* Column 2 */
				TableNextColumn();
				{
					SectionTitle("Chams Main");
					Checkbox("Chams###ChamsMasterSwitch", &Vars::Chams::Main::Active.m_Var); HelpMarker("Chams master switch");

					static std::vector chamOptions{ 
						"Local",
						"Friends",
						"Enemies",
						"Teammates",
						"Target"
					};

					static int currentSelected = 0; // 0 - local, 1 - friends, 2 - enemy, 3 - team
					static std::vector pchamsMaterials { "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic" };

					SectionTitle("Player Chams");
					Checkbox("Player chams###PlayerChamsBox", &Vars::Chams::Players::Active.m_Var); HelpMarker("Player chams master switch");
					MultiCombo({ "Render Wearable", "Render Weapon" }, { &Vars::Chams::Players::Wearables.m_Var, &Vars::Chams::Players::Weapons.m_Var }, "Customize Chams", "Flags");
					WCombo("Config", &currentSelected, chamOptions);

					switch (currentSelected) // please find a better way to do this, i have tried so many things and i cant get it to work properly
					{
					case 0:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Local.chamsActive, &Vars::Chams::Players::Local.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::Players::Local.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
						ColorPickerL("Fresnel base colour", Vars::Chams::Players::Local.fresnelBase);
						break;
					}
					case 1:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Friend.chamsActive, &Vars::Chams::Players::Friend.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::Players::Friend.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
						ColorPickerL("Fresnel base colour", Vars::Chams::Players::Friend.fresnelBase);
						break;
					}
					case 2:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Enemy.chamsActive, &Vars::Chams::Players::Enemy.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::Players::Enemy.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
						ColorPickerL("Fresnel base colour", Vars::Chams::Players::Enemy.fresnelBase);
						break;
					}
					case 3:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Team.chamsActive, &Vars::Chams::Players::Team.showObstructed, }, "", "Options");
						WCombo("Material", &Vars::Chams::Players::Team.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
						ColorPickerL("Fresnel base colour", Vars::Chams::Players::Team.fresnelBase);
						break;
					}
					case 4:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Players::Target.chamsActive, &Vars::Chams::Players::Target.showObstructed, }, "", "Options");
						WCombo("Material", &Vars::Chams::Players::Target.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
						ColorPickerL("Fresnel base colour", Vars::Chams::Players::Target.fresnelBase);
						break;
					}
					}

					SectionTitle("DME Chams");
					Checkbox("DME chams###dmeactive", &Vars::Chams::DME::Active.m_Var); HelpMarker("DME chams master switch");
					ColorPickerL("Weapon colour", Colors::Weapon);
					ColorPickerL("Hand colour", Colors::Hands, 1);

					static std::vector handsMaterial {
						"Original",
						"Shaded",
						"Shiny",
						"Flat",
						"Wireframe shaded",
						"Wireframe shiny",
						"Wireframe flat",
						"Fresnel",
						"Brick",
						"What",
						"Wacky"
					};
					WCombo("Hand material", &Vars::Chams::DME::Hands.m_Var, handsMaterial);
					ColorPickerL("Fresnel Hands Base", Colors::FresnelBaseHands); HelpMarker("What material to put on your viewmodels arms/hands");

					static std::vector handsProxyMaterial {
						"None",
						"Spectrum splattered",
						"Electro skulls",
						"Jazzy",
						"Frozen aurora",
						"Hana",
						"IDK",
						"Ghost thing",
						"Flames",
						"Spook wood",
						"Edgy",
						"Starlight serenity",
						"Fade"
					};
					WCombo("Hand proxy material", &Vars::Chams::DME::HandsProxySkin.m_Var, handsProxyMaterial); HelpMarker("Puts a cool looking animated skin on your hands");

					static std::vector dmeGlowMaterial {
						"None",
						"Fresnel Glow",
						"Wireframe Glow"
					};
					WCombo("Hand Glow", &Vars::Chams::DME::HandsGlowOverlay.m_Var, dmeGlowMaterial);
					ColorPickerL("Hand glow colour", Colors::HandsOverlay);

					static std::vector weaponMaterial {
						"Original",
						"Shaded",
						"Shiny",
						"Flat",
						"Wireframe shaded",
						"Wireframe shiny",
						"Wireframe flat",
						"Fresnel",
						"Brick",
						"What",
						"Wacky"
					};
					WCombo("Weapon material", &Vars::Chams::DME::Weapon.m_Var, weaponMaterial);
					ColorPickerL("Fresnel Weapons Base", Colors::FresnelBaseWeps); HelpMarker("What material to put on your viewmodels weapon");

					static std::vector weaponProxyMaterial {
						"None",
						"Spectrum splattered",
						"Electro skulls",
						"Jazzy",
						"Frozen aurora",
						"Hana",
						"IDK",
						"Ghost thing",
						"Flames",
						"Spook wood",
						"Edgy",
						"Starlight serenity",
						"Fade"
					};
					WCombo("Weapon proxy material", &Vars::Chams::DME::WeaponsProxySkin.m_Var, weaponProxyMaterial); HelpMarker("Puts a cool looking animated skin on your weapons");
					WCombo("Weapon Glow", &Vars::Chams::DME::WeaponGlowOverlay.m_Var, dmeGlowMaterial);
					ColorPickerL("Weapon glow colour", Colors::WeaponOverlay);
					MultiCombo({ "Hands", "Hands overlay", "Weapon", "Weapon overlay" }, { &Vars::Chams::DME::HandsRainbow.m_Var, &Vars::Chams::DME::HandsOverlayRainbow.m_Var, &Vars::Chams::DME::WeaponRainbow.m_Var, &Vars::Chams::DME::WeaponOverlayRainbow.m_Var }, "Rainbow DME chams", "Rainbow DME###RainbowDMEChams");
					WSlider("Hands glow amount", &Vars::Chams::DME::HandsGlowAmount.m_Var, 150, 1, "%.0f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput);
					WSlider("Weapon glow amount", &Vars::Chams::DME::WeaponGlowAmount.m_Var, 150, 1, "%.0f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_ClampOnInput);

					SectionTitle("Backtrack chams", 20);
					Checkbox("Backtrack chams", &Vars::Backtrack::BtChams::Enabled.m_Var); HelpMarker("Draws chams to show where a player is");
					ColorPickerL("Backtrack colour", Vars::Backtrack::BtChams::BacktrackColor);
					Checkbox("Only draw last tick", &Vars::Backtrack::BtChams::LastOnly.m_Var); HelpMarker("Only draws the last tick (can save FPS)");
					Checkbox("Enemy only", &Vars::Backtrack::BtChams::EnemyOnly.m_Var); HelpMarker("You CAN backtrack your teammates. (Whip, medigun)");

					static std::vector backtrackMaterial {
						"Shaded",
						"Shiny",
						"Flat",
						"Wireframe shaded",
						"Wireframe shiny",
						"Wireframe flat",
						"Fresnel",
						"Brick"
					};
					WCombo("Backtrack material", &Vars::Backtrack::BtChams::Material.m_Var, backtrackMaterial);
					Checkbox("Ignore Z###BtIgnoreZ", &Vars::Backtrack::BtChams::IgnoreZ.m_Var); HelpMarker("Draws them through walls");

					SectionTitle("Fakelag chams", 20);
					Checkbox("Fakelag chams", &Vars::Misc::CL_Move::FakelagIndicator.m_Var); HelpMarker("Draws chams to show your fakelag position");
					ColorPickerL("Fakelag colour", Vars::Misc::CL_Move::FLGChams::FakelagColor);
					WCombo("Fakelag material", &Vars::Misc::CL_Move::FLGChams::Material.m_Var, backtrackMaterial);
				}

				/* Column 3 */
				TableNextColumn();
				{
					SectionTitle("Glow Main");
					Checkbox("Glow", &Vars::Glow::Main::Active.m_Var);
					Checkbox("Stencil glow", &Vars::Glow::Main::Stencil.m_Var);
					if (!Vars::Glow::Main::Stencil.m_Var) { WSlider("Glow scale", &Vars::Glow::Main::Scale.m_Var, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp); }
					
					SectionTitle("Player Glow", 20);
					Checkbox("Player glow###PlayerGlowButton", &Vars::Glow::Players::Active.m_Var); HelpMarker("Player glow master switch");
					Checkbox("Self glow###SelfGlow", &Vars::Glow::Players::ShowLocal.m_Var); HelpMarker("Draw glow on the local player");
					Checkbox("Self rainbow glow###SelfGlowRainbow", &Vars::Glow::Players::LocalRainbow.m_Var); HelpMarker("Homosapien");
					WCombo("Ignore team###IgnoreTeamGlowp", &Vars::Glow::Players::IgnoreTeammates.m_Var, { "Off", "All", "Only friends" }); HelpMarker("Which teammates the glow will ignore drawing on");
					Checkbox("Wearable glow###PlayerWearableGlow", &Vars::Glow::Players::Wearables.m_Var); HelpMarker("Will draw glow on player cosmetics");
					Checkbox("Weapon glow###PlayerWeaponGlow", &Vars::Glow::Players::Weapons.m_Var); HelpMarker("Will draw glow on player weapons");
					WSlider("Glow alpha###PlayerGlowAlpha", &Vars::Glow::Players::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
					WCombo("Glow colour###GlowColour", &Vars::Glow::Players::Color.m_Var, { "Team", "Health" }); HelpMarker("Which colour the glow will draw");
				}

				EndTable();
			}
			break;
		}

		// Visuals: Building
		case VisualsTab::Buildings:
		{
			if (BeginTable("VisualsBuildingsTable", 3))
			{
				/* Column 1 */
				TableNextColumn();
				{
					SectionTitle("Building ESP");
					Checkbox("Building ESP###BuildinGESPSwioifas", &Vars::ESP::Buildings::Active.m_Var); HelpMarker("Will draw useful information/indicators on buildings");
					Checkbox("Ignore team buildings###BuildingESPIgnoreTeammates", &Vars::ESP::Buildings::IgnoreTeammates.m_Var); HelpMarker("Whether or not to draw ESP on your teams buildings");
					Checkbox("Name ESP###BuildingNameESP", &Vars::ESP::Buildings::Name.m_Var); HelpMarker("Will draw the players name");
					Checkbox("Name ESP box###BuildingNameESPBox", &Vars::ESP::Buildings::NameBox.m_Var); HelpMarker("Will draw a box around the buildings name to make it stand out");
					Checkbox("Health bar###Buildinghelathbar", &Vars::ESP::Buildings::HealthBar.m_Var); HelpMarker("Will draw a bar visualizing how much health the building has");
					Checkbox("Health text###buildinghealth", &Vars::ESP::Buildings::Health.m_Var); HelpMarker("Will draw the building's health, as well as its max health");
					Checkbox("Building owner###Buildingowner", &Vars::ESP::Buildings::Owner.m_Var); HelpMarker("Shows who built the building");
					Checkbox("Building level###Buildinglevel", &Vars::ESP::Buildings::Level.m_Var); HelpMarker("Will draw what level the building is");
					Checkbox("Building condition###Buildingconditions", &Vars::ESP::Buildings::Cond.m_Var); HelpMarker("Will draw what conditions the building is under");
					Checkbox("Lines###buildinglines", &Vars::ESP::Buildings::Lines.m_Var); HelpMarker("Draws lines from the local players position to the buildings position");
					WCombo("Box###PBuildingBoxESP", &Vars::ESP::Buildings::Box.m_Var, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on buildings");
					Checkbox("Dlights###PlayerDlights", &Vars::ESP::Buildings::Dlights.m_Var); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
					WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Buildings::DlightRadius.m_Var, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
					WSlider("ESP alpha###BuildingESPAlpha", &Vars::ESP::Buildings::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the ESP should be");
				}

				/* Column 2 */
				TableNextColumn();
				{
					static std::vector chamOptions {
							"Local",
							"Friends",
							"Enemies",
							"Teammates",
							"Target"
					};

					static int currentSelected = 0; // 0 - local, 1 - friends, 2 - enemy, 3 - team
					static std::vector pchamsMaterials { "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic" };

					SectionTitle("Building Chams");
					Checkbox("Building chams###BuildingChamsBox", &Vars::Chams::Buildings::Active.m_Var); HelpMarker("Building chams master switch");

					WCombo("Config", &currentSelected, chamOptions);
					switch (currentSelected) // please find a better way to do this, i have tried so many things and i cant get it to work properly
					{
					case 0:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Local.chamsActive, &Vars::Chams::Buildings::Local.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::Buildings::Local.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
						ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Local.fresnelBase);
						break;
					}
					case 1:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Friend.chamsActive, &Vars::Chams::Buildings::Friend.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::Buildings::Friend.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
						ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Friend.fresnelBase);
						break;
					}
					case 2:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Enemy.chamsActive, &Vars::Chams::Buildings::Enemy.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::Buildings::Enemy.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
						ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Enemy.fresnelBase);
						break;
					}
					case 3:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Team.chamsActive, &Vars::Chams::Buildings::Team.showObstructed, }, "", "Options");
						WCombo("Material", &Vars::Chams::Buildings::Team.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
						ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Team.fresnelBase);
						break;
					}
					case 4:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::Buildings::Target.chamsActive, &Vars::Chams::Buildings::Target.showObstructed, }, "", "Options");
						WCombo("Material", &Vars::Chams::Buildings::Target.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the building");
						ColorPickerL("Fresnel base colour", Vars::Chams::Buildings::Target.fresnelBase);
						break;
					}
					}
				}

				/* Column 3 */
				TableNextColumn();
				{
					SectionTitle("Building Glow");
					Checkbox("Building glow###BuildiongGlowButton", &Vars::Glow::Buildings::Active.m_Var);
					Checkbox("Ignore team buildings###buildingglowignoreteams", &Vars::Glow::Buildings::IgnoreTeammates.m_Var);
					WSlider("Glow alpha###BuildingGlowAlpha", &Vars::Glow::Buildings::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
					WCombo("Glow colour###GlowColourBuildings", &Vars::Glow::Buildings::Color.m_Var, { "Team", "Health" });
				}

				EndTable();
			}
			break;
		}

		// Visuals: World
		case VisualsTab::World:
		{
			if (BeginTable("VisualsWorldTable", 3))
			{
				/* Column 1 */
				TableNextColumn();
				{
					SectionTitle("World ESP");
					Checkbox("World ESP###WorldESPActive", &Vars::ESP::World::Active.m_Var); HelpMarker("World ESP master switch");
					Checkbox("Health packs###WorldESPHealthPacks", &Vars::ESP::World::HealthText.m_Var); HelpMarker("Will draw ESP on health packs");
					ColorPickerL("Health pack colour", Colors::Health); HelpMarker("Color for health pack ESP");
					Checkbox("Ammo packs###WorldESPAmmoPacks", &Vars::ESP::World::AmmoText.m_Var); HelpMarker("Will draw chams on ammo packs");
					ColorPickerL("Ammo pack colour", Colors::Ammo); HelpMarker("Color for ammo pack ESP");
					WSlider("ESP alpha###WordlESPAlpha", &Vars::ESP::World::Alpha.m_Var, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the world ESP should be");
				}

				/* Column 2 */
				TableNextColumn();
				{
					SectionTitle("World Chams");
					Checkbox("World chams###woldchamsbut", &Vars::Chams::World::Active.m_Var);

					static std::vector chamOptions {
						"Health",
						"Ammo",
						"Projectiles"
					};

					static int currentSelected = 0;
					static std::vector pchamsMaterials { "None", "Shaded", "Shiny", "Flat", "Brick", "Blur", "Fresnel", "Plastic" };

					WCombo("Config", &currentSelected, chamOptions);
					switch (currentSelected) // please find a better way to do this, i have tried so many things and i cant get it to work properly
					{
					case 0:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::World::Health.chamsActive, &Vars::Chams::World::Health.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::World::Health.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
						ColorPickerL("Fresnel base colour", Vars::Chams::World::Health.fresnelBase);
						break;
					}
					case 1:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::World::Ammo.chamsActive, &Vars::Chams::World::Ammo.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::World::Ammo.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
						ColorPickerL("Fresnel base colour", Vars::Chams::World::Ammo.fresnelBase);
						break;
					}
					case 2:
					{
						MultiCombo({ "Active", "Obstructed" }, { &Vars::Chams::World::Projectiles.chamsActive, &Vars::Chams::World::Projectiles.showObstructed }, "", "Options");
						WCombo("Material", &Vars::Chams::World::Projectiles.drawMaterial, pchamsMaterials); HelpMarker("Which material the chams will apply to the player");
						ColorPickerL("Fresnel base colour", Vars::Chams::World::Projectiles.fresnelBase);
						WCombo("Team###WorldChamsProjectiles", &Vars::Chams::World::Projectilez.m_Var, { "All", "Enemy only" });
						break;

					}
					}
				}

				/* Column 3 */
				TableNextColumn();
				{
					SectionTitle("World Glow");
					Checkbox("World glow###Worldglowbutton", &Vars::Glow::World::Active.m_Var);
					Checkbox("Health packs###worldhealthpackglow", &Vars::Glow::World::Health.m_Var);
					Checkbox("Ammo packs###worldammopackglow", &Vars::Glow::World::Ammo.m_Var);
					WCombo("Projectile glow###teamprojectileglow", &Vars::Glow::World::Projectiles.m_Var, { "Off", "All", "Only enemies" });
					WSlider("Glow alpha###WorldGlowAlpha", &Vars::Glow::World::Alpha.m_Var, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				}

				EndTable();
			}
			break;
		}

		// Visuals: Font
		case VisualsTab::Font: break;

		// Visuals: Misc
		case VisualsTab::Misc:
		{
			if (BeginTable("VisualsMiscTable", 3))
			{
				/* Column 1 */
				TableNextColumn();
				{
					SectionTitle("World & UI");
					WSlider("Field of view", &Vars::Visuals::FieldOfView.m_Var, 70, 150, "%d"); HelpMarker("How many degrees of field of vision you would like");
					WCombo("Vision modifiers", &Vars::Visuals::Vision.m_Var, { "Off", "Pyrovision", "Halloween", "Romevision" }); HelpMarker("Vision modifiers");
					MultiCombo({ "World", "Sky", "Prop Wireframe" }, { &Vars::Visuals::WorldModulation.m_Var, &Vars::Visuals::SkyModulation.m_Var, &Vars::Visuals::PropWireframe.m_Var }, "Select which types of modulation you want to enable", "Modulations");
					ColorPickerL("World modulation colour", Colors::WorldModulation);
					ColorPickerL("Sky modulation colour", Colors::SkyModulation, 1);
					ColorPickerL("Prop modulation colour", Colors::StaticPropModulation, 2);
					MultiCombo({ "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "View Punch" }, { &Vars::Visuals::RemoveScope.m_Var, &Vars::Visuals::RemoveZoom.m_Var, &Vars::Visuals::RemoveDisguises.m_Var, &Vars::Visuals::RemoveTaunts.m_Var, &Vars::Misc::DisableInterpolation.m_Var, &Vars::Visuals::RemovePunch.m_Var }, "Select what you want to remove", "Removals");
					MultiCombo({ "Aimbot Crosshair", "Render Proj Line", "Bullet Tracers", "Viewmodel Aimbot", "Weapon Sway", "Move sim line" }, { &Vars::Visuals::CrosshairAimPos.m_Var, &Vars::Visuals::AimPosSquare.m_Var, &Vars::Visuals::BulletTracer.m_Var, &Vars::Visuals::AimbotViewmodel.m_Var, &Vars::Visuals::ViewmodelSway.m_Var, &Vars::Visuals::MoveSimLine.m_Var }, "What misc visual features should be run", "Misc");
					ColorPickerL("Bullet tracer colour", Colors::BulletTracer);
					MultiCombo({ "Votes (Console)", "Votes (Text)", "Votes (Chat)", "Votes (Party)", "Damage Logs (Console)", "Damage Logs (Text)", "Damage Logs (Chat)", "Class Changes (Text)", "Class Changes (Chat)" }, { &Vars::Misc::VoteRevealerConsole.m_Var, &Vars::Misc::VoteRevealerText.m_Var, &Vars::Misc::VoteRevealerChat.m_Var, &Vars::Misc::VoteRevealerParty.m_Var, &Vars::Visuals::damageLoggerConsole.m_Var, &Vars::Visuals::damageLoggerText.m_Var, &Vars::Visuals::damageLoggerChat.m_Var, &Vars::Visuals::ChatInfoText.m_Var, &Vars::Visuals::ChatInfoChat.m_Var }, "What & How should events be logged", "Event Logging");
					ColorPickerL("GUI Notif Background", Colors::NotifBG);
					ColorPickerL("GUI Notif Outline", Colors::NotifOutline, 1);
					ColorPickerL("GUI Notif Colour", Colors::NotifText, 2);
					WSlider("GUI Notif Time", &Vars::Visuals::despawnTime.m_Var, 0.5f, 3.f, "%.1f");
					WCombo("Particle tracer", &Vars::Visuals::ParticleTracer.m_Var, { "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom" });
					if (Vars::Visuals::ParticleTracer.m_Var == 9)
					{
						WInputText("Custom Tracer", &Vars::Visuals::ParticleName); HelpMarker("If you want to use a custom particle tracer");
					}
					if (Vars::Visuals::BulletTracer.m_Var)
					{
						Checkbox("Rainbow tracers", &Vars::Visuals::BulletTracerRainbow.m_Var); HelpMarker("Bullet tracer color will be dictated by a changing color");
					}
					if (Vars::Visuals::RemoveScope.m_Var)
					{
						Checkbox("Noscope lines", &Vars::Visuals::ScopeLines.m_Var); HelpMarker("Will draw a custom overlay");
						ColorPickerL("Inner line color", Colors::NoscopeLines1);
						ColorPickerL("Outer line color", Colors::NoscopeLines2, 1);
					}
					Checkbox("Pickup Timers", &Vars::Visuals::PickupTimers.m_Var); HelpMarker("Displays the respawn time of health and ammo packs");
					Checkbox("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.m_Var); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
					ColorPickerL("Hitbox matrix face colour", Colors::HitboxFace);
					ColorPickerL("Hitbox matrix edge colour", Colors::HitboxEdge);
					Checkbox("Clear Hitboxes", &Vars::Aimbot::Global::clearPreviousHitbox.m_Var); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
					WSlider("Hitbox Draw Time", &Vars::Aimbot::Global::hitboxTime.m_Var, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");

					WCombo("Spectator list", &Vars::Visuals::SpectatorList.m_Var, { "Off", "Draggable", "Static", "Static + Avatars" });

					SectionTitle("BEAMS (I love beams)", 20);
					{
						using namespace Vars::Visuals;

						Checkbox("Enable beams", &BEAMS::Active.m_Var); HelpMarker("I LOVE BEAMS!!!!!!!!!!");
						Checkbox("Rainbow beams", &BEAMS::Rainbow.m_Var);
						ColorPickerL("Beam colour", BEAMS::BeamColour);
						Checkbox("Custom model", &BEAMS::UseCustomModel.m_Var);
						if (BEAMS::UseCustomModel.m_Var)
						{
							WInputText("Model", &BEAMS::Model);
						}
						WSlider("Beam lifespan", &BEAMS::Life.m_Var, 0.0f, 10.f);
						WSlider("Beam width", &BEAMS::Width.m_Var, 0.0f, 10.f);
						WSlider("Beam end width", &BEAMS::EndWidth.m_Var, 0.0f, 10.f);
						WSlider("Beam fade length", &BEAMS::FadeLength.m_Var, 0.0f, 30.f);
						WSlider("Beam amplitude", &BEAMS::Amplitude.m_Var, 0.0f, 10.f);
						WSlider("Beam brightness", &BEAMS::Brightness.m_Var, 0.0f, 255.f);
						WSlider("Beam speed", &BEAMS::Speed.m_Var, 0.0f, 5.f);

						// TODO: Reward this ugly code
						{
							static const char* flags[]{ "FBEAM_STARTENTITY", "FBEAM_ENDENTITY","FBEAM_FADEIN","FBEAM_FADEOUT","FBEAM_SINENOISE","FBEAM_SOLID","FBEAM_SHADEIN","FBEAM_SHADEOUT","FBEAM_ONLYNOISEONCE","FBEAM_NOTILE","FBEAM_USE_HITBOXES","FBEAM_STARTVISIBLE","FBEAM_ENDVISIBLE","FBEAM_ISACTIVE","FBEAM_FOREVER","FBEAM_HALOBEAM","FBEAM_REVERSED", };
							static int fontflags[]{ 0x00000001, 0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,0x00000100,0x00000200,0x00000400,0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,0x00010000 };
							static bool flagbools[17]{
								(BEAMS::Flags.m_Var & 0x00000001) != 0,
								(BEAMS::Flags.m_Var & 0x00000002) != 0,
								(BEAMS::Flags.m_Var & 0x00000004) != 0,
								(BEAMS::Flags.m_Var & 0x00000008) != 0,
								(BEAMS::Flags.m_Var & 0x00000010) != 0,
								(BEAMS::Flags.m_Var & 0x00000020) != 0,
								(BEAMS::Flags.m_Var & 0x00000040) != 0,
								(BEAMS::Flags.m_Var & 0x00000080) != 0,
								(BEAMS::Flags.m_Var & 0x00000100) != 0,
								(BEAMS::Flags.m_Var & 0x00000200) != 0,
								(BEAMS::Flags.m_Var & 0x00000400) != 0,
								(BEAMS::Flags.m_Var & 0x00000800) != 0,
								(BEAMS::Flags.m_Var & 0x00001000) != 0,
								(BEAMS::Flags.m_Var & 0x00002000) != 0,
								(BEAMS::Flags.m_Var & 0x00004000) != 0,
								(BEAMS::Flags.m_Var & 0x00008000) != 0,
								(BEAMS::Flags.m_Var & 0x00010000) != 0
							};
							static std::string previewValue = "None";
							std::vector < std::string > vec;
							if (ImGui::BeginCombo("Beam flags", previewValue.c_str()))
							{
								previewValue = "";
								for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
								{
									ImGui::Selectable(flags[i], &flagbools[i]);
									if (flagbools[i])
										vec.push_back(flags[i]);
								}
								for (size_t i = 0; i < vec.size(); i++)
								{
									if (vec.size() == 1)
										previewValue += vec.at(i);
									else if (!(i == vec.size() - 1))
										previewValue += vec.at(i) + ",";
									else
										previewValue += vec.at(i);
								}
								ImGui::EndCombo();
							}
							for (size_t i = 0; i < IM_ARRAYSIZE(flags); i++)
							{
								if (flagbools[i])
								{
									BEAMS::Flags.m_Var |= fontflags[i];
								}
								else
								{
									BEAMS::Flags.m_Var &= ~fontflags[i];
								}
							}
						}
					}

					SectionTitle("Viewmodel Offset", 20);
					WSlider("VM Off X", &Vars::Visuals::VMOffsets.x, -45.f, 45.f);
					WSlider("VM Off Y", &Vars::Visuals::VMOffsets.y, -45.f, 45.f);
					WSlider("VM Off Z", &Vars::Visuals::VMOffsets.z, -45.f, 45.f);
					WSlider("VM Roll", &Vars::Visuals::VMRoll.m_Var, -180, 180);

					SectionTitle("DT Indicator", 20);
					WCombo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.m_Var, { "Off", "Default", "Nitro", "Rijin" }); HelpMarker("Which style to do the bar style");
					ColorPickerL("DT charging right", Colors::DTBarIndicatorsCharging.endColour);
					ColorPickerL("DT charging left", Colors::DTBarIndicatorsCharging.startColour, 1);
					if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3)
					{
						WSlider("DT Bar height###dtBHeightNitro", &Vars::Misc::CL_Move::DTBarScaleY.m_Var, 1, 25);
						ColorPickerL("DT charged right", Colors::DTBarIndicatorsCharged.endColour);
						ColorPickerL("DT charged left", Colors::DTBarIndicatorsCharged.startColour, 1);
						WSlider("DT Bar width###dtBWidthNitro", &Vars::Misc::CL_Move::DTBarScaleX.m_Var, 100, 1000);
					}
					else
					{
						WSlider("DT Bar height###dtBHeight", &Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var, 1, 30);
						ColorPickerL("DT charged right", Colors::DTBarIndicatorsCharged.endColour);
						ColorPickerL("DT charged left", Colors::DTBarIndicatorsCharged.startColour, 1);
						WSlider("DT Bar width###dtBWidth", &Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var, 1, 30);
					}

					SectionTitle("Attribute Changer", 20);

					static std::vector unuEffects{
						"None",
						"Hot",
						"Isotope",
						"Cool",
						"Energy orb"
					};
					static std::vector unuEffects2{
						"None",
						"Hot",
						"Isotope",
						"Cool",
						"Energy orb"
					};

					if (WCombo("Unusual effect 1", &unuPrimary, unuEffects))
					{
						switch (unuPrimary)
						{
						case 0:
							Vars::Visuals::Skins::Particle.m_Var = 0;
							break;
						case 1:
							Vars::Visuals::Skins::Particle.m_Var = 701;
							break;
						case 2:
							Vars::Visuals::Skins::Particle.m_Var = 702;
							break;
						case 3:
							Vars::Visuals::Skins::Particle.m_Var = 703;
							break;
						case 4:
							Vars::Visuals::Skins::Particle.m_Var = 704;
							break;
						default:
							break;
						}
					}
					HelpMarker("The first unusual effect to be applied to the weapon");
					
					if (WCombo("Unusual effect 2", &unuSecondary, unuEffects2))
					{
						switch (unuSecondary)
						{
						case 0:
							Vars::Visuals::Skins::Effect.m_Var = 0;
							break;
						case 1:
							Vars::Visuals::Skins::Effect.m_Var = 701;
							break;
						case 2:
							Vars::Visuals::Skins::Effect.m_Var = 702;
							break;
						case 3:
							Vars::Visuals::Skins::Effect.m_Var = 703;
							break;
						case 4:
							Vars::Visuals::Skins::Effect.m_Var = 704;
							break;
						default:
							break;
						}
					}
					HelpMarker("The second unusual effect to be applied to the weapon");

					static std::vector sheens {
						"None",
						"Team shine",
						"Deadly daffodil",
						"Manndarin",
						"Mean green",
						"Agonizing emerald",
						"Villainous violet",
						"Hot rod"
					};
					WCombo("Sheen", &Vars::Visuals::Skins::Sheen.m_Var, sheens); HelpMarker("Which sheen to apply to the weapon");
					Checkbox("Style override", &Vars::Visuals::Skins::Override.m_Var);

					if (Button("Apply", ImVec2(45, 20)))
					{
						g_AttributeChanger.m_bSet = true;
					}
					SameLine();
					if (Button("Save", ImVec2(45, 20)))
					{
						g_AttributeChanger.m_bSave = true;
					}
					SameLine();
					if (Button("Load", ImVec2(44, 20)))
					{
						g_AttributeChanger.m_bLoad = true;
					}

					SectionTitle("Ragdoll effects", 20);
					Checkbox("Enemy only###RagdollEnemyOnly", &Vars::Visuals::RagdollEffects::EnemyOnly.m_Var); HelpMarker("Only runs it on enemies");
					MultiCombo({ "Burning", "Electrocuted", "Become ash", "Dissolve" }, { &Vars::Visuals::RagdollEffects::Burning.m_Var, &Vars::Visuals::RagdollEffects::Electrocuted.m_Var, &Vars::Visuals::RagdollEffects::BecomeAsh.m_Var, &Vars::Visuals::RagdollEffects::Dissolve.m_Var }, "Ragdoll particle effects", "Effects###RagdollEffects");
					if (Checkbox("Gold ragdoll", &Vars::Visuals::RagdollEffects::Gold.m_Var))
					{
						Vars::Visuals::RagdollEffects::Ice.m_Var = false;
					}
					HelpMarker("Will make their ragdoll gold");
					if (Checkbox("Ice ragdoll", &Vars::Visuals::RagdollEffects::Ice.m_Var))
					{
						Vars::Visuals::RagdollEffects::Gold.m_Var = false;
					}
					HelpMarker("Will make their ragdoll ice");
					
					SectionTitle("Freecam", 20);
					InputKeybind("Freecam Key", Vars::Visuals::FreecamKey);  HelpMarker("Allows you to freely move your camera when holding the key");
					WSlider("Freecam Speed", &Vars::Visuals::FreecamSpeed.m_Var, 1.f, 20.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Movement speed of freecam");
					
					SectionTitle("Camera", 20);
					WCombo("Camera mode", &Vars::Visuals::CameraMode.m_Var, { "Off", "Mirror", "Spy" }); HelpMarker("What the camera should display");
					WSlider("Camera FOV", &Vars::Visuals::CameraFOV.m_Var, 40.f, 130.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("FOV of the camera window");
				}

				/* Column 2 */
				TableNextColumn();

				/* Column 3 */
				TableNextColumn();

				EndTable();
			}
			break;
		}

		case VisualsTab::Radar: break;
	}
}

/* Tab: HvH */
void CMenu::MenuHvH()
{
	using namespace ImGui;
	if (BeginTable("HvhTable", 2))
	{
		/* Column 1 */
		TableNextColumn();
		{

		}

		/* Column 2 */
		TableNextColumn();
		{

		}

		EndTable();
	}
}

/* Tab: Misc */
void CMenu::MenuMisc()
{
	using namespace ImGui;
	if (BeginTable("MiscTable", 3))
	{
		/* Column 1 */
		TableNextColumn();
		{
			SectionTitle("Movement");
			Checkbox("No push", &Vars::Misc::NoPush.m_Var); HelpMarker("Will make teammates unable to push you around");
			Checkbox("Bunnyhop", &Vars::Misc::AutoJump.m_Var); HelpMarker("Will jump as soon as you touch the ground again, keeping speed between jumps");
			if (Vars::Misc::AutoJump.m_Var)
			{
				WCombo("Autostrafe", &Vars::Misc::AutoStrafe.m_Var, { "Off", "Legit", "Directional" }); HelpMarker("Will strafe for you in air automatically so that you gain speed");
			}
			Checkbox("Edge jump", &Vars::Misc::EdgeJump.m_Var); HelpMarker("Will jump at the very end of whatever platform you're on, allowing you to perfectly make longer jumps.");
			if (Vars::Misc::EdgeJump.m_Var)
			{
				InputKeybind("Edge jump key", Vars::Misc::EdgeJumpKey, true);  HelpMarker("Edge jump bind, leave as None for always on");
			}
			Checkbox("Auto rocket jump", &Vars::Misc::AutoRocketJump.m_Var); HelpMarker("Will rocket jump at the angle you're looking at when you press mouse2 with a rocket launcher");
			Checkbox("Anti-AFK", &Vars::Misc::AntiAFK.m_Var); HelpMarker("Will make you jump every now and then so you don't get kicked for idling");
			Checkbox("Taunt slide", &Vars::Misc::TauntSlide.m_Var); HelpMarker("Allows you to input in taunts");
			Checkbox("Taunt control", &Vars::Misc::TauntControl.m_Var); HelpMarker("Gives full control if enabled with taunt slide");
			WCombo("Crouch speed", &Vars::Misc::Roll.m_Var, { "Off", "Backwards", "Fake forward" }); HelpMarker("Allows you to go at normal walking speed when crouching (affects many things, use with caution)");

			SectionTitle("Misc", 20);
			Checkbox("Auto-Vote", &Vars::Misc::AutoVote.m_Var); HelpMarker("Automatically vote F2 on votes called against friends/ignored and F1 on votes called by friends/randoms/on randoms");
			MultiCombo({ "Console", "Text", "Chat", "Party" }, { &Vars::Misc::AnnounceVotesConsole.m_Var, &Vars::Misc::AnnounceVotesText.m_Var, &Vars::Misc::AnnounceVotesChat.m_Var, &Vars::Misc::AnnounceVotesParty.m_Var }, "If and where should votes be announced", "Vote announcer");
			WCombo("Vote announcement mode", &Vars::Misc::AnnounceVotes.m_Var, { "Basic", "Detailed" });
			MultiCombo({ "Chat Censor", "Anti-Autobal", "sv_cheats Bypass", "Pseudo Spectator", "Noisemaker Spammer" }, { &Vars::Misc::ChatCensor.m_Var, &Vars::Misc::AntiAutobal.m_Var, &Vars::Misc::CheatsBypass.m_Var, &Vars::Misc::ExtendFreeze.m_Var, &Vars::Misc::NoisemakerSpam.m_Var }, "Enable/Disable Misc. Options", "Misc");
			WCombo("Chat spam", &Vars::Misc::ChatSpam.m_Var, { "Off", "Fedoraware", "Lmaobox", "Cathook" });
			WCombo("Pick Class", &Vars::Misc::AutoJoin.m_Var, { "Off", "Scout", "Soldier", "Pyro", "Demoman", "Heavy", "Engineer", "Medic", "Sniper", "Spy" }); HelpMarker("Automatically joins the given class");
			Checkbox("Rage retry", &Vars::Misc::RageRetry.m_Var); HelpMarker("Will automatically reconnect when your health is low");
			if (Vars::Misc::RageRetry.m_Var)
			{
				WSlider("Rage Retry health", &Vars::Misc::RageRetryHealth.m_Var, 1, 99, "%d%%"); HelpMarker("Minimum health percentage that will cause a retry");
			}
			//Checkbox("Cat identify", &Vars::Misc::BeCat.m_Var); HelpMarker("Will mark you as a cathook instance to other cathook instances (basically catbots)");

			Checkbox("Ping reducer", &Vars::Misc::PingReducer.m_Var); HelpMarker("Reduces your ping on the scoreboard");
			if (Vars::Misc::PingReducer.m_Var)
			{
				WSlider("Target ping", &Vars::Misc::PingTarget.m_Var, 0, 200); HelpMarker("Target ping that should be reached");
			}
			Checkbox("Killstreak weapon", &Vars::Misc::KillstreakWeapon.m_Var); HelpMarker("Enables the killstreak counter on any weapon");

			SectionTitle("Party Networking", 20);
			Checkbox("Enable", &Vars::Misc::PartyNetworking.m_Var); HelpMarker("Enables party networking between Fedoraware users");
			Checkbox("Party crasher", &Vars::Misc::PartyCrasher.m_Var); HelpMarker("Annoy your friends by crashing their game");
			InputKeybind("Party marker", Vars::Misc::PartyMarker, true);  HelpMarker("Sends a marker to other Fedoraware users in your party");
			Checkbox("Party ESP", &Vars::Misc::PartyESP.m_Var); HelpMarker("Sends player locations to your party members");
		}

		/* Column 2 */
		TableNextColumn();
		{
			SectionTitle("Tickbase Exploits");
			Checkbox("Doubletap", &Vars::Misc::CL_Move::Enabled.m_Var); HelpMarker("Shifts ticks when shooting for a rapid-fire effect");
			ColorPickerL("DT bar outline colour", Colors::DtOutline);
			InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey); HelpMarker("Recharges ticks for shifting");
			InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey); HelpMarker("Shifts ticks to warp");
			if (Vars::Misc::CL_Move::DTMode.m_Var == 0 || Vars::Misc::CL_Move::DTMode.m_Var == 2)
			{
				InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey); HelpMarker("Only doubletap when the key is pressed. Leave as (None) for always active.");
			}

			MultiCombo({ "Recharge While Dead", "Auto Recharge", "Wait for DT", "Anti-warp", "Avoid airborne" }, { &Vars::Misc::CL_Move::RechargeWhileDead.m_Var, &Vars::Misc::CL_Move::AutoRecharge.m_Var, &Vars::Misc::CL_Move::WaitForDT.m_Var, &Vars::Misc::CL_Move::AntiWarp.m_Var, &Vars::Misc::CL_Move::NotInAir.m_Var }, "Enable various features regarding tickbase exploits", "Options");
			WCombo("DT Mode", &Vars::Misc::CL_Move::DTMode.m_Var, { "On key", "Always", "Disable on key", "Disabled" }); HelpMarker("How should DT behave");
			WSlider("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.m_Var, 1, 24, "%d"); HelpMarker("How many ticks to shift");
			Checkbox("SpeedHack", &Vars::Misc::CL_Move::SEnabled.m_Var); HelpMarker("Speedhack Master Switch");
			if (Vars::Misc::CL_Move::SEnabled.m_Var)
			{
				WSlider("SpeedHack Factor", &Vars::Misc::CL_Move::SFactor.m_Var, 1, 66, "%d");
			}
			HelpMarker("High values are not recommended");

			SectionTitle("HvH", 20);
			Checkbox("Anti-aim", &Vars::AntiHack::AntiAim::Active.m_Var);
			WCombo("Pitch", &Vars::AntiHack::AntiAim::Pitch.m_Var, { "None", "Up", "Down", "Fake up", "Fake down", "Random" }); HelpMarker("Which way to look up/down");
			WCombo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.m_Var, { "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge", "On Hurt" }); HelpMarker("Which way to look horizontally");
			WCombo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.m_Var, { "None", "Left", "Right", "Backwards", "Random", "Spin", "Edge", "On Hurt" }); HelpMarker("Which way to appear to look horizontally");
			if (Vars::AntiHack::AntiAim::YawFake.m_Var == 5 || Vars::AntiHack::AntiAim::YawReal.m_Var == 5)
			{
				WSlider("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.m_Var, -30.f, 30.f, "%.1f", 0); HelpMarker("You spin me right 'round, baby, right 'round");
			}
			if (Vars::AntiHack::AntiAim::Pitch.m_Var == 5 || Vars::AntiHack::AntiAim::YawFake.m_Var == 4 || Vars::AntiHack::AntiAim::YawReal.m_Var == 4)
			{
				WSlider("Random Interval", &Vars::AntiHack::AntiAim::RandInterval.m_Var, 0, 100, "%d"); HelpMarker("How often the random Anti-Aim should update");
			}
			Checkbox("Resolver", &Vars::AntiHack::Resolver::Resolver.m_Var); HelpMarker("Enables Anti-aim resolver in the playerlist");
			MultiCombo({ "AntiBackstab", "HidePitchOS", "LegJitter", "No Overlap" }, { &Vars::AntiHack::AntiAim::AntiBackstab.m_Var, &Vars::AntiHack::AntiAim::invalidshootpitch.m_Var, &Vars::AntiHack::AntiAim::legjitter.m_Var, &Vars::AntiHack::AntiAim::AntiOverlap.m_Var }, "", "Misc Anti-Aim");
			WCombo("Fakelag Mode###FLmode", &Vars::Misc::CL_Move::FakelagMode.m_Var, { "None", "Plain", "Random", "Velocity Based" }); HelpMarker("Controls how fakelag will be controlled.");
			if (Vars::Misc::CL_Move::FakelagMode.m_Var > 0)
			{
				Vars::Misc::CL_Move::Fakelag.m_Var = true;
			}
			else
			{
				Vars::Misc::CL_Move::Fakelag.m_Var = false;
			}

			if (Vars::Misc::CL_Move::Fakelag.m_Var)
			{
				SectionTitle("Fakelag", 20);
				if (Vars::Misc::CL_Move::FakelagMode.m_Var == 1 || Vars::Misc::CL_Move::FakelagMode.m_Var == 3)
				{
					WSlider("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 22, "%d"); HelpMarker("How much lag you should fake(?)");
					if (Vars::Misc::CL_Move::FakelagMode.m_Var == 1)
					{
						Checkbox("Fakelag on key", &Vars::Misc::CL_Move::FakelagOnKey.m_Var); HelpMarker("Fakelag will only activate when an assigned key is held");
						if (Vars::Misc::CL_Move::FakelagOnKey.m_Var)
						{
							InputKeybind("Fakelag key", Vars::Misc::CL_Move::FakelagKey); HelpMarker("The key to activate fakelag as long as it's held");
						}
					}
				}
				if (Vars::Misc::CL_Move::FakelagMode.m_Var == 2)
				{
					WSlider("Random max###flRandMax", &Vars::Misc::CL_Move::FakelagMax.m_Var, Vars::Misc::CL_Move::FakelagMin.m_Var + 1, 22, "%d"); HelpMarker("Maximum random fakelag value");
					WSlider("Random min###flRandMin", &Vars::Misc::CL_Move::FakelagMin.m_Var, 1, Vars::Misc::CL_Move::FakelagMax.m_Var - 1, "%d"); HelpMarker("Minimum random fakelag value");
				}
			}

			SectionTitle("Auto peek", 20);
			InputKeybind("Autopeek Key", Vars::Misc::CL_Move::AutoPeekKey); HelpMarker("Hold this key while peeking and use A/D to set the peek direction");
			WSlider("Max Distance", &Vars::Misc::CL_Move::AutoPeekDistance.m_Var, 50.f, 400.f, "%.0f", 0); HelpMarker("Maximum distance that auto peek can walk");
			Checkbox("Free move", &Vars::Misc::CL_Move::AutoPeekFree.m_Var); HelpMarker("Allows you to move freely while peeking");
		}

		/* Column 3 */
		TableNextColumn();
		{
			SectionTitle("Discord RPC");
			Checkbox("Discord RPC", &Vars::Misc::Discord::EnableRPC.m_Var); HelpMarker("Enable Discord Rich Presence");
			Checkbox("Include map", &Vars::Misc::Discord::IncludeMap.m_Var); HelpMarker("Should Discord Rich Presence contain current map name?");
			Checkbox("Include class", &Vars::Misc::Discord::IncludeClass.m_Var); HelpMarker("Should Discord Rich Presence contain current class?");
			Checkbox("Include timestamp", &Vars::Misc::Discord::IncludeTimestamp.m_Var); HelpMarker("Should time since you started playing TF2 be included?");
			WCombo("Image Options", &Vars::Misc::Discord::WhatImagesShouldBeUsed.m_Var, { "Big fedora + Small TF2", "Big TF2 + Small fedora" });

			SectionTitle("Steam RPC", 20);
			Checkbox("Steam RPC", &Vars::Misc::Steam::EnableRPC.m_Var); HelpMarker("Enable Steam Rich Presence"); HelpMarker("Enable Steam Rich Presence");
			WCombo("Match group", &Vars::Misc::Steam::MatchGroup.m_Var, { "Special Event", "MvM Mann Up", "Competitive", "Casual", "MvM Boot Camp" }); HelpMarker("Which match group should be used?");
			Checkbox("Override in menu", &Vars::Misc::Steam::OverrideMenu.m_Var); HelpMarker("Override match group to \"Main Menu\" when in main menu");
			WCombo("Map text", &Vars::Misc::Steam::MapText.m_Var, { "Custom", "Fedoraware", "Figoraware", "Meowhook.club", "Rathook.cc", "Nitro.tf" }); HelpMarker("Which map text should be used?");
			if (Vars::Misc::Steam::MapText.m_Var == 0)
			{
				WInputText("Custom map text", &Vars::Misc::Steam::CustomText.m_Var); HelpMarker("For when \"Custom\" is selcted in \"Map text\". Sets custom map text.");
			}
			WInputInt("Group size", &Vars::Misc::Steam::GroupSize.m_Var); HelpMarker("Sets party size");

			SectionTitle("Utilities", 20);

			auto a = GetColumnWidth() - 12;
			if (Button("Full update", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("cl_fullupdate");
			if (Button("Reload HUD", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("hud_reloadscheme");
			if (Button("Restart sound", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("snd_restart");
			if (Button("Stop sound", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("stopsound");
			if (Button("Status", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("status");
			if (Button("Ping", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("ping");
			if (Button("Retry", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("retry");
			if (Button("Exit", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("exit");
			if (Button("Console", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("showconsole");
			if (Button("Demo playback", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("demoui");
			if (Button("Demo trackbar", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("demoui2");
			if (Button("Itemtest", ImVec2(a, 20)))
				g_Interfaces.Engine->ClientCmd_Unrestricted("itemtest");

			if (Button("Unlock all achievements", ImVec2(a, 20)))
			{
				g_Misc.UnlockAchievements();
			}
			if (Button("Lock all achievements", ImVec2(a, 20)))
			{
				g_Misc.LockAchievements();
			}
		}

		EndTable();
	}
}

/* Tab: Configs */
void CMenu::MenuConfigs()
{
	using namespace ImGui;
	if (BeginTable("ConfigTable", 2))
	{
		/* Column 1 */
		TableNextColumn();
		{

		}

		/* Column 2 */
		TableNextColumn();
		{

		}

		EndTable();
	}
}

/* Window for the camera feature */
void CMenu::DrawCameraWindow()
{
	if (g_Interfaces.Engine->IsInGame() && Vars::Visuals::CameraMode.m_Var != 0)
	{
		int windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (Vars::Visuals::CameraMode.m_Var <= 1 || g_CameraWindow.CanDraw) {
			windowFlags |= ImGuiWindowFlags_NoBackground;
		}

		// Draw the camera window
		ImGui::SetNextWindowSize({ static_cast<float>(g_CameraWindow.ViewRect.w), static_cast<float>(g_CameraWindow.ViewRect.h) }, ImGuiCond_Once);
		ImGui::SetNextWindowPos({ static_cast<float>(g_CameraWindow.ViewRect.x), static_cast<float>(g_CameraWindow.ViewRect.y) }, ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 60.f, 60.f });
		ImGui::PushStyleColor(ImGuiCol_Text, TextDark.Value);
		if (ImGui::Begin("Camera", nullptr, windowFlags))
		{
			const ImVec2 winPos = ImGui::GetWindowPos();
			const ImVec2 winSize = ImGui::GetWindowSize();

			g_CameraWindow.ViewRect.x = static_cast<int>(winPos.x);
			g_CameraWindow.ViewRect.y = static_cast<int>(winPos.y);
			g_CameraWindow.ViewRect.w = static_cast<int>(winSize.x);
			g_CameraWindow.ViewRect.h = static_cast<int>(winSize.y);

			ImGui::End();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}

void CMenu::Render(IDirect3DDevice9* pDevice)
{
	static bool initialized = false;
	if (!ConfigLoaded) { return; }

	if (!initialized)
	{
		Init(pDevice);
		initialized = true;
	}

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	// Toggle menu
	if (GetAsyncKeyState(MENU_KEY) & 1)
	{
		// TOOD: & 8000?
		g_Menu.IsOpen = !g_Menu.IsOpen;
		g_Interfaces.Surface->SetCursorAlwaysVisible(g_Menu.IsOpen);
	}

	// Begin current frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (g_Menu.IsOpen)
	{
		ImGui::PushFont(Segoe);
		DrawMenu();
		DrawCameraWindow();

		// TODO: Draw DT-Bar, Playerlist, Spectator list etc.
		ImGui::PopFont();
	}

	// End frame and render
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}

void CMenu::Init(IDirect3DDevice9* pDevice)
{
	// Initialize ImGui and device
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowA(nullptr, "Team Fortress 2"));
	ImGui_ImplDX9_Init(pDevice);

	const auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();

	// Fonts
	{
		auto fontConfig = ImFontConfig();
		fontConfig.OversampleH = 2;

		auto wideFontConfig = ImFontConfig();
		wideFontConfig.GlyphExtraSpacing = { 1.f, 0.f };

		constexpr ImWchar fontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F,0 }; // Basic Latin, Latin Supplement and Cyrillic
		SegoeLight = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 16.0f, &fontConfig, fontRange);
		Segoe = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeui.ttf", 16.0f, &fontConfig, fontRange);
		SegoeBold = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 16.0f, &fontConfig, fontRange);

		SectionFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeui.ttf", 18.0f, &wideFontConfig, fontRange);
		TabFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuisl.ttf", 20.0f, &fontConfig, fontRange);
		TitleFont = io.Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\segoeuib.ttf", 22.0f, &fontConfig, fontRange);

		io.Fonts->Build();
	}

	// Style
	{
		// https://raais.github.io/ImStudio/

		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);	// Center window title
		style.WindowMinSize = ImVec2(700, 700);
		style.WindowPadding = ImVec2(0, 0);
		style.WindowBorderSize = 1.f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.4f);		// Center button text
		// style.CellPadding = ImVec2(12.f, 0.f);
		style.FrameBorderSize = 1.f;	// Old menu feeling
		style.FrameRounding = 0.f;
		style.ChildBorderSize = 0.f;
		style.ChildRounding = 0.f;
		style.ScrollbarSize = 4.f;
		style.GrabMinSize = 15.f;
		style.ItemSpacing = ImVec2(8.f, 5.f);

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Border] = ImColor(110, 110, 128);
		colors[ImGuiCol_WindowBg] = Background;
		colors[ImGuiCol_TitleBg] = BackgroundDark;
		colors[ImGuiCol_TitleBgActive] = Accent;
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.4f);
		colors[ImGuiCol_Button] = BackgroundLight;
		colors[ImGuiCol_ButtonHovered] = ImColor(69, 69, 77);
		colors[ImGuiCol_ButtonActive] = ImColor(82, 79, 87);
		colors[ImGuiCol_PopupBg] = BackgroundDark;
		colors[ImGuiCol_FrameBg] = ImColor(50, 50, 50);
		colors[ImGuiCol_FrameBgHovered] = ImColor(60, 60, 60);
		colors[ImGuiCol_FrameBgActive] = ImColor(60, 60, 60);
		colors[ImGuiCol_CheckMark] = Accent;
		colors[ImGuiCol_Text] = TextLight;

		colors[ImGuiCol_SliderGrab] = Accent;
		colors[ImGuiCol_SliderGrabActive] = AccentDark;
		colors[ImGuiCol_ResizeGrip] = AccentDark;
		colors[ImGuiCol_ResizeGripActive] = Accent;
		colors[ImGuiCol_ResizeGripHovered] = Accent;
		colors[ImGuiCol_Header] = ImColor(60, 60, 60);
		colors[ImGuiCol_HeaderActive] = ImColor(40, 40, 40);
		colors[ImGuiCol_HeaderHovered] = ImColor(50, 50, 50);
	}

	// Misc
	{
		titleGradient.ClearMarks();
		titleGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(0.3f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(0.5f, Accent);
		titleGradient.AddMark(0.7f, ImColor(0, 0, 0, 0));
		titleGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}

	{
		mainGradient.ClearMarks();
		mainGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(0.15f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(0.45f, Accent);
		mainGradient.AddMark(0.75f, ImColor(0, 0, 0, 0));
		mainGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}
}