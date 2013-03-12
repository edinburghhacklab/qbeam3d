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
// C++ Interface: interval
//
#ifndef _INTERVAL_H_
#define _INTERVAL_H_

#include <iostream>
#include <cmath>

namespace mtl {

template <class T>
class interval
{
public:
	typedef enum { CLOSED=0, LEFTOPEN=1, RIGHTOPEN=2, OPEN=3 } interval_type;

	interval(	T from=std::numeric_limits<T>::max(),
				T to=std::numeric_limits<T>::min(),
				interval_type type = CLOSED )
	: m_type(type), m_from(from), m_to(to)
	{}

	void clear()				{ m_from = std::numeric_limits<T>::max(); m_to = std::numeric_limits<T>::min(); }

	T from() const				{ return m_from; }
	T to() const				{ return m_to; }
	interval_type type() const	{ return m_type; }
	bool isOpen() const			{ return m_type == OPEN; }
	bool isClosed() const		{ return m_type == CLOSED; }
	bool isHalfOpen() const		{ return m_type != CLOSED && m_type != OPEN; }
	bool isOpenLeft() const		{ return m_type & LEFTOPEN != 0; }
	bool isOpenRight() const	{ return m_type & RIGHTOPEN != 0; }

	// interval is set to the range x1,x2
	void setRange( T x1, T x2 )
	{
		m_from = x1;
		m_to = x2;
		if( m_from>m_to )
			std::swap(m_from,m_to);
	}

	// widen interval with x1
	void widen( T x )
	{
		if( x < m_from ) // left of start
			m_from=x;
		if( x > m_to ) // right of end
			m_to=x;
	}
	// shrink interval by minimum possible
	void shrink( T x )
	{
		if( isInside(x) )
		{
			if( std::abs( m_from-x ) < std::abs( x-m_to ) )
				m_from = x;
			else if( std::abs( m_from-x ) < std::abs( x-m_to ) )
				m_to = x;
		}
	}

	void setType( interval_type t )	{ m_type = t; }

	bool isInside( T value ) const
	{
		switch( m_type )
		{
			case CLOSED:		return  value >= m_from && value <= m_to;
			case OPEN:			return  value > m_from && value < m_to;
			case LEFTOPEN:		return  value > m_from && value <= m_to;
			case RIGHTOPEN:		return  value >= m_from && value < m_to;
		}
		return false;
	}

	T center() const
	{
		if( m_from <= m_to )
		{
			return (m_from + m_to)/T(2);
		}
		return T(0);
	}

	T length() const
	{
		if( m_from <= m_to )
			return m_to-m_from;
		else
			return T(0);
	}

private:
	interval_type	m_type;
	T				m_from;
	T				m_to;
};

} // namespace mtl

template<class T>
std::ostream& operator<<(std::ostream& str, const mtl::interval<T>& i )
{
	if( i.isOpenLeft() ) str << "("; else str << "[";
	str << i.from() << ", " << i.to();
	if( i.isOpenRight() ) str << ")"; else str << "]";
	return str;
}

#endif
