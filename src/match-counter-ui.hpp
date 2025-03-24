/*
Match Counter for OBS
Copyright (C) 2025 Yudai Udagawa

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <memory>

extern "C" {
#include "match-counter.h"
}

class MatchCounterDialog : public QDialog {
	Q_OBJECT

public:
	explicit MatchCounterDialog(QWidget *parent = nullptr);
	~MatchCounterDialog();

private slots:
	void onAddWin();
	void onAddLoss();
	void onSubtractWin();
	void onSubtractLoss();
	void onReset();
	void onFormatChanged(const QString &text);
	void onWinsChanged(int value);
	void onLossesChanged(int value);
	void updateDisplay();

private:
	QVBoxLayout *mainLayout;
	QHBoxLayout *counterLayout;
	QFormLayout *formLayout;
	QHBoxLayout *buttonLayout;

	QLabel *displayLabel;
	QLineEdit *formatEdit;
	QSpinBox *winsSpinBox;
	QSpinBox *lossesSpinBox;
	QLabel *winRateValueLabel;

	QPushButton *addWinButton;
	QPushButton *addLossButton;
	QPushButton *subtractWinButton;
	QPushButton *subtractLossButton;
	QPushButton *resetButton;

	match_counter_t *counter;
};

void showMatchCounterDialog();
