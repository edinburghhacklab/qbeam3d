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

#include "rational.h"
#include "../algorithm.h"

#include <iostream>


namespace mtl {
namespace number {

export template<class T>
unsigned rational<T>::m_base = std::numeric_limits<double>::radix;

export template<class T>
bool     rational<T>::m_printAsDouble = false;

export template<class T>
rational<T>::rational( const T& _z, const T& _n ) :
	z(_z),
	n(_n)
{
	assert( n != 0 );
	cancel();
}

export template<class T>
rational<T>::rational( const rational<T>& rat ) :
	z(rat.z),
	n(rat.n)
{}

export template<class T>
rational<T>::rational( double f ) :
	z(0),
	n(1)
{
	assert( m_base > 0 );

	double base_f = double(m_base);
	T      base_t = T(m_base);

	//std::cout << "rational(double): " << m_base << std::endl;
	bool sign = false;
	if( f < 0.0 )	{ f = -f; sign = true; }

	//std::cout << "start: " << (sign?"-":"+") << f << ", " << z << "/" << n << std::endl;

	double e = 0.00000001;
	do
	{
		if( f >= 1.0 )
		{
			unsigned dum = static_cast<unsigned>(std::floor(f));
			f -= std::floor(f);
			z += dum;
			//std::cout << "-= " << dum << std::endl;
		}
		else
		{
			f*= base_f;
			e*= base_f;
			z *= base_t;
			n *= base_t;

			//std::cout << "*= " << base_t << " -> " << z << ", " <<  n << std::endl;
		}
		//std::cout << f << ", " << e << " -> " << (*this) << std::endl;
	}
	while( f > e );

	if( sign )
		z = -z;

	//std::cout << "before cancel: " << (*this) << std::endl;
	cancel();
	//std::cout << "after  cancel: " << (*this) << std::endl;
}

export template<class T>
rational<T>::~rational()
{}

/*
export template<class T>
rational<T>::rational( const T& x ) :
	z(x),
	n(1)
{}
*/
/*
export template<class T>
rational<T>::rational( int z ) :
	z(x),
	n(1)
{}

export template<class T>
rational<T>::rational( long z ) :
	z(x),
	n(1)
{}

export template<class T>
rational<T>::rational( long long z ) :
	z(x),
	n(1)
{}
*/

export template<class T>
void rational<T>::norm()
{
	assert( n != 0 );
	if( n < 0 )
	{
		z = -z;
		n = -n;
	}
}

export template<class T>
void rational<T>::tooHuge( T z1, T z2 )
{
	int s = sizeof(T)*8;
	T b = 1;
	int hb1 = 0;
	int hb2 = 0;
	if( z1 < 0 ) z1*=-1;
	if( z2 < 0 ) z2*=-1;

	for( int i=0; i<s; i++ ) {
		if( z1 & b ) hb1 = i;
		if( z2 & b ) hb2 = i;
		b <<= 1;
	}
	if( (hb1+hb2) >= s ) {
		cerr << endl << "Zahlen zu groß für Produkt: " << z1 << ", " << z2 << " (" << hb1+hb2 << ")" << endl;
	}
}

//** kuerzt den Bruch nach Möglichkeit, gibt den ggT zurück
export template<class T>
T rational<T>::cancel()
{
	if( n != 1 )
	{
	//	std::cout << "Cancel: " << z << "/" << n << std::endl;
		norm();
		// Euklid...
		T ggTeiler = mtl::gcd( z,n );

	//	std:: cout << "ccd = " << ggTeiler << std::endl;

		if( ggTeiler < 0 )
			ggTeiler *= -1;
		if( ggTeiler != 1 ) {
			z /= ggTeiler;
			n /= ggTeiler;
		}
		//   // nach canceln runde! wenn Zahlen zu groß
		//   if( n > 16777216 ) // 2^24
		//   {
		//     cout << "RUNDE: " << z << "/" << n << " = " << toDouble() << endl;
		//     if( z >= 1048576 || z<= 1048576 ) // 2^20
		//     {
		//      z>>=4;
		//      n>>=4;
		//     }
		//     cout << "-->: " <<  z << "/" << n << " = " << toDouble() << endl;
		//   }
		return ggTeiler;
	}
	return 0;
}

export template<class T>
rational<T>& rational<T>::set( T _z, T _n )
{
	z = _z;
	n = _n;
	norm();
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator=( const rational<T>& x )
{
	z = x.z;
	n = x.n;
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator=( const T& x )
{
	z = x;
	n = 1;
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator=( double x )
{
	*this = rational<T>( x );
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator+=( const rational<T>& x )
{
	if( n == x.n )
	{
		z += x.z;
	}
	else
	{
		z *= x.n;
		z += x.z * n;
		n *= x.n;
	}
	cancel();
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator-=( const rational<T>& x )
{
	if( n == x.n )
	{
		z -= x.z;
	}
	else
	{
		z *= x.n;
		z -= x.z * n;
		n *= x.n;
	}
	cancel();
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator/=( const rational<T>& x )
{
	z *= x.n;
	n *= x.z;
	cancel();
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator*=( const rational<T>& x )
{
	z *= x.z;
	n *= x.n;
	cancel();
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator+=( const T& x )
{
	z += n * x;
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator-=( const T& x )
{
	z -= n * x;
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator/=( const T& x )
{
	assert( x != 0 );
	n *= x;
	cancel();
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator*=( const T& x )
{
	z *= x;
	cancel();
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator+=( double x )
{
	rational<T> r(x);
	this->operator+=(r);
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator-=( double x )
{
	rational<T> r(x);
	this->operator-=(r);
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator/=( double x )
{
	rational<T> r(x);
	this->operator/=(r);
	return *this;
}

export template<class T>
rational<T>& rational<T>::operator*=( double x )
{
	rational<T> r(x);
	this->operator*=(r);
	return *this;
}
/*
export template<class T>
rational<T>::operator double() const
{
	rational<T> c(*this);
	double result;
	c.cancel();
	bool sign = false;
	T r,d;
	d = c.z/c.n; // whole number part
	r = c.z%c.n; // fractional part
	if( c.z < 0 ) {
		sign = true;
		d *= -1;
	}
	if( r < 0 ) {
		r *= -1;
	}
	if( c.n < 0 ) {
		c.n *= -1;
	}
	result = double(d);
	double add = 0.5;
	int accuracy = 20;
	while( r!=0 && accuracy>0 ) {
		r <<= 1;
		if( r >= n ) {
			r -= n;
			result += add;
		}
		accuracy--;
		add *= 0.5;
	}
	if( sign )
		result = -result;
	return result;
}
*/

export template<class T>
double rational<T>::toDouble( double error ) const
{
	double  result = 0.0;
	double	add = 1.0;
	T		part = z / n;
	T		remainder = z % n;
	// only works for positive numbers, (part,remainder):
	// (+,+) -> (+,+) -> +
	// (+,-) -> (-,+) -> -
	// (-,+) -> (-,-) -> -
	// (-,-) -> (+,-) -> +
	bool sign = ( z<0 || n<0 ) ? true:false;
	if( part < 0 )
	{ part = -part; }
	if( remainder < 0 )
	{ remainder = -remainder; }

	unsigned whole = part;

	while( remainder != 0 && add > error )
	{
		while( remainder < n )
		{
			remainder <<= 1;
			add /= 2.0;
		}
		part = remainder / n;
		remainder %= n;
		unsigned d = unsigned(part);
		result += add * double(d);
	}


	result += whole;
	if( sign )
		result = -result;
	return result;
}

}
} // namespace mtl

export template<class T>
std::ostream& operator<<( std::ostream& str, const mtl::number::rational<T>& x )
{
	if( mtl::number::rational<T>::printAsFloat() )
	{
		using namespace std;
		ios_base::fmtflags old = str.flags();
		ios_base::fmtflags newf = old;
		newf |= ios_base::fixed;

		mtl::number::rational<T> y(x);
		bool sign = y.numerator() < 0;
		if( sign )
			y = -y;

		T whole = y.numerator() / y.denominator();
		y -= whole;
		y += whole % 10;
		whole /= 10;

		if( sign )
			str << "-";
		if( whole > 0 )
			str << whole;
		//str.precision(5);
		str.flags(newf);
		str << y.toDouble();
		str.flags(old);
	}
	else
	{
		str << x.numerator();
		if( x.denominator() != 1 ) str << "/" << x.denominator();
	}
	return str;
}

export template<class T>
mtl::number::rational<T> operator-( const mtl::number::rational<T>& x )
{
	mtl::number::rational<T> r(0);
	r-= x;
	return r;
}

export template<class T>
mtl::number::rational<T> operator+( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	mtl::number::rational<T> r(x);
	r+=y;
	return r;
}

export template<class T>
mtl::number::rational<T> operator-( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	mtl::number::rational<T> r(x);
	r-=y;
	return r;
}

export template<class T>
mtl::number::rational<T> operator*( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	mtl::number::rational<T> r(x);
	r*=y;
	return r;
}

export template<class T>
mtl::number::rational<T> operator/( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	mtl::number::rational<T> r(x);
	r/=y;
	return r;
}

export template<class T>
bool operator==( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	return x.z*y.n == y.z*x.n;
}

export template<class T>
bool operator!=( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	return !(x==y);
}

export template<class T>
bool operator<=( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	return x.z*y.n <= y.z*x.n;
}

export template<class T>
bool operator>=( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	return x.z*y.n >= y.z*x.n;
}

export template<class T>
bool operator>( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	return x.z*y.n > y.z*x.n;
}

export template<class T>
bool operator<( const mtl::number::rational<T>& x, const mtl::number::rational<T>& y )
{
	return x.z*y.n < y.z*x.n;
}

// int
template <class T>
bool operator==( const mtl::number::rational<T>& x, int y )
{
	return x.denominator()*x.numerator() == x.denominator()*y;
}
template <class T>
bool operator!=( const mtl::number::rational<T>& x, int y )
{
	return !( x == y );
}
template <class T>
bool operator<=( const mtl::number::rational<T>& x, int y )
{
	T r(y);
	return x <= r;
}
template <class T>
bool operator>=( const mtl::number::rational<T>& x, int y )
{
	T r(y);
	return x >= r;
}
template <class T>
bool operator> ( const mtl::number::rational<T>& x, int y )
{
	T r(y);
	return x > r;
}
template <class T>
bool operator< ( const mtl::number::rational<T>& x, int y )
{
	T r(y);
	return x < r;
}

template <class T>
bool operator==( int x, const mtl::number::rational<T>& y ) { return y == x; }
template <class T>
bool operator!=( int x, const mtl::number::rational<T>& y ) { return y != x; }
template <class T>
bool operator<=( int x, const mtl::number::rational<T>& y ) { return  y > x; }
template <class T>
bool operator>=( int x, const mtl::number::rational<T>& y ) { return y < x; }
template <class T>
bool operator> ( int x, const mtl::number::rational<T>& y ) { return y <= x; }
template <class T>
bool operator< ( int x, const mtl::number::rational<T>& y ) { return y >= x; }

// double
template <class T>
bool operator==( const mtl::number::rational<T>& x, double y ) { mtl::number::rational<T> r(y);	return x==r; }
template <class T>
bool operator!=( const mtl::number::rational<T>& x, double y )	{ mtl::number::rational<T> r(y); return x!=r; }
template <class T>
bool operator<=( const mtl::number::rational<T>& x, double y )	{ mtl::number::rational<T> r(y); return x<=r; }
template <class T>
bool operator>=( const mtl::number::rational<T>& x, double y )	{ mtl::number::rational<T> r(y); return x>=r; }
template <class T>
bool operator> ( const mtl::number::rational<T>& x, double y )	{ mtl::number::rational<T> r(y); return x>r; }
template <class T>
bool operator< ( const mtl::number::rational<T>& x, double y )	{ mtl::number::rational<T> r(y); return x>r; }

template <class T>
bool operator==( double x, const mtl::number::rational<T>& y ){ return y==x; }
template <class T>
bool operator!=( double x, const mtl::number::rational<T>& y ){ return y!=x; }
template <class T>
bool operator<=( double x, const mtl::number::rational<T>& y ){ return y>x; }
template <class T>
bool operator>=( double x, const mtl::number::rational<T>& y ){ return y<x; }
template <class T>
bool operator> ( double x, const mtl::number::rational<T>& y ){ return y<=x; }
template <class T>
bool operator< ( double x, const mtl::number::rational<T>& y ){ return y>=x; }



