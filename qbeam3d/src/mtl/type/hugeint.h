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
#include <string>

#ifndef MTL_HUGEINT_H
#define MTL_HUGEINT_H

/**
  */

 #include <vector>

namespace mtl {
namespace type {

template <unsigned int N>
class hugeint
{
public:
	friend bool operator== <>( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
	friend bool operator!= <>( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
	friend bool operator<= <>( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
	friend bool operator>= <>( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
	friend bool operator<  <>( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
	friend bool operator>  <>( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
	friend bool operator!  <>( const mtl::type::hugeint<N>& i );
	friend mtl::type::hugeint<N> operator- <>( const mtl::type::hugeint<N>& i );

	enum { SIZE=N, FRAGSIZE=32, BITSIZE=FRAGSIZE*SIZE }; // adjustable for template
	enum { DEC=0,HEX,BIN,OCT };

	hugeint();
	hugeint( const hugeint<N>& i );
	hugeint( unsigned int i, int idx=0 );
	explicit hugeint( unsigned int i[SIZE] );
	hugeint( signed int i );

	~hugeint();
	void clear( unsigned i=0 );

	hugeint<N>& operator=( const hugeint<N>& i );
	hugeint<N>& operator=( const unsigned int i );
	hugeint<N>& operator=( const signed int i );

	//operator double() const;

// bitwise operators
	hugeint<N>& operator|=( const hugeint<N>& i );
	hugeint<N>& operator&=( const hugeint<N>& i );
	hugeint<N>& operator^=( const hugeint<N>& i );

	hugeint<N>& operator|=( int i );
	hugeint<N>& operator&=( int i );
	hugeint<N>& operator^=( int i );

	hugeint<N>& operator>>=( unsigned int );
	hugeint<N>& operator<<=( unsigned int i );

	hugeint<N>& operator+=( const hugeint<N>& i );
	hugeint<N>& operator-=( const hugeint<N>& i );
	hugeint<N>& operator*=( const hugeint<N>& i );
	hugeint<N>& operator/=( const hugeint<N>& i );
	hugeint<N>& operator%=( const hugeint<N>& i );

	hugeint<N>& operator+=( int i );
	hugeint<N>& operator-=( int i );
	hugeint<N>& operator*=( int i );
	hugeint<N>& operator/=( int i );
	hugeint<N>& operator%=( int i );

	hugeint<N>& operator+=( unsigned int i );
	hugeint<N>& operator-=( unsigned int i );
	hugeint<N>& operator*=( unsigned int i );
	hugeint<N>& operator/=( unsigned int i );
	hugeint<N>& operator%=( unsigned int i );

	hugeint<N>& operator--();
	hugeint<N>  operator--(int);
	hugeint<N>& operator++();
	hugeint<N>  operator++(int);

	hugeint<N>& neg();

	bool sign() const;

	static std::ostream& print( std::ostream& str, const hugeint<N>& i );

	operator unsigned int() const;

	int bitsUsed() const;
	static bool overflowOnAdd( const hugeint<N>& i1, const hugeint<N>& i2 );
	static bool overflowOnMul( const hugeint<N>& i1, const hugeint<N>& i2 );

protected:
	static void div( hugeint<N>& result, hugeint<N>& i, hugeint<N>& remainder );

private:
	std::vector<unsigned int> m_data;

	// makes both numbers positive, returns bitmask, set bit for changed sign
	static int  remove_sign( hugeint<N>& a, hugeint<N>& b );
	static void restore_sign( hugeint<N>& a, hugeint<N>& b, int signmask );
};

} // namespace type

typedef type::hugeint<2> int64;
typedef type::hugeint<3> int96;
typedef type::hugeint<4> int128; // 38 decimal digits
typedef type::hugeint<5> int160; // 48 decimal digits
typedef type::hugeint<6> int192; // 57 decimal digits
typedef type::hugeint<7> int224; // 67 decimal digits
typedef type::hugeint<8> int256; // 77 decimal digits

}

template<unsigned int N>
std::ostream& operator <<( std::ostream& str, const mtl::type::hugeint<N>& i );

// comparators - native
template<unsigned int N> bool operator==( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator!=( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator<=( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator>=( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator< ( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator> ( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
// int
template<unsigned int N> bool operator==( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator!=( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator<=( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator>=( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator< ( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator> ( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator==( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> bool operator!=( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> bool operator<=( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> bool operator>=( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> bool operator< ( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> bool operator> ( const mtl::type::hugeint<N>& i, int j );
// unsigned
template<unsigned int N> bool operator==( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator!=( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator<=( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator>=( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator< ( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator> ( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> bool operator==( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> bool operator!=( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> bool operator<=( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> bool operator>=( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> bool operator< ( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> bool operator> ( const mtl::type::hugeint<N>& i, unsigned int j );

// arithmetic
template<unsigned int N> mtl::type::hugeint<N> operator+( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator-( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator*( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator/( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator%( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );

template<unsigned int N> mtl::type::hugeint<N> operator+( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> mtl::type::hugeint<N> operator-( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> mtl::type::hugeint<N> operator*( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> mtl::type::hugeint<N> operator/( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> mtl::type::hugeint<N> operator%( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> mtl::type::hugeint<N> operator+( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator-( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator*( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator/( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator%( int i, const mtl::type::hugeint<N>& j );

template<unsigned int N> mtl::type::hugeint<N> operator+( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> mtl::type::hugeint<N> operator-( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> mtl::type::hugeint<N> operator*( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> mtl::type::hugeint<N> operator/( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> mtl::type::hugeint<N> operator%( const mtl::type::hugeint<N>& i, unsigned int j );
template<unsigned int N> mtl::type::hugeint<N> operator+( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator-( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator*( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator/( unsigned int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator%( unsigned int i, const mtl::type::hugeint<N>& j );

template<unsigned int N> mtl::type::hugeint<N> operator|( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator&( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator^( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator|( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> mtl::type::hugeint<N> operator&( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> mtl::type::hugeint<N> operator^( const mtl::type::hugeint<N>& i, int j );
template<unsigned int N> mtl::type::hugeint<N> operator|( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator&( int i, const mtl::type::hugeint<N>& j );
template<unsigned int N> mtl::type::hugeint<N> operator^( int i, const mtl::type::hugeint<N>& j );

template<unsigned int N> mtl::type::hugeint<N>  operator-( const mtl::type::hugeint<N>& i );
template<unsigned int N> bool operator!( const mtl::type::hugeint<N>& i );

template<unsigned int N> mtl::type::hugeint<N> operator~( const mtl::type::hugeint<N>& x );
template<unsigned int N> mtl::type::hugeint<N> operator>>( const mtl::type::hugeint<N>& x, unsigned int i );
template<unsigned int N> mtl::type::hugeint<N> operator<<( const mtl::type::hugeint<N>& x, unsigned int i );

namespace std {

template<unsigned int N>
class numeric_limits< mtl::type::hugeint<N> >
{
public:
	static const bool is_specialized 	= true;
	static const int digits 			= N*sizeof(unsigned int)*8-1;
	static const bool is_signed 		= true;
	static const bool is_integer		= true;
};

}

#include "hugeint.cpp"

#endif

