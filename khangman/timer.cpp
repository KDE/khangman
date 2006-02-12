/***************************************************************************
 *   Copyright (C) 2005 by Michael Goettsche                               *
 *   michael.goettsche@kdemail.net                                         *
 *   Copyright (C) 2006 by Anne-Marie Mahfouf                               *
 *   annemarie.mahfouf@free.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include <klocale.h>

#include <qlabel.h>
#include <qslider.h>

#include "timer.h"
#include "prefs.h"


Timer::Timer()
    :QWidget()
{
    ui_timer.setupUi(this);
    ui_timer.timeHintLabel->setText(QString::number(Prefs::hintTimer()) + " " + i18n("seconds"));
    ui_timer.timeMissedLabel->setText(QString::number(Prefs::missedTimer()) + " " + i18n("seconds"));

    connect(ui_timer.kcfg_HintTimer,   SIGNAL(valueChanged(int)),
	    this,             SLOT(sliderValueChanged()));
    connect(ui_timer.kcfg_MissedTimer, SIGNAL(valueChanged(int)),
	    this,             SLOT(sliderValueChanged()));
}


void Timer::sliderValueChanged()
{
    ui_timer.timeMissedLabel->setText(QString::number(ui_timer.kcfg_MissedTimer->value()) + " " + i18n("seconds"));
    ui_timer.timeHintLabel->setText(QString::number(ui_timer.kcfg_HintTimer->value()) + " " + i18n("seconds"));
}


#include "timer.moc"