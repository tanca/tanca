#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QDialog>
#include "ui_PlayerWindow.h"
#include "DbManager.h"

class PlayerWindow : public QDialog
{
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = 0);

    Player GetPlayer();

private:
    Ui::PlayerWindow ui;
    Player mPlayer;
};

#endif // PLAYERWINDOW_H
