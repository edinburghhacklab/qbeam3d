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

#ifndef _L2000READING_H_
#define _L2000READING_H_

#include <mtl/interval.h>

namespace L2000
{

class Calibration;

// Raw single Reading
struct Reading
{
	struct out_of_range
	{
		out_of_range( mtl::interval<int>& ccd1, mtl::interval<int>& ccd2 );
		bool operator()( const Reading& r );

		mtl::interval<int> m_ccd1;
		mtl::interval<int> m_ccd2;
	};

	Reading( int _a=0, int _i=0, int _c1=0, int _c2=0 ) :
		alpha(_a),i(_i),ccd1(_c1),ccd2(_c2)
	{}


	bool valid() const		{ return ccd1 != 0 || ccd2 != 0; }
	bool invalid() const	{ return !valid(); }

	int alpha;
	int i;
	int ccd1;
	int ccd2;
};

// laser plane point with stepper info
struct Vertex
{
	Vertex( const Calibration& calibrationData, const Reading& r, int ccdMask, bool correct, const Reading& lastR );
	Vertex( double _z, double _y, int l, int c1, int c2 );

	bool	valid() const		{ return ccd1 != 0 || ccd2 != 0; }
	bool	invalid() const		{ return !valid(); }

	double	z;
	double	y;
	int		laserPos;
	int		ccd1;
	int		ccd2;
};

}

#endif
