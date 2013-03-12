/***************************************************************************
 *   Copyright (C) 2004,2005 by Maximilian Hrabowski                       *
 *   maximilian@hrabowski.net                                              *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SCANNERSETTINGS_H
#define SCANNERSETTINGS_H

#include "ScannerSettingsWidget.h"
#include "L2000ScanOptions.h"

#include <map>

class QPaintEvent;

class ScannerSettings : public ScannerSettingsWidget
{
  Q_OBJECT

public:
  ScannerSettings(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~ScannerSettings();
  /*$PUBLIC_FUNCTIONS$*/

	void setStepperBegin(int);
	void setStepperEnd(int);
	void setStepperStep(int);
	void setStepperResolution(int);

	void setTableRange(int);
	void setTableStep(int);
	void setTableResolution(int);

	void setScanMode(int);
	void setContrastMode(int);

	const L2000::ScanOptions& settings();

protected slots:
  /*$PUBLIC_SLOTS$*/
	void stepperBeginChanged(int);
	void stepperEndChanged(int);
	void stepperStepChanged(int);
	void stepperResChanged(int);

	void tableRangeChanged(int);
	void tableStepChanged(int);
	void tableResolutionChanged(int);

	void scanModeChanged(int);
	void contrastModeChanged(int);

protected:

	void paintEvent( QPaintEvent* e );
	void paintRangeDisplay();

	L2000::ScanOptions m_settings;

	std::map<QString,L2000::ScanOptions::ContrastMode>	m_contrastMap;
	std::map<QString,L2000::ScanOptions::ScanMode>		m_scanMap;

};

#endif

