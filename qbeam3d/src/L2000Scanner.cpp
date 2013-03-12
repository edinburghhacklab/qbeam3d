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

#include <string>
#include <vector>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <functional>

#include "SpecSizedInt.h"
#include "L2000InstructionSet.h"
#include "L2000Scanner.h"
#include "L2000Calibration.h"
#include "L2000ScanOptions.h"

#include "Plane2DMediator.h"
#include "ObservableContainer.h"
#include "Plane2DPoints.h"

#include "macros.h"

#include <qcolor.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include "Mesh.h"
#include "MeshTools.h"

#ifdef MAKE_THREADSAFE
QMutex L2000::Scanner::m_scanMutex(true);
int L2000::Scanner::m_scanMutexLockCount=0;
#endif

L2000::Scanner::Scanner() :
	m_calibrationData(),
	m_name("Conmark L2000 Laser 3D Scanner"),
	m_description("Laserscanner mit einem Drehteller, einem Laserstrahl und zwei Kameras"),
	m_scanner(),
	m_modes(),
	m_status(UNKNOWN),
	m_laserStep(1),
	m_laserPos(0),
	m_tableStep(1),
	m_tablePos(0),
	m_layer(2),
	m_laserOn(false),
	m_tableOn(false),
	m_contrast1(0),
	m_thresHold1(0),
	m_intTime1(0),
	m_contrast2(0),
	m_thresHold2(0),
	m_intTime2(0)
{
	// right ccd "steerboard" -> green
	m_ccd1Points.setColor( QColor(0,255,0) );
	m_ccd1ErrorPoints.setColor( QColor(0,128,0) );
	// left camera, backboard -> red
	m_ccd2Points.setColor( QColor(255,0,0) );
	m_ccd2ErrorPoints.setColor( QColor(128,0,0) );

	if( m_calibrationData.readCalibrationData() ) {
		m_calibrationData.calcTransformation(4);
	}
}

/* low level functions */

bool  L2000::Scanner::setLaserOn(bool on, int delay)
{
	MUTEX_LOCK

	std::vector<char> data;
	std::ostringstream program;
	program << "VERSION ";
	program << "LONOFF " << (on ? "1 ":"0 ");

	if( delay != 0 )
		program << "DELL " << delay << " ";

	//std::cout << "setLaserOn():" << program.str() << std::endl;

	bool ret = false;
	if( m_status >= READY_WAITING &&
		executeParsedProgramChar( program.str(), std::back_inserter(data)))
	{
		m_laserOn = on;
		ret = true;
	}

	MUTEX_UNLOCK
	return ret;
}

bool  L2000::Scanner::setTableOn(bool on, int delay)
{
	MUTEX_LOCK

	std::vector<char> data;
	std::ostringstream program;
	program << "VERSION TONOFF " << (on ? "1 ":"0 ");
	if( on )
	program << "DELT " << delay << " ";

	//std::cout << "setTableOn():" << program.str() << std::endl;

	bool ret=false;
	if( m_status >= READY_WAITING &&
		executeParsedProgramChar( program.str(), std::back_inserter(data)))
	{
		m_tableOn=on;
		ret = true;
	}

	MUTEX_UNLOCK
	return ret;
}

bool L2000::Scanner::moveLaser( int delta )
{
	//std::cout << "moveLaser(" << delta << "):" << std::endl;
	if( delta < 0 )
		return moveLaserDown(-delta);
	else if( delta > 0 )
		return moveLaserUp(delta);
	return true;
}

bool L2000::Scanner::moveTable( int delta )
{
	//std::cout << "moveTable(" << delta << "):" << std::endl;
	if( delta < 0 )
		return moveTableCCW(-delta);
	else if( delta > 0 )
		return moveTableCW(delta);
	return true;
}

L2000::Scanner::~Scanner()
{
	m_scanner.close();
	m_scanner.init();
}

const std::string& L2000::Scanner::name() const
{
	return m_name;
}
const std::string& L2000::Scanner::description() const
{
	return m_description;
}

const std::vector<std::pair<std::string,std::string> >& L2000::Scanner::modes() const
{
	return m_modes;
}

bool L2000::Scanner::init()
{
	MUTEX_LOCK

	bool ret=false;
	if( m_scanner.init() )
	{
		m_status = INITIALIZED;
		ret = true;
	}

	MUTEX_UNLOCK
	return ret;
}

bool  L2000::Scanner::open()
{
	MUTEX_LOCK

	bool ret = false;
	if( m_scanner.open() ) {
		m_status = m_calibrationData.ready() ? CALIBRATED : READY_WAITING;
		ret = true;
	}

	MUTEX_UNLOCK
	return ret;
}

bool  L2000::Scanner::close()
{
	MUTEX_LOCK

	bool ret=false;
	if( m_scanner.close() )
	{
		m_status = INITIALIZED;
		ret=true;
	}

	MUTEX_UNLOCK
	return ret;
}

bool L2000::Scanner::getCurrentContrast()
{
	MUTEX_LOCK

	if( m_status >= READY_WAITING )
	{
		std::vector<char> r;
		if( executeParsedProgramChar(
			"ITSET1 0, 0 "
			"ITSET2 0, 0 ",
			std::back_inserter(r)) )
		{
			m_contrast1 = m_contrast2 = 0;
			if( (r[1] & 0x15) == 0 )
				m_contrast1 = int(r[0]);
			if( (r[3] & 0x2A) == 0 )
				m_contrast2 = int(r[2]);

			MUTEX_UNLOCK
			return true;
		}
	}
	MUTEX_UNLOCK
	return false;
}

bool L2000::Scanner::fineAdjustContrast()
{
	using namespace std;

	MUTEX_LOCK

	bool ret=false;
	if( m_status >= READY_WAITING )
	{
		std::vector<char>   r;
		if( executeParsedProgramChar(
			"LONOFF 0 "
			"ITSET1 0, 0 "
			"ITSET2 0, 0 "
			"READ 104 "
			"READ 105 ",
			std::back_inserter(r)) )
		{
			m_contrast1 = r[0];
			m_contrast2 = r[2];
			m_thresHold1 = unsigned(r[5]-0x0f);
			m_thresHold2 = unsigned(r[7]-0x0f);
			m_intTime1   = 1;
			m_intTime2   = 1;

			std::ostringstream program;
			program << "ITSET1 " << m_thresHold1  << ", " << m_intTime1 << endl
					<< "ITSET2 " << m_thresHold2 << ", " << m_intTime2  << endl
					<< "LONOFF 0" << endl;
			ret = executeParsedProgramChar(program.str(), std::back_inserter(r));
		}
	}
	MUTEX_UNLOCK
	return ret;
}

bool L2000::Scanner::prepareScan( ScanOptions& so )
{
	MUTEX_LOCK

	// setup laser and table delays
	// setup contrast
	bool ret=false;
	if( m_status >= READY_WAITING )
	{
		setTableOn(true,255);
		if( so.contrastMode != ScanOptions::Calibration ) {
			recalcLaserPosition( DEFAULT_CONTRAST_POS );
			fineAdjustContrast();
		}
		//setLaserOn(true,1);
		setLaserOn(false);
		ret=true;
	}

	MUTEX_UNLOCK
	return ret;
}

void L2000::Scanner::addCCDPointToView( int ccd, int alpha, int value )
{
	if( ccd & 1 )
	{
		if( ccd & 4 ) {
			m_ccd1ErrorPoints.lock();
			m_ccd1ErrorPoints.container().push_back( Plane2DPoints::value_type( value-200.0, alpha ));
			m_ccd1ErrorPoints.unlock();
		}
		else {
			m_ccd1Points.lock();
			m_ccd1Points.container().push_back( Plane2DPoints::value_type( value-200.0, alpha ));
			m_ccd1Points.unlock();
		}
	}
	else {
		if( ccd & 4 )
		{
			m_ccd2ErrorPoints.lock();
			m_ccd2ErrorPoints.container().push_back( Plane2DPoints::value_type( value-200.0, alpha ) );
			m_ccd2ErrorPoints.unlock();
		}
		else {
			m_ccd2Points.lock();
			m_ccd2Points.container().push_back( Plane2DPoints::value_type( value-200.0, alpha ) );
			m_ccd2Points.unlock();
		}
	}
}

void L2000::Scanner::interpolateReadings( int ccd, std::vector<Reading>& readings, const ScanOptions& options )
{
	int L2000::Reading::*ccdPtr = (ccd & 1) ? &Reading::ccd1 : &Reading::ccd2;
	
	std::vector<Reading>::iterator firstGood = readings.end(); // marks first valid before a hole
	std::vector<Reading>::iterator holeBegin = readings.end(); // marks begin of a hole
	std::vector<Reading>::iterator holeEnd   = readings.end(); // marks end of a hole
	std::vector<Reading>::iterator lastGood  = readings.end(); // marks last valid after a hole
		
	for( 	std::vector<Reading>::iterator iter = readings.begin();
		iter != readings.end();
		++iter )
	{
		if( (*iter).*ccdPtr == 0 ) // a Reading was bad
		{
			if( firstGood != readings.end() &&
			    holeBegin == readings.end() ) 	// not yet inside hole, start hole
			    holeBegin = iter;
			if( firstGood != readings.end() && 	// everything ready?
			    holeBegin != readings.end() &&
			    holeEnd != readings.end() )
			{
				lastGood = iter;
				
				if( lastGood-holeEnd >= options.minBorderPoints ) 
				{ // enough border points to fill hole?
					int startPoints = std::min( holeBegin-firstGood, options.minBorderPoints);
					
					if( startPoints < options.minBorderPoints )
						std::cerr << "WARNING: too few start points for interpolation, results may be inaccurate" << std::endl;
					int endPoints = options.minBorderPoints;
					if( holeEnd-holeBegin <= options.holeThreshold ) // real hole?
					{
						std::cout << "Start interpolation:" << std::endl;
						// create vector for interpolation
						std::vector<double> points;
						std::vector<double> result;
						//std::vector<Reading>::iterator cp = firstGood;
						std::vector<Reading>::iterator cp = holeBegin-startPoints;
						while( cp < holeEnd+endPoints )
						{
							if( (*cp).*ccdPtr == 0 ) {
								result.push_back( double(cp->alpha) );
								result.push_back( 0.0 );
							}
							else {
								points.push_back( double(cp->alpha) );
								points.push_back( double((*cp).*ccdPtr) );
							}
							++cp;
						}
						std::cout << "Existing Values: ";
						std::copy(points.begin(),points.end(), std::ostream_iterator<double>( std::cout, " ") );
						std::cout << std::endl;
						std::cout << "Target Values  : ";
						std::copy(result.begin(),result.end(), std::ostream_iterator<double>( std::cout, " ") );
						std::cout << std::endl;
						// interpolate
						MeshTools::newton_interpolation<double>( points.begin(), points.end(), result.begin(), result.end() );
						// fill new values into readings
						std::vector<double>::iterator ri = result.begin();
						cp = holeBegin-startPoints;
						while(	cp < holeEnd+endPoints && 
							ri != result.end() )
						{
							if( (*cp).*ccdPtr == 0 ) {
								if( cp->alpha == int(*ri) ) 
								{
									(*cp).*ccdPtr = int(*(ri+1));
									++ri;
									++ri;
	
									addCCDPointToView( ccd|4, cp->alpha-options.laserStart, (*cp).*ccdPtr);
								}
							}
							++cp;
						}
						std::cout << "Calculated Values: ";
						std::copy(result.begin(),result.end(), std::ostream_iterator<double>( std::cout, " ") );
						std::cout << std::endl;
					}
					firstGood = holeEnd;
					holeBegin = lastGood;
					holeEnd = readings.end();
					lastGood = readings.end();
				}
				else { // not enough points to fill hole
					std::cout << "Widening hole..." << std::endl;
					holeEnd = readings.end();
					lastGood = readings.end();
				}
			}
		}
		else 
		{
			if( firstGood == readings.end() )
				firstGood = iter;
			if( holeBegin != readings.end() &&
			    holeEnd == readings.end() ) // inside hole, finish hole!
				holeEnd = iter;
		}
	}
}

bool L2000::Scanner::collectScanLine( std::vector<Vertex>& result, ScanOptions& options, std::vector<Vertex>& ccd1, std::vector<Vertex>& ccd2 )
{
	MUTEX_LOCK

	using namespace std;
	bool ret=false;
	if( m_status >= CALIBRATED )
	{
		std::vector<Reading> readings;

		if( options.laserStep < 1.0 )
			options.laserStep = 1.0;

		if( recalcLaserPosition(options.laserStart) )
		{
			if( options.contrastMode == ScanOptions::PerColumn ) {
				fineAdjustContrast();
				setLaserOn(true);
			}

			m_ccd1Points.lock(); m_ccd1Points.clear(); m_ccd1Points.unlock();
			m_ccd1ErrorPoints.lock(); m_ccd1ErrorPoints.clear(); m_ccd1ErrorPoints.unlock();
			m_ccd2Points.lock(); m_ccd2Points.clear(); m_ccd2Points.unlock();
			m_ccd2ErrorPoints.lock(); m_ccd2ErrorPoints.clear(); m_ccd2ErrorPoints.unlock();
			
			Reading r;
			double realPos = options.laserStart;
			bool ok = setLaserOn(true);
			while( ok && int(realPos) <= options.laserEnd )
			{
				std::cout << "[" << int(realPos) << ", " << options.laserStep << "; " << options.laserEnd <<"] ";
				moveLaserTo( int(realPos) );

				if(	options.contrastMode == ScanOptions::Calibration ||
					(options.contrastMode == ScanOptions::PerColumn &&
					 realPos == (options.laserEnd+options.laserStart)/2) )
				{
					fineAdjustContrast();
					setLaserOn(true);
				}

				ok = getReading( r );
				realPos += options.laserStep;
				
				if( ok )
				{
					// Automatic mode makes second try on bad Reading
					if(	options.contrastMode == ScanOptions::Automatic &&
						(r.ccd1 == 0 || r.ccd2 == 0) )
					{
						fineAdjustContrast();
						setLaserOn(true);
						ok = getReading( r );
					}
					if( r.valid() )
					{
						addCCDPointToView( 1, r.alpha-options.laserStart, r.ccd1); 
						addCCDPointToView( 2, r.alpha-options.laserStart, r.ccd2); 
					}
				}
				else {
					// error occurred!
					r.ccd1=0; r.ccd1=0;
				}
				readings.push_back(r);
			}
			setLaserOn(false);
		}
		
		std::cout << std::endl << "Readings collected: " << readings.size() << std::endl;


		// try to fill holes and correct readings
		// 1. interpolate locally
		if( options.holeThreshold >= 0 ) // all holes bigger than this will not be filled
		{
			interpolateReadings( 1, readings, options );
			interpolateReadings( 2, readings, options );
		}
		
		// 2. transform Readings to Points on projection plane
		for( 	std::vector<Reading>::iterator iter = readings.begin();
			iter != readings.end();
			++iter )
		{
			result.push_back( Vertex(m_calibrationData, *iter, 3, options.correctReadings, *iter) );
			
			if( options.createCCD1Phantom ) 
			{
				if( iter->ccd1 > 0 )
				ccd1.push_back( Vertex(m_calibrationData, *iter, 1, options.correctReadings, *iter) );
				else
					ccd1.push_back(result.back());
			}
			if( options.createCCD2Phantom ) 
			{
				if(  iter->ccd2 > 0  )
					ccd2.push_back( Vertex(m_calibrationData, *iter, 2, options.correctReadings, *iter) );
				else
					ccd2.push_back(result.back());
			}
		}
		std::cout << "Vertices created  : " << readings.size() << std::endl;

		ret=true;
	}

	MUTEX_UNLOCK
	return ret;
}

// only "SuperScan for now
bool L2000::Scanner::collectScanLineRaw( std::vector<unsigned>& result, ScanMode mode, int start_pos, int end_pos)
{
	MUTEX_LOCK

	using namespace std;

	if( m_status >= READY_WAITING )
	{
		if( start_pos < LOWER_BOUND )
			start_pos = DEFAULT_LOWER_BOUND;
		if( start_pos > UPPER_BOUND )
			start_pos = DEFAULT_UPPER_BOUND;
		if( end_pos < LOWER_BOUND )
			end_pos = start_pos+64;
		if( end_pos < start_pos )
			end_pos = start_pos;

		m_ccd1Points.clear();
		m_ccd1Points.setUnit( 1.0 );
		m_ccd2Points.clear();
		m_ccd2Points.setUnit( 1.0 );

		// 1. Init Sequenz 1
		ostringstream program;
		program << "VERSION" << std::endl
				<< "LONOFF 0" << std::endl
				<< "VERSION" << std::endl
				<< "LONOFF 0" << std::endl
				<< "LONOFF 1" << std::endl
				<< "VERSION" << std::endl
				<< "DELT 255" << std::endl
				<< "TONOFF 1" << std::endl
				<< "GETIR " << start_pos  << std::endl
				<< "LONOFF 0"  << std::endl;

		//cerr << "Execute Program part: " << endl << program.str() << endl;

		std::vector<char>   r;
		executeParsedProgramChar(program.str(), std::back_inserter(r) );

		while( start_pos < end_pos )
		{

			// IT setup
			r.clear();
			// alias Superscan!
			program.str("");
			program << "LONOFF 0" << endl
					<< "ITSET1 0, 0" << endl
					<< "ITSET2 0, 0" << endl
					<< "READ 104 " << endl
					<< "READ 105 " << endl;

			//cerr << "Execute Program part: " << endl << program.str() << endl;

			if( !executeParsedProgramChar( program.str(), std::back_inserter(r) ) )
				break;

			//cerr << "-----" << endl;


			program.str("");
			program << "ITSET1 " << unsigned(r[5]-0x0f) << ", " << "1" << endl
					<< "ITSET2 " << unsigned(r[7]-0x0f) << ", " << "1" << endl;

			//cerr << "Execute Program part: " << endl << program.str() << endl;

			if( !executeParsedProgramChar( program.str(), std::back_inserter(r) ) )
				break;

			program.str("");
			program	<< "CCD12F" << endl
					<< "LONOFF 0" << endl
					<< "LONOFF 1 " << endl
					<< "CCD1M " << endl	// get CCD1 Middle
					<< "CCD2M " << endl	// get CCD2 Middle
					<< "LUP1"	<< endl // laser one pixel up
			;

			//cerr << "Execute Program part: " << endl << program.str() << endl;

			vector<Int16> data;
			if( !executeParsedProgramInt16( program.str(), std::back_inserter(data) ) )
				break;
			// Coordinaten kopieren

			assert( data.size() == 2+2 );

			result.push_back( start_pos );
			// what to do with first pixels?
			result.push_back( data[2] ); // CCD 1
			result.push_back( data[3] ); // CCD 2
			// update viewable containers
			m_ccd1Points.container().push_back( Plane2DPoints::value_type( double( data[2] )-200.0, start_pos-100.0 ) );
			m_ccd2Points.container().push_back( Plane2DPoints::value_type( double( data[3] )-200.0, start_pos-100.0 ) );
			// next position!
			start_pos++;
			
			qApp->processEvents(10);
		}
		m_scanner.execute("LONOFF", 0, r );
	}
	MUTEX_UNLOCK
	return start_pos == end_pos;
}

bool L2000::Scanner::ready() const
{
	return m_status == READY_WAITING;
}

bool L2000::Scanner::recalcLaserPosition( int pos )
{
	MUTEX_LOCK

	//std::cerr << "recalcPosition():" << pos << std::endl;
	if( m_scanner.status() == SerialPort::READY && pos < UPPER_BOUND )
	{
		std::vector<char> r;
		std::ostringstream cmd;
		cmd <<
		"VERSION "
		"LONOFF 0 "
		"VERSION "
		"LONOFF 0 "
		"LONOFF 1 "
		"GETIR " << pos << " "
		"LONOFF 0 ";
		if( executeParsedProgramChar(cmd.str(), std::back_inserter(r) ) ) {
			m_laserPos = pos;

			MUTEX_UNLOCK
			return true;
		}
	}
	MUTEX_UNLOCK
	return false;
}

bool L2000::Scanner::moveLaserTo( int pos )
{
	//std::cout << "moveLaserTo(" << pos << "):" << std::endl;
	int step = pos-m_laserPos;
	return	moveLaser(step);
}

bool L2000::Scanner::moveLaserUp(  int pos )
{
	MUTEX_LOCK
	bool ret=false;

	if( m_status >= READY_WAITING && pos > 0 )
	{
		std::ostringstream cmd;
		if( pos != m_laserStep )
			m_laserStep = pos;

		if( m_laserStep != 1 &&
			m_laserPos+m_laserStep <= UPPER_BOUND )
		{
				int left = m_laserStep;
				do {
					cmd << " WIDL ";
					cmd << ((left > 128) ? 128 : left);
					cmd << " LUP ";
					left -= 128;
				} while( left > 0 );
		}
		else if( m_laserPos < UPPER_BOUND )
			cmd << "LUP1 ";
		char *p;
		if( executeParsedProgramChar( cmd.str(), p ) ) {
			m_laserPos+=m_laserStep;
			ret=true;
		}
	}
	MUTEX_UNLOCK
	return ret;
}

bool L2000::Scanner::moveLaserDown( int pos )
{
	MUTEX_LOCK
	bool ret=false;

	if( m_status >= READY_WAITING && pos > 0 )
	{
		std::ostringstream cmd;
		if( pos != m_laserStep )
			m_laserStep = pos;

		if( m_laserStep != 1 &&
			m_laserPos>=m_laserStep )
		{
				int left = m_laserStep;
				do {
					cmd << " WIDL ";
					cmd << ((left > 128) ? 128 : left);
					cmd << " LDOWN ";
					left -= 128;
				} while( left > 0 );
		}
		else if( m_laserPos > 0 )
			cmd << "LDOWN1 ";
		char *p;
		if( executeParsedProgramChar( cmd.str(), p ) ) {
			m_laserPos-=m_laserStep;
			ret=true;
		}
	}
	MUTEX_UNLOCK
	return ret;
}


bool L2000::Scanner::moveTableTo( int pos )
{
	//std::cout << "moveTableTo(" << pos << "):" << std::endl;
	int step = pos-m_tablePos;
	return	moveTable(step);
}

bool L2000::Scanner::moveTableCW( int pos )
{
	MUTEX_LOCK
	bool ret=false;

	if( m_status >= READY_WAITING && pos > 0 )
	{
		std::ostringstream cmd;
		if( pos != m_tableStep )
		{
			m_tableStep = pos;
			if( m_tableStep != 1 ) {
				cmd << " WIDT " << m_tableStep;
				cmd << " TLEFT ";
			}
		}
		else {
			cmd << "TLEFT";
			if( m_tableStep == 1 )
				cmd << "1";
		}
		char *p;
		if( executeParsedProgramChar( cmd.str(), p ) ) {
			m_tablePos+=m_tableStep;
			m_tablePos %= 720;
			ret=true;
		}
	}
	MUTEX_UNLOCK
	return ret;
}

bool L2000::Scanner::moveTableCCW( int pos )
{
	if( m_status >= READY_WAITING && pos > 0 )
	{
		// NOT SUPPORTED BY SCANNER
		// so do a complete turn
		pos = 720 - pos;
		return moveTableCW(pos);
	}
	return false;
}

// set laser to zero position and switch off
bool L2000::Scanner::resetLaser()
{
	MUTEX_LOCK

	m_laserPos=0;
	m_laserOn=false;
	m_laserStep=1;
	recalcLaserPosition(0);
	setLaserOn(false);

	MUTEX_UNLOCK

	return true;
}
// set table to zero position and switch off
bool L2000::Scanner::resetTable()
{
	MUTEX_LOCK

	m_tablePos=0;
	m_tableStep=1;
	m_tableOn=false;
	setTableOn(false);

	MUTEX_UNLOCK

	return true;
}

void L2000::Scanner::adjustCCDReading( Int16& first, Int16& last )
{
	// limit check
	if( first < 200 || first > 850 )
		first = 0;
	if( last < 200 || last > 850 )
		last = 0;
	// replace zero reading
	if( first == 0 )
		first = last;
	else if( last == 0 )
		last = first;
}

bool L2000::Scanner::getReading( Reading& r, ReadingMode mode )
{
	if( m_status >= READY_WAITING )
	{
		// Möglichkeiten:
		//
		// CCD1, CCD2			-> first and last pixel
		// CCD12F				-> 1,2 first pixel
		// CCD1M, CCD2M			-> middle pixel
		// MIDDLE				-> 1,2 middle
		// CCD1ML, CCD2ML		-> middle pixel with limit check
		// CCDML				-> 1,2 middle pixel with limit check
		r.alpha = m_laserPos;
		r.ccd1  = 0;
		r.ccd2  = 0;
		r.i     = m_layer;
		std::vector<Int16> ccd;
		if( executeParsedProgramInt16("CCD1 CCD2 ",std::back_inserter(ccd) ) &&
			ccd.size() == 4 )
		{
			adjustCCDReading( ccd[0], ccd[1] );
			adjustCCDReading( ccd[2], ccd[3] );
			if( mode == FIRST )
			{
				r.ccd1 = ccd[0];
				r.ccd2 = ccd[2];
			}
			else if( mode == LAST )
			{
				r.ccd1 = ccd[1];
				r.ccd2 = ccd[3];
			}
			else {
				r.ccd1 = ccd[0];
				r.ccd1+= int(ccd[1]);
				r.ccd2 = ccd[2];
				r.ccd2+= int(ccd[3]);
				r.ccd1 >>=1;
				r.ccd2 >>=1;
			}
			return true;
		}
	}
	return false;
}

bool L2000::Scanner::getPoint( Mesh::Point& p )
{
	MUTEX_LOCK

	if( ready() )
	{
		if( m_mode == CONTRAST_ON_POINT )
		{
			// TO DO: add contrast measurement here
		}
		std::vector<Int16> ccd;
		if( executeParsedProgramInt16("CCD1M CCD2M", std::back_inserter(ccd) ) &&
			ccd.size() == 2 )
		{
			unsigned ccd1 = ccd[0];
			unsigned ccd2 = ccd[1];
			Calibration::Point c = m_calibrationData.transform( m_laserStep, m_layer, ccd1, ccd2 );
			// rotate
			// polar coords, c.x = radius, _tableStep = degree, c.y = c.y;
			double rad = m_tableStep*0.5*M_PI/180.0;
			p[0] = std::cos( rad ) * c.first;
			p[1] = c.second;
			p[2] = std::sin( rad ) * c.first;

			MUTEX_UNLOCK
			return true;
		}
	}
	MUTEX_UNLOCK
	return false;
}

template <class BackInserter>
bool L2000::Scanner::executeParsedProgramChar(const std::string& program, BackInserter result )
{
	// parsed mnemonic
	std::string mnemonic;
	// parsed args
	Int16 arg1, arg2;
	// vector for storing params to call
	std::vector<char> params(2);

	std::istringstream input( program );

	// first: parse program to see, if any errors occur
	while( input.good() && !input.eof() )
	{
		const L2000::InstructionSet::Instruction *inst = 0;

		mnemonic.clear();
		input >> mnemonic;

		if( input.fail() )
			break;

		inst = static_cast<const L2000::InstructionSet::Instruction*>(m_scanner.instructionSet()->instruction(mnemonic));

		if( inst )
		{
			if( inst->argCount() > 0 )
			{
				input >> arg1;
				if( !input.good() )
				{
					//std::cerr << "executeParsedProgram(): Parse Error: " << mnemonic << " parsing ARG1 at #" << input.tellg() << std::endl;
					return false;
				}

				if( inst->argCount() == 2 )
				{
					char c;
					input >> c;
					if( input.good() && c == ',' )
					{
						input >> arg2;
						if( !input.good() )
						{
							//std::cerr << "executeParsedProgram(): Parse Error: " << mnemonic << " parsing ARG2 at #" << input.tellg() << std::endl;
							return false;
						}
					}
					else
					{
						//std::cerr << "executeParsedProgram(): Parse Error: " << mnemonic << " parsing ARG2 at #" << input.tellg() << std::endl;
						return false;
					}
				}
			}

			// fill param
			params[0] = params[1] = 0;
			if( inst->argCount() == 1 && inst->paramLength()>0 )
			{
				if( inst->paramLength() == 1 )
					arg1.storeInByteArray( &params[0], Endianess::BIG, 1 );
				else if( inst->paramLength() == 2 )
					arg1.storeInByteArray( &params[0], Endianess::BIG, 2 );
			}
			else if( inst->argCount() == 2 && inst->paramLength()>0 )
			{
				arg1.storeInByteArray( &params[0], Endianess::BIG, 1 );
				arg2.storeInByteArray( &params[1], Endianess::BIG, 1 );
			}
			// create and execute command
			std::vector<char> dummy(0);
			if( !inst->execute( &m_scanner, params, dummy ) )
			{
				//std::cerr << "executeParsedProgram(): Error on execute()!" << std::endl;
				return false;
			}
			// convert char array to Int16
			for( unsigned j=0; j<dummy.size(); j++ )
				*result++ = dummy[j];
		}
		else
		{
			//std::cerr << "executeParsedProgram(): Parse Error: " << mnemonic << " at #" << input.tellg() << std::endl;
			return false;
		}
	}
	return true;
}

template <class BackInserter>
bool L2000::Scanner::executeParsedProgramInt16(const std::string& program, BackInserter result )
{
	// parsed mnemonic
	std::string mnemonic;
	// parsed commandline args
	Int16 arg1,arg2;
	// vector for storing params to call
	std::vector<char> params(2);

	std::istringstream input( program );

	// first: parse program to see, if any errors occur
	while( input.good() )
	{
		const L2000::InstructionSet::Instruction *inst = 0;

		mnemonic.clear();
		input >> mnemonic;

		if( input.eof() )
			break;

		inst = dynamic_cast<const L2000::InstructionSet::Instruction*>(m_scanner.instructionSet()->instruction(mnemonic));

		if( inst )
		{
			if( inst->argCount() > 0 )
			{
				input >> arg1;
				if( !input.good() )
				{
					//std::cerr << "executeParsedProgram(): Parse Error: " << mnemonic << " parsing ARG1 at #" << input.tellg() << std::endl;
					return false;
				}

				if( inst->argCount() == 2 )
				{
					char c;
					input >> c;
					if( input.good() && c == ',' )
					{
						input >> arg2;
						if( !input.good() )
						{
							//std::cerr << "executeParsedProgram(): Parse Error: " << mnemonic << " parsing ARG2 at #" << input.tellg() << std::endl;
							return false;
						}
					}
					else
					{
						//std::cerr << "executeParsedProgram(): Parse Error: " << mnemonic << " parsing ARG2 at #" << input.tellg() << std::endl;
						return false;
					}
				}
			}

			// fill param
			params[0] = params[1] = 0;
			if( inst->argCount() == 1 && inst->paramLength()>0 )
			{
				if( inst->paramLength() == 1 )
					arg1.storeInByteArray( &params[0], Endianess::BIG, 1 );
				else if( inst->paramLength() == 2 )
					arg1.storeInByteArray( &params[0], Endianess::BIG, 2 );
			}
			else if( inst->argCount() == 2 && inst->paramLength()>0 )
			{
				arg1.storeInByteArray( &params[0], Endianess::BIG, 1 );
				arg2.storeInByteArray( &params[1], Endianess::BIG, 1 );
			}
			// create and execute command
			// appended to result vector
			std::vector<char> dummy(0);
			if( !inst->execute( &m_scanner, params, dummy ) ) {
				//std::cerr << "executeParsedProgram(): Error on execute()!" << std::endl;
				return false;
			}
			// convert char array to Int16
			for( unsigned j=0; j<dummy.size(); j+=2 )
				*result++ = Int16( &dummy[j], Endianess::BIG, 2 );
		}
		else
		{
			//std::cerr << "executeParsedProgram(): Parse Error: " << mnemonic << " at #" << input.tellg() << std::endl;
			return false;
		}
	}
	return true;
}

void L2000::Scanner::dumpMemory()
{
	MUTEX_LOCK

	std::vector<char> p(2);
	std::vector<char> r;
	unsigned i;
	for( i=0; i<256; i++ )
	{
		p[0]=i&255;
		p[1]=0x00;
		m_scanner.execute("READ", p, r );
	}
	for( i=0; i<r.size(); ) {
		std::cout << std::hex << "0x" << int(r[i]) << ": " << std::dec << int(r[i+1]) << std::endl;
		i+=2;
	}

	MUTEX_UNLOCK
}



