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

#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlcdnumber.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qwidgetstack.h>
#include <qtoolbutton.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qprogressdialog.h>

#include "CalibrationDialog.h"

#include "Plane2DMediator.h"
#include "Plane2DPoints.h"
#include "Plane2DView.h"

CalibrationDialog::CalibrationDialog(L2000::Scanner& scanner, QWidget* parent, const char* name, bool modal, WFlags fl) :
	CalibrationWizard(parent,name, modal,fl),
	m_scanner(scanner)
{
	helpButton()->hide();

	// enables precalibration on check box click
	connect( m_precalibCheckBox, SIGNAL(toggled(bool)), this, SLOT(setPrecalibration(bool)) );
	// does a layer change, when a radio box is clicked during precalibration
	connect( m_layerGroup, SIGNAL(clicked(int)), this, SLOT(layerChanged(int)) );

	// enable cursor block
	connect( m_up, SIGNAL(clicked()), this, SLOT( laserCursorUp() ) );
	connect( m_5up, SIGNAL(clicked()), this, SLOT( laserCursorPageUp() ) );
	connect( m_fineUp, SIGNAL(clicked()), this, SLOT( laserCursorFineUp() ) );
	connect( m_down, SIGNAL(clicked()), this, SLOT( laserCursorDown() ) );
	connect( m_5down, SIGNAL(clicked()), this, SLOT( laserCursorPageDown() ) );
	connect( m_fineDown, SIGNAL(clicked()), this, SLOT( laserCursorFineDown() ) );

	connect( m_previousLayerButton, SIGNAL(clicked()), this, SLOT(previousLayer()) );

	connect( m_use1CheckBox, SIGNAL(toggled(bool)), this, SLOT(setLayer1Enabled(bool)) );
	connect( m_use2CheckBox, SIGNAL(toggled(bool)), this, SLOT(setLayer2Enabled(bool)) );
	connect( m_use3CheckBox, SIGNAL(toggled(bool)), this, SLOT(setLayer3Enabled(bool)) );
	connect( m_use4CheckBox, SIGNAL(toggled(bool)), this, SLOT(setLayer4Enabled(bool)) );
	connect( m_use5CheckBox, SIGNAL(toggled(bool)), this, SLOT(setLayer5Enabled(bool)) );
	connect( m_use6CheckBox, SIGNAL(toggled(bool)), this, SLOT(setLayer6Enabled(bool)) );
	connect( m_use7CheckBox, SIGNAL(toggled(bool)), this, SLOT(setLayer7Enabled(bool)) );

	connect( m_calibrateButton, SIGNAL(clicked()), this, SLOT(doCalibration()) );
	connect( m_optimizeButton,  SIGNAL(clicked()), this, SLOT(doOptimization()) );
}

void CalibrationDialog::closeEvent( QCloseEvent* e )
{
	m_scanner.setLaserOn(false);
	m_scanner.close();
	CalibrationWizard::closeEvent(e);
}

CalibrationDialog::~CalibrationDialog()
{
}

void CalibrationDialog::setLayer1Enabled( bool f )
{
	m_scanner.calibrationData().setStepperPosUseFlag(0,f);
	updatePreCabResult();
}
void CalibrationDialog::setLayer2Enabled( bool f )
{
	m_scanner.calibrationData().setStepperPosUseFlag(1,f);
	updatePreCabResult();
}
void CalibrationDialog::setLayer3Enabled( bool f )
{
	m_scanner.calibrationData().setStepperPosUseFlag(2,f);
	updatePreCabResult();
}
void CalibrationDialog::setLayer4Enabled( bool f )
{
	m_scanner.calibrationData().setStepperPosUseFlag(3,f);
	updatePreCabResult();
}
void CalibrationDialog::setLayer5Enabled( bool f )
{
	m_scanner.calibrationData().setStepperPosUseFlag(4,f);
	updatePreCabResult();
}
void CalibrationDialog::setLayer6Enabled( bool f )
{
	m_scanner.calibrationData().setStepperPosUseFlag(5,f);
	updatePreCabResult();
}
void CalibrationDialog::setLayer7Enabled( bool f )
{
	m_scanner.calibrationData().setStepperPosUseFlag(6,f);
	updatePreCabResult();
}
void CalibrationDialog::updatePreCabResult()
{
	// insert value into calibration object
	L2000::Calibration& c = m_scanner.calibrationData();
	if( c.preCabReady() )
	{
		c.preCalibrate(7);
		m_angleLabel->setText( QString::number(c.alpha(0)*180.0/M_PI,'f',2) + "°" );
		m_angleIntLabel->setText("+-" + QString::number(c.alphaError()*180.0/M_PI,'f',2) + "°");
		m_distanceLabel->setText(QString::number(c.laserPos().first,'f',2)+"mm");
		m_distanceIntLabel->setText("+-" + QString::number(c.laserPosError().first,'f',2)+"mm");
	}
	else {
		// enable results
		m_angleLabel->setText("0°");
		m_angleIntLabel->setText("+- 0°");
		m_distanceLabel->setText("0mm");
		m_distanceIntLabel->setText("+- 0mm");
	}
}

void CalibrationDialog::initIntroduction()
{
	if( !m_scanner.init() )
	{
		QMessageBox::critical(this,tr("Scanner not found"),tr("There could no L2000 scanner be detected on any COM port.\nPlease make sure it is plugged in and switched on."));
		done(QDialog::Rejected);
		return;
	}
	m_scanner.open();

	m_scanner.calibrationData().clearStepperPos();
	m_scanner.calibrationData().readCalibrationData();

	if( m_scanner.calibrationData().preCabReady() ) {
		m_calibrateOriginText->hide();
		m_precalibCheckBox->setChecked(false);
		setAppropriate( m_preCalibrationPage, false );
		setAppropriate( m_ccdCalibrationPage, true );
	}
	else {
		m_precalibCheckBox->setChecked(true);
		m_precalibCheckBox->setEnabled(false);
		m_calibrateOriginText->show();
		setAppropriate( m_preCalibrationPage, true );
		setAppropriate( m_ccdCalibrationPage, true );
	}
	setNextEnabled( m_introductionPage, true );
}

void CalibrationDialog::setPrecalibration( bool toggle )
{
	setAppropriate( m_preCalibrationPage, toggle );
}

void CalibrationDialog::initPrecalibration()
{
	m_originTextStack->raiseWidget( m_originStepText1 );
	// set LCDs to old values
	m_lcd1->setText(QString::number(m_scanner.calibrationData().stepperPos(0)));
	m_lcd2->setText(QString::number(m_scanner.calibrationData().stepperPos(1)));
	m_lcd3->setText(QString::number(m_scanner.calibrationData().stepperPos(2)));
	m_lcd4->setText(QString::number(m_scanner.calibrationData().stepperPos(3)));
	m_lcd5->setText(QString::number(m_scanner.calibrationData().stepperPos(4)));
	m_lcd6->setText(QString::number(m_scanner.calibrationData().stepperPos(5)));
	m_lcd7->setText(QString::number(m_scanner.calibrationData().stepperPos(6)));
	// enable all LCDs
	m_use1CheckBox->setChecked(m_scanner.calibrationData().stepperPosUsed(0));
	m_use2CheckBox->setChecked(m_scanner.calibrationData().stepperPosUsed(1));
	m_use3CheckBox->setChecked(m_scanner.calibrationData().stepperPosUsed(2));
	m_use4CheckBox->setChecked(m_scanner.calibrationData().stepperPosUsed(3));
	m_use5CheckBox->setChecked(m_scanner.calibrationData().stepperPosUsed(4));
	m_use6CheckBox->setChecked(m_scanner.calibrationData().stepperPosUsed(5));
	m_use7CheckBox->setChecked(m_scanner.calibrationData().stepperPosUsed(6));
	// disable positioner
	setCursorBlockEnabled(false);

	m_laserPositionLCD->setDecMode();
	m_laserPositionLCD->setNumDigits(5);
	m_laserPositionLCD->setSmallDecimalPoint(true);
	m_laserPositionLCD->display(0.00);
	// enable start button
	m_trackLayerButton->setEnabled(true);

	// clear position array
	//m_scanner.calibrationData().clearStepperPos();
	m_currentLayer	= 0;
	m_currentPos	= m_scanner.calibrationData().stepperPos(0);
	m_currentId		= -1;
	// set start to first layer
	//m_radio1->setChecked(true);
	disconnect( m_trackLayerButton, 0, this, 0 );
	m_layerGroup->setButton( m_layerGroup->id(m_radio1) );
	layerChanged( m_layerGroup->id(m_radio1) );
	m_automatic = true;

	setNextEnabled( m_preCalibrationPage, m_scanner.calibrationData().preCabReady() );
	updatePreCabResult();
/*
  // vars and methods for precalibration
  int					m_currentLayer;
  int					m_currentPos;

  void					layerChanged(int id);   // called when radio button clicked
  void					startLayer( int i );	// called when button "Track Border" clicked
  void					stopLayer( int i );		// called when button "Ok" clicked
*/
}

// setup layer for pressing "track border"
void CalibrationDialog::layerChanged(int id)
{
	if( id != m_currentId )
	{
		int layer = radioIdToIndex(id);
		if( layer < 0 || layer >= 7 )
			return;
		m_currentPos = 0;

		if(m_automatic && m_currentLayer>0)
			m_currentPos = m_scanner.calibrationData().stepperPos(m_currentLayer-1)-10.0;
		else
			m_currentPos = m_scanner.calibrationData().stepperPos(layer);

		m_automatic = false;
		m_currentLayer = layer;
		m_currentId = id;

		//m_laserPositionLCD->diplay( m_currentPos );

		disconnect( m_trackLayerButton, 0, this, 0 );
		connect( m_trackLayerButton, SIGNAL(clicked()), this, SLOT(startLayer()) );
		m_trackLayerButton->setText(tr("&Track border"));
		m_previousLayerButton->setEnabled( m_currentLayer > 0 );
	}
}

void CalibrationDialog::startLayer()
{
	if( m_currentLayer < 0 || m_currentLayer >= 7 )
		return;

	m_currentPos = m_scanner.calibrationData().stepperPos(m_currentLayer);
	if( m_currentLayer>0 && m_currentPos == 0 )
		m_currentPos = m_scanner.calibrationData().stepperPos(m_currentLayer-1)-10.0;
	if( m_currentPos < 0 )
		m_currentPos = 0;

	setEnabled(false);
	// position laser to initial m_currentPos and turn it on
	int pos = static_cast<int>(m_currentPos);
	if( m_currentPos-std::floor(m_currentPos) >= 0.5 )
		pos++;
	m_scanner.recalcLaserPosition( pos);
	m_scanner.setLaserOn(true);

	m_currentPos = pos;

	m_laserPositionLCD->display( m_currentPos );
	disconnect( m_trackLayerButton, 0, this, 0 );
	connect( m_trackLayerButton, SIGNAL(clicked()), this, SLOT(stopLayer()) );
	m_trackLayerButton->setText(tr("&Ok"));
	setEnabled(true);
	setCursorBlockEnabled(true);
	m_previousLayerButton->setEnabled( m_currentLayer > 0 );
	m_originTextStack->raiseWidget(m_originStepText2);
}

void CalibrationDialog::previousLayer()
{
	setCursorBlockEnabled(false);
	disconnect( m_trackLayerButton, 0, this, 0 );
	connect( m_trackLayerButton, SIGNAL(clicked()), this, SLOT(startLayer()) );
	m_trackLayerButton->setText(tr("&Track border"));
	lcd( m_currentLayer )->setText( QString::number( m_scanner.calibrationData().stepperPos(m_currentLayer)) );

	if( m_currentLayer > 0 )
		m_currentLayer--;

	bool s = m_automatic;
	m_layerGroup->setButton( indexToRadioId(m_currentLayer) );
	m_automatic = s;

	m_previousLayerButton->setEnabled( m_currentLayer > 0 );
	setNextEnabled( m_preCalibrationPage, m_scanner.calibrationData().preCabReady() );
}

void CalibrationDialog::stopLayer()
{
	m_scanner.setLaserOn( false );
	setCursorBlockEnabled(false);

	disconnect( m_trackLayerButton, 0, this, 0 );
	connect( m_trackLayerButton, SIGNAL(clicked()), this, SLOT(startLayer()) );
	m_trackLayerButton->setText(tr("&Track border"));

	// insert value into calibration object
	L2000::Calibration& c = m_scanner.calibrationData();
	c.setStepperPos(m_currentLayer, m_laserPositionLCD->value() );
	c.preCalibrate(4,0);
	//c.setStepperPosUseFlag(m_currentLayer, true );
	updatePreCabResult();

	lcd( m_currentLayer )->setText( QString::number( m_scanner.calibrationData().stepperPos(m_currentLayer)) );

	if( m_automatic ) {
		if( m_currentLayer < 6 ) {
			++m_currentLayer;
			m_layerGroup->setButton( indexToRadioId(m_currentLayer) );
			m_automatic = true;
		}
	}

	m_previousLayerButton->setEnabled( m_currentLayer > 0 );
	setNextEnabled( m_preCalibrationPage, m_scanner.calibrationData().preCabReady() );
	if( m_scanner.calibrationData().preCabReady() )
		m_originTextStack->raiseWidget(m_originStepText4);
}

void CalibrationDialog::updatePixelView()
{
	m_calibrationPlotter->update();
}


void CalibrationDialog::laserCursorUp()			{ updateLaserPosition(+1); }
void CalibrationDialog::laserCursorDown()		{ updateLaserPosition(-1); }
void CalibrationDialog::laserCursorPageUp()		{ updateLaserPosition(+5); }
void CalibrationDialog::laserCursorPageDown()	{ updateLaserPosition(-5); }
void CalibrationDialog::laserCursorFineUp()		{ updateLaserPosition(+0.1); }
void CalibrationDialog::laserCursorFineDown()	{ updateLaserPosition(-0.1); }

void CalibrationDialog::updateLaserPosition( double delta )
{
	std::cout << "Current Laser Position: " << m_currentPos << ", Delta: " << delta;

	int oldPos = int(((m_currentPos-std::floor(m_currentPos)) >= 0.5) ? std::ceil(m_currentPos) : std::floor(m_currentPos) );
	m_currentPos += delta;
	if( m_currentPos < 0.0 )
		m_currentPos = 0.0;
	else if( m_currentPos > 180.0 )
		m_currentPos = 180.0;
	int newPos = int(((m_currentPos-std::floor(m_currentPos)) >= 0.5) ? std::ceil(m_currentPos) : std::floor(m_currentPos) );
	int step = newPos - oldPos;

	std::cout << ", STEP = " << step << std::endl;
	m_scanner.moveLaser( step );

	m_laserPositionLCD->display(m_currentPos);
	m_originTextStack->raiseWidget(m_originStepText3);
}

// CCD calibration stuff

void CalibrationDialog::initCCDCalibration()
{
	// disable finished button

	m_ccdStack->raiseWidget( m_ccdText1 );

	setNextEnabled( m_ccdCalibrationPage, false );
	setFinishEnabled( m_ccdCalibrationPage, false );
	m_calibrateButton->setEnabled(true);
	m_currentLayer=1;
	m_optimizeButton->setEnabled(false);
}

void CalibrationDialog::doOptimization()
{
	L2000::Calibration& c = m_scanner.calibrationData();
	// maximum likelihood test for CCD1 and CCD2 together
	if( c.ccd1Ready() && c.ccd2Ready() )
	{
		double error1,error2,maxError1,maxError2;
		double initialDist = c.dist();
		double initialAlpha = c.alpha(0);
		double initialH0 = c.H(1);
		// Initial values
		c.calcTransformation(4);
		c.probeTransformation(error1,maxError1,error2,maxError2);
		
		std::cout << "Dist " << c.dist() << ": " << error1 << ", " << error2 << std::endl;
		
		QProgressDialog bar(this);
		bar.setLabelText("Current errror: " + QString::number(error1));
		bar.setTotalSteps(0);
		bar.show();
		
		// optimize dist:
		double epsilon = 0.1;
		double e1 = error1;
		double e2 = error2;
		double me1 = maxError1;
		double me2 = maxError2;
		int cntPlus=0;
		int cntMinus=0;
		do {
			e1 = error1; me1=maxError1;
			e2 = error2; me2=maxError2;
			c.setDist(c.dist()+epsilon); 
			c.calcTransformation(4);
			c.probeTransformation(error1,maxError1,error2,maxError2);
			std::cout << "Dist " << c.dist() << ": " << error1 << ", " << maxError1 << std::endl;
			bar.setLabelText("Current error at "+QString::number(c.dist())+": " + QString::number(error1));
			bar.setProgress(cntPlus,0);
			cntPlus++;
		} while( error1 < e1 && maxError1 < me1 );
		double plusError=e1;
		c.setDist(initialDist);
		do {
			e1 = error1; me1=maxError1;
			e2 = error2; me2=maxError2;
			c.setDist(c.dist()-epsilon); 
			c.calcTransformation(4);
			c.probeTransformation(error1,maxError1,error2,maxError2);
			std::cout << "Dist " << c.dist() << ": " << error1 << ", " << maxError1 << std::endl;
			bar.setLabelText("Current error at "+QString::number(c.dist())+": " + QString::number(error1));
			bar.setProgress(cntMinus,0);
			cntMinus++;
		} while( error1 < e1 && maxError1 < me1 );
		double minusError=e1;
		c.setDist(initialDist);
		c.calcTransformation(4);
		c.probeTransformation(error1,maxError1,error2,maxError2);
		std::cout << "Using Dist " << c.dist() << ": " << error1 << ", " << maxError1 << std::endl;
		std::cout << "->" << plusError << ", <-" << minusError << std::endl;
	}	
	else 
	{
		QMessageBox::warning(this,tr("Nicht genug Daten zur Optimierung"),tr("Die Optimierung kann nur duchgeführt werden,\nwenn genug Daten von beiden Kameras vorliegen."));
	}
}

void CalibrationDialog::doCalibration()
{
	Plane2DMediator plotter( m_calibrationPlotter );
	plotter.addPoints( &m_scanner.ccd1View() );
	plotter.addPoints( &m_scanner.ccd2View() );
	m_scanner.ccd1View().setUnitY( 2.0 );
	m_scanner.ccd2View().setUnitY( 2.0 );

	std::vector<unsigned> data;
	m_scanner.setLayer(2);
	m_scanner.collectScanLineRaw(	data,
					L2000::Scanner::CONTRAST_ON_POINT,
					m_scanner.calibrationData().stepperPos(1),
					m_scanner.calibrationData().stepperPos(1)+70 );

	L2000::Calibration& c = m_scanner.calibrationData();
	c.clearReadings();
	for( unsigned i=0; i<data.size(); )
	{
		// stepper pos, layer no, ccd1 reading, ccd2 reading
		c.addReading( data[i], 2, data[i+1], data[i+2] );
		i+=3;
	}
	plotter.removePoints( &m_scanner.ccd1View() );
	plotter.removePoints( &m_scanner.ccd2View() );

	if(	c.calcTransformation(4) )
	{
		double e1,e2,e1max,e2max;
		c.probeTransformation(e1,e1max,e2,e2max);
		setFinishEnabled(m_ccdCalibrationPage, true);
		m_optimizeButton->setEnabled(true);

		if( c.ccd1Ready() ) {
			m_leftAverage->setText(QString::number(e1) + "mm");
			m_leftMax->setText(QString::number(e1max) + "mm");
		} else {
			m_leftAverage->setText(tr("not enough data"));
			m_leftMax->setText(tr("collected"));
			QMessageBox::warning(this, tr("Could not collect enough data for the left camera"), tr("There could not be collected enough data for any camera, please change the ambient light and restart the process"), 1, 0 );
		}
		if( c.ccd2Ready() ) {
			m_rightAverage->setText(QString::number(e2) + "mm");
			m_rightMax->setText(QString::number(e2max) + "mm");
		} else {
			m_rightAverage->setText(tr("not enough data"));
			m_rightMax->setText(tr("collected"));
			QMessageBox::warning(this, tr("Could not collect enough data for the right camera"), tr("There could not be collected enough data for any camera, please change the ambient light and restart the process"), 1, 0 );
		}
		m_ccdStack->raiseWidget( m_ccdText2 );
	}
	else {
		QMessageBox::warning(this, tr("Could not collect enough data"), tr("There could not be collected enough data for any camera, please change the ambient light and restart the process"), 1, 0 );
	}
}

void CalibrationDialog::showPage( QWidget* page )
{
	if( page == m_introductionPage ) {
		initIntroduction();
	}
	else if( page == m_preCalibrationPage ) {
		initPrecalibration();
	}
	else if( page == m_ccdCalibrationPage ) {
		initCCDCalibration();
	}
	CalibrationWizard::showPage(page);
}

/*$SPECIALIZATION$*/
void CalibrationDialog::help()
{
	m_scanner.setLaserOn( false );

	CalibrationWizard::help();
}

void CalibrationDialog::back()
{
	m_scanner.setLaserOn( false );

	CalibrationWizard::back();
}

void CalibrationDialog::next()
{
	m_scanner.setLaserOn( false );

	if( currentPage() == m_introductionPage ) {
		CalibrationWizard::next();
	}
	else if( currentPage() == m_preCalibrationPage ) {
		m_scanner.calibrationData().writeCalibrationData();
		CalibrationWizard::next();
	}
	else if( currentPage() == m_ccdCalibrationPage ) {
		setResult( QDialog::Accepted );
		close();
	}
}

void CalibrationDialog::reject()
{
	m_scanner.setLaserOn( false );
	return CalibrationWizard::reject();
}

void CalibrationDialog::accept()
{
	m_scanner.setLaserOn( false );
	m_scanner.calibrationData().writeCalibrationData();
	return CalibrationWizard::accept();
}

int CalibrationDialog::radioIdToIndex( int id ) const
{
	if( id == m_layerGroup->id(m_radio1) )
		return 0;
	if( id == m_layerGroup->id(m_radio2) )
		return 1;
	if( id == m_layerGroup->id(m_radio3) )
		return 2;
	if( id == m_layerGroup->id(m_radio4) )
		return 3;
	if( id == m_layerGroup->id(m_radio5) )
		return 4;
	if( id == m_layerGroup->id(m_radio6) )
		return 5;
	if( id == m_layerGroup->id(m_radio7) )
		return 6;
	return -1;
}

int CalibrationDialog::indexToRadioId( int index ) const
{
	switch( index )
	{
		case 0: return m_layerGroup->id( m_radio1 );
		case 1: return m_layerGroup->id( m_radio2 );
		case 2: return m_layerGroup->id( m_radio3 );
		case 3: return m_layerGroup->id( m_radio4 );
		case 4: return m_layerGroup->id( m_radio5 );
		case 5: return m_layerGroup->id( m_radio6 );
		case 6: return m_layerGroup->id( m_radio7 );
	}
	return -1;
}

QLabel* CalibrationDialog::lcd( int i ) const
{
	switch( i )
	{
		case 0: return m_lcd1;
		case 1: return m_lcd2;
		case 2: return m_lcd3;
		case 3: return m_lcd4;
		case 4: return m_lcd5;
		case 5: return m_lcd6;
		case 6: return m_lcd7;
	}
	return 0;
}

void CalibrationDialog::setCursorBlockEnabled( bool flag )
{
	m_5down->setEnabled(flag);
	m_5up->setEnabled(flag);
	m_up->setEnabled(flag);
	m_down->setEnabled(flag);
	m_fineDown->setEnabled(flag);
	m_fineUp->setEnabled(flag);
}

