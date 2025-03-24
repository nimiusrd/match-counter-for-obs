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

#include "match-counter-ui.hpp"
#include <obs-frontend-api.h>
#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>

extern "C" {
#include <obs-module.h>
#include <plugin-support.h>
}

MatchCounterDialog::MatchCounterDialog(QWidget *parent) : QDialog(parent), counter(match_counter_get_global())
{
	setWindowTitle(obs_module_text("MatchCounterTitle"));
	setMinimumWidth(400);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// メインレイアウト
	mainLayout = new QVBoxLayout(this);

	// 表示部分
	displayLabel = new QLabel(this);
	displayLabel->setAlignment(Qt::AlignCenter);
	displayLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
	mainLayout->addWidget(displayLabel);

	// フォーム部分
	QGroupBox *settingsGroup = new QGroupBox(obs_module_text("Settings"), this);
	formLayout = new QFormLayout(settingsGroup);

	playerNameEdit = new QLineEdit(this);
	playerNameEdit->setText(counter->player_name);
	connect(playerNameEdit, &QLineEdit::textChanged, this, &MatchCounterDialog::onPlayerNameChanged);
	formLayout->addRow(obs_module_text("PlayerName"), playerNameEdit);

	formatEdit = new QLineEdit(this);
	formatEdit->setText(counter->format);
	formatEdit->setToolTip(obs_module_text("FormatTooltip"));
	connect(formatEdit, &QLineEdit::textChanged, this, &MatchCounterDialog::onFormatChanged);
	formLayout->addRow(obs_module_text("Format"), formatEdit);

	// カウンター部分
	QGroupBox *counterGroup = new QGroupBox(obs_module_text("Counter"), this);
	counterLayout = new QHBoxLayout(counterGroup);

	QLabel *winsLabel = new QLabel(obs_module_text("Wins"), this);
	winsSpinBox = new QSpinBox(this);
	winsSpinBox->setRange(0, 999);
	winsSpinBox->setValue(counter->wins);
	connect(winsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MatchCounterDialog::onWinsChanged);

	QLabel *lossesLabel = new QLabel(obs_module_text("Losses"), this);
	lossesSpinBox = new QSpinBox(this);
	lossesSpinBox->setRange(0, 999);
	lossesSpinBox->setValue(counter->losses);
	connect(lossesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MatchCounterDialog::onLossesChanged);

	counterLayout->addWidget(winsLabel);
	counterLayout->addWidget(winsSpinBox);
	counterLayout->addWidget(lossesLabel);
	counterLayout->addWidget(lossesSpinBox);

	// ボタン部分
	QGroupBox *buttonGroup = new QGroupBox(obs_module_text("Actions"), this);
	buttonLayout = new QHBoxLayout(buttonGroup);

	addWinButton = new QPushButton(obs_module_text("AddWin"), this);
	connect(addWinButton, &QPushButton::clicked, this, &MatchCounterDialog::onAddWin);

	addLossButton = new QPushButton(obs_module_text("AddLoss"), this);
	connect(addLossButton, &QPushButton::clicked, this, &MatchCounterDialog::onAddLoss);

	subtractWinButton = new QPushButton(obs_module_text("SubtractWin"), this);
	connect(subtractWinButton, &QPushButton::clicked, this, &MatchCounterDialog::onSubtractWin);

	subtractLossButton = new QPushButton(obs_module_text("SubtractLoss"), this);
	connect(subtractLossButton, &QPushButton::clicked, this, &MatchCounterDialog::onSubtractLoss);

	resetButton = new QPushButton(obs_module_text("Reset"), this);
	connect(resetButton, &QPushButton::clicked, this, &MatchCounterDialog::onReset);

	buttonLayout->addWidget(addWinButton);
	buttonLayout->addWidget(addLossButton);
	buttonLayout->addWidget(subtractWinButton);
	buttonLayout->addWidget(subtractLossButton);
	buttonLayout->addWidget(resetButton);

	// レイアウトに追加
	mainLayout->addWidget(settingsGroup);
	mainLayout->addWidget(counterGroup);
	mainLayout->addWidget(buttonGroup);

	// 表示を更新
	updateDisplay();
}

MatchCounterDialog::~MatchCounterDialog()
{
	// QTが自動的にクリーンアップするので何もしない
}

void MatchCounterDialog::onAddWin()
{
	match_counter_add_win(counter);
	winsSpinBox->setValue(counter->wins);
	updateDisplay();
}

void MatchCounterDialog::onAddLoss()
{
	match_counter_add_loss(counter);
	lossesSpinBox->setValue(counter->losses);
	updateDisplay();
}

void MatchCounterDialog::onSubtractWin()
{
	match_counter_subtract_win(counter);
	winsSpinBox->setValue(counter->wins);
	updateDisplay();
}

void MatchCounterDialog::onSubtractLoss()
{
	match_counter_subtract_loss(counter);
	lossesSpinBox->setValue(counter->losses);
	updateDisplay();
}

void MatchCounterDialog::onReset()
{
	match_counter_reset(counter);
	winsSpinBox->setValue(counter->wins);
	lossesSpinBox->setValue(counter->losses);
	updateDisplay();
}

void MatchCounterDialog::onPlayerNameChanged(const QString &text)
{
	match_counter_set_player_name(counter, text.toUtf8().constData());
	updateDisplay();
}

void MatchCounterDialog::onFormatChanged(const QString &text)
{
	match_counter_set_format(counter, text.toUtf8().constData());
	updateDisplay();
}

void MatchCounterDialog::onWinsChanged(int value)
{
	counter->wins = value;
	updateDisplay();
}

void MatchCounterDialog::onLossesChanged(int value)
{
	counter->losses = value;
	updateDisplay();
}

void MatchCounterDialog::updateDisplay()
{
	char *text = match_counter_get_formatted_text(counter);
	displayLabel->setText(QString::fromUtf8(text));
	bfree(text);
}

// グローバル変数
// 将来的に使用する可能性があるため宣言しておく
// static QAction *action = nullptr;

// メニューアクションのコールバック
static void menu_action_clicked()
{
	QMainWindow *main_window = (QMainWindow *)obs_frontend_get_main_window();
	MatchCounterDialog dialog(main_window);
	dialog.exec();
}

// ダイアログを表示する関数
void showMatchCounterDialog()
{
	QMainWindow *main_window = (QMainWindow *)obs_frontend_get_main_window();
	MatchCounterDialog dialog(main_window);
	dialog.exec();
}

// フロントエンドのイベントコールバック
static void frontend_event_callback(enum obs_frontend_event event, void *)
{
	if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
		QAction *action = (QAction *)obs_frontend_add_tools_menu_qaction(obs_module_text("MatchCounter"));

		QObject::connect(action, &QAction::triggered, menu_action_clicked);
	}
}

// モジュールロード時に呼ばれる関数
extern "C" void match_counter_ui_init()
{
	obs_frontend_add_event_callback(frontend_event_callback, nullptr);
}
