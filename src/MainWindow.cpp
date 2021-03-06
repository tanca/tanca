/*=============================================================================
 * Tanca - MainWindow.cpp
 *=============================================================================
 * Main window widget
 *=============================================================================
 * Tanca ( https://github.com/belegar/tanca ) - This file is part of Tanca
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony.rabine@tarotclub.fr
 *
 * Tanca is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tanca is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tanca.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include <QStandardPaths>
#include <iostream>
#include <QMessageBox>
#include <QFileDialog>

#include "Value.h"
#include "Log.h"
#include "Util.h"
#include "MainWindow.h"
#include "TableHelper.h"
#include "ui_MainWindow.h"
#include "ui_RewardWindow.h"

static const QString gVersion = "1.13";

// Table headers
static QStringList gGamesTableHeader;
static QStringList gEventsTableHeader;
static QStringList gPlayersTableHeader;
static QStringList gTeamsTableHeader;
static QStringList gRewardsTableHeader;

#ifdef USE_WINDOWS_OS
static QString gAppDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tanca";
#else
QString gAppDataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.tanca";
#endif
static QString gDbFullPath = gAppDataPath + "/tanca.db";



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mDatabase(gDbFullPath)
    , mCurrentRankingRound(1)
{
    Log::SetLogPath(gAppDataPath.toStdString());

    std::cout << "Application path set to: " << gAppDataPath.toStdString() << std::endl;

    // Setup UI
    ui->setupUi(this);

    playerWindow = new PlayerWindow(this);
    playerWindow->hide();

    datePickerWindow = new DatePickerWindow(this);
    datePickerWindow->hide();

    teamWindow = new TeamWindow(this);
    teamWindow->hide();

    gameWindow = new GameWindow(this);
    gameWindow->hide();

    scoreWindow = new ScoreWindow(this);
    scoreWindow->hide();

    eventWindow = new EventWindow(this);
    eventWindow->hide();

    setWindowTitle("Tanca " + gVersion + " - Logiciel de gestion de club et de concours de pétanque.");

    // Setup signals for the menu
    connect(ui->actionImporter, &QAction::triggered, this, &MainWindow::slotImportPlayerFile);
    connect(ui->actionQuitter, &QAction::triggered, this, &QCoreApplication::quit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::slotAboutBox);

    // Setup signals for TAB 1: players management
    connect(ui->buttonAddPlayer, &QPushButton::clicked, this, &MainWindow::slotAddPlayer);
    connect(ui->buttonEditPlayer, &QPushButton::clicked, this, &MainWindow::slotEditPlayer);
    connect(ui->buttonDeletePlayer, &QPushButton::clicked, this, &MainWindow::slotDeletePlayer);
    connect(ui->buttonExportPlayers, &QPushButton::clicked, this, &MainWindow::slotExportPlayers);
    connect(ui->lineEditPlayerFilter, &QLineEdit::textChanged, this, &MainWindow::slotFilterPlayer);

    // Setup signals for TAB 3: club championship management
    connect(ui->buttonAddEvent, &QPushButton::clicked, this, &MainWindow::slotAddEvent);
    connect(ui->buttonEditEvent, &QPushButton::clicked, this, &MainWindow::slotEditEvent);
    connect(ui->buttonDeleteEvent, &QPushButton::clicked, this, &MainWindow::slotDeleteEvent);

    connect(ui->buttonAddTeam, &QPushButton::clicked, this, &MainWindow::slotAddTeam);
    connect(ui->buttonEditTeam, &QPushButton::clicked, this, &MainWindow::slotEditTeam);
    connect(ui->buttonDeleteTeam, &QPushButton::clicked, this, &MainWindow::slotDeleteTeam);
    connect(ui->buttonExportTeams, &QPushButton::clicked, this, &MainWindow::slotExportTeams);

    connect(ui->btnAddReward, &QPushButton::clicked, this, &MainWindow::slotAddReward);
    connect(ui->btnDeleteReward, &QPushButton::clicked, this, &MainWindow::slotDeleteReward);

    connect(ui->eventTable, SIGNAL(itemSelectionChanged()), this, SLOT(slotEventItemActivated()));
    connect(ui->teamTable, SIGNAL(itemSelectionChanged()), this, SLOT(slotTeamItemActivated()));
    connect(ui->comboSeasons, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSeasonChanged(int)));
    connect(ui->buttonCreateGames, &QPushButton::clicked, this, &MainWindow::slotGenerateGames);

    connect(ui->buttonAddGame, &QPushButton::clicked, this, &MainWindow::slotAddGame);
    connect(ui->buttonEditGame, &QPushButton::clicked, this, &MainWindow::slotEditGame);
    connect(ui->buttonDeleteGame, &QPushButton::clicked, this, &MainWindow::slotDeleteGame);
    connect(ui->buttonDeleteAllGames, &QPushButton::clicked, this, &MainWindow::slotDeleteAllGames);
    connect(ui->buttonExportGames, &QPushButton::clicked, this, &MainWindow::slotExportGames);

    connect(ui->tabWidget_2, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
    connect(ui->radioEvent, &QRadioButton::toggled, this, &MainWindow::slotRankingOptionChanged);
    connect(ui->radioSeason, &QRadioButton::toggled, this, &MainWindow::slotRankingOptionChanged);

    connect(ui->btnRankingLeft, &QPushButton::clicked, this,  &MainWindow::slotRankingLeft);
    connect(ui->btnRankingRight,&QPushButton::clicked, this,  &MainWindow::slotRankingRight);
    connect(ui->buttonExportRanking, &QPushButton::clicked, this, &MainWindow::slotExportRanking);

    // Setup other stuff
    mDatabase.Initialize();
    gGamesTableHeader << tr("Id") << tr("Partie") << tr("Équipe 1") << tr("Équipe 2") << tr("Score 1") << tr("Score 2");
    gEventsTableHeader << tr("Id") << tr("Date") << tr("Type") << tr("Titre") << tr("État");
    gPlayersTableHeader << tr("Id") << tr("UUID") << tr("Prénom") << tr("Nom") << tr("Pseudonyme") << tr("E-mail") << tr("Téléphone (mobile)") << tr("Téléphone (maison)") << tr("Date de naissance") << tr("Rue") << tr("Code postal") << tr("Ville") << tr("Licences") << tr("Commentaires") << tr("Statut") << tr("Divers");
    gTeamsTableHeader << tr("Id") << tr("Numéro") << tr("Joueur 1") << tr("Joueur 2") << tr("Joueur 3") << ("Nom de l'équipe");
    gRewardsTableHeader << tr("Id") << tr("Montant") << tr("Commentaire");
}

MainWindow::~MainWindow()
{
    mServer.Stop();
    delete ui;
}

void MainWindow::Initialize()
{
    mServer.Initialize();

    // Initialize views
    UpdatePlayersTable();
    UpdateSeasons();
}

void MainWindow::ExportTable(QTableWidget *table, const QString &title)
{
    QString fileName = QFileDialog::getSaveFileName(this, title,
                                 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                 tr("Excel CSV (*.csv);;JSON (*.json)"));
    if (!fileName.isEmpty())
    {
        TableHelper helper(table);
        helper.Export(fileName);
    }
}

void MainWindow::slotTabChanged(int index)
{
    Q_UNUSED(index);
    // Refresh ranking
    UpdateRanking();
}

void MainWindow::slotAboutBox()
{
    QDialog about;
    uiAboutBox.setupUi(&about);
    about.exec();
}

// ===========================================================================================
// PLAYERS MANAGEMENT
// ===========================================================================================
void MainWindow::slotFilterPlayer()
{
    QString filter = ui->lineEditPlayerFilter->text();
    for( int i = 0; i < ui->playersWidget->rowCount(); ++i )
    {
        bool match = false;
        for( int j = 0; j < ui->playersWidget->columnCount(); ++j )
        {
            QTableWidgetItem *item = ui->playersWidget->item( i, j );
            if( item->text().contains(filter, Qt::CaseInsensitive) )
            {
                match = true;
                break;
            }
        }
        ui->playersWidget->setRowHidden( i, !match );
    }
}


void MainWindow::UpdatePlayersTable()
{
    TableHelper helper(ui->playersWidget);
    std::deque<Player> &list = mDatabase.GetPlayerList();
    helper.Initialize(gPlayersTableHeader, list.size());

    foreach (Player p, list)
    {
        std::list<Value> rowData = {p.id, p.uuid, p.name, p.lastName, p.nickName, p.email
               , p.mobilePhone, p.homePhone, Util::ToISODateTime(p.birthDate), p.road, p.postCode
               , p.city, p.membership, p.comments, p.state, p.document};

        helper.AppendLine(rowData, false);
    }

    ui->playersWidget->hideColumn(1); // don't show the UUID
    ui->playersWidget->hideColumn(14); // don't show the State
    ui->playersWidget->hideColumn(15); // don't show the Document

    helper.Finish();
}

void MainWindow::slotAddPlayer()
{
    if (playerWindow->AddPlayer(mDatabase))
    {
        UpdatePlayersTable();
    }
}

void MainWindow::slotEditPlayer()
{
    if (playerWindow->EditPlayer(mDatabase, ui->playersWidget))
    {
        UpdatePlayersTable();
    }
}

void MainWindow::slotDeletePlayer()
{
    if (playerWindow->DeletePlayer(mDatabase, ui->playersWidget))
    {
        UpdatePlayersTable();
    }
}

void MainWindow::slotExportPlayers()
{
    ExportTable(ui->playersWidget, tr("Exporter la base de joueurs au format Excel (CSV)"));
}

void MainWindow::slotImportPlayerFile()
{
    if (playerWindow->ImportPlayerFile(mDatabase))
    {
        UpdatePlayersTable();
    }
}

// ===========================================================================================
// TEAMS MANAGEMENT
// ===========================================================================================
void MainWindow::slotExportTeams()
{
     ExportTable(ui->teamTable, tr("Exporter la liste des équipes au format Excel (CSV)"));
}

void MainWindow::UpdateTeamList()
{
    mTeams = mDatabase.GetTeams(mCurrentEvent.id);
    mPlayersInTeams.clear();

    TableHelper helper(ui->teamTable);
    helper.Initialize(gTeamsTableHeader, mTeams.size());
    teamWindow->ClearIds();

    foreach (Team team, mTeams)
    {
        Player p1, p2, p3;
        if (mDatabase.FindPlayer(team.player1Id, p1))
        {
            mPlayersInTeams.push_back(team.player1Id);
        }

        if (mDatabase.FindPlayer(team.player2Id, p2))
        {
            mPlayersInTeams.push_back(team.player2Id);
        }

        if (mDatabase.FindPlayer(team.player3Id, p3))
        {
            mPlayersInTeams.push_back(team.player3Id);
        }

        teamWindow->AddId(team.number);

        std::list<Value> rowData = {team.id, team.number, p1.FullName(), p2.FullName(), p3.FullName(), team.teamName};
        helper.AppendLine(rowData, false);
    }

    helper.Finish();

    teamWindow->ListIds();
}

void MainWindow::slotAddTeam()
{
    int selection = ui->eventTable->currentRow();
    if (selection > -1)
    {
        // Prepare widget contents
        teamWindow->Initialize(mDatabase.GetPlayerList(), mPlayersInTeams, false);

        if (teamWindow->exec() == QDialog::Accepted)
        {
            Team team;
            teamWindow->GetTeam(team);
            team.eventId = mCurrentEvent.id;
            team.number = teamWindow->GetNumber();
            if (mDatabase.AddTeam(team))
            {
                UpdateTeamList();
            }
        }
    }
}

void MainWindow::slotEditTeam()
{
    TableHelper helper(ui->teamTable);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        Team team;
        if (Team::Find(mTeams, id, team))
        {
            // Prepare widget contents
            teamWindow->Initialize(mDatabase.GetPlayerList(), mPlayersInTeams, true);

            Player p1, p2;
            (void) mDatabase.FindPlayer(team.player1Id, p1);
            (void) mDatabase.FindPlayer(team.player2Id, p2);

            teamWindow->SetTeam(p1, p2, team);

            if (teamWindow->exec() == QDialog::Accepted)
            {
                teamWindow->GetTeam(team);
                team.number = teamWindow->GetNumber();
                if (mDatabase.EditTeam(team))
                {
                    UpdateTeamList();
                    UpdateGameList();
                }
            }
        }
    }
}

void MainWindow::slotDeleteTeam()
{
    if (mGames.size() == 0)
    {
        TableHelper helper(ui->teamTable);

        int id;
        if (helper.GetFirstColumnValue(id))
        {
            if (mDatabase.DeleteTeam(id))
            {
                UpdateTeamList();
            }
        }
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Impossible de supprimer une équipe, des rencontres existent."),
                                    QMessageBox::Ok);
    }
}

void MainWindow::slotTeamItemActivated()
{
    int row = ui->teamTable->currentRow();
    if (row > -1)
    {
        int id;
        TableHelper helper(ui->teamTable);

        if (helper.GetFirstColumnValue(id))
        {
            mSelectedTeam = id;
        }

        mTournament.GenerateTeamRanking(mGames, mTeams, 99);

        Rank rank;
        if (mTournament.GetTeamRank(mSelectedTeam, rank))
        {
            ui->lblPlayedGames->setText(QString("%1").arg(rank.gamesLost + rank.gamesWon + rank.gamesDraw));
            ui->lblWonGames->setText(QString("%1").arg(rank.gamesWon));
        }

        UpdateRewards();
    }
}


// ===========================================================================================
// REWARDS MANAGEMENT
// ===========================================================================================
void MainWindow::UpdateRewards()
{
    TableHelper helper(ui->tableRewards);
    QList<Reward> rewards = mDatabase.GetRewardsForTeam(mSelectedTeam);

    helper.Initialize(gRewardsTableHeader, rewards.size());

    foreach (Reward reward, rewards)
    {
        std::list<Value> rewardData = {reward.id, std::to_string(reward.total), reward.comment};
        helper.AppendLine(rewardData, false);
    }

    helper.Finish();
}

void MainWindow::slotAddReward()
{
    QDialog dialog;
    Ui::RewardWindow ui;
    ui.setupUi(&dialog);

    if (dialog.exec() == QDialog::Accepted)
    {
        Reward reward;

        reward.eventId = mCurrentEvent.id;
        reward.teamId = mSelectedTeam;
        reward.total = ui.spinReward->value();
        reward.state = Reward::cStateRewardOk;
        reward.comment = ui.lineRewardComment->text().toStdString();

        if (mDatabase.AddReward(reward))
        {
            UpdateRewards();
        }
        else
        {
            TLogError("Add reward failure");
        }
    }
}

void MainWindow::slotDeleteReward()
{
    TableHelper helper(ui->tableRewards);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        if (mDatabase.DeleteReward(id))
        {
            UpdateRewards();
        }
    }
}

// ===========================================================================================
// RANKING MANAGEMENT
// ===========================================================================================
void MainWindow::UpdateRanking()
{
    bool isSeason = ui->radioSeason->isChecked(); // Display option
    TableHelper helper(ui->tableContest);

    if (isSeason)
    {
        ui->lblRankingRound->setEnabled(false);
        std::deque<Game> games;
        std::deque<Team> teams;

        for (auto const &i : mEvents)
        {
            std::deque<Game> g = mDatabase.GetGamesByEventId(i.id);
            games.insert(games.end(), g.begin(), g.end());

            std::deque<Team> t = mDatabase.GetTeams(i.id);
            teams.insert(teams.end(), t.begin(), t.end());
        }

        mTournament.GeneratePlayerRanking(games, teams, mEvents);
    }
    else
    {
        ui->lblRankingRound->setEnabled(true);
        ui->lblRankingRound->setText(QString().number(mCurrentRankingRound));
        mTournament.GenerateTeamRanking(mGames, mTeams, mCurrentRankingRound);
    }
    helper.Show(mDatabase.GetPlayerList(), mTeams, isSeason, mTournament.GetRanking());

    UpdateBrackets();
}


void MainWindow::slotRankingOptionChanged(bool checked)
{
    Q_UNUSED(checked);
    UpdateRanking();
}

void MainWindow::slotRankingLeft()
{
    if (mCurrentRankingRound > 1)
    {
        mCurrentRankingRound--;
    }
    UpdateRanking();
}

void MainWindow::slotRankingRight()
{
    int nbGames = mTeams.size() / 2;
    if (mTeams.size()%2)
    {
        nbGames += 1;
    }

    int maxTurn = (nbGames == 0) ? 0 : (mGames.size() / nbGames);

    if (mCurrentRankingRound < maxTurn)
    {
        mCurrentRankingRound++;
    }
    UpdateRanking();
}

void MainWindow::slotExportRanking()
{
    ExportTable(ui->tableContest, tr("Exporter le classement au format Excel (CSV)"));
}

// ===========================================================================================
// EVENT MANAGEMENT
// ===========================================================================================
void MainWindow::UpdateEventsTable()
{
    TableHelper helper(ui->eventTable);
    helper.Initialize(gEventsTableHeader, mEvents.size());

    foreach (Event event, mEvents)
    {
        std::list<Value> rowData = {event.id, Util::DateTimeFormat(event.date, "%e %B"), TypeToString(event), event.title, StateToString(event)};
        helper.AppendLine(rowData, false);
    }

    helper.Finish();

    if (mEvents.size() > 0)
    {
        // Will refresh all the UI elements for that event
        ui->eventTable->selectRow(mEvents.size() - 1);
    }
    else
    {
        // Empty teams and games
        ui->teamTable->clear();
        ui->gameTable->clear();
    }
}

void MainWindow::slotEventItemActivated()
{
    mCurrentRankingRound = 1;
    int row = ui->eventTable->currentRow();
    if (row > -1)
    {
        int id;
        TableHelper helper(ui->eventTable);

        if (helper.GetFirstColumnValue(id))
        {
            mCurrentEvent = mDatabase.GetEvent(id);

            if (mCurrentEvent.IsValid())
            {
                std::cout << "Current event id: " << mCurrentEvent.id << std::endl;
                UpdateTeamList();
                UpdateGameList();
                UpdateRanking();
            }
            else
            {
                TLogError("Invalid event!");
            }
        }
    }
}

void MainWindow::slotAddEvent()
{
    Event event;
    eventWindow->SetEvent(event);
    if (eventWindow->exec() == QDialog::Accepted)
    {
        eventWindow->GetEvent(event);
        event.year = Util::GetYear(event.date);
        if (mDatabase.AddEvent(event))
        {
            UpdateSeasons();
        }
    }
}

void MainWindow::slotEditEvent()
{
    TableHelper helper(ui->eventTable);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        eventWindow->SetEvent(mCurrentEvent);
        if (eventWindow->exec() == QDialog::Accepted)
        {
            eventWindow->GetEvent(mCurrentEvent);
            if (!mDatabase.EditEvent(mCurrentEvent))
            {
                TLogError("Cannot edit event!");
            }
            else
            {
                slotSeasonChanged(ui->comboSeasons->currentIndex());
            }
        }
    }
}

void MainWindow::slotDeleteEvent()
{
    TableHelper helper(ui->eventTable);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        if (QMessageBox::warning(this, tr("Suppression d'un événement"),
                                    tr("Attention ! Toutes les parties associées seront perdues. Continuer ?"),
                                    QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
        {
            bool success = mDatabase.DeleteGameByEventId(id);
            success = success && mDatabase.DeleteTeamByEventId(id);
            success = success && mDatabase.DeleteEvent(id);

            if (!success)
            {
                TLogError("Delete event failure");
            }
            UpdateSeasons();
        }
    }
}

// ===========================================================================================
// SEASONS MANAGEMENT
// ===========================================================================================
void MainWindow::UpdateSeasons()
{
    QStringList seasons = mDatabase.GetSeasons();

    ui->comboSeasons->clear();
    ui->comboSeasons->addItems(seasons);
    ui->comboSeasons->setCurrentIndex(seasons.size() - 1);

    UpdateEventsTable();
}

void MainWindow::slotSeasonChanged(int index)
{
    mEvents = mDatabase.GetEvents(ui->comboSeasons->itemText(index).toInt());
    UpdateEventsTable();
}

// ===========================================================================================
// GAMES MANAGEMENT
// ===========================================================================================
void MainWindow::slotGenerateGames()
{
    if (mTeams.size()%2)
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Nombre impair d'équipes, ajoutez une équipe fictive"),
                                    QMessageBox::Ok);
    }
    else
    {
        std::deque<Game> games;
        std::string error;

        if (mCurrentEvent.type == Event::cRoundRobin)
        {
            int rounds = ui->spinNbRounds->value();
            error = mTournament.BuildRoundRobinRounds(mTeams, rounds, games);
        }
        else
        {
            // Swiss algorithm
            error = mTournament.BuildSwissRounds(mGames, mTeams, games);
        }

        if (games.size() > 0)
        {
            mCurrentEvent.state = Event::cStarted;
            mDatabase.UpdateEventState(mCurrentEvent);

            if (!mDatabase.AddGames(games))
            {
                TLogError("Cannot store rounds!");
            }

            UpdateGameList();
        }
        else
        {
            TLogError("Cannot build rounds!");
            (void) QMessageBox::warning(this, tr("Tanca"),
                                        tr("Impossible de générer les parties : ") + QString(error.c_str()),
                                        QMessageBox::Ok);
        }
    }
}


bool MainWindow::FindGame(const int id, Game &game)
{
    bool found = false;
    for (size_t i = 0; i < mGames.size(); i++)
    {
        if (mGames[i].id == id)
        {
            found = true;
            game = mGames[i];
            break;
        }
    }

    return found;
}

void MainWindow::UpdateBrackets()
{
    QString json; // FIXME = mTournament.ToJsonString(mGames, mTeams);

    mServer.SetGames(json.toStdString());
}


void MainWindow::UpdateGameList()
{
    mGames = mDatabase.GetGamesByEventId(mCurrentEvent.id);

    UpdateBrackets();

    TableHelper helper(ui->gameTable);
    helper.Initialize(gGamesTableHeader, mGames.size());

    foreach (Game game, mGames)
    {
        Team t1, t2;

        // Be tolerant: only print found teams (forget return code)
        (void) Team::Find(mTeams, game.team1Id, t1);
        (void) Team::Find(mTeams, game.team2Id, t2);

        std::list<Value> gameData = {game.id, (int)(game.turn + 1)
                , "(" + std::to_string(t1.number) + ") " + t1.teamName
                , "(" + std::to_string(t2.number) + ") " + t2.teamName
                , game.team1Score, game.team2Score};
        helper.AppendLine(gameData, game.IsPlayed());

    }

    helper.Finish();
    ui->gameTable->sortByColumn(1, Qt::AscendingOrder);
}

void MainWindow::slotAddGame()
{
    // Prepare widget contents
    gameWindow->Initialize(mTeams);
    gameWindow->AllowZeroNumber(false);

    if (gameWindow->exec() == QDialog::Accepted)
    {
        Game game;
        gameWindow->GetGame(game);
        game.eventId = mCurrentEvent.id;
        game.turn = gameWindow->GetNumber() - 1; // turns begin internally @ zero

        std::deque<Game> list;
        list.push_back(game);
        if (mDatabase.AddGames(list))
        {
            if (mCurrentEvent.state != Event::cStarted)
            {
                mCurrentEvent.state = Event::cStarted;
                mDatabase.UpdateEventState(mCurrentEvent);
            }
            UpdateGameList();
        }
    }
}

void MainWindow::slotEditGame()
{
    QModelIndexList indexes = ui->gameTable->selectionModel()->selection().indexes();

    if (indexes.size() > 1)
    {
        QModelIndex index = indexes.at(0);
        QMap<int, QVariant> data = ui->gameTable->model()->itemData(index);

        if (data.contains(0))
        {
            int id = data[0].toInt();
            std::cout << "Found team id: " << id << std::endl;
            Game game;

            if (FindGame(id, game))
            {
                Team team1;
                Team team2;

                // Be tolerant if teams are not found
                (void) Team::Find(mTeams, game.team1Id, team1);
                (void) Team::Find(mTeams, game.team2Id, team2);

                scoreWindow->SetGame(game, team1, team2);
                if (scoreWindow->exec() == QDialog::Accepted)
                {
                    scoreWindow->GetGame(game);
                    if (!mDatabase.EditGame(game))
                    {
                        TLogError("Cannot edit game!");
                    }
                    else
                    {
                        UpdateGameList();
                    }
                }
            }
        }
        else
        {
            TLogError("Cannot find any valid game in the list");
        }
    }
}


void MainWindow::slotDeleteGame()
{
    TableHelper helper(ui->gameTable);

    int id;
    if (helper.GetFirstColumnValue(id))
    {
        if (QMessageBox::warning(this, tr("Suppression d'une rencontre"),
                                    tr("Attention ! Tous les points associées seront perdus. Continuer ?"),
                                    QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
        {
            if (!mDatabase.DeleteGame(id))
            {
                TLogError("Delete game failure");
            }
            UpdateGameList();
        }
    }
}

void MainWindow::slotDeleteAllGames()
{
    if (QMessageBox::warning(this, tr("Suppression de toutes les rencontres"),
                                tr("Attention ! Tous les points associées seront perdus. Continuer ?"),
                                QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
    {


        for( int r = 0; r < ui->gameTable->rowCount(); ++r )
        {
            int id = ui->gameTable->item(r, 0)->text().toInt();

            if (!mDatabase.DeleteGame(id))
            {
                TLogError("Delete game failure");
            }
        }

        UpdateGameList();
    }
}

void MainWindow::slotExportGames()
{
    ExportTable(ui->gameTable, tr("Exporter la liste des parties au format Excel (CSV)"));
}

std::string StateToString(const Event &event)
{
    if (event.state == Event::cNotStarted)
    {
        return "Non démarré";
    }
    else if (event.state == Event::cStarted)
    {
        return "En cours";
    }
    else if (event.state == Event::cCanceled)
    {
        return "Annulé";
    }
    else
    {
        return "";
    }
}

std::string TypeToString(const Event &event)
{
    if (event.type == Event::cRoundRobin)
    {
        return "Tournoi type toutes rondes";
    }
    else if (event.type == Event::cSwissRounds)
    {
        return "Tournoi type Suisse";
    }
    else
    {
        return "";
    }
}
