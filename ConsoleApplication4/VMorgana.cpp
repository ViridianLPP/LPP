#include "stdafx.h"
#include "PluginSDK.h"




PluginSetup("VMorgana");

IMenu* MainMenu;
IMenu* QMenu;
IMenu* WMenu;
IMenu* EMenu;
IMenu* RMenu;
IMenu* Misc;
IMenu* Drawings;
IMenuOption* ComboQ;
IMenuOption* GapcloseQ;
IMenuOption* AutoLogicQ;
IMenuOption* ComboW;
IMenuOption* FarmW;
IMenuOption* MinimumMinionsW;
IMenuOption* AutoLogicW;
IMenuOption* SettingsE;
IMenuOption* ComboR;
IMenuOption* ComboREnemies;
IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawW;
IMenuOption* DrawE;
IMenuOption* DrawR;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

void Menu()
{
	MainMenu = GPluginSDK->AddMenu("VMorgana");
	QMenu = MainMenu->AddMenu("Q Settings");
	WMenu = MainMenu->AddMenu("W Settings");
	EMenu = MainMenu->AddMenu("E Settings");
	RMenu = MainMenu->AddMenu("R Settings");
	Drawings = MainMenu->AddMenu("Drawings");

	ComboQ = QMenu->CheckBox("Use Q", true);
	GapcloseQ = QMenu->CheckBox("Q on Gapcloser", true);
	AutoLogicQ = QMenu->CheckBox("Use Q on CC", false);


	ComboW = WMenu->CheckBox("Use W", true);
	FarmW = WMenu->CheckBox("Farm W", true);
	MinimumMinionsW = WMenu->AddInteger("Min. minions for W", 1, 5, 3);
	AutoLogicW = WMenu->CheckBox("Use W on CC", true);

	SettingsE = EMenu->CheckBox("E - WORK IN PROGRESS", false);

	ComboR = RMenu->CheckBox("Use R", true);
	ComboREnemies = RMenu->AddInteger("Min. enemies for R", 0, 5, 2);
	DrawReady = Drawings->CheckBox("Draw only ready spells", true);

	DrawQ = Drawings->CheckBox("Draw Q", true);
	DrawW = Drawings->CheckBox("Draw W", true);
	DrawE = Drawings->CheckBox("Draw E", true);
	DrawR = Drawings->CheckBox("Draw R", true);
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithMinions | kCollidesWithYasuoWall));
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, true, true, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
	Q->SetOverrideRange(1075);
	W->SetOverrideRange(900);
	E->SetOverrideRange(800);
	R->SetOverrideRange(600);
	Q->SetOverrideDelay(0.25f);
	W->SetOverrideDelay(0.05f);
	E->SetOverrideDelay(0.05f);
	R->SetOverrideDelay(0.25f);
	Q->SetOverrideRadius(80);
	W->SetOverrideRadius(275);
	R->SetOverrideRadius(600);
	Q->SetOverrideSpeed(1200);
}

void Combo()
{
	if (ComboQ->Enabled())
	{
		if (Q->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			Q->CastOnTarget(target, kHitChanceVeryHigh);

		}
	}
	if (ComboW->Enabled())
	{
		if (W->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
			W->CastOnPosition(target->ServerPosition());
		}
	}
	if (ComboR->Enabled())
	{
		if (R->IsReady())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, R->Range());
			auto EnemiesInRangeOfHero = [&](IUnit* Hero, float range) -> int
			{
				int ret = 0;


				for (auto hero : GEntityList->GetAllHeros(false, true))
				{
					if (hero == nullptr || hero->IsDead() || hero == Hero)
						continue;

					float distance = (hero->ServerPosition() - Hero->ServerPosition()).Length2D();

					if (distance <= 600)
						ret++;
				}

				return ret;
			};

			if (EnemiesInRangeOfHero(GEntityList->Player(), 600) >= ComboREnemies->GetInteger())
				R->CastOnTarget(target);
		}
	}
}



void Farm()
{
	if (W->IsReady() && FarmW->Enabled())
	{
		Vec3 vecCastPosition;
		int iMinionsHit = 0;

		W->FindBestCastPosition(true, false, vecCastPosition, iMinionsHit);

		if (W->IsReady() && iMinionsHit >= MinimumMinionsW->GetInteger()) {
			W->CastOnPosition(vecCastPosition);
		}
	}
}

bool CanMove(IUnit* target)
{
	if (target->HasBuffOfType(BUFF_Stun) || target->HasBuffOfType(BUFF_Snare) || target->HasBuffOfType(BUFF_Fear) || target->HasBuffOfType(BUFF_Knockup) ||
		target->HasBuff("Recall") || target->HasBuffOfType(BUFF_Knockback) || target->HasBuffOfType(BUFF_Charm) || target->HasBuffOfType(BUFF_Taunt) || target->HasBuffOfType(BUFF_Suppression))
	{
		return true;
	}
	else
		return false;
}

void AutoWLogic()
{
	if (W->IsReady() && AutoLogicW->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, W->Range());
		if (target != nullptr)
		{
			if (CanMove(target))
			{
				W->CastOnPosition(target->ServerPosition());
			}


		}
	}
}

	void AutoQLogic()
	{
		if (Q->IsReady() && AutoLogicQ->Enabled())
		{
			auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
			if (target != nullptr)
			{
				if (CanMove(target))
				{
					Q->CastOnTarget(target, kHitChanceVeryHigh);
				}


			}
		}

	}




	float GetDistancePos(Vec3 pos1, Vec3 pos2)
	{
		return (pos1 - pos2).Length();
	}

	PLUGIN_EVENT(void) OnDash(UnitDash* Args)
	{
		if (Q->IsReady() && GetDistancePos(GEntityList->Player()->ServerPosition(), Args->EndPosition) < Q->Range()) { Q->CastOnPosition(Args->EndPosition); }
	}

	PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
	{
		if (Args.Sender != GEntityList->Player()
			&& Args.Sender->GetTeam() != GEntityList->Player()->GetTeam()
			&& GEntityList->Player()->IsValidTarget(Args.Sender, Q->Range())
			&& GapcloseQ->Enabled() && Q->IsReady())
		{
			Q->CastOnTarget(Args.Sender);
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





		if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)

		{

			Farm();

		}

		if (W->IsReady())
		{
			AutoWLogic();
		}

		if (Q->IsReady())
		{
			AutoQLogic();
		}
	}

	PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
	{




		PluginSDKSetup(PluginSDK);

		Menu();

		LoadSpells();


		GEventManager->AddEventHandler(kEventOnDash, OnDash);

		GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);

		GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

		GEventManager->AddEventHandler(kEventOnRender, OnRender);





	}


	PLUGIN_API void OnUnload()

	{

		MainMenu->Remove();


		GEventManager->RemoveEventHandler(kEventOnDash, OnDash);

		GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);

		GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);

		GEventManager->RemoveEventHandler(kEventOnRender, OnRender);



	}





