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

#ifndef _L2000CALIBRATION_H_
#define _L2000CALIBRATION_H_

#define USE_BUILTIN

#include <vector>
#include <list>
#include <cmath>

#include <mtl/interval.h>

#ifndef USE_BUILTIN

#include <mtl/type/hugeint.h>
#include <mtl/number/rational.h>

#endif

#include "L2000Reading.h"

namespace L2000
{

class Calibration
{
public:
	static const double H0_MEASURE = 37.0;
	static const double D_MEASURE = 142.5;
	static const double ALPHA_MEASURE = 18.0;
	
#ifdef USE_BUILTIN
	typedef long double bigNum;
#else
	typedef mtl::type::hugeint<20>			bigInt;
	typedef mtl::number::rational<bigInt>	bigNum;
#endif

	static const double					ANGLE_STEP;
	static const double					TABLE_RADIUS;
	static const double					X_PLANE_NEAR;
	static const double					X_PLANE_CENTER;
	static const double					X_PLANE_FAR;

	typedef mtl::interval<double>		Interval;
	typedef mtl::interval<int>			planeInterval;
	typedef std::pair<double,double>	Point;
	typedef std::pair<int,int>			Tupel;

	static const Point InvalidPoint;

    Calibration();
    ~Calibration();

	void  addReading( const Reading& r ) { m_data.push_back(r); }
	void  addReading( int alpha, int i, int ccd1, int ccd2 );
	void  clearReadings();

	bool  calcTransformation(int max_layer);
	bool  probeTransformation( double& midError1, double& maxError1, double& midError2, double& maxError2 );
	Point transform(unsigned step,unsigned i, unsigned ccd1, unsigned ccd2, int ccd=3) const;
	Point transform(const Reading& r, int ccd=3) const;

	Point projectToLaserLine( const Point& p, unsigned step, unsigned i ) const;

	// Vorkalibrierung der Laserposition vornehmen
	void preCalibrate(int max_index, int method=0);

	const Point laserPos() const { return Point(-(m_dist+TABLE_RADIUS),m_H0); }
	const Point laserPosError() const { return m_laserPosError; }

	double  alpha( int ) const;
	double  alphaError() const { return m_alphaError; }
	double H(unsigned i) const;
	double dist() const { return m_dist; }

	bool readCalibrationData();
	bool writeCalibrationData() const;

	void clearStepperPos() {
		std::fill(m_stepperPos.begin(), m_stepperPos.end(), 0.0 );
		std::fill(m_stepperPosFlag.begin(), m_stepperPosFlag.end(), false );
	}
	double stepperPos( unsigned i ) const { return m_stepperPos[i]; }
	bool stepperPosUsed( unsigned i ) const { return m_stepperPos[i]; }
	void setStepperPos( unsigned i, double pos )
	{
		if( i<7 ) m_stepperPos[i] = pos;
	}
	void setStepperPosUseFlag( unsigned i, bool flag=true )
	{
		if( i<7 ) m_stepperPosFlag[i] = flag;
	}

	bool preCabReady() const
	{
		if( m_alpha != 0.0 &&
			m_dist != 0.0 &&
			m_H0 != 0.0 &&
			m_stepperPos.size() == 7 )
		{
			int cnt = 0;
			double last = 1000.0;
			for( unsigned i=0; i<=6; i++ ) {
				if( m_stepperPosFlag[i] && m_stepperPos[i] > 0.0 )
				{
					cnt++;
					if( m_stepperPos[i] >= last )
						return false;
					last = m_stepperPos[i];
				}
			}
			return cnt >= 2;
		}
		return false;
	}
	bool ready() const
	{
		bool good = preCabReady();
		good = good && m_data.size() >= 20;
		good = good && (ccd1Ready() || ccd2Ready());
		good = good && H(1) > 30.0 && H(1) < 50.0 && (m_dist+TABLE_RADIUS) > 200.0;
		return good;
	}

	bool	ccd1Ready() const { return m_ccd1Ready; }
	bool	ccd2Ready() const { return m_ccd2Ready; }

	
	void   setH0(double x);
	void   setAlpha(double x);
	void   setDist(double x);

protected:
	// calculates all plane intervals
	void	calcPlanes();
	// calculates ccd1 and ccd2 plane interval from coarse Readings list
	std::pair<planeInterval,planeInterval>
		calcCCDPlane( std::list<Reading>& readings, int i );

	double  intersectPlane( int, int, double ) const;

	// useful
	double   calcDistOfTan( double tanA, int first, int last );
	double   calcTanOfDist( double tanA, int first, int last );

	// for Method 1
	// Calculates Distqance for given Positions i and j
	Point calcDistance(const int i, const int j) const;
	Point calcTangens(const int i, const int j) const;
	// estimate an Intervall where Tanges Alpha is within
	Interval estimateTangens(int first, int last, int min=1) const;
	Interval estimateDistance(int first, int last, int min=1) const;
	// use all possible Estimations and change H0 to get the smallest possible Interval
	Interval approximateDistance(double error, int first, int last, int min=1);
	Interval approximateTangens (double error, int first, int last, int min=1);
	// for Method two
	std::pair<Interval,Interval> intersectDistance( int first, int last, int min=1);
	Interval intersectTangens( int first, int last, int min=1);

private:

	//*****************************************************
	// COLLECTED DATA
	//*****************************************************
	// array der messwerte von kamera und stepper
	std::vector<Reading>	m_data;
	// Winkelstellungen des Lasers bei Vorkalibrierung
	// bool -> für kalibrierung benutzen oder nicht
	std::vector<double>		m_stepperPos;
	std::vector<int>		m_stepperPosFlag;

	//******************************************************
	// INTERMEDIATE DATA
	//******************************************************
	// intervals, in which ccd readings can be assigned to one of three planes
	// for each ccd
	std::pair<planeInterval,planeInterval > m_planeNear;
	std::pair<planeInterval,planeInterval > m_planeCenter;
	std::pair<planeInterval,planeInterval > m_planeFar;
	//Point	m_laserPos;	// Berechnete Position des Lasers
	Point	m_laserPosError; // maximimale Abweichung
	double  m_alpha;	// Winkel des Lasers in Nullstellung
	double  m_alphaError; // maximaler Fehler für alpha
	double  m_dist;		// Abstand des Lasers vom Meßpunkt der Kalibrierung 1
	double  m_H0;		// Höhe der untersten Laserstellung

	//******************************************************
	// RESULT coefficients
	//******************************************************
	std::vector<double>			m_coeff;
	bool						m_ccd1Ready;
	bool						m_ccd2Ready;
};

} // namespace

#endif
