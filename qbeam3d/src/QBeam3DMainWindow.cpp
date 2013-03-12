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

#include "QBeam3DMainWindow.h"

#include <algorithm>
#include <iterator>
#include <functional>
#include <vector>
#include <list>

#include <qlabel.h>
#include <qprogressbar.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qscrollview.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qfiledialog.h>
#include <qspinbox.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qbuttongroup.h>
#include <qsizepolicy.h>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include "MeshTools.h"
#include <GLView.h>
#include <MeshView.h>
#include <Plane2DView.h>
#include <Plane2DMediator.h>


#include "CalibrationDialog.h"
#include "ProgressDialog.h"

#include "L2000Calibration.h"

#include <qmessagebox.h>

#include "ScannerControl.h"
#include "ScannerSettings.h"
#include "ScanThread.h"

#include "UpdateEvents.h"

QBeam3DMainWindow::QBeam3DMainWindow( QWidget* parent, const char* name ) :
	QWidget(parent, name),
	m_mesh(),
	m_fileName(),
	m_scanThread(0),
	m_scanProgressDialog(0),
	m_scanMediator(0),
	m_calibrationWizard(0)
{
	m_mainLayout = new QVBoxLayout(this);
	//QHBoxLayout* hL = new QHBoxLayout(0,11,6);

	m_lrSplitter					= new QSplitter(Qt::Horizontal, this, "LRSplitter");
		m_udSplitter				= new QSplitter(Qt::Vertical, m_lrSplitter, "UDSplitter");
			QHBox* box 				= new QHBox(m_udSplitter);
				QVBox* box2			= new QVBox(box);
					m_control			= new ScannerControl( box2, "scanner_control");
					QVBox* bg			= new QVBox(box2);//= new QButtonGroup(2, Qt::Horizontal,box2);
					m_startButton		= new QPushButton(tr("&Scan starten"),bg,"startButton");
					m_calibrateButton	= new QPushButton(tr("&Kalibrieren"),bg,"calibrateButton");
					QFrame* f			= new QFrame(bg); f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
					m_loadButton		= new QPushButton(tr("&Datei öffen..."),bg,"loadButton");
					m_saveButton		= new QPushButton(tr("&Speichern"),bg,"saveButton");
					m_saveAsButton		= new QPushButton(tr("Speichern &unter..."),bg,"saveAsButton");
					f					= new QFrame(bg); f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
					m_quitButton		= new QPushButton(tr("&Beenden"),bg,"quitButton");
				m_settings			= new ScannerSettings( box, "scanner_settings");
			m_pixelView				= new Plane2DView(m_udSplitter);
		m_worldView					= new MeshView(&m_mesh, m_lrSplitter);


	m_statusBar = new QStatusBar(this);

	m_mainLayout->addWidget(m_lrSplitter);
	m_mainLayout->addWidget(m_statusBar);

	m_statusBar->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );


	m_udSplitter->setSizePolicy( QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding,1,10) );
		//box->setSizePolicy( QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum,1,1) );
			//box2->setSizePolicy( QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum,1,1) );
				//m_control->setSizePolicy( QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum,1,1) );
				//bg->setSizePolicy( QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum,1,1) );
			//m_settings->setSizePolicy( QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred,1,1) );
		m_pixelView->setSizePolicy( QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding,1,10) );
	m_worldView->setSizePolicy( QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding,10,10) );

	//m_udSplitter->setResizeMode( box, QSplitter::Stretch );
	//m_udSplitter->setResizeMode( m_pixelView, QSplitter::Stretch );
	m_lrSplitter->setResizeMode( m_udSplitter, QSplitter::Auto );
	m_lrSplitter->setResizeMode( m_worldView, QSplitter::Auto );

	resize( QSize(1024,768).expandedTo( minimumSizeHint()) );

	connect( m_calibrateButton, SIGNAL(clicked()), this, SLOT(calibrate()) );
	connect( m_startButton, SIGNAL(clicked()), this, SLOT(startScan()) );
	//connect( m_stopButton, SIGNAL(clicked()), this, SLOT(stopScan()) );
	connect( m_loadButton, SIGNAL(clicked()), this, SLOT(load()) );
	connect( m_saveButton, SIGNAL(clicked()), this, SLOT(save()) );
	connect( m_saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()) );
	connect( m_quitButton, SIGNAL(clicked()), this, SLOT(quit()));

	connect( m_settings->m_actualPosToLowerButton, SIGNAL(clicked()), this, SLOT(setActualToLower()) );
	connect( m_settings->m_actualPosToUpperButton, SIGNAL(clicked()), this, SLOT(setActualToUpper()) );
	connect( m_settings->m_upperBoundSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateObjectHeight()) );

	m_control->m_layerSpinBox->setValue( m_scanner.layer() );
	m_control->m_laserPositionSpinBox->setValue( m_scanner.laserPos() );
	m_control->m_tablePositionSpinBox->setValue( m_scanner.tablePos() );

	connect(m_control->m_layerSpinBox,			SIGNAL(valueChanged(int)),
			this, SLOT(setLaserLayer(int)) );
	connect(m_control->m_laserResetButton,		SIGNAL(clicked()),
			this, SLOT(resetLaser()) );
	connect(m_control->m_laserPositionSpinBox,	SIGNAL(valueChanged(int)),
			this, SLOT(setLaserPosition(int)) );
	connect(m_control->m_laserOnOffButton,		SIGNAL(toggled(bool)),
			this, SLOT(setLaserOnOff(bool)) );
	connect(m_control->m_repositionButton,		SIGNAL(clicked()),
			this, SLOT(recalcLaserPosition()) );

	connect(m_control->m_tablePositionSpinBox,	SIGNAL(valueChanged(int)),
			this, SLOT(setTablePosition(int)) );
	connect(m_control->m_tableOnOffButton,		SIGNAL(toggled(bool)),
			this, SLOT(setTableOnOff(bool)) );
	connect(m_control->m_tableResetButton,		SIGNAL(clicked()),
			this, SLOT(resetTable()) );

	m_startButton->setEnabled( m_scanner.calibrationData().ready() );
}

void QBeam3DMainWindow::setActualToLower()
{
	m_settings->m_lowerBoundSpinBox->setValue(m_scanner.laserPos());
	updateObjectHeight();
}

void QBeam3DMainWindow::setActualToUpper()
{
	m_settings->m_upperBoundSpinBox->setValue(m_scanner.laserPos());
}

void QBeam3DMainWindow::detectUpper()
{
/*
	m_settings->m_detectUpperButton->setEnabled(false);
	m_settings->m_detectLowerButton->setEnabled(false);

	// Mitte
	// solange nach unten bis Messwert
	// solange nach oben bis kein Messwert für
	// im Kreis bis Messwert oder wieder bei null

	int pos = m_scanner.laserPos();
	std::vector<unsigned int> result;
	if( m_scanner.collectScanLineRaw(	result,
										L2000::Scanner::CONTRAST_ONCE,
										m_scanner.calibrationData().stepperPos(2),
										L2000::Scanner::UPPER_BOUND ) )
	{
		// step, ccd1, ccd2
		int idx =result.size()-3;
		while( idx >= 0 ) {
			if( result[idx+1] == 0 && result[idx+2] == 0 )
			{
				pos = result[idx]+5;
				m_settings->m_upperBoundSpinBox->setValue(pos);
				break;
			}
			idx-=3;
		}
	}
	m_scanner.recalcLaserPosition(pos);

	m_settings->m_detectUpperButton->setEnabled(true);
	m_settings->m_detectLowerButton->setEnabled(true);
*/
}

void QBeam3DMainWindow::detectLower()
{
/*
	m_settings->m_detectUpperButton->setEnabled(false);
	m_settings->m_detectLowerButton->setEnabled(false);

	// layer start position
	// nach oben bis Messwert
	// 1 nach unten
	//
	m_settings->m_detectUpperButton->setEnabled(true);
	m_settings->m_detectLowerButton->setEnabled(true);
*/
}

void QBeam3DMainWindow::polish()
{
	//m_stopButton->setEnabled(false);
	m_saveButton->setEnabled(false);
	m_saveAsButton->setEnabled(false);

	m_statusBar->message(tr("Ready"));

	setCaption("QBeam3D - " + m_fileName );

	m_scanner.init();
	m_scanner.open();
}

// laser control
void QBeam3DMainWindow::setLaserLayer(int l)
{
	m_scanner.setLayer(l);
	m_control->m_layerSpinBox->setValue( m_scanner.layer() );
}

void QBeam3DMainWindow::detectLaserLayer()
{
	// TO DO: idea: Drehteller scannen: y muß nahe bei Null sein
}

void QBeam3DMainWindow::resetLaser()
{
	m_scanner.close();
	m_scanner.init();
	m_scanner.open();
	m_scanner.setLaserOn( false );
	m_scanner.resetLaser();
	m_scanner.recalcLaserPosition( m_control->m_laserPositionSpinBox->value() );
	m_scanner.setLaserOn( m_control->m_laserOnOffButton->isOn() );
	m_scanner.setTableOn( m_control->m_tableOnOffButton->isOn() );
	m_scanner.calibrationData().readCalibrationData();
}

void QBeam3DMainWindow::setLaserPosition( int p )
{
	m_scanner.moveLaserTo( p );
}

void QBeam3DMainWindow::setLaserOnOff( bool on )
{
	m_scanner.setLaserOn(on);
}

void QBeam3DMainWindow::recalcLaserPosition()
{
	bool b = m_scanner.laserOn();
	m_scanner.recalcLaserPosition( m_control->m_laserPositionSpinBox->value() );
	m_scanner.setLaserOn(b);
}

// table control
void QBeam3DMainWindow::setTablePosition(int p)
{
	m_scanner.moveTableTo( p );
}

void QBeam3DMainWindow::setTableOnOff(bool on)
{
	m_scanner.setTableOn(on);
}

void QBeam3DMainWindow::updateObjectHeight()
{
	double alpha = m_scanner.calibrationData().alpha( m_scanner.laserPos() );
	L2000::Calibration::Point lp = m_scanner.calibrationData().laserPos();
	double h = lp.second + lp.first*std::sin(alpha)/std::cos(alpha);
	m_settings->m_objectHeightLabel->setText(QString::number(h,'f',2)+" mm");
}

void QBeam3DMainWindow::resetTable()
{
	m_scanner.close();
	m_scanner.init();
	m_scanner.open();
	m_scanner.setTableOn( false );
	m_scanner.setLaserOn( false );
	m_scanner.setLaserOn( m_control->m_laserOnOffButton->isOn() );
	m_scanner.recalcLaserPosition( m_control->m_laserPositionSpinBox->value() );
	m_scanner.resetTable();
	m_scanner.setTableOn( m_control->m_tableOnOffButton->isOn() );
}


void QBeam3DMainWindow::calibrate()
{
	m_calibrationWizard = new CalibrationDialog(m_scanner, this,"calibration_wizard",false);
	if( m_calibrationWizard->exec() != QDialog::Accepted ) {

		if( m_scanner.status() != L2000::Scanner::CALIBRATED )
		{
			QMessageBox::warning(this,tr("Kalibrierung abgebrochen"),tr("Der scanner wurde nicht erfolgreich kalibriert und somit noch nicht benutzt werden."));
			m_startButton->setEnabled(false);
		}
		else
			m_startButton->setEnabled(true);
	}
	else {
			m_startButton->setEnabled(true);
	}
	delete m_calibrationWizard;
	m_calibrationWizard=0;
	resetLaser();
	resetTable();
}

void QBeam3DMainWindow::updateProgressDialog()
{
	if( m_scanThread )
	{
		QString str = QString::number(m_scanner.contrast1()) + ", " + QString::number(m_scanner.contrast2());
		m_scanProgressDialog->m_contrastLabel->setText(str);
		str = QString::number(m_scanner.intTime1()*3) + "ms, " + QString::number(m_scanner.intTime2()*3) + "ms";
		m_scanProgressDialog->m_intTimeLabel->setText(str);
		double th1 = 1.5 + (double(m_scanner.thresHold1())-80.0)*0.02285714;
		double th2 = 1.5 + (double(m_scanner.thresHold2())-80.0)*0.02285714;
		str = QString::number(th1) + "V, " + QString::number(th2) + "V";
		m_scanProgressDialog->m_thresholdLabel->setText(str);
		unsigned long currentLine = m_scanner.tablePos()/m_scanner.tableStep();
		m_scanProgressDialog->m_currentScanLine->setText(QString::number(currentLine));
		m_scanProgressDialog->m_totalProgressBar->setProgress(currentLine);
		// time elapsed:
		unsigned tableRes = m_scanProgressDialog->m_totalProgressBar->totalSteps();
		unsigned long msecs			= m_scanStartTime.elapsed();
		unsigned long total_msecs	= msecs * tableRes / (currentLine==0?1:currentLine);
		unsigned long needed_msecs	= total_msecs - msecs;

		m_scanProgressDialog->m_endTime->setText(
			m_scanStartTime.addMSecs(needed_msecs).toString());
		m_scanProgressDialog->m_elapsedTime->setText(
			QTime((msecs/3600000)%24,(msecs/60000)%60,(msecs/1000)%60,msecs%1000).toString());
		m_scanProgressDialog->m_totalTime->setText(
			QTime((total_msecs/3600000)%24,(total_msecs/60000)%60,(total_msecs/1000)%60,total_msecs%1000).toString());

		if( m_scanThread->finished() )
			stopScan();

		QTimer::singleShot( 1000, this, SLOT(updateProgressDialog()) );
	}
}

void QBeam3DMainWindow::stopScan()
{
	if( m_scanThread )
	{
		m_scanProgressDialog->m_cancelButton->setEnabled(false);
		m_scanThread->cancel();
		while( !m_scanThread->wait(10) ) {
			qApp->processEvents(10);
		}

		delete m_scanThread;
		m_scanThread = 0;

		double radius = MeshTools::centerMesh( m_mesh );
		m_worldView->setScenePos(OpenMesh::Vec3f(0.0,0.0,0.0), radius);

		delete m_scanProgressDialog;
		m_scanProgressDialog = 0;

		m_scanMediator->removePoints( &m_scanner.ccd1View() );
		m_scanMediator->removePoints( &m_scanner.ccd2View() );
		m_scanMediator->removePoints( &m_scanner.ccd1ErrorView() );
		m_scanMediator->removePoints( &m_scanner.ccd2ErrorView() );
		delete m_scanMediator;

		m_startButton->setEnabled(true);
		m_control->setEnabled(true);
		m_settings->setEnabled(true);
		m_saveButton->setEnabled(true);
		m_saveAsButton->setEnabled(true);
		m_statusBar->message(tr("Done..."));
		resetLaser();
		resetTable();
	}
}

void QBeam3DMainWindow::startScan()
{
	if( m_scanThread )
		return;

	if( !m_mesh.vertices_empty() &&
		m_saveButton->isEnabled() &&
		QMessageBox::No == QMessageBox::question(this,tr("Objekt verwerfen?"),tr("Das momentane Objekt wurde noch nicht gespeichert. Wollen die das Objekt verwerfen?"),QMessageBox::Yes, QMessageBox::No)
	)
		return;

	m_mesh.clear();
	m_saveButton->setEnabled(true);
		
	m_scanner.close();
	m_scanner.init();
	m_scanner.open();
	m_scanner.calibrationData().readCalibrationData();

	m_control->setEnabled(false);
	m_settings->setEnabled(false);
	m_statusBar->message(tr("Scanning..."));
	//m_stopButton->setEnabled(true);

	// prepare 2D plotter for scanning
	m_scanMediator = new Plane2DMediator( m_pixelView );
	m_scanMediator->addPoints( &m_scanner.ccd1View() );
	m_scanMediator->addPoints( &m_scanner.ccd2View() );
	m_scanMediator->addPoints( &m_scanner.ccd1ErrorView() );
	m_scanMediator->addPoints( &m_scanner.ccd2ErrorView() );

	m_scanner.ccd1View().setUnitX(1.0);
	m_scanner.ccd1View().setUnitY(0.5);
	m_scanner.ccd2View().setUnitX(1.0);
	m_scanner.ccd2View().setUnitY(0.5);
	m_scanner.ccd1ErrorView().setUnitX(1.0);
	m_scanner.ccd1ErrorView().setUnitY(0.5);
	m_scanner.ccd2ErrorView().setUnitX(1.0);
	m_scanner.ccd2ErrorView().setUnitY(0.5);

	m_worldView->setScenePos(OpenMesh::Vec3f(0.0, 100.0, 0.0), 100.0);
	L2000::ScanOptions so = m_settings->settings();

	// start timer
	m_scanStartTime.start();

	// prepare Progress Dialog
	m_scanProgressDialog = new ProgressDialog(this,"progress",false);
	int tableRes = int(double(so.tableRange)/so.tableStep);
	m_scanProgressDialog->m_totalScanLines->setText(QString::number(tableRes));
	m_scanProgressDialog->m_currentScanLine->setText("0");
	m_scanProgressDialog->m_startTime->setText(m_scanStartTime.toString());	// start absolut
	m_scanProgressDialog->m_endTime->setText(m_scanStartTime.toString());		// geschätztes Ende absolut
	m_scanProgressDialog->m_elapsedTime->setText("00:00:00");					// bisher
	m_scanProgressDialog->m_totalTime->setText("00:00:00");						// geschätzte Dauer
	m_scanProgressDialog->m_totalProgressBar->reset();
	m_scanProgressDialog->m_totalProgressBar->setTotalSteps( tableRes );
	m_scanProgressDialog->show();

	// create and start scanning thread

	m_scanThread = new ScanThread(m_mesh, m_scanner, so, this );
	m_scanThread->start();

	connect( m_scanProgressDialog->m_cancelButton, SIGNAL(clicked()), this, SLOT(stopScan()) );
	QTimer::singleShot( 1000, this, SLOT(updateProgressDialog()) );
}

void QBeam3DMainWindow::load()
{
	QFileDialog fDlg(".","Meshes (*.off *.stl *.om *.obj)",this,"open file dialog",true);

    if ( fDlg.exec() == QDialog::Accepted )
	{
        m_fileName = fDlg.selectedFile();
		std::string str( m_fileName.latin1() );
		m_mesh.clear();
		if( OpenMesh::IO::read_mesh( m_mesh, str ) )
		{
				// center the mesh, get radius
				m_mesh.update_normals();

				double radius = MeshTools::centerMesh( m_mesh );
				//MeshTools::calcVertexNormals( m_mesh );

				//m_worldView->setMesh(m_mesh);
				m_worldView->setScenePos(OpenMesh::Vec3f(0.0,0.0,0.0), radius);

				m_saveButton->setEnabled(false);
				m_saveAsButton->setEnabled(true);
		}
		else {
			m_mesh.clear();
			m_fileName = "unnamed";
		}
	}
	else {
		m_fileName = "unnamed";
	}
	setCaption("QBeam3D - " + m_fileName );
/*
  OpenMesh::MeshIO::ImporterT<Mesh> importer(_mesh);
  if( MeshTools::importOFF(str, importer) )
  {
		// center the mesh, get radius
		_radius = MeshTools::centerMesh( _mesh );
		MeshTools::calcVertexNormals( _mesh );
		emit documentChanged();
		return true;
  }
*/
	m_saveButton->setEnabled(true);
	m_saveAsButton->setEnabled(true);
}

void QBeam3DMainWindow::save()
{
	if( m_scanThread )
		return;
	if( m_fileName.isEmpty() || m_fileName.length()<5 || m_fileName == "unnamed")
		saveAs();
	else 
	{
		std::string str( m_fileName.latin1() );
		
		bool ret = OpenMesh::IO::write_mesh( m_mesh, str );
		
		if( !ret )
		{
			m_fileName = "unnamed";
			m_statusBar->message(tr("Fehler beim Speichern!"));
			m_saveButton->setEnabled(true);
		}
		else {
			m_statusBar->message(tr("Erfolgreich gespeichert."));
			m_saveButton->setEnabled(false);
		}
	}
	setCaption("QBeam3D - " + m_fileName );
}

void QBeam3DMainWindow::saveAs()
{
	QFileDialog fDlg(".","Meshes (*.off *.stl *.om *.obj)",this,"save file dialog",true);
	fDlg.setMode( QFileDialog::AnyFile );
    if ( fDlg.exec() == QDialog::Accepted )
	{
        m_fileName = fDlg.selectedFile();
		save();
	}
}

void QBeam3DMainWindow::quit()
{
	close();
}

void QBeam3DMainWindow::customEvent( QCustomEvent* e )
{
	if( e->type() == QEvent::Type(Points2DChangeEvent::TypeId) )
	{
		// wir scannen gerade.. also update!
		if( m_scanThread )
			m_pixelView->update();
		// wir scannen nicht, also sag ichs dem Kalibrierungs-druiden
		else if( m_calibrationWizard )
			m_calibrationWizard->updatePixelView();
	}
	else if( e->type() == QEvent::Type(MeshChangeEvent::TypeId) )
	{
		m_worldView->update();
	}
}

QBeam3DMainWindow::~QBeam3DMainWindow()
{
}


