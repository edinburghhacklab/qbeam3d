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
// C++ Interface: circle
//

#ifndef MTL_CIRCLE_H_
#define MTL_CIRCLE_H_

#include <vector>
#include <iostream>

namespace mtl {

template<class T>
class circle
{
public:

	circle(T x=0.0, T y=0.0, T radius=1.0);

	//compiler does it well:
	//~circle();
	//circle(const circle& c);
	//circle& operator=(const circle& c );

	bool    operator==(const circle& c ) const;
	bool    operator<(const circle& c ) const;
	bool    operator>(const circle& c ) const;
	bool    operator<=(const circle& c ) const;
	bool    operator>=(const circle& c ) const;
	bool    operator!=(const circle& c ) const;

	std::vector<T> intersect(const circle& c) const;

	T x() const 			{ return m_x; }
	T y() const 			{ return m_y; }
	T radius() const 		{ return m_radius; }

	void setX(T p)			{ m_x=p; }
	void setY(T p)			{ m_y=p; }
	void setRadius(T p)	{ m_radius=p; }

protected:

	T m_x;
	T m_y;
	T m_radius;

private:
	static std::vector<T> intersectCircles( T x0, T y0, T r0, T x1, T y1, T r1 );

};

}; // namespace mtl

template<class T>
std::ostream& operator<<(std::ostream&, const mtl::circle<T>& c);

#include "circle.cpp"

#endif
