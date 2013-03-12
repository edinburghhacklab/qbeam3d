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

#ifndef MTL_RATIONAL_H
#define MTL_RATIONAL_H

#include <iostream>

/**
	Schablone für rationale Zahlen
*/

namespace mtl { namespace number {

template <class T>
class rational
{
public:

	rational( const rational<T>& rat );
	rational( const T& _z=T(0), const T& _n=T(1) );
/*	explicit rational( int z );
	explicit rational( long z );
	explicit rational( long long z );
*/
	explicit rational( double f );
	~rational();

public:

	inline T numerator() const { return z; }
	inline T denominator() const { return n; }

//** kuerzt den Bruch nach Möglichkeit, gibt den ggT zurück
	T cancel();

	void norm();

	inline rational<T> &set( T _z, T _n );

	rational<T> &operator= ( const rational<T>& x );
	rational<T> &operator= ( const T& x );
	rational<T> &operator= ( double x );

// rational<T>
	rational<T> &operator+=( const rational<T>& x );
	rational<T> &operator-=( const rational<T>& x );
	rational<T> &operator/=( const rational<T>& x );
	rational<T> &operator*=( const rational<T>& x );
// Type
	rational<T> &operator+=( const T& x );
	rational<T> &operator-=( const T& x );
	rational<T> &operator/=( const T& x );
	rational<T> &operator*=( const T& x );
// double
	rational<T> &operator+=( double x );
	rational<T> &operator-=( double x );
	rational<T> &operator/=( double x );
	rational<T> &operator*=( double x );

	friend bool operator==<T>( const rational<T>& x, const rational<T>& y );
	friend bool operator!=<T>( const rational<T>& x, const rational<T>& y );
	friend bool operator<=<T>( const rational<T>& x, const rational<T>& y );
	friend bool operator>=<T>( const rational<T>& x, const rational<T>& y );
	friend bool operator> <T>( const rational<T>& x, const rational<T>& y );
	friend bool operator< <T>( const rational<T>& x, const rational<T>& y );

	double toDouble( double error = 0.0000000001 ) const;

	//operator char() const;
	//operator int() const;
	//operator long() const;
	//operator float() const;
	//operator double() const;

	// calculate ggT
	static T 	ggT( T u, T v );
	static void tooHuge( T z1, T z2 );

	static void setBase( unsigned b=std::numeric_limits<double>::radix ) { if( b > 0 ) m_base = b; }
	static unsigned base() { return m_base; }

	static bool printAsFloat() { return m_printAsDouble; }
	static void setPrintAsFloat( bool f ) { m_printAsDouble=f; }

private:

	T z; // zaehler
	T n; // nenner

	static unsigned m_base;
	static bool		m_printAsDouble;
};

}} // namespaces

template <class T>
std::ostream& operator <<( std::ostream& str, const mtl::number::rational<T> &x );

template <class T>
mtl::number::rational<T> operator-( const mtl::number::rational<T>& x );
template <class T>
mtl::number::rational<T> operator+( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
template <class T>
mtl::number::rational<T> operator-( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
template <class T>
mtl::number::rational<T> operator*( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
template <class T>
mtl::number::rational<T> operator/( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );

template <class T>
bool operator==( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
template <class T>
bool operator!=( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
template <class T>
bool operator<=( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
template <class T>
bool operator>=( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
template <class T>
bool operator> ( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
template <class T>
bool operator< ( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y );
// int
template <class T>
bool operator==( const mtl::number::rational<T>& x, int y );
template <class T>
bool operator!=( const mtl::number::rational<T>& x, int y );
template <class T>
bool operator<=( const mtl::number::rational<T>& x, int y );
template <class T>
bool operator>=( const mtl::number::rational<T>& x, int y );
template <class T>
bool operator> ( const mtl::number::rational<T>& x, int y );
template <class T>
bool operator< ( const mtl::number::rational<T>& x, int y );
template <class T>
bool operator==( int x, const mtl::number::rational<T>& y );
template <class T>
bool operator!=( int x, const mtl::number::rational<T>& y );
template <class T>
bool operator<=( int x, const mtl::number::rational<T>& y );
template <class T>
bool operator>=( int x, const mtl::number::rational<T>& y );
template <class T>
bool operator> ( int x, const mtl::number::rational<T>& y );
template <class T>
bool operator< ( int x, const mtl::number::rational<T>& y );

// double
template <class T>
bool operator==( const mtl::number::rational<T>& x, double y );
template <class T>
bool operator!=( const mtl::number::rational<T>& x, double y );
template <class T>
bool operator<=( const mtl::number::rational<T>& x, double y );
template <class T>
bool operator>=( const mtl::number::rational<T>& x, double y );
template <class T>
bool operator> ( const mtl::number::rational<T>& x, double y );
template <class T>
bool operator< ( const mtl::number::rational<T>& x, double y );
template <class T>
bool operator==( double x, const mtl::number::rational<T>& y );
template <class T>
bool operator!=( double x, const mtl::number::rational<T>& y );
template <class T>
bool operator<=( double x, const mtl::number::rational<T>& y );
template <class T>
bool operator>=( double x, const mtl::number::rational<T>& y );
template <class T>
bool operator> ( double x, const mtl::number::rational<T>& y );
template <class T>
bool operator< ( double x, const mtl::number::rational<T>& y );

#include "rational.cpp"

#endif
