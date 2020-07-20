/*
 * This file is part of the Colobot: Gold Edition source code
 * Copyright (C) 2001-2020, Daniel Roux, EPSITEC SA & TerranovaTeam
 * http://epsitec.ch; http://colobot.info; http://github.com/colobot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */

#include "ui/screen/screen_mod_list.h"

#include "common/config.h"

#include "app/app.h"
#include "app/modman.h"

#include "common/logger.h"
#include "common/restext.h"
#include "common/stringutils.h"

#include "common/resources/resourcemanager.h"

#include "common/system/system.h"

#include "math/func.h"

#include "ui/controls/button.h"
#include "ui/controls/edit.h"
#include "ui/controls/interface.h"
#include "ui/controls/label.h"
#include "ui/controls/list.h"
#include "ui/controls/window.h"

namespace Ui
{

CScreenModList::CScreenModList(CMainDialog* dialog, CModManager* modManager)
    : m_dialog(dialog),
      m_modManager(modManager)
{
}

void CScreenModList::CreateInterface()
{
    CWindow*        pw;
    CEdit* pe;
    CLabel*         pl;
    CButton*        pb;
    CList*          pli;
    Math::Point     pos, ddim;
    std::string     name;

    m_changes = false;
    ApplyChanges();

    // Display the window
    pos.x = 0.10f;
    pos.y = 0.10f;
    ddim.x = 0.80f;
    ddim.y = 0.80f;
    pw = m_interface->CreateWindows(pos, ddim, 12, EVENT_WINDOW5);
    pw->SetClosable(true);
    GetResource(RES_TEXT, RT_TITLE_MODS, name);
    pw->SetName(name);

    pos.x  = 0.10f;
    pos.y  = 0.40f;
    ddim.x = 0.50f;
    ddim.y = 0.50f;
    pw->CreateGroup(pos, ddim, 5, EVENT_INTERFACE_GLINTl);  // orange corner
    pos.x  = 0.40f;
    pos.y  = 0.10f;
    ddim.x = 0.50f;
    ddim.y = 0.50f;
    pw->CreateGroup(pos, ddim, 4, EVENT_INTERFACE_GLINTr);  // blue corner

    // Display the list of mods
    pos.x = ox+sx*3;
    pos.y = oy+sy*10.5f;
    ddim.x = dim.x*7.5f;
    ddim.y = dim.y*0.6f;
    GetResource(RES_TEXT, RT_MOD_LIST, name);
    pl = pw->CreateLabel(pos, ddim, 0, EVENT_LABEL11, name);
    pl->SetTextAlign(Gfx::TEXT_ALIGN_LEFT);

    pos.y = oy+sy*6.7f;
    ddim.y = dim.y*4.5f;
    ddim.x = dim.x*6.5f;
    pli = pw->CreateList(pos, ddim, 0, EVENT_INTERFACE_MOD_LIST);
    pli->SetState(STATE_SHADOW);
    pli->SetState(STATE_EXTEND);

    UpdateModList();

    // Displays the mod details
    pos.x = ox+sx*9.5f;
    pos.y = oy+sy*10.5f;
    ddim.x = dim.x*7.5f;
    ddim.y = dim.y*0.6f;
    GetResource(RES_TEXT, RT_MOD_DETAILS, name);
    pl = pw->CreateLabel(pos, ddim, 0, EVENT_LABEL12, name);
    pl->SetTextAlign(Gfx::TEXT_ALIGN_LEFT);

    pos.y = oy+sy*6.7f;
    ddim.y = dim.y*4.5f;
    ddim.x = dim.x*6.5f;
    pli = pw->CreateList(pos, ddim, 0, EVENT_INTERFACE_LIST);
    pli->SetState(STATE_SHADOW);

    UpdateModDetails();

    pos = pli->GetPos();
    ddim = pli->GetDim();

    // Displays the mod summary
    pos.x = ox+sx*3;
    pos.y = oy+sy*5.4f;
    ddim.x = dim.x*6.5f;
    ddim.y = dim.y*0.6f;
    GetResource(RES_TEXT, RT_MOD_SUMMARY, name);
    pl = pw->CreateLabel(pos, ddim, 0, EVENT_LABEL13, name);
    pl->SetTextAlign(Gfx::TEXT_ALIGN_LEFT);

    pos.x = ox+sx*3;
    pos.y = oy+sy*3.6f;
    ddim.x = dim.x*13.4f;
    ddim.y = dim.y*1.9f;
    pe = pw->CreateEdit(pos, ddim, 0, EVENT_INTERFACE_MOD_SUMMARY);
    pe->SetState(STATE_SHADOW);
    pe->SetMaxChar(500);
    pe->SetEditCap(false);  // just to see
    pe->SetHighlightCap(false);

    UpdateModSummary();

    // Apply button
    pos.x = ox+sx*13.75f;
    pos.y = oy+sy*2;
    ddim.x = dim.x*2.0f;
    ddim.y = dim.y*1;
    pb = pw->CreateButton(pos, ddim, -1, EVENT_INTERFACE_MODS_APPLY);
    pb->SetState(STATE_SHADOW);

    UpdateApplyButton();

    // Display the enable/disable button
    pos.x -= dim.x*2.3f;
    ddim.x = dim.x*2.0f;
    pb = pw->CreateButton(pos, ddim, -1, EVENT_INTERFACE_MOD_ENABLE_OR_DISABLE);
    pb->SetState(STATE_SHADOW);

    UpdateEnableDisableButton();

    // Display the move up button
    pos.x -= dim.x*1.3f;
    ddim.x = dim.x*1;
    pb = pw->CreateButton(pos, ddim, 49, EVENT_INTERFACE_MOD_MOVE_UP);
    pb->SetState(STATE_SHADOW);

    // Display the move down button
    pos.x -= dim.x*1.3f;
    pb = pw->CreateButton(pos, ddim, 50, EVENT_INTERFACE_MOD_MOVE_DOWN);
    pb->SetState(STATE_SHADOW);

    UpdateUpDownButtons();

    // Display the refresh button
    pos.x -= dim.x*1.3f;
    pb = pw->CreateButton(pos, ddim, 87, EVENT_INTERFACE_MODS_REFRESH);
    pb->SetState(STATE_SHADOW);

    // Display the open website button
    pos.x -= dim.x*1.3f;
    pb = pw->CreateButton(pos, ddim, 40, EVENT_INTERFACE_WORKSHOP);
    pb->SetState(STATE_SHADOW);

    // Display the open directory button
    pos.x -= dim.x*1.3f;
    pb = pw->CreateButton(pos, ddim, 57, EVENT_INTERFACE_MODS_DIR);
    pb->SetState(STATE_SHADOW);

    // Back button
    pos.x = ox+sx*3;
    ddim.x = dim.x*2.3f;
    pb = pw->CreateButton(pos, ddim, -1, EVENT_INTERFACE_BACK);
    pb->SetState(STATE_SHADOW);

    // Background
    SetBackground("textures/interface/interface.png");
    CreateVersionDisplay();
}

bool CScreenModList::EventProcess(const Event &event)
{
    CWindow* pw;
    CList* pl;

    const std::string workshopUrl = "https://www.moddb.com/games/colobot-gold-edition";
    const std::string modDir = CResourceManager::GetSaveLocation() + "/mods";

    auto systemUtils = CSystemUtils::Create(); // platform-specific utils

    Mod const * mod;

    pw = static_cast<CWindow*>(m_interface->SearchControl(EVENT_WINDOW5));
    if (pw == nullptr) return false;

    if (event.type == pw->GetEventTypeClose() ||
        event.type == EVENT_INTERFACE_BACK    ||
        (event.type == EVENT_KEY_DOWN && event.GetData<KeyEventData>()->key == KEY(ESCAPE)))
    {
        if (m_changes)
        {
            m_dialog->StartQuestion(RT_DIALOG_CHANGES_QUESTION, true, true, false,
                [this]()
                {
                    ApplyChanges();
                    CloseWindow();
                },
                [this]()
                {
                    m_changes = false; // do not save changes on "No"
                    ApplyChanges();
                    CloseWindow();
                });
        }
        else
        {
            ApplyChanges();
            CloseWindow();
        }
        return false;
    }

    switch( event.type )
    {
    case EVENT_INTERFACE_MOD_LIST:
        pl = static_cast<CList*>(pw->SearchControl(EVENT_INTERFACE_MOD_LIST));
        if (pl == nullptr) break;
        m_modSelectedIndex = pl->GetSelect();
        UpdateModSummary();
        UpdateModDetails();
        UpdateEnableDisableButton();
        UpdateUpDownButtons();
        break;

    case EVENT_INTERFACE_MOD_ENABLE_OR_DISABLE:
        mod = &m_modManager->GetMod(m_modSelectedIndex);
        if (mod->enabled)
        {
            m_modManager->DisableMod(m_modSelectedIndex);
        }
        else
        {
            m_modManager->EnableMod(m_modSelectedIndex);
        }
        UpdateModList();
        UpdateEnableDisableButton();
        m_changes = true;
        UpdateApplyButton();
        break;

    case EVENT_INTERFACE_MOD_MOVE_UP:
        m_modSelectedIndex = m_modManager->MoveUp(m_modSelectedIndex);
        UpdateModList();
        UpdateUpDownButtons();
        m_changes = true;
        UpdateApplyButton();
        break;

    case EVENT_INTERFACE_MOD_MOVE_DOWN:
        m_modSelectedIndex = m_modManager->MoveDown(m_modSelectedIndex);
        UpdateModList();
        UpdateUpDownButtons();
        m_changes = true;
        UpdateApplyButton();
        break;

    case EVENT_INTERFACE_MODS_REFRESH:
    case EVENT_INTERFACE_MODS_APPLY:
        ApplyChanges();
        // Update the whole UI
        UpdateModList();
        UpdateModSummary();
        UpdateModDetails();
        UpdateEnableDisableButton();
        UpdateApplyButton();
        UpdateUpDownButtons();
        break;

    case EVENT_INTERFACE_MODS_DIR:
        if (!systemUtils->OpenPath(modDir))
        {
            std::string title, text;
            GetResource(RES_TEXT, RT_DIALOG_OPEN_PATH_FAILED_TITLE, title);
            GetResource(RES_TEXT, RT_DIALOG_OPEN_PATH_FAILED_TEXT, text);

            // Workaround for Windows: the label skips everything after the first \\ character
            std::string modDirWithoutBackSlashes = modDir;
            std::replace(modDirWithoutBackSlashes.begin(), modDirWithoutBackSlashes.end(), '\\', '/');

            m_dialog->StartInformation(title, title, StrUtils::Format(text.c_str(), modDirWithoutBackSlashes.c_str()));
        }
        break;

    case EVENT_INTERFACE_WORKSHOP:
        if (!systemUtils->OpenWebsite(workshopUrl))
        {
            std::string title, text;
            GetResource(RES_TEXT, RT_DIALOG_OPEN_WEBSITE_FAILED_TITLE, title);
            GetResource(RES_TEXT, RT_DIALOG_OPEN_WEBSITE_FAILED_TEXT, text);
            m_dialog->StartInformation(title, title, StrUtils::Format(text.c_str(), workshopUrl.c_str()));
        }
        break;

    default:
        return true;
    }
    return false;
}

void CScreenModList::ApplyChanges()
{
    if (m_changes)
    {
        m_changes = false;
        m_modManager->SaveMods();
    }

    m_modManager->FindMods();
    m_modManager->SaveMods();

    m_empty = (m_modManager->CountMods() == 0);

    m_modManager->UpdatePaths();
    m_modManager->ReloadResources();

    m_modSelectedIndex = Math::Clamp(m_modSelectedIndex, static_cast<size_t>(0), m_modManager->CountMods() - 1);
}

void CScreenModList::CloseWindow()
{
    m_main->ChangePhase(PHASE_MAIN_MENU);
}

void CScreenModList::UpdateModList()
{
    CWindow* pw = static_cast<CWindow*>(m_interface->SearchControl(EVENT_WINDOW5));
    if (pw == nullptr) return;

    CList* pl = static_cast<CList*>(pw->SearchControl(EVENT_INTERFACE_MOD_LIST));
    if (pl == nullptr) return;

    pl->Flush();

    if (m_empty)
    {
        return;
    }

    auto mods = m_modManager->GetMods();
    for (size_t i = 0; i < mods.size(); ++i)
    {
        const auto& mod = mods[i];
        pl->SetItemName(i, mod.name);
        pl->SetCheck(i, mod.enabled);
        pl->SetEnable(i, true);
    }

    pl->SetSelect(m_modSelectedIndex);
    pl->ShowSelect(false);
}

void CScreenModList::UpdateModDetails()
{
    //TODO
}

void CScreenModList::UpdateModSummary()
{
    //TODO
}

void CScreenModList::UpdateEnableDisableButton()
{
    CWindow* pw = static_cast<CWindow*>(m_interface->SearchControl(EVENT_WINDOW5));
    if (pw == nullptr) return;

    CButton* pb = static_cast<CButton*>(pw->SearchControl(EVENT_INTERFACE_MOD_ENABLE_OR_DISABLE));
    if (pb == nullptr) return;

    std::string buttonName{};

    if (m_empty)
    {
        pb->ClearState(STATE_ENABLE);

        // Set some default name
        GetResource(RES_TEXT, RT_MOD_ENABLE, buttonName);
        pb->SetName(buttonName);

        return;
    }

    const auto& mod = m_modManager->GetMod(m_modSelectedIndex);

    if (mod.enabled)
    {
        GetResource(RES_TEXT, RT_MOD_DISABLE, buttonName);
        pb->SetName(buttonName);
    }
    else
    {
        GetResource(RES_TEXT, RT_MOD_ENABLE, buttonName);
        pb->SetName(buttonName);
    }
}

void CScreenModList::UpdateApplyButton()
{
    CWindow* pw = static_cast<CWindow*>(m_interface->SearchControl(EVENT_WINDOW5));
    if (pw == nullptr) return;

    CButton* pb = static_cast<CButton*>(pw->SearchControl(EVENT_INTERFACE_MODS_APPLY));
    if (pb == nullptr) return;

    if (m_empty)
    {
        pb->ClearState(STATE_ENABLE);
        return;
    }

    if (m_changes)
    {
        pb->SetState(STATE_ENABLE);
    }
    else
    {
        pb->ClearState(STATE_ENABLE);
    }
}

void CScreenModList::UpdateUpDownButtons()
{
    CWindow* pw = static_cast<CWindow*>(m_interface->SearchControl(EVENT_WINDOW5));
    if (pw == nullptr) return;

    CButton* pb_up = static_cast<CButton*>(pw->SearchControl(EVENT_INTERFACE_MOD_MOVE_UP));
    if (pb_up == nullptr) return;

    CButton* pb_down = static_cast<CButton*>(pw->SearchControl(EVENT_INTERFACE_MOD_MOVE_DOWN));
    if (pb_down == nullptr) return;

    if (m_empty)
    {
        pb_up->ClearState(STATE_ENABLE);
        pb_down->ClearState(STATE_ENABLE);
        return;
    }

    if (m_modSelectedIndex == 0)
    {
        pb_up->ClearState(STATE_ENABLE);
    }
    else
    {
        pb_up->SetState(STATE_ENABLE);
    }

    if (m_modSelectedIndex >= m_modManager->CountMods() - 1)
    {
        pb_down->ClearState(STATE_ENABLE);
    }
    else
    {
        pb_down->SetState(STATE_ENABLE);
    }
}

} // namespace Ui
