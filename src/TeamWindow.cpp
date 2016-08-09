#include "TeamWindow.h"
#include "TableHelper.h"
#include "Log.h"



TeamWindow::TeamWindow(QWidget *parent)
    : SelectionWindow(parent, tr("Créer/modifier une équipe"))
{
    QStringList header;
    header << tr("Id") << tr("Prénom") << tr("Nom") << tr("Pseudo");
    SetHeader(header);
}

void TeamWindow::SetTeam(const Player &p1, const Player &p2)
{
    mSelection.clear();
    mSelection.append(p1);
    mSelection.append(p2);
    Update();
}

void TeamWindow::GetTeam(Team &team)
{
    if (mSelection.size() >= 2)
    {
        team.player1Id = mSelection.at(0).id;
        team.player2Id = mSelection.at(1).id;
    }

    if (mSelection.size() >= 3)
    {
        team.player3Id = mSelection.at(2).id;
    }

    // Update team name
    team.CreateName(mSelection.at(0).name, mSelection.at(1).name);
}

void TeamWindow::Initialize(const QList<Player> &players, const QList<int> &inTeams)
{
    // Create a list of players that are still alone
    mList.clear();
    foreach (Player p, players)
    {
        // This player has no team, add it to the list of available players
        if (!inTeams.contains(p.id))
        {
            mList.append(p);
        }
    }

    mSelection.clear();
    Update();
}

void TeamWindow::ClickedRight(int index)
{
    const Player &p = mSelection.at(index);
    // transfer to the left
    mList.append(p);
    mSelection.removeAt(index);

    Update();
}

void TeamWindow::ClickedLeft(int id)
{
    Player p;
    if (Player::Find(mList, id, p) && (mSelection.size() < GetMaxSize()))
    {
        // transfer to the right and remove the player from the list
        mSelection.append(p);
        int index;
        if (Player::Index(mList, id, index))
        {
            mList.removeAt(index);
        }
    }

    Update();
}

void TeamWindow::Update()
{
    StartUpdate(mList.size());

    foreach (Player p, mList)
    {
        QList<QVariant> rowData;
        rowData << p.id << p.name << p.lastName << p.nickName;
        AddLeftEntry(rowData);
    }

    foreach (Player p, mSelection)
    {
        AddRightEntry(p.name + " " + p.lastName);
    }

    FinishUpdate();
}