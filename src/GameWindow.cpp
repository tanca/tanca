/*=============================================================================
 * Tanca - GameWindow.cpp
 *=============================================================================
 * Dialog used to add a new game (match between two teams)
 *=============================================================================
 * Tanca ( https://github.com/belegar/tanca ) - This file is part of Tanca
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony.rabine@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include "GameWindow.h"

GameWindow::GameWindow(QWidget *parent)
    : SelectionWindow(parent, tr("Créer un jeu"))
{
    QStringList header;
    header << tr("Id") << tr("Équipe");
    SetHeader(header);
}

void GameWindow::GetGame(Game &game)
{
    if (mSelection.size() >= 2)
    {
        game.team1Id = mSelection.at(0).id;
        game.team2Id = mSelection.at(1).id;
    }
}

void GameWindow::Initialize(const QList<Team> &teams)
{
    mList = teams;
    mSelection.clear();
    Update();
}


void GameWindow::ClickedRight(int index)
{
    const Team &t = mSelection.at(index);
    // transfer to the left
    mList.append(t);
    mSelection.removeAt(index);

    Update();
}

void GameWindow::ClickedLeft(int id)
{
    Team t;
    if (Team::Find(mList, id, t) && (mSelection.size() < GetMaxSize()))
    {
        // transfer to the right and remove the player from the list
        mSelection.append(t);
        int index;
        if (Team::Index(mList, id, index))
        {
            mList.removeAt(index);
        }
    }

    Update();
}

void GameWindow::Update()
{
    StartUpdate(mList.size());

    foreach (Team t, mList)
    {
        QList<QVariant> rowData;
        rowData << t.id << t.teamName;
        AddLeftEntry(rowData);
    }

    foreach (Team t, mSelection)
    {
        AddRightEntry(t.teamName);
    }

    FinishUpdate();
}

