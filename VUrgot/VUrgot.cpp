#include "PluginSDK.h"

PluginSetup("VUrgot")

IMenu* MainMenu;
IMenu* QMenu;
IMenu* WMenu;
IMenu* EMenu;
IMenu* RMenu;
IMenu* Misc;
IMenu* Drawings;

IMenuOption* ComboQ;
IMenuOption* ComboQBuff;
IMenuOption* HarassQ;
IMenuOption* AutoQ;
IMenuOption* ComboW;
IMenuOption* HarassW;
IMenuOption* ComboE;
IMenuOption* HarassE;
IMenuOption* UnderTowerR;
IMenuOption* DrawQ;
IMenuOption* DrawW;
IMenuOption* DrawE;
IMenuOption* DrawR;
IMenuOption* DrawReady;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("VUrgot 1.0");


	QMenu = MainMenu->AddMenu("Q Settings");
	WMenu = MainMenu->AddMenu("W Settings");
	EMenu = MainMenu->AddMenu("E Settings");
	RMenu = MainMenu->AddMenu("R Settings");
	Misc = MainMenu->AddMenu("Miscellaneous");
	Drawings = MainMenu->AddMenu("Drawings");

	ComboQ = QMenu->CheckBox("Use Q in combo", true);
	ComboQBuff = QMenu->CheckBox("Use Q if enemy has E debuff", true);
	HarassQ = QMenu->CheckBox("Use Q in harass", true);
	AutoQ = QMenu->CheckBox("Use Q automatically", false);

	ComboW = WMenu->CheckBox("Use W in combo", true);
	HarassW = WMenu->CheckBox("Use W in harass", true);

	ComboE = EMenu->CheckBox("Use E in combo", true);
	HarassE = EMenu->CheckBox("Use E in harass", true);

	UnderTowerR = RMenu->CheckBox("Use R (WIP)", true);

	DrawQ = Drawings->CheckBox("Draw Q", true);
	DrawW = Drawings->CheckBox("Draw W", true);
	DrawE = Drawings->CheckBox("Draw E", true);
	DrawR = Drawings->CheckBox("Draw R", false);
	DrawReady = Drawings->CheckBox("Draw Ready", true);

}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, false, static_cast<eCollisionFlags>(kCollidesWithMinions | kCollidesWithYasuoWall));
	W = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, true, kCollidesWithYasuoWall);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);

}


int GetEnemiesInRange(float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr && target->GetTeam() != GEntityList->Player()->GetTeam())
		{
			auto flDistance = (target->GetPosition() - GEntityList->Player()->GetPosition()).Length();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}

void Combo()
{
	if (ComboQ->Enabled() && Q->IsReady())
	{
		auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		Q->CastOnTarget(Target, kHitChanceHigh);
	}

	if (ComboQBuff->Enabled() && Q->IsReady())
	{

		for (auto Target : GEntityList->GetAllHeros(false, true))
		{
			if (Target->IsEnemy(GEntityList->Player()) && (GEntityList->Player()->GetPosition() - Target->GetPosition()).Length() <= 1200)
			{
				if (Target->HasBuff("urgotcorrosivedebuff"))

				{
					Q->CastOnTarget(Target);
				}
			}

		}
	}

	if (ComboW->Enabled() && W->IsReady() && !(Q->IsReady()))
	{
		if (GetEnemiesInRange(455) >= 1);
		W->CastOnPlayer();
	}

	if (ComboE->Enabled() && E->IsReady())
	{
		auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, E->Range());
		E->CastOnTarget(Target, kHitChanceVeryHigh);
	}
 }


PLUGIN_EVENT(void) OnRender()
{

	if (DrawReady->Enabled())

	{

		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }



		if (E->IsReady() && DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }



		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }



		if (R->IsReady() && DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }



	}

	else

	{

		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }



		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }



		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }



		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }

	}

}




PLUGIN_EVENT(void) OnGameUpdate()

{

	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)

	{

		Combo();

	}


}




PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{




	PluginSDKSetup(PluginSDK);

	Menu();

	LoadSpells();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GEventManager->AddEventHandler(kEventOnRender, OnRender);





}


PLUGIN_API void OnUnload()

{

	MainMenu->Remove();

	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);



}