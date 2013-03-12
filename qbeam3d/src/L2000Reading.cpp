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

#include "L2000Reading.h"
#include "L2000Calibration.h"

namespace L2000
{

Reading::out_of_range::out_of_range( mtl::interval<int>& ccd1, mtl::interval<int>& ccd2 ) :
	m_ccd1(ccd1),
	m_ccd2(ccd2)
{}

bool Reading::out_of_range::operator()( const Reading& r )
{
	return	m_ccd1.isInside(r.ccd1) &&
		m_ccd2.isInside(r.ccd2);
}


Vertex::Vertex( const Calibration& calibrationData, const Reading& r, int ccdMask, bool correct, const Reading& lastR ) :
	z(0.0),
	y(0.0),
	laserPos( r.alpha ),
	ccd1(r.ccd1),
	ccd2(r.ccd2)
{
	Calibration::Point p1(0,0),p2(0,0);
	bool f1=false,f2=false;
	// get 1st point
	if( r.ccd1 != 0 && (ccdMask&1) ) {
		p1 = calibrationData.transform( r, ccdMask );
		f1=true;
		p2 = p1;
	}
	// get 2nd point
	if( r.ccd2 != 0 && (ccdMask&2) ) {
		p2 = calibrationData.transform( r, ccdMask );
		f2=true;
		if( !f1 )
			p1=p2;
	}

	if( correct && f1 && f2 )
	{
		// wenn |p1,p2| > N mm -> Fehler
		double d = std::sqrt((p1.first-p2.first)*(p1.first-p2.first) + (p1.second-p2.second)*(p1.second-p2.second));
		if( d > 5.0 ) {
			std::cerr << "INBOUND Reading Error: d = " << d << " -> Using ccd1 for now!" << std::endl;
			//p2 = p1;
			// howto correct?
		}
	}
	z = 0.5*(p1.first+p2.first);
	y = 0.5*(p1.second+p2.second);
}

Vertex::Vertex( double _z, double _y, int l, int c1, int c2 ) :
	z(_z),
	y(_y),
	laserPos(l),
	ccd1(c1),
	ccd2(c2)
{}


}
