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

#ifndef _QBEAM3DMAINWINDOW_H_
#define _QBEAM3DMAINWINDOW_H_

#include <qwidget.h>
#include <qdatetime.h>

#include "Mesh.h"

#include <L2000Calibration.h>
#include <L2000Scanner.h>


class QVBoxLayout;
class QSplitter;
class QStatusBar;
class QScrollView;
class QPushButton;

class MeshView;
class Plane2DView;

class ScannerControl;
class ScannerSettings;
class ScanThread;
class ProgressDialog;
class Plane2DMediator;
class QCustomEvent;
class CalibrationDialog;

class QBeam3DMainWindow : public QWidget
{
	Q_OBJECT

public:
	QBeam3DMainWindow( QWidget* parent=0, const char* name=0 );
	virtual ~QBeam3DMainWindow();

public slots:

	void calibrate();

	void startScan();
	void stopScan();

	void load();
	void save();
	void saveAs();
	void quit();

	// laser control
	void setLaserLayer(int);
	void detectLaserLayer();
	void resetLaser();

	void setLaserPosition( int );
	void setLaserOnOff( bool );
	void recalcLaserPosition();

	// table control
	void setTablePosition(int);
	void setTableOnOff(bool);
	void resetTable();

	void updateProgressDialog();

	void setActualToLower();
	void setActualToUpper();

	void detectUpper();
	void detectLower();

	void updateObjectHeight();

protected:

	virtual void customEvent( QCustomEvent* e );
	virtual void polish();

	QVBoxLayout*		m_mainLayout;

	//QScrollView*		m_leftControlPane;
	QPushButton*			m_calibrateButton;
	QPushButton*			m_startButton;
	//QPushButton*			m_stopButton;

	ScannerControl*			m_control;
	ScannerSettings*		m_settings;

	QPushButton*			m_loadButton;
	QPushButton*			m_saveButton;
	QPushButton*			m_saveAsButton;
	QPushButton*			m_quitButton;

	QSplitter*			m_lrSplitter;
	MeshView*				m_worldView;
	QSplitter*				m_udSplitter;
	//MeshView*					m_cutView;
	Plane2DView* 				m_pixelView;
	//QScrollView*				m_rightControlPane;

	QStatusBar*		m_statusBar;

private:

	LockableMesh			m_mesh;		// the resulting mesh
	QString 				m_fileName; // filename

	L2000::Scanner			m_scanner;			// the scanner

	// needed for scanning thread stuff
	ScanThread*				m_scanThread;
	ProgressDialog*			m_scanProgressDialog;
	Plane2DMediator*		m_scanMediator;
	QTime					m_scanStartTime;
	
	CalibrationDialog*		m_calibrationWizard;

};

#endif
