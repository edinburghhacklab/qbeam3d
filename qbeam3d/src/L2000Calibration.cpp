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

#include <iostream>
#include <fstream>
#include <list>
#include <iterator>
#include <functional>

#include <mtl/interval.h>
#include <mtl/circle.h>
#include <mtl/matrix.h>
#include <mtl/algorithm.h>
#include <mtl/vector.h>

// XML stuff
#include <qdom.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>

#include <limits>

#ifndef USE_BUILTIN

#include <mtl/type/hugeint.h>
#include <mtl/number/rational.h>

#endif

#include "L2000Calibration.h"

namespace L2000
{

const double Calibration::ANGLE_STEP 	= 0.45;
const double Calibration::TABLE_RADIUS	= 90.0;
const double Calibration::X_PLANE_NEAR	= -65.0;
const double Calibration::X_PLANE_CENTER= 0.0;
const double Calibration::X_PLANE_FAR	= 65.0;

const Calibration::Point Calibration::InvalidPoint( std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN());

Calibration::Calibration() :
	m_data(0),
	m_stepperPos(7,0.0),
	m_coeff(12,0.0),
	m_stepperPosFlag(7,false),
	m_planeNear(),
	m_planeCenter(),
	m_planeFar(),
	//m_laserPos(0,0),
	m_laserPosError(0,0),
	m_alpha(0.0),	// Winkel des Lasers in Nullstellung
	m_alphaError(0.0),
	m_dist(0.0),				// Abstand des Lasers vom Meßpunkt der Kalibrierung 1
	m_H0(0.0),					// Höhe der untersten Laserstellung
	m_ccd1Ready(false),
	m_ccd2Ready(false)
{
}

Calibration::~Calibration()
{
}

bool Calibration::readCalibrationData()
{
	std::cout << "readCalibrationData():" << std::endl;

	using namespace std;
	QString filePath = QDir::currentDirPath()  + "/L2000Calibration.xml";
	QFile in(filePath);
	QDomDocument xml("L2000");

	if( !in.open( IO_ReadOnly ) )
	{
		std::cerr << "Error opening " << filePath.latin1() << std::endl;
	}
	if( xml.setContent( &in ) )
	{
		std::cout << "XML ok" << std::endl;
	// maximum likelihood test for CCD1

		// PreCalibration Data
		QDomNodeList nodeList = xml.elementsByTagName("PreCalibration");
		if( nodeList.count() == 1 )
		{
			std::cout << "Precalibration Tag found:" << std::endl;

			m_stepperPos.clear();
			m_stepperPos.resize(7,0.0);

			QDomElement preCabTag = nodeList.item(0).toElement();
			if( !preCabTag.isNull() )
			{
				QDomNode n = preCabTag.firstChild();
				while( !n.isNull() ) {
					QDomElement position = n.toElement();
					if( !position.isNull() )
					{
						std::cout << "Next Tag: " << position.tagName().latin1() << ": ";

						if( position.tagName() == "StepperPosition" )
						{
							int level = position.attribute("level").toInt();
							double angle = position.attribute("angle").toDouble();
							bool flag = position.attribute("use") == "true";
							if( level > 0 && level < 8 ) {
								m_stepperPos[level-1] = angle;
								m_stepperPosFlag[level-1] = flag;
							}
							std::cout << level << ": " << angle << "(" << flag << ")";
						}
						else if(  position.tagName() == "LaserPosition" )
						{
							double d = position.attribute("alpha","0").toDouble();
							if( d != 0.0 ) m_alpha=d;
							d = position.attribute("distance","0").toDouble();
							if( d != 0.0 ) m_dist=d;
							d = position.attribute("h0","0").toDouble();
							if( d != 0.0 ) m_H0=d;

							//m_laserPos.first = -( m_dist + TABLE_RADIUS);
							//m_laserPos.second = m_H0;

							std::cout << "A,D,H0 = " << m_alpha << ", " << m_dist << ", " <<  m_H0 << ", ";
						}
						std::cout << std::endl;
					}
					n = n.nextSibling();
				}
			}
		}
		// Main Calibration CCD data
		nodeList = xml.elementsByTagName("MainCalibration");
		if( nodeList.count() == 1 )
		{

			std::cout << "MainCalibration Tag found:" << std::endl;

			m_data.clear();
			QDomElement mainCabTag = nodeList.item(0).toElement();
			if( !mainCabTag.isNull() )
			{
				QDomNode n = mainCabTag.firstChild();
				while( !n.isNull() ) {
					QDomElement reading = n.toElement();
					if( !reading.isNull() )
					{
						std::cout << "Next Tag: " << reading.tagName().latin1() << ": ";

						if( reading.tagName() == "Reading" )
						{
							int level = reading.attribute("level").toInt();
							int angle = reading.attribute("angle").toInt();
							int ccd1  = reading.attribute("ccd1").toInt();
							int ccd2  = reading.attribute("ccd2").toInt();
							m_data.push_back( Reading(angle,level,ccd1,ccd2) );

							std::cout << level << "-" << angle << "°: " << ccd1 << ", " << ccd2;
						}
						else if( reading.tagName() == "CCD1" )
						{
							m_coeff[0] = reading.attribute("x0","1").toDouble();
							m_coeff[1] = reading.attribute("x1","1").toDouble();
							m_coeff[2] = reading.attribute("x2","1").toDouble();
							m_coeff[3] = reading.attribute("x3","1").toDouble();
							m_coeff[4] = reading.attribute("x4","1").toDouble();
							m_coeff[5] = reading.attribute("x5","1").toDouble();
							m_ccd1Ready=true;

							std::cout << m_coeff[0] << ", "
							<< m_coeff[1] << ", "
							<< m_coeff[2] << ", "
							<< m_coeff[3] << ", "
							<< m_coeff[4] << ", "
							<< m_coeff[5] << ", ";
						}
						else if( reading.tagName() == "CCD2" )
						{
							m_coeff[6] = reading.attribute("x0","1").toDouble();
							m_coeff[7] = reading.attribute("x1","1").toDouble();
							m_coeff[8] = reading.attribute("x2","1").toDouble();
							m_coeff[9] = reading.attribute("x3","1").toDouble();
							m_coeff[10]= reading.attribute("x4","1").toDouble();
							m_coeff[11]= reading.attribute("x5","1").toDouble();
							m_ccd2Ready=true;

							std::cout << m_coeff[0] << ", "
							<< m_coeff[1] << ", "
							<< m_coeff[2] << ", "
							<< m_coeff[3] << ", "
							<< m_coeff[4] << ", "
							<< m_coeff[5] << ", ";
						}
						std::cout << std::endl;
					}
					n = n.nextSibling();
				}
			}
		}
		in.close();
		return true;
	}
	else {
		std::cerr << "Error opening XML" << std::endl;
	}
	in.close();
	return false;
}

bool Calibration::writeCalibrationData() const
{
	if( m_stepperPos.size() != 7 )
		return false;
//	if( m_data.size() < 20 )
//		return false;

	using namespace std;

	QDomDocument xml("L2000");
	QDomElement root = xml.createElement("L2000");
	xml.appendChild(root);
	QDomElement preCabTag = xml.createElement("PreCalibration");
	QDomElement mainCabTag = xml.createElement("MainCalibration");
	root.appendChild( preCabTag );
	root.appendChild( mainCabTag );

	// precalibration results:
	{
		QDomElement e = xml.createElement("LaserPosition");
		preCabTag.appendChild(e);
		e.setAttribute("h0",QString::number(m_H0) );
		e.setAttribute("distance",QString::number(m_dist) );
		e.setAttribute("alpha",QString::number(m_alpha) );
	}
	// precalibration positions and data:
	for( unsigned i=0; i<m_stepperPos.size(); i++ ) {
		QDomElement e = xml.createElement("StepperPosition");
		preCabTag.appendChild(e);
		e.setAttribute("level",QString::number(i+1));
		e.setAttribute("angle",QString::number(m_stepperPos[i],'f',2));
		e.setAttribute("use", (m_stepperPosFlag[i] ? "true":"false") );
	}
	// CCD calibration results:
	{
		QDomElement e = xml.createElement("CCD1");
		mainCabTag.appendChild( e );
		e.setAttribute("x0",QString::number(m_coeff[0]) );
		e.setAttribute("x1",QString::number(m_coeff[1]) );
		e.setAttribute("x2",QString::number(m_coeff[2]) );
		e.setAttribute("x3",QString::number(m_coeff[3]) );
		e.setAttribute("x4",QString::number(m_coeff[4]) );
		e.setAttribute("x5",QString::number(m_coeff[5]) );
		e = xml.createElement("CCD2");
		mainCabTag.appendChild( e );
		e.setAttribute("x0",QString::number(m_coeff[6]) );
		e.setAttribute("x1",QString::number(m_coeff[7]) );
		e.setAttribute("x2",QString::number(m_coeff[8]) );
		e.setAttribute("x3",QString::number(m_coeff[9]) );
		e.setAttribute("x4",QString::number(m_coeff[10]) );
		e.setAttribute("x5",QString::number(m_coeff[11]) );
	}
	// CCD calibration data:
	for( unsigned idx=0; idx<m_data.size(); idx++ ) {
		QDomElement reading = xml.createElement("Reading");
		mainCabTag.appendChild(reading);
		reading.setAttribute("level",QString::number(m_data[idx].i));
		reading.setAttribute("angle",QString::number(m_data[idx].alpha));
		reading.setAttribute("ccd1",QString::number(m_data[idx].ccd1));
		reading.setAttribute("ccd2",QString::number(m_data[idx].ccd2));
	}

	QString filePath = QDir::currentDirPath()  + "/L2000Calibration.xml";
	QFile out(filePath);
	if( out.open( IO_WriteOnly ) )
	{
		QTextStream stream( &out );
		stream << xml.toString();
		out.close();
	}
	return out.status() == IO_Ok;
}

double  Calibration::alpha( int step ) const
{
	return m_alpha + (double(step)*ANGLE_STEP*M_PI/180.0);
}

// intersect beam from laser point with height H(i) at angle alpha(step) with
// orthogonal plane at x
double  Calibration::intersectPlane( int angle_step, int i, double x ) const
{
// beam:
// |x| |d|  |sin a| => t = (x-d)/(sin a)
// | |=| |+t|     |
// |y| |h|  |-cos a|
//
// intersection with plane at x at:
// |x| |x                      |
// | |=|                       |
// |y| |h+(x-d)*(cos a)*(sin a)|
	double a = alpha(angle_step);
	double h = H(i);
	double d = -(m_dist+TABLE_RADIUS);
	double y = h - (x-d)/std::tan(a);

	//std::cout << "(" << d << ", " << h << ") at " << (a/M_PI*180.0) << "° -> (" << x << ", " << y << ")" << std::endl;

	return y;
}

bool  Calibration::calcTransformation( int max_layer )
{
	m_coeff.clear();
	m_ccd1Ready=false;
	m_ccd2Ready=false;
	// calculate laser position and step 0 angle
	if( !preCabReady() )
		preCalibrate(max_layer);
	if( !preCabReady() )
		return false;
	// calculate plane intervals to sperate readings
	calcPlanes();
	// Do it for ccd1 and ccd2
	// calc intersection point of laser and planes
	// collect coords and Points
	std::vector<std::pair<int,Point> > cam1,cam2;
	unsigned c1n=0,c1c=0,c1f=0,c2n=0,c2f=0,c2c=0;

	std::vector<Reading>::iterator iter = m_data.begin();
	while( iter != m_data.end() )
	{
		int ccd1 = iter->ccd1;
		int ccd2 = iter->ccd2;
		int step = iter->alpha;
		int i    = iter->i;
		++iter;

		// check and add to plane -65
		// look an laser plane:
		// x = lx + t* sin a -> x = x
		// y = ly + t* cos a -> y = ly + (x-lx)*cos a / sin a

		// CCD1
		if( m_planeNear.first.isInside(ccd1) ) {
			double y = intersectPlane( step, i, X_PLANE_NEAR);
			cam1.push_back( std::pair<int,Point>(ccd1, Point(X_PLANE_NEAR,y)) );
			c1n++;
		}
		else if( m_planeCenter.first.isInside(ccd1) ) {
			double y = intersectPlane( step, i, X_PLANE_CENTER );
			cam1.push_back( std::pair<int,Point>(ccd1, Point(X_PLANE_CENTER,y)));
			c1c++;
		}
		else if( m_planeFar.first.isInside(ccd1) ) {
			double y = intersectPlane( step, i, X_PLANE_FAR );
			cam1.push_back( std::pair<int,Point>(ccd1, Point(X_PLANE_FAR,y)));
			c1f++;
		}
		// CCD2
		if( m_planeNear.second.isInside(ccd2) ) {
			double y = intersectPlane( step, i, X_PLANE_NEAR);
			cam2.push_back( std::pair<int,Point>(ccd2, Point(X_PLANE_NEAR,y)) );
			c2n++;
		}
		else if( m_planeCenter.second.isInside(ccd2) ) {
			double y = intersectPlane( step, i, X_PLANE_CENTER );
			cam2.push_back( std::pair<int,Point>(ccd2, Point(X_PLANE_CENTER,y)));
			c2c++;
		}
		else if( m_planeFar.second.isInside(ccd2) ) {
			double y = intersectPlane( step, i, X_PLANE_FAR );
			cam2.push_back( std::pair<int,Point>(ccd2, Point(X_PLANE_FAR,y)));
			c2f++;
		}
	}

	// calc transformation for each ccd
	//
	// try: x = (a+by+cz)/(d+ey+fz)
	// resolves to
	// -> a + y*b + z*c -x*d -xy*e -xz*f =0
	// -> a=-1!
	// | y  z  -x  -xy  -xz |  |b|
	// |                    |  |c|
	// |        ..          |* |d| = 1
	//                         |e|
	//                         |f|
	// solve least Squares for this matrix
	//std::cout << "Build matrix A" << std::endl;
	if( c1n >= 10 && c1c >= 5 && c1f >= 4 )
	{
#ifndef USE_BUILTIN
		bigNum::setPrintAsFloat(true);
#endif
		unsigned rows1 = cam1.size();
		mtl::dyn_vector<bigNum> b(rows1);
		mtl::dyn_matrix<bigNum> A(rows1,5);
		for(unsigned i=0; i<cam1.size(); i++)
		{
#ifndef USE_BUILTIN
			bigNum x = bigInt(cam1[i].first);
#else
			bigNum x = cam1[i].first;
#endif
			bigNum y(cam1[i].second.first);
			bigNum z(cam1[i].second.second);
			A[i][0] = y;
			A[i][1] = z;
			A[i][2] = -x;
			A[i][3] = -(x*y);
			A[i][4] = -(x*z);
			b[i]    = 1.0;
//			std::cout << A[i] << std::endl;
		}
		mtl::dyn_vector<bigNum> l = mtl::leastSquares( A, b );
		m_coeff.push_back(-1.0);
#ifdef USE_BUILTIN
		m_coeff.push_back(l[0]);
		m_coeff.push_back(l[1]);
		m_coeff.push_back(l[2]);
		m_coeff.push_back(l[3]);
		m_coeff.push_back(l[4]);
#else
		m_coeff.push_back(l[0].toDouble());
		m_coeff.push_back(l[1].toDouble());
		m_coeff.push_back(l[2].toDouble());
		m_coeff.push_back(l[3].toDouble());
		m_coeff.push_back(l[4].toDouble());
#endif
		std::ios_base::fmtflags oldf = std::cout.flags();
		std::ios_base::fmtflags newf = oldf;
		newf |= std::ios_base::fixed;
		std::cout.flags(newf);
		std::cout.precision(20);
		std::cout << "u = f2(x,y) = (" <<
		m_coeff[0] << " + " << m_coeff[1] << "*x + " << m_coeff[2] << "*y)/(" <<
		m_coeff[3] << " + " << m_coeff[4] << "*x + " << m_coeff[5] << "*y)" << std::endl;
		std::cout.flags(oldf);
		m_ccd1Ready=true;
	}
	if( c2n >= 10 && c2c >= 5 && c2f >= 4 )
	{
#ifndef USE_BUILTIN
		bigNum::setPrintAsFloat(true);
#endif
		//std::cout << "Build matrix A" << std::endl;
		unsigned rows2 = cam2.size();
		mtl::dyn_vector<bigNum> b(rows2);
		mtl::dyn_matrix<bigNum> A(rows2,5);
		for(unsigned i=0; i<cam2.size(); i++)
		{
#ifndef USE_BUILTIN
			bigNum x = bigInt(cam2[i].first);
#else
			bigNum x = cam2[i].first;
#endif
			bigNum y(cam2[i].second.first);
			bigNum z(cam2[i].second.second);
			A[i][0] = y;
			A[i][1] = z;
			A[i][2] = -x;
			A[i][3] = -(x*y);
			A[i][4] = -(x*z);
			b[i]    = 1.0;
		}
		mtl::dyn_vector<bigNum> l = mtl::leastSquares( A, b );
		m_coeff.push_back(-1.0);
#ifdef USE_BUILTIN
		m_coeff.push_back(l[0]);
		m_coeff.push_back(l[1]);
		m_coeff.push_back(l[2]);
		m_coeff.push_back(l[3]);
		m_coeff.push_back(l[4]);
#else
		m_coeff.push_back(l[0].toDouble());
		m_coeff.push_back(l[1].toDouble());
		m_coeff.push_back(l[2].toDouble());
		m_coeff.push_back(l[3].toDouble());
		m_coeff.push_back(l[4].toDouble());
#endif
		std::ios_base::fmtflags oldf = std::cout.flags();
		std::ios_base::fmtflags newf = oldf;
		newf |= std::ios_base::fixed;
		std::cout.flags(newf);
		std::cout.precision(20);
		std::cout << "u = f2(x,y) = (" <<
		m_coeff[6] << " + " << m_coeff[7] << "*x + " << m_coeff[8] << "*y)/(" <<
		m_coeff[9] << " + " << m_coeff[10] << "*x + " << m_coeff[11] << "*y)" << std::endl;
		std::cout.flags(oldf);
		m_ccd2Ready=true;
	}
	return m_ccd1Ready || m_ccd2Ready;
}

bool Calibration::probeTransformation( double& midError1, double& maxError1, double& midError2, double& maxError2 )
{
// for all valid Readgins
// calculate Point via Transformation and check against intersected Point

	int n1 = 0;
	int n2 = 0;
	midError1=0.0;
	midError2=0.0;
	maxError1=0.0;
	maxError2=0.0;

	for(std::vector<Reading>::iterator iter = m_data.begin(); iter != m_data.end(); ++iter )
	{
		int ccd1 = iter->ccd1;
		int ccd2 = iter->ccd2;
		int step = iter->alpha;
		int i    = iter->i;
		Point p1(0.0,0.0);
		Point p2(0.0,0.0);
		// check and add to plane -65
		// look an laser plane:
		// x = lx + t* sin a -> x = x
		// y = ly + t* cos a -> y = ly + (x-lx)*cos a / sin a

		// CCD1
		if( m_planeNear.first.isInside(ccd1) )
			p1 = Point(X_PLANE_NEAR,intersectPlane( step, i, X_PLANE_NEAR));
		else if( m_planeCenter.first.isInside(ccd1) )
			p1 = Point(X_PLANE_CENTER,intersectPlane( step, i, X_PLANE_CENTER));
		else if( m_planeFar.first.isInside(ccd1) )
			p1 = Point(X_PLANE_FAR,intersectPlane( step, i, X_PLANE_FAR));
		// CCD2
		if( m_planeNear.second.isInside(ccd2) )
			p2 = Point(X_PLANE_NEAR,intersectPlane( step, i, X_PLANE_NEAR));
		else if( m_planeCenter.second.isInside(ccd2) )
			p2 = Point(X_PLANE_CENTER,intersectPlane( step, i, X_PLANE_CENTER));
		else if( m_planeFar.second.isInside(ccd2) )
			p2 = Point(X_PLANE_FAR,intersectPlane( step, i, X_PLANE_FAR));

		if( p1.first != 0.0 ) {
			// calc error CCD1
			Point t1 = transform( step, i, ccd1, ccd2 );

			//std::cout << "CCD1: d([" << p1.first << ", " << p1.second << "],[" << t1.first << ", " << t1.second << "] = ";

			p1.first -= t1.first;
			p1.second -= t1.second;
			double d = sqrt( p1.first*p1.first + p1.second*p1.second);
			midError1 += d;
			maxError1 = std::max( maxError1, d );

			//std::cout << d << std::endl;

			n1++;
		}
		if( p2.first != 0.0 ) {
			// calc error CCD2
			Point t2 = transform( step, i, ccd1, ccd2 );

			//std::cout << "CCD2: d([" << p2.first << ", " << p2.second << "],[" << t2.first << ", " << t2.second << "] = ";

			p2.first -= t2.first;
			p2.second -= t2.second;
			double d = sqrt( p2.first*p2.first + p2.second*p2.second);
			midError2 += d;
			maxError2 = std::max( maxError2, d );

			//std::cout << d << std::endl;

			n2++;
		}
	}
	midError1 /= double(n1);
	midError2 /= double(n2);
	return true;
}


void Calibration::addReading( int alpha, int i, int ccd1, int ccd2 )
{
	m_data.push_back( Reading( alpha, i, ccd1, ccd2 ) );
}

void Calibration::clearReadings()
{
	m_data.clear();

	m_planeNear.first.clear();
	m_planeNear.second.clear();
	m_planeFar.first.clear();
	m_planeFar.second.clear();
	m_planeCenter.first.clear();
	m_planeCenter.second.clear();
}

void Calibration::calcPlanes()
{
	// calculate the three intervals for the three planes
	// 1st plane at x = -65 mmm, readings are about 570-580
	// 2nd plane at x = 0, readings are about 400-410
	// 3rd plane at x = 65 mm, readings are about 280-290
	// we do a coarse separation with +-50
	// for the planes distances are more than 100 ccd pixel we can use quite a
	// large starting interval to get a coarse idea which reading belongs to which plane

	std::list<Reading> plane_65;
	std::list<Reading> plane0;
	std::list<Reading> plane65;

	std::vector<Reading>::const_iterator iter = m_data.begin();
	while( iter != m_data.end() )
	{
		mtl::interval<int> iv_65(575-25,575+25);
		mtl::interval<int> iv0(405-25,405+25);
		mtl::interval<int> iv65(285-25,285+25);
		int ccd1 = iter->ccd1;
		int ccd2 = iter->ccd2;
		int stepper = iter->alpha;

		std::cout << iter-m_data.begin() << ": " << "(" << ccd1 << ", " << ccd2 << ")[" << stepper << "] -> ";

		if( iv_65.isInside( ccd1 ) || iv_65.isInside( ccd2 ) )
		{
			plane_65.push_back( *iter );
			//std::cout << "-65 (" << plane_65.size() << ")" << std::endl;
		}
		else if( iv0.isInside( ccd1 ) || iv0.isInside( ccd2 ) )
		{
			plane0.push_back( *iter );
			//std::cout << "0 (" << plane0.size() << ")" << std::endl;
		}
		else if( iv65.isInside( ccd1 ) || iv65.isInside( ccd2 ) )
		{
			plane65.push_back( *iter );
			//std::cout << "65 (" << plane65.size() << ")" << std::endl;
		}
		else {
			//std::cout << "INVALID" << std::endl;
		}
		++iter;
	}
	m_planeNear = calcCCDPlane( plane_65, 5 );
	m_planeCenter   = calcCCDPlane( plane0, 5 );
	m_planeFar  = calcCCDPlane( plane65, 5 );

	std::cout << "Plane at -65 mm: " << m_planeNear.first << ", " << m_planeNear.second << std::endl;
	std::cout << "Plane at +-0 mm: " << m_planeCenter.first << ", " << m_planeCenter.second << std::endl;
	std::cout << "Plane at +65 mm: " << m_planeFar.first << ", " << m_planeFar.second << std::endl;
}

std::pair<Calibration::planeInterval,Calibration::planeInterval>
	Calibration::calcCCDPlane( std::list<Reading>& readings, int i )
{
	std::pair<planeInterval,planeInterval> result;
	// 1st calculate arithmetical mean value for both ccds
	double mean1=0.0,mean2=0.0;
	double n1=0.0,n2=0.0;
	for(std::list<Reading>::iterator item = readings.begin();
		item != readings.end(); ++item )
	{
		if( item->ccd1 != 0 )
		{
			mean1 += double( item->ccd1 );
			n1+=1.0;
		}
		if( item->ccd2 != 0 )
		{
			mean2 += double( item->ccd2 );
			n2+=1.0;
		}
	}
	mean1 /= n1;
	mean2 /= n2;
	// 2nd use i to create interval
	result.first.setRange( int(std::floor(mean1-double(i))), int(std::ceil(mean1+double(i))) );
	result.second.setRange( int(std::floor(mean2-double(i))), int(std::ceil(mean2+double(i))) );
	// 3rd remove all readings that don't fit into resulting interval
	readings.remove_if( std::mem_fun_ref(&Reading::invalid) );
	return result;
}

Calibration::Point Calibration::transform(const Reading& r, int ccd) const
{
	return transform( r.alpha, r.i, r.ccd1, r.ccd2, ccd );
}
// transform a given tupel (k,step) to a point on the laser plane (x,y)
// unit in mm
Calibration::Point Calibration::transform(unsigned step, unsigned ii, unsigned ccd1, unsigned ccd2, int ccd) const
{
// Do for each CCD:
// x = {(kf-c)[y(l)-mx(l)]-a+dk}/{(b-ke)-m*(fk-c)}
// y = {(ke-b)[x(l)m-y(l)]-m*(a-dk)}/{m*(c-kf)-ek+b}
// Take Point in the middle of connecting line
	assert( m_coeff.size() == 12 || m_coeff.size()==6 );

	long double m = -(std::cos(alpha(step))/std::sin(alpha(step)));
	long double lx = -(m_dist+TABLE_RADIUS); //m_laserPos.first;
	long double ly = H(2);
	long double x1=0.0,x2=0.0,y1=0.0,y2=0.0;

	int offset = 0;
	bool useCamera1=false;
	bool useCamera2=false;
	// CCD1
	if( m_ccd1Ready && ccd1 > 0 && (ccd & 1) )
	{
		useCamera1=true;
		long double a = m_coeff[0];
		long double b = m_coeff[1];
		long double c = m_coeff[2];
		long double d = m_coeff[3];
		long double e = m_coeff[4];
		long double f = m_coeff[5];
		long double k = ccd1;

		x1 = (a-d*k)/(f*k-c);
		x1 += m*lx-ly;
		x1 /= (m-((b-e*k)/(f*k-c)));
		//x1 = ((k*f-c)*(ly-m*lx)-a+k*d)/((b-k*e)-m*(k*f-c));
		//y1 = ((k*e-b)*(lx*m-ly)-m*(a-k*d))/(m*(c-k*f)-(k*e-b));
		// y = m*x + b
		// b = ly-m*lx
		y1 = m*x1 + ly - m*lx;
	}
	
	if( m_ccd1Ready )
		offset += 6;

	// CCD2
	if( m_ccd2Ready && ccd2 > 0 && (ccd & 2))
	{
		useCamera2=true;
		long double a = m_coeff[offset];
		long double b = m_coeff[offset+1];
		long double c = m_coeff[offset+2];
		long double d = m_coeff[offset+3];
		long double e = m_coeff[offset+4];
		long double f = m_coeff[offset+5];
		long double k = ccd2;

		x2 = (a-d*k)/(f*k-c);
		x2 += m*lx-ly;
		x2 /= (m-((b-e*k)/(f*k-c)));
		//x2 = ((k*f-c)*(ly-m*lx)-a+k*d)/((b-k*e)-m*(k*f-c));
		//y2 = ((k*e-b)*(lx*m-ly)-m*(a-k*d))/(m*(c-k*f)-(k*e-b));
		y2 = m*x2 + ly - m*lx;
	}

	long double x=InvalidPoint.first;
	long double y=InvalidPoint.second;

	if( useCamera1 && useCamera2 ){
		x = double(x1)+double(x2);
		x *= 0.5;
		y = double(y1)+double(y2);
		y *= 0.5;
		y += (ii-2)*35.0;
	}
	else if( useCamera1 ){
		x = x1;
		y = y1;
		y += (ii-2)*35.0;
	}
	else if( useCamera2 ){
		x = x2;
		y = y2;
		y += (ii-2)*35.0;
	}

	//std::cout << "P  =(" << x << ", " << y << ") from {"
				//<< std::endl;
	// std::cout
	//			<< "P1 =(" << x1 << ", " << y1 << "), "
	//			<< "P2 =(" << x2 << ", " << y2 << ")}"
	//			<< std::endl;

	return Calibration::Point(x,y);
}

// Project Point p onto laser line and return point that lies on line
Calibration::Point Calibration::projectToLaserLine( const Point& pp, unsigned step, unsigned i ) const
{

	mtl::vector<double,2> l,t1,t2,p,r;

	// laser line: l + x*t1 = y
	l[0] = -(m_dist+TABLE_RADIUS); //m_laserPos.first;
	l[1] = H(i);
	t1[0] = std::sin(alpha(step));
	t1[1] = -std::cos(alpha(step));
	// point p + x*t2 = y
	p[0] = pp.first;
	p[1] = pp.second;
	t2[0] = -t1[1];
	t2[1] = t1[0];

	// cut both lines:
	// l+x1*t1 = p+x2*t2
	// (p-l) = x1*t1 - x2*t2 ->

	// x1[0]*t1 - x2[0]*t2 = p[0]-l[0]
	// x1[1]*t1 - x2[1]*t2 = p[1]-l[1] ->

	// t1 = (p[0]-l[0])+x2[0]*t2)/x1[0] ->

	// x1[1]*((p[0]-l[0])+x2[0]*t2)/x1[0]) - x2[1]*t2 = p[1]-l[1]; ->
	// x1[1]/x1[0]*(p[0]-l[0]) + (x1[1]/x1[0]*x2[0] - x2[1])*t2 = p[1]-l[1] ->

	// t2 = (p[1]-l[1]-x1[1]/x1[0]*(p[0]-l[0]))/(x1[1]/x1[0]*x2[0]-x2[1])

	double x2 = (p[1]-l[1]-t1[1]/t1[0]*(p[0]-l[0]))/(t1[1]/t1[0]*t2[0]-t2[1]);
	r = p + t1*x2;

	//std::cout << "g: " << l << "+ x*" << t1 << " => x = " << x2 << " => r = " << r << std::endl;
	//std::cout << "|p,g| = " << (r-p).length() << std::endl;

	return Point(r[0],r[1]);
}

// Does nothing now, just set params to standard values
// alpha = 18°
// H = 37
// D = 142.5
void Calibration::preCalibrate(int max_index, int method)
{
	Interval d,t,h;
	switch( method )
	{
		case 1:
			std::cout << "Not implemented:!";
		case 0:
		default:
		/*
			d = approximateDistance(0.5, 1, max_index, true);
			std::cout << "D inside " << d << std::endl;
			m_dist = d.center();
			t = approximateTangens(0.0001, 1,  max_index, true );
			std::cout << "T inside " << t << std::endl;
		*/
//			d = Interval( calcDistOfTan(t.from(), 1, 4 ), calcDistOfTan(t.from(), 1, 4 ) );
//			m_dist = d.center();
//			m_alpha = std::atan( t.center() );

			t = Interval(0.32492,0.32492);
			m_dist  = D_MEASURE;					// measured!
			m_alpha = ALPHA_MEASURE*M_PI/180.0;		// 18° measured!
			m_H0    = H0_MEASURE;					// measured!
			break;
	}

	std::cout << "D     = " << m_dist << ", max error: " << d.length()*0.5 << std::endl;
	std::cout << "H0    = " << m_H0 << ", max error: " << h.length()*0.5 << std::endl;
	std::cout << "alpha = " << (m_alpha*180.0/M_PI) << "°, max error: "
			  << ((std::atan(t.to())*180.0/M_PI)-((std::atan(t.from())*180.0/M_PI)))*0.5 << std::endl;

	//m_laserPos.first  = -(m_dist + TABLE_RADIUS);
	//m_laserPos.second = m_H0;
	m_laserPosError.first = d.length()*0.5;
	m_laserPosError.second = h.length()*0.5;
	m_alphaError = (std::atan(t.to()) - std::atan(t.from()))*0.5;
}



void Calibration::setH0( double x )
{
	m_H0 = x;
}
void Calibration::setAlpha( double x )
{
	m_alpha = x;
}
void Calibration::setDist( double x )
{
	m_dist = x;
}

double Calibration::H( unsigned i ) const
{
	assert( i>0 );
	assert( i<8 );
	// H(i) = i*35 oder (i-1)*35 + H0
	// initial value
	double H = (i-1)*35.0;
	return H+m_H0;
}

	// Calculates Distqance for given Positions i and j
Calibration::Point Calibration::calcDistance(const int i, const int j) const
{
	assert( i!=j );
	assert( i>0 && i<8 );
	assert( j>0 && j<8 );

	double vRAD[7];
	double result[4];

	for( int idx =0; idx<7; idx++ )
		vRAD[idx] = (m_stepperPos[idx] * M_PI * 0.45)/180.0;

	// solve equation D^2 + cot(a(j)-a(i))(H(j)-(H(i)) D + H(j)H(i) = 0

	// D = -P/2 +- sqrt( P/2^2 - Q );
	// P = cot( a(j)-a(i) )*(H(j)-H(i));
	//
#ifdef _DEBUG
			std::cout << "a(i) = " << vRAD[i-1] << " ("<< m_stepperPos[i-1] << ")" << std::endl;
			std::cout << "a(j) = " << vRAD[j-1] << " ("<< m_stepperPos[j-1] << ")" << std::endl;
#endif

	double beta  = vRAD[j-1]-vRAD[i-1];
	double alpha = m_stepperPos[j-1]-m_stepperPos[i-1]; alpha *= M_PI*0.45; alpha/=180.0;

#ifdef _DEBUG
		std::cout << "alpha      = " << alpha << " (" << beta << ")" << std::endl;
#endif

	double c1 = 1.0 / tan( alpha );
	double c2 = 1.0 / tan( beta );

#ifdef _DEBUG
		std::cout << "cot(alpha) = " << c1 << " (" << c2 << ")" << std::endl;
#endif

	double P1 = c1*(H(j)-H(i));
	double P2 = c2*(H(j)-H(i));
	double Q  = H(j)*H(i);

#ifdef _DEBUG
		std::cout << "P          = " << P1 << " (" << P2 << ")" << std::endl;
		std::cout << "Q          = " << Q << std::endl;
#endif

	result[0] = -0.5*P1;
	result[1] = -0.5*P2;
	result[2] = -0.5*P1;
	result[3] = -0.5*P2;

	double root1 = sqrt( (0.25*P1*P1) - Q );
	double root2 = sqrt( (0.25*P2*P2) - Q );

	result[0] += root1;
	result[1] += root2;
	result[2] -= root1;
	result[3] -= root2;

#ifdef _DEBUG
		std::cout << "D+         = " << result[0] << " (" << result[1] << ")" << std::endl;
		std::cout << "D-         = " << result[2] << " (" << result[3] << ")" << std::endl;
#endif

	return Point(result[0],result[2]);
}

Calibration::Point Calibration::calcTangens(const int i, const int j) const
{
	assert( i!=j );
	assert( i>0 && i<8 );
	assert( j>0 && j<8 );

	double vRAD[7];
	double result[4];

	for( int idx =0; idx<7; idx++ )
		vRAD[idx] = (m_stepperPos[idx] * M_PI * 0.45)/180.0;

#ifdef _DEBUG
		std::cout << "a(i) = " << vRAD[i-1] << " ("<< m_stepperPos[i-1] << ")" << std::endl;
		std::cout << "a(j) = " << vRAD[j-1] << " ("<< m_stepperPos[j-1] << ")" << std::endl;
#endif

	double beta  = vRAD[i-1];
	double gamma = vRAD[j-1];

#ifdef _DEBUG
		std::cout << "beta      = " << beta << std::endl;
		std::cout << "gamma     = " << gamma << std::endl;
#endif

	double A = cos(gamma+beta) * (H(i)-H(j));
	double B = H(i)*sin(beta)*cos(gamma) - H(j)*cos(beta)*sin(gamma);
	double C = H(j)*cos(gamma)*sin(beta) - H(i)*sin(gamma)*cos(beta);
	double P = A/C;
	double Q = B/C;

#ifdef _DEBUG
		std::cout << "P          = " << P << std::endl;
		std::cout << "Q          = " << Q << std::endl;
#endif

	result[0] = -0.5*P;
	result[1] = -0.5*P;

	double root = sqrt( (0.25*P*P) - Q );

	result[0] += root;
	result[1] -= root;

#ifdef _DEBUG
		std::cout << "TAN+       = " << result[0] << std::endl;
		std::cout << "TAN-       = " << result[1] << std::endl;
#endif

	return Point(result[0],result[1]);
}

	// estimate an Intervall where Tanges Alpha is within
Calibration::Interval Calibration::estimateTangens(int first, int last, int min) const
{
	Interval rangeTp;
	Interval rangeTm;

	for( int i=first; i<=last; i++ )
		for( int j=first; j<=last; j++ )
		{
			if( m_stepperPos[i-1] > 0.0 &&
				m_stepperPos[j-1] > 0.0 &&
				m_stepperPosFlag[i-1] &&
				m_stepperPosFlag[j-1] )
			{
				int abs = j-i;
				if( abs < 0 ) abs*=(-1);

				if( abs >= min )
				{
					Point T = calcTangens( i, j );

					//std::cout << "TAN(" << i << ", " << j << ") = " << std::endl << T << std::endl;

					rangeTp.widen( T.first );
					rangeTm.widen( T.second );
				}
			}
		}

#ifdef _DEBUG
			std::cout << "TAN+ e " << rangeTp  << std::endl;
			std::cout << "TAN- e " << rangeTm  << std::endl;
#endif

	return rangeTp;
}

Calibration::Interval Calibration::estimateDistance(int first, int last, int min) const
{
	assert( min > 0 );

	Interval rangePlus(1000.0, -1000.0);
	Interval rangeMinus(1000.0, -1000.0);

	for( int i=first; i<=last; i++ )
	{
		for( int j=first; j<=last; j++ )
		{
			if( m_stepperPos[i-1] > 0.0 &&
				m_stepperPos[j-1] > 0.0 &&
				m_stepperPosFlag[i-1] &&
				m_stepperPosFlag[j-1] )
			{
				int abs = j-i;
				if( abs < 0 ) abs*=(-1);

				if( abs >= min )
				{
					Point D = calcDistance( i, j );
					rangePlus.widen(  D.first );
					rangeMinus.widen( D.second );
				}
			}
		}
	}
#ifdef _DEBUG
		std::cout << "D+ = " << rangePlus <<  std::endl;
		std::cout << "D- = " << rangeMinus << std::endl;
#endif

	return rangePlus;
}

	// use all possible Estimations and change H0 to get the smallest possible Interval
Calibration::Interval Calibration::approximateDistance(double error, int first, int last, int min)
{
	Interval interval;
	double epsilon = -1.0;
	interval = estimateDistance( first, last, min );
	double medError = interval.length()/2.0;
	double lastError = medError;

	std::cout << "Starting Approximation of D with error: " << medError << std::endl;

	while( medError > error )
	{
		// change H0
		m_H0+=epsilon;
		lastError = medError;
		interval = estimateDistance( first, last, min );
		medError = interval.length()/2.0;

#ifdef _DEBUG
			std::cout << "Mittlerer Fehler mit H0=" << H(1) << ": " << medError << " (" << medError-lastError << ")" << std::endl;
#endif
		// medError grows
		// so change direction and step size
		if( medError-lastError > 0.0 )
			epsilon *= -0.5;
		// medError shrinks
		// very good, continue
		else if( medError-lastError < 0.0 )
			continue;
		// lastError = medError -> Extremum
		else
			break;
	}

	return interval;
}

Calibration::Interval Calibration::approximateTangens (double error, int first, int last, int min)
{
	Interval interval;
	// initial calc
	double epsilon = -1.0;
	interval = estimateTangens( first, last, min);
	double medError = interval.length()/2.0;
	double lastError = medError;

	std::cout << "Starting Approximation of Tan with error: " << medError << std::endl;

	while( medError > error )
	{
		// change H0
		m_H0+=epsilon;

		lastError = medError;
		interval = estimateTangens( first, last, min );
		medError = interval.length()/2.0;

#ifdef _DEBUG
			std::cout << "Mittlerer Fehler mit H0=" << H(1) << ": " << medError << " (" << medError-lastError << ")" << std::endl;
#endif
		// medError grows
		// so change direction and step size
		if( medError-lastError > 0.0 )
			epsilon *= -0.5;
		// medError shrinks
		// very good, continue
		else if( medError-lastError < 0.0 )
			continue;
		// lastError = medError -> Extremum, ready
		else
			break;
	}

 	return interval;
}

double Calibration::calcDistOfTan( double tanA, int first, int last)
{
	assert( first <= last );
	assert(first>=1);
	assert(last<=7);
	double D = 0.0;

	std::cout << tanA << " -> [";
	for( int i=first; i<=last; i++ )
	{
		double rad = double(m_stepperPos[i-1])*M_PI/180.0;
		double iH  = 35*i;
		double val = iH *(sin(rad)+tanA*cos(rad));
		val /= (cos(rad)-tanA*sin(rad));
		std::cout << val << ", ";
		D +=val;
	}

	D /= double(last-first+1);

	std::cout << "] = " << D << std::endl;

	return D;
}

double Calibration::calcTanOfDist( double dist, int first, int last)
{
	// tan alpha = (D*cos a[i] - iH*sin a[i])/(D*sin a[i]+iH*cos a[i])
	assert( first <= last );
	assert(first>=1);
	assert(last<=7);

	double tanA = 0.0;

	std::cout << dist << " -> [";
	for( int i=first; i<=last; i++ )
	{
		double rad = double(m_stepperPos[i-1])*M_PI/180.0;
		double iH  = 35*i;
		double val = dist*cos(rad) - iH*sin(rad);
		val       /= dist*sin(rad) + iH*cos(rad);
		std::cout << val << ", ";
		tanA += val;
	}
	tanA /= double(last-first+1);

	std::cout << "] = " << tanA << std::endl;

	return tanA;
}


std::pair<Calibration::Interval,Calibration::Interval> Calibration::intersectDistance( int first, int last, int min)
{
	assert(first<=last);
	assert(first>=1);
	assert(last<=7);

	int i,j;

	// Calculate the Circles
	std::vector<mtl::circle<double> > circles;

	for( i=1; i<7; i++ )
	{
		double tau = 0.45*double(m_stepperPos[i-1]-m_stepperPos[i]);
		tau = 90.0 - tau;

#ifdef _DEBUG
			std::cout << "Tau = " << tau << "°, Kreis: ";
#endif

		tau *= M_PI/180.0;
		double R   = 17.5 / std::cos( tau );
		double D   = 17.5 * std::tan( tau );
		circles.push_back( mtl::circle<double>( D, H(i)+17.5, R) );

#ifdef _DEBUG
			std::cout << circles.back() << std::endl;
#endif
	}
	std::cout << std::endl;

	std::vector<Point> coords;
	// Intersect the Circles
	// NOT <= because number of circles is already one less
	for( i=first; i<last; i++ )
		for( j=first; j<last; j++ )
		{
			if( i != j )
			{
#ifdef _DEBUG
	std::cout << "intersecting Circles " << i << " and " << j << std::endl;
#endif
				std::vector<double> points = circles[i-1].intersect( circles[j-1] );

				if( points.size() >= 2 )
				{
					if( std::abs( points[0] - 150.0 ) < 50.0 )
					{
						coords.push_back( Point(points[0],points[1]) );

#ifdef _DEBUG
	std::cout << "Added points: (" << coords.back().first << ", " << coords.back().second << ")" << std::endl;
#endif
					}
				}
				if( points.size() >= 4 )
				{
					if( std::abs( points[2] - 150.0 ) < 50.0 )
					{
						coords.push_back( Point(points[2],points[3]) );

#ifdef _DEBUG
	std::cout << "Added points: (" << coords.back().first << ", " << coords.back().second << ")" << std::endl;
#endif
					}
				}

#ifdef _DEBUG
					std::cout << std::endl;
#endif
			}
		}

	double D=0.0;
	double H0=0.0;
	double N =0.0;

	Interval rangeD;
	Interval rangeH;

#ifdef _DEBUG
		std::cout << "Collected with H0= " << H(1) << std::endl;
#endif

	for( std::vector<Point>::iterator iter = coords.begin();
		 iter != coords.end();
		 ++iter )
	{
		D += iter->first;
		H0 += H(1)-iter->second;
		N += 1.0;

		rangeD.widen( iter->first );
		rangeH.widen( iter->second );

#ifdef _DEBUG
			std::cout << "(" << iter->first << ", " << iter->second << ")" << std::endl;
#endif
	}

	std::cout << "D inside " << rangeD << std::endl;
	std::cout << "H inside " << H(1)-rangeH.from() << ", " << H(1)-rangeH.to() << "]" << std::endl;

	return std::pair<Interval,Interval>( rangeD, rangeH );
}

Calibration::Interval Calibration::intersectTangens( int first, int last, int min)
{
	int i;
	double t=0.0;
	Interval rangeT;

	for( i=first; i<=last; i++ )
	{
		double v = m_dist/H(i);
		double ai = double(m_stepperPos[i-1])*0.45*M_PI/180.0;

		v = std::atan( v );
		v -= ai;
		v = std::tan(v);

		rangeT.widen( v );
		t += v;
	}
	std::cout << "Tan inside " << rangeT << std::endl;
	//t /= Double(last-first+1);
	//return t;
	return rangeT;
}

};
