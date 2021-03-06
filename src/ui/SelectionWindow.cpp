/*=============================================================================
 * Tanca - SelectionWindow.cpp
 *=============================================================================
 * Base class widget that is used by several selection dialogs
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

#include "SelectionWindow.h"
#include "TableHelper.h"
#include "Log.h"

SelectionWindow::SelectionWindow(QWidget *parent, const QString &title, int minSize, int maxSize)
    : QDialog(parent)
    , mHelper(NULL)
    , mMinSize(minSize)
    , mMaxSize(maxSize)
{
    ui.setupUi(this);
    ui.labelTitle->setText(title);
    ui.spinSelectionNumber->setMinimum(0);

    mHelper.SetTableWidget(ui.playersTable);

    connect(ui.buttonCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(ui.buttonSwap, &QPushButton::clicked, this, &SelectionWindow::slotClicked);

    connect(ui.playersTable, SIGNAL(itemSelectionChanged()), this, SLOT(slotPlayerItemActivated()));
    connect(ui.selectionList, &QListWidget::itemClicked, this, &SelectionWindow::slotSelectionItemActivated);
    connect(ui.lineEditFilter, &QLineEdit::textChanged, this, &SelectionWindow::slotFilter);
}

void SelectionWindow::slotFilter()
{
    QString filter = ui.lineEditFilter->text();
    for( int i = 0; i < ui.playersTable->rowCount(); ++i )
    {
        bool match = false;
        for( int j = 0; j < ui.playersTable->columnCount(); ++j )
        {
            QTableWidgetItem *item = ui.playersTable->item( i, j );
            if( item->text().contains(filter, Qt::CaseInsensitive) )
            {
                match = true;
                break;
            }
        }
        ui.playersTable->setRowHidden( i, !match );
    }
}

void SelectionWindow::SetLabelNumber(const QString &name)
{
    ui.label_number->setText(name);
}

void SelectionWindow::SetHeader(const QStringList &tableHeader)
{
    mTableHeader = tableHeader;
}

void SelectionWindow::StartUpdate(int size)
{
    ui.playersTable->clear();
    ui.selectionList->clear();

    mHelper.Initialize(mTableHeader, size);
}

void SelectionWindow::FinishUpdate()
{
    mHelper.Finish();
}

void SelectionWindow::AddLeftEntry(const std::list<Value> &rowData)
{
    mHelper.AppendLine(rowData, false);
}

void SelectionWindow::AddRightEntry(const QString &text)
{
    ui.selectionList->addItem(text);
}

void SelectionWindow::SetName(const QString &name)
{
    ui.lineName->setText(name);
}

void SelectionWindow::SetNumber(std::uint32_t number)
{
    ui.spinSelectionNumber->setValue(number);
}

uint32_t SelectionWindow::GetNumber()
{
    return ui.spinSelectionNumber->value();
}

QString SelectionWindow::GetName()
{
    return ui.lineName->text();
}

void SelectionWindow::AllowZeroNumber(bool enable)
{
    if (enable)
    {
        ui.spinSelectionNumber->setMinimum(0);
    }
    else
    {
        ui.spinSelectionNumber->setMinimum(1);
    }
}

void SelectionWindow::slotPlayerItemActivated()
{
    ui.selectionList->clearSelection();
    ui.selectionList->clearFocus();
}

void SelectionWindow::slotSelectionItemActivated(QListWidgetItem *item)
{
    (void) item;
    ui.playersTable->clearSelection();
    ui.playersTable->clearFocus();
}

void SelectionWindow::slotClicked()
{
    int selectionRight = ui.selectionList->currentRow();
    TableHelper helper(ui.playersTable);

    int id = -1;
    (void) helper.GetFirstColumnValue(id);

    if (id > -1)
    {
        ClickedLeft(id);
    }
    else if (selectionRight > -1)
    {
        ClickedRight(selectionRight);
    }
}

void SelectionWindow::slotAccept()
{
    if ((ui.selectionList->count() >= (int)mMinSize) && (ui.selectionList->count() <= (int)mMaxSize))
    {
        accept();
    }
}

