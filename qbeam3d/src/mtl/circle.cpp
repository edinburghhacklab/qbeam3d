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
// C++ Implementation: circle
//
// Description:
//
//

//#include "Circle.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <cassert>

namespace mtl {

template<class T>
circle<T>::circle(T x, T y, T radius)
: m_x(x),m_y(y),m_radius(radius)
{
}

template<class T>
bool circle<T>::operator==(const circle& c ) const
{
	return m_x==c.m_x && m_y==c.m_y && m_radius==c.m_radius;
}

template<class T>
bool circle<T>::operator<(const circle& c ) const
{
	return m_radius < c.m_radius;
}

template<class T>
bool circle<T>::operator>(const circle& c ) const
{
	return m_radius > c.m_radius;
}

template<class T>
bool circle<T>::operator<=(const circle& c ) const
{
	return m_radius <= c.m_radius;
}

template<class T>
bool circle<T>::operator>=(const circle& c ) const
{
	return m_radius >= c.m_radius;
}

template<class T>
bool circle<T>::operator!=(const circle& c ) const
{
	return !operator==(c);
}

template<class T>
std::vector<T> circle<T>::intersect(const circle& c) const
{
	//std::cout << *this << " ~ " << c << std::endl;
	return intersectCircles(m_x, m_y, m_radius,
							c.m_x, c.m_y, c.m_radius);
}

// zwei Kreise schneiden
template<class T>
std::vector<T> circle<T>::intersectCircles( T x0, T y0, T r0, T x1, T y1, T r1 )
{
	assert( r0>0.0 && r1>0.0 );

	bool flip = false;

	std::vector<T> result(0);

	// Spezialfälle abfangen, gegebenenfalls Koordinatensystem drehen
	// um höhere Genauigkeit zu erlangen
	if( y0 == y1 )
	//if( y0 == y1 )
	{
		//std::cout << "Flip!" << std::endl;
		flip = true;
		if( x0 == x1 )
		{
			/*
			if( r0 == r1 )
				std::cout << "Circles are equal -> unlimited intersection Points" << std::endl;
			else
				std::cout << "No intersection, same centers, different diameters" << std::endl;

			std::cout << "[" << result.size() << "]" << std::endl;
			*/
			return result;
		}
		std::swap( y0, x0 );
		std::swap( y1, x1 );
	}

	T A = (x0-x1)/(y1-y0);
	T B = (y1*y1-y0*y0)+(x1*x1-x0*x0)+(r0*r0-r1*r1);
		   B/= (y1-y0);
	T C = 0.5*B - y0;

	T P = (A*C - x0)/(A*A + 1.0);
	T Q = (C*C + x0*x0 - r0*r0)/(A*A + 1.0);

	T X1 = -P;
	T X2 = -P;
	T root = P*P - Q;

	//std::cout << "A, B, C = " << A << ", " << B << ", " << C << std::endl;
	//std::cout << "P, Q, r = " << P << ", " << Q << ", " << root << std::endl;

	if( root >= 0.0 )
	{
		root = sqrt( root );
		X1 += root;
		X2 -= root;
		result.push_back( X1 );
		//result.push_back( 0.5*((sqrt(r0*r0-((X1-x0)*(X1-x0)))+y0)+(sqrt(r1*r1-((X1-x1)*(X1-x1)))+y1)) );
		result.push_back( y0-sqrt(r0*r0-((X1-x0)*(X1-x0))) );

		if( X1 != X2 )
		{
			result.push_back( X2 );
			//result.push_back( 0.5*((sqrt(r0*r0-((X2-x0)*(X2-x0)))+y0)+(sqrt(r1*r1-((X2-x1)*(X2-x1)))+y1)) );
			result.push_back( y0+sqrt(r0*r0-((X1-x0)*(X1-x0))) );
		}
	}

	if( flip )
	{
		if( result.size() >= 2 )
			std::swap( result[0], result[1] );
		if( result.size() >= 4 )
			std::swap( result[2], result[3] );
	}

	/*
	if( result.size() >= 2 )
		std::cout << "Intersection 1 at: (" << result[0] << ", " << result[1] << ")" << std::endl;
	if( result.size() >= 4 )
		std::cout << "Intersection 2 at: (" << result[2] << ", " << result[3] << ")" << std::endl;
	if( result.empty() )
		std::cout << "No intersection." << std::endl;
	std::cout << "[" << result.size() << "]" << std::endl;
	*/

	return result;
}
}; //namespace

template<class T>
std::ostream& operator<<(std::ostream& out, const mtl::circle<T>& c)
{
	out << "[" << c.x() << ", " << c.y() << "] R " << c.radius();
	return out;
}


