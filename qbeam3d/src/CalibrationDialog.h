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
//
//
// C++ Interface:
//
// Description:
//
//
// Author: Maximilian Hrabowski <maximilian@hrabowski.net>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include "L2000Calibration.h"
#include "L2000Scanner.h"

#include "CalibrationWizard.h"

class QLCDNumber;

class CalibrationDialog : public CalibrationWizard
{
  Q_OBJECT

public:
  CalibrationDialog(L2000::Scanner& scanner, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~CalibrationDialog();
  /*$PUBLIC_FUNCTIONS$*/

	virtual void CalibrationDialog::showPage( QWidget* page );

	void initIntroduction();
	void initPrecalibration();
	void initCCDCalibration();

	void updatePixelView();

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

  virtual void reject();
  virtual void accept();
  
  void closeEvent( QCloseEvent* e );
  
protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void          help();
  virtual void          back();
  virtual void          next();

  void setPrecalibration( bool toggle );

private slots:



	// for precalibration
	void laserCursorUp();
	void laserCursorDown();
	void laserCursorPageUp();
	void laserCursorPageDown();
	void laserCursorFineUp();
	void laserCursorFineDown();

	void layerChanged(int id);   // called when radio button clicked
	void startLayer();			// called when button "Track Border" clicked
	void stopLayer();			// called when button "Ok" clicked
	void previousLayer();		// called when button "previous" clicked

	void updateLaserPosition( double pos );

	void setLayer1Enabled( bool f );
	void setLayer2Enabled( bool f );
	void setLayer3Enabled( bool f );
	void setLayer4Enabled( bool f );
	void setLayer5Enabled( bool f );
	void setLayer6Enabled( bool f );
	void setLayer7Enabled( bool f );

	// for calibration
	void doCalibration();		// start the Calibration process
	void doOptimization();		// after Calibration, optimize Laser geometry

private:

  void updatePreCabResult();
  void setCursorBlockEnabled( bool flag );

  int radioIdToIndex( int id ) const;
  int indexToRadioId( int index ) const;
  QLabel* lcd( int i ) const;

  L2000::Scanner&	 	m_scanner;

  // vars and methods for precalibration
  bool					m_automatic;
  int					m_currentLayer;
  int					m_currentId;
  double				m_currentPos;

};

#endif

