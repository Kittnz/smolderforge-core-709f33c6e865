 /*
  * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
  * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
  * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
  * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
  *
  * This program is free software; you can redistribute it and/or modify it
  * under the terms of the GNU General Public License as published by the
  * Free Software Foundation; either version 2 of the License, or (at your
  * option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but WITHOUT
  * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  * more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program. If not, see <http://www.gnu.org/licenses/>.
  */

/* ScriptData
SDName: Stonetalon_Mountains
SD%Complete: 95
SDComment: Quest support: 6627, 6523
SDCategory: Stonetalon Mountains
EndScriptData */

/* ContentData
npc_braug_dimspirit
npc_kaya_flathoof
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## npc_braug_dimspirit
######*/

#define GOSSIP_HBD1 "Ysera"
#define GOSSIP_HBD2 "Neltharion"
#define GOSSIP_HBD3 "Nozdormu"
#define GOSSIP_HBD4 "Alexstrasza"
#define GOSSIP_HBD5 "Malygos"

bool GossipHello_npc_braug_dimspirit(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    if (player->GetQuestStatus(6627) == QUEST_STATUS_INCOMPLETE)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(5820, creature->GetGUID());
    }
    else
        player->SEND_GOSSIP_MENU(5819, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_braug_dimspirit(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->CLOSE_GOSSIP_MENU();
        creature->CastSpell(player, 6766, false);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        player->CLOSE_GOSSIP_MENU();
        player->AreaExploredOrEventHappens(6627);
    }
    return true;
}

/*######
## npc_kaya_flathoof
######*/

enum eKaya
{
    FACTION_ESCORTEE_H          = 775,

    NPC_GRIMTOTEM_RUFFIAN       = 11910,
    NPC_GRIMTOTEM_BRUTE         = 11912,
    NPC_GRIMTOTEM_SORCERER      = 11913,

    SAY_START                   = -1000357,
    SAY_AMBUSH                  = -1000358,
    SAY_END                     = -1000359,

    QUEST_PROTECT_KAYA          = 6523
};

struct npc_kaya_flathoofAI : public npc_escortAI
{
    npc_kaya_flathoofAI(Creature* c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* player = GetPlayerForEscort();

        if (!player)
            return;

        switch (i)
        {
        case 16:
            DoScriptText(SAY_AMBUSH, me);
            me->SummonCreature(NPC_GRIMTOTEM_BRUTE, -48.53f, -503.34f, -46.31f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            me->SummonCreature(NPC_GRIMTOTEM_RUFFIAN, -38.85f, -503.77f, -45.90f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            me->SummonCreature(NPC_GRIMTOTEM_SORCERER, -36.37f, -496.23f, -45.71f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            break;
        case 18: me->SetInFront(player);
            DoScriptText(SAY_END, me, player);
            if (player)
                player->GroupEventHappens(QUEST_PROTECT_KAYA, me);
            break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }

    void Reset(){}
};

bool QuestAccept_npc_kaya_flathoof(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_PROTECT_KAYA)
    {
        if (npc_escortAI* pEscortAI = CAST_AI(npc_kaya_flathoofAI, creature->AI()))
            pEscortAI->Start(true, false, player->GetGUID());

        DoScriptText(SAY_START, creature);
        creature->setFaction(113);
        creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }
    return true;
}

CreatureAI* GetAI_npc_kaya_flathoofAI(Creature* creature)
{
    return new npc_kaya_flathoofAI(creature);
}

/*######
## AddSC
######*/

void AddSC_stonetalon_mountains()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_braug_dimspirit";
    newscript->pGossipHello = &GossipHello_npc_braug_dimspirit;
    newscript->pGossipSelect = &GossipSelect_npc_braug_dimspirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kaya_flathoof";
    newscript->GetAI = &GetAI_npc_kaya_flathoofAI;
    newscript->pQuestAccept = &QuestAccept_npc_kaya_flathoof;
    newscript->RegisterSelf();
}

