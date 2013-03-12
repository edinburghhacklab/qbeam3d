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

#include "ScannerSettings.h"
#include <qspinbox.h>
#include <qslider.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qevent.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qstring.h>
#include <qwidget.h>

ScannerSettings::ScannerSettings(QWidget* parent, const char* name, WFlags fl) :
	ScannerSettingsWidget(parent,name,fl),
	m_settings(),
	m_contrastMap(),
	m_scanMap()
{
	m_contrastMap[tr("Einmal vor dem Scan")]=L2000::ScanOptions::Once;
	m_contrastMap[tr("Jede Rasterspalte")]=L2000::ScanOptions::PerColumn;
	m_contrastMap[tr("Automatik (langsam)")]=L2000::ScanOptions::Automatic;
	m_contrastMap[tr("Kalibriermodus")]=L2000::ScanOptions::Calibration;
	m_scanMap[tr("Ganzer Bereich")]=L2000::ScanOptions::Full;
	m_scanMap[tr("Schnitt horizontal")]=L2000::ScanOptions::HorizontalCut;
	m_scanMap[tr("Schnitt vertikal")]=L2000::ScanOptions::VerticalCut;

	m_settings.contrastMode		= m_contrastMap[m_contrastModeComboBox->currentText()];
	m_settings.scanMode			= m_scanMap[m_scanModeComboBox->currentText()];


	m_lowerBoundSpinBox->setValue( m_settings.laserStart);
	m_lowerBoundSpinBox->setMaxValue( m_settings.laserEnd-1 );
	m_upperBoundSpinBox->setValue( m_settings.laserEnd);
	m_upperBoundSpinBox->setMinValue( m_settings.laserStart+1 );
	m_laserStepSpinBox->setValue( m_settings.laserStart);
	m_laserResSpinBox->setValue( int(m_settings.laserRange()/m_settings.laserStep) );

	m_tableRangeSpinBox->setValue( m_settings.tableRange );
	m_tableStepSpinBox->setValue( int(m_settings.tableStep) );
	m_tableResSpinBox->setMaxValue( m_settings.tableRange );
	m_tableResSpinBox->setValue( m_settings.tableRange );
	//m_settings.contrastMode
	//m_settings.scanMode
	//m_settings.layer

	connect(m_lowerBoundSpinBox,	SIGNAL(valueChanged(int)), this, SLOT(stepperBeginChanged(int)) );
	connect(m_upperBoundSpinBox,	SIGNAL(valueChanged(int)), this, SLOT(stepperEndChanged(int)) );
	connect(m_laserStepSpinBox,		SIGNAL(valueChanged(int)), this, SLOT(stepperStepChanged(int)) );
	connect(m_laserResSpinBox,		SIGNAL(valueChanged(int)), this, SLOT(stepperResChanged(int)) );

	// table stuff
	connect(m_tableRangeSpinBox,	SIGNAL(valueChanged(int)), this, SLOT(tableRangeChanged(int)) );
	connect(m_tableResSpinBox,		SIGNAL(valueChanged(int)), this, SLOT(tableResolutionChanged(int)) );
	connect(m_tableStepSpinBox,		SIGNAL(valueChanged(int)), this, SLOT(tableStepChanged(int)) );

	// modes
	connect(m_scanModeComboBox,		SIGNAL(activated(int)), this, SLOT(scanModeChanged(int)) );
	connect(m_contrastModeComboBox,	SIGNAL(activated(int)), this, SLOT(contrastModeChanged(int)) );

	//connect(m_actualPosToUpperButton, SIGNAL(), this, SLOT() );
	//connect(m_actualPosToLowerButton, SIGNAL(), this, SLOT() );
	//connect(m_detectUpperButton, SIGNAL(), this, SLOT() );
	//connect(m_detectLowerButton, SIGNAL(), this, SLOT() );
	paintRangeDisplay();
}

ScannerSettings::~ScannerSettings()
{}

const L2000::ScanOptions& ScannerSettings::settings()
{
	m_settings.correctReadings	= m_errorCorrectionCheckBox->isChecked();
	m_settings.createBottom		= m_createBottomCheckBox->isChecked();
	m_settings.createLid		= m_createLidCheckBox->isChecked();
	m_settings.holeThreshold	= m_closeHolesCheckBox->isChecked() ? m_closeHoleSpinBox->value() : -1;
	m_settings.minBorderPoints  = m_borderHoleSpinBox->value();
	m_settings.smoothBorders	= m_smoothBorderCheckBox->isChecked();
	m_settings.createCCD1Phantom	= m_createCCD1Phantom->isChecked();
	m_settings.createCCD2Phantom	= m_createCCD2Phantom->isChecked();
	return m_settings;
}


void ScannerSettings::setStepperBegin(int x)
{
	m_settings.laserStart = x;
	m_upperBoundSpinBox->setMinValue(x+1);
	m_laserResSpinBox->setMaxValue(m_settings.laserRange());
}

void ScannerSettings::setStepperEnd(int x)
{
	m_settings.laserEnd = x;
	m_lowerBoundSpinBox->setMaxValue(x-1);
	m_laserResSpinBox->setMaxValue(m_settings.laserRange());
}

void ScannerSettings::setStepperStep(int x)
{
	m_settings.laserStep = double(x);
	m_laserStepSpinBox->setValue(x);
	m_laserResSpinBox->setValue((m_settings.laserRange())/x);
}

void ScannerSettings::setStepperResolution(int x)
{
	m_laserResSpinBox->setValue(x);
	m_settings.laserStep = double(m_settings.laserRange())/double(x);
	m_laserStepSpinBox->setValue( m_settings.laserStep < 1.0 ? 1 : int(m_settings.laserStep) );
}

void ScannerSettings::setTableRange(int x)
{
	m_settings.tableRange = x;
	m_tableRangeSpinBox->setValue(x);
	m_tableResSpinBox->setMaxValue(x);
	m_tableResSpinBox->setValue( int(double(m_settings.tableRange)/m_settings.tableStep) );
	paintRangeDisplay();
}

void ScannerSettings::setTableStep(int x)
{
	m_settings.tableStep = x;
	m_tableStepSpinBox->setValue(x);
	m_tableResSpinBox->setValue(int(double(m_settings.tableRange)/m_settings.tableStep));
}

void ScannerSettings::setTableResolution(int x)
{
	m_settings.tableStep = double(m_settings.tableRange)/double(x);
	m_tableResSpinBox->setValue(x);
	x = m_settings.tableStep < 1.0 ? 1 : int(m_settings.tableStep);
	m_tableStepSpinBox->setValue(x);
}

void ScannerSettings::setScanMode(int x)
{
	m_settings.scanMode = m_scanMap[m_scanModeComboBox->text(x)];
}

void ScannerSettings::setContrastMode(int x)
{
	m_settings.contrastMode = m_contrastMap[m_contrastModeComboBox->text(x)];
}

//***************************************************
// SLOTS for keeping display units consistent
//***************************************************

void ScannerSettings::stepperBeginChanged(int x)
{
	setStepperBegin(x);
}

void ScannerSettings::stepperEndChanged(int x)
{
	setStepperEnd(x);
}

void ScannerSettings::stepperStepChanged(int x)
{
	setStepperStep(x);
}

void ScannerSettings::stepperResChanged(int x)
{
	setStepperResolution(x);
}

void ScannerSettings::tableRangeChanged(int x)
{
	setTableRange(x);
}

void ScannerSettings::tableStepChanged(int x)
{
	setTableStep(x);
}

void ScannerSettings::tableResolutionChanged(int x)
{
	setTableResolution(x);
}

void ScannerSettings::scanModeChanged(int m)
{
	setScanMode(m);
}

void ScannerSettings::contrastModeChanged(int m)
{
	setContrastMode(m);
}

void ScannerSettings::paintEvent( QPaintEvent* e )
{
	ScannerSettingsWidget::paintEvent(e);
	paintRangeDisplay();
}

void ScannerSettings::paintRangeDisplay()
{
	QPainter p(m_tableScanRangeDisplay);
	p.setBrush( QBrush(colorGroup().shadow(),QBrush::Dense4Pattern) );

	p.eraseRect(2, 2,
				m_tableScanRangeDisplay->width()-4, m_tableScanRangeDisplay->height()-4 );
	p.drawPie(	2, 2,
				m_tableScanRangeDisplay->width()-4, m_tableScanRangeDisplay->width()-4,
				0, m_tableRangeSpinBox->value()*8 );
	p.drawText(	2, m_tableScanRangeDisplay->width()+2,
				m_tableScanRangeDisplay->width()-4, m_tableScanRangeDisplay->height()-m_tableScanRangeDisplay->width()-4,
				Qt::AlignRight, QString::number(double(m_tableRangeSpinBox->value())/2.0)+"°");
	p.end();
}


