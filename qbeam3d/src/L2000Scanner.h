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

#ifndef _L2000SCANNER_H_
#define _L2000SCANNER_H_

#include <string>
#include <vector>
#include <Mesh.h>

#include <Plane2DPoints.h>

#include "L2000Device.h"
#include "L2000Calibration.h"
#include "L2000Reading.h"
#include "L2000InstructionSet.h"

#include "Mesh.h"

#include <qmutex.h>

//#define MAKE_THREADSAFE

#ifdef MAKE_THREADSAFE
#define MUTEX_LOCK \
	m_scanMutex.lock(); \
	m_scanMutexLockCount++; \
	std::cerr << "Scanner locked(" << m_scanMutexLockCount << ")" << std::endl;
#define MUTEX_UNLOCK \
	m_scanMutexLockCount--; \
	m_scanMutex.unlock(); \
	std::cerr << "Scanner unlocked(" << m_scanMutexLockCount << ")" << std::endl;
#else
#define MUTEX_LOCK
#define MUTEX_UNLOCK
#endif

namespace L2000 {

struct ScanOptions;

class Scanner
{
public:

	typedef enum {
		FIRST=0,
		MIDDLE,
		LAST,
	} ReadingMode;

	typedef enum {  UNKNOWN=0,			// intitial state
					INITIALIZED,		// init() success,
					READY_WAITING,		// init() && open() success
					CALIBRATED,			// init() && open() && calibrate() success
					SCANNING,			// during command execution
					ERROR				// error occured
	} ScannerState;

	typedef	enum {
					LOWER_BOUND=0,
					DEFAULT_LOWER_BOUND=40,
					DEFAULT_CONTRAST_POS=150,
					DEFAULT_UPPER_BOUND=200,
					UPPER_BOUND=285,
	} LaserRange;

	typedef enum {
		CONTRAST_CUSTOM=0,
		CONTRAST_ONCE,
		CONTRAST_ON_COL,
		CONTRAST_ON_POINT
	} ScanMode;

	// Axis, only two defined
	typedef enum { LASER=0, TABLE=1 } Axis;

	Scanner();
	virtual ~Scanner();

	bool  open();
	bool  close();
	/** Der Scanner wird initialisiert. Gibt true zurück wenn der Scanner einsatzbereit ist */
	bool init();
	/** gibt true zurück wenn der scanner bereit ist */
	bool ready() const;

	/* low level stuff */
	// read state
	int		layer() const		{ return m_layer; }
	int		laserPos() const	{ return m_laserPos; }
	int		tablePos() const	{ return m_tablePos; }
	int		laserStep() const	{ return m_laserStep; }
	int		tableStep() const 	{ return m_tableStep; }
	bool	laserOn() const		{ return m_laserOn; }
	bool	tableOn() const		{ return m_tableOn; }

	bool	setLaserOn(bool on, int delay=0);
	bool	setTableOn(bool on, int delay=128);

	void	setLayer( int l ) { if( l>=1 && l<=7 ) m_layer=l; }

	/** drive back to zero, reposition reader on axis */
	bool recalcLaserPosition( int pos );
	// set laser to zero position and switch off
	bool resetLaser();
	// set table to zero position and switch off
	bool resetTable();

	bool	moveTable( int delta );
	bool	moveLaser( int delta );

	/** move "reader" to point, absolut */
	bool	moveLaserTo( int pos );
	bool	moveTableTo( int pos );

	/** increment */
	bool moveLaserUp( int pos=1 );
	bool moveTableCW( int pos=1 );

	/** decrement */
	bool moveLaserDown( int pos=1 );
	bool moveTableCCW(  int pos=1 );

	/** get point at current pos */
	bool getPoint( Mesh::Point& p );

	const std::string& name() const;
	const std::string& description() const;

	const std::vector<std::pair<std::string,std::string> >& modes() const;

	Plane2DPoints& ccd1View() { return m_ccd1Points; }
	Plane2DPoints& ccd2View() { return m_ccd2Points; }
	Plane2DPoints& ccd1ErrorView() { return m_ccd1ErrorPoints; }
	Plane2DPoints& ccd2ErrorView() { return m_ccd2ErrorPoints; }

	ScanMode	mode() const { return m_mode; }
	void		setMode( ScanMode mode ) { m_mode = mode; }

	/** Startet den scanvorgang */
	//bool start();
	/** Stoppt den scanvorgang */
	//bool stop();

	ScannerState status() const { return m_status; }
	const Device& device() const { return m_scanner; }

	void dumpMemory();

	Calibration& calibrationData() { return m_calibrationData; }

	bool collectScanLine( std::vector<Vertex>& result, ScanOptions& options,  std::vector<Vertex>& ccd1, std::vector<Vertex>& ccd2 );
	bool collectScanLineRaw( std::vector<unsigned>& result, ScanMode mode = CONTRAST_ONCE, int start_pos=-1, int end_pos=-1 );
	
	void interpolateReadings( int ccd, std::vector<Reading>& readings, const ScanOptions& options );

	bool prepareScan( ScanOptions& so );
	bool fineAdjustContrast();
	bool getCurrentContrast();

	unsigned contrast1() const { return m_contrast1; }
	unsigned thresHold1() const { return m_thresHold1; }
	unsigned intTime1() const { return m_intTime1; }
	unsigned contrast2() const { return m_contrast2; }
	unsigned thresHold2() const { return m_thresHold2; }
	unsigned intTime2() const { return m_intTime2; }

protected:

	bool getReading( Reading& r, ReadingMode m=MIDDLE );

	template <class BackInserter>
	bool executeParsedProgramChar(const std::string& program, BackInserter next );
	template <class BackInserter>
	bool executeParsedProgramInt16(const std::string& program, BackInserter next );

	void adjustCCDReading( Int16& first, Int16& last );
	
	void addCCDPointToView( int ccd, int alpha, int value );

private:

	L2000::Calibration		m_calibrationData;

	// buffer for Observing View
	Plane2DPoints	m_ccd1Points;
	Plane2DPoints	m_ccd2Points;
	Plane2DPoints	m_ccd1ErrorPoints;
	Plane2DPoints	m_ccd2ErrorPoints;

	std::string		m_name;
	std::string		m_description;
	Device			m_scanner;

	std::vector< std::pair< std::string, std::string > > m_modes;

	ScannerState	m_status;

	int				m_laserStep;
	int				m_laserPos;
	int				m_tableStep;
	int				m_tablePos;
	int				m_layer;

	bool			m_laserOn;
	bool			m_tableOn;

	ScanMode		m_mode;

	unsigned		m_contrast1;
	unsigned		m_thresHold1;
	unsigned		m_intTime1;

	unsigned		m_contrast2;
	unsigned		m_thresHold2;
	unsigned		m_intTime2;

#ifdef MAKE_THREADSAFE
	static QMutex	m_scanMutex;
	static int		m_scanMutexLockCount;
#endif
};

} // namespace

#endif
