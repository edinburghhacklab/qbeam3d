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
// C++ Implementation: hugeint<N>::hugeint
//

#include <iostream>
#include <iomanip>
#include <string>
#include "hugeint.h"

namespace mtl {
namespace type {

template<unsigned int N>
hugeint<N>::hugeint() :
	m_data(N,0)
{
	assert( m_data.size() == N );
}

template<unsigned int N>
hugeint<N>::hugeint( const hugeint<N>& i ) :
	m_data(i.m_data)
{
	assert( m_data.size() == N );
}

template<unsigned int N>
hugeint<N>::hugeint( unsigned int i, int idx ) :
	m_data(N,0)
{
	m_data[idx] = i;
	assert( m_data.size() == N );
}

template<unsigned int N>
hugeint<N>::hugeint( unsigned int i[N] ) :
	m_data(N,0)
{
	for( unsigned j=0; j<N; j++ )
		m_data[j] = i[j];
	assert( m_data.size() == N );
}

template<unsigned int N>
hugeint<N>::hugeint( signed int i ) :
	m_data(N,0)
{
 if( i < 0 )
 {
 	i = 0-i;
	m_data[0] = i;
	neg();
 }
 else
 {
	m_data[0]=i;
 }
	assert( m_data.size() == N );
}

template<unsigned int N>
hugeint<N>::~hugeint()
{
	assert( m_data.size() == N );
}

template<unsigned int N>
void hugeint<N>::clear(unsigned int i)
{
	std::fill( m_data.begin(),m_data.end(), i);
	assert( m_data.size() == N );
}

template<unsigned int N>
hugeint<N>& hugeint<N>::neg()
{
	for( unsigned j=0; j<N; j++ )
		m_data[j] = ~m_data[j];
	++(*this);
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
bool hugeint<N>::overflowOnAdd( const hugeint<N>& i1, const hugeint<N>& i2 )
{
	return (std::max( i1.bitsUsed(), i2.bitsUsed() )+1)>=(N*8*sizeof(unsigned int));
}

template<unsigned int N>
bool hugeint<N>::overflowOnMul( const hugeint<N>& i1, const hugeint<N>& i2 )
{
	bool of = (i1.bitsUsed() + i2.bitsUsed()) > (N*8*sizeof(unsigned int));
	if( of )
	{
		std::cerr << "posibble Overflow: " << i1 << " * " << i2;
		std::cerr << " (" << i1.bitsUsed()+i2.bitsUsed() << ")" << std::endl;


	}
	return of;
}

template<unsigned int N>
int hugeint<N>::bitsUsed() const
{
	hugeint<N> copy(*this);
	if( sign() )
		copy.neg();
	unsigned int bits = N*8*sizeof(unsigned int);

	for( int i=N-1; i>=0; i-- )
	{
		bits-=sizeof(unsigned int)*8;
		if( copy.m_data[i] != 0 ){
			int r			 = sizeof(unsigned int)*8;
			unsigned int bit = 1 << (r-1);
			for( ; r >= 0; r-- ) {
				if( copy.m_data[i] & bit )
				{
					bits += r;
					return bits;
				}
				bit >>= 1;
			}

		}

	}
	return bits;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator=( const hugeint<N>& i )
{
	if( this != &i )
		m_data = i.m_data;
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator=( const unsigned int i )
{
	clear(0);
	m_data[0] = i;
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator=( const signed int i )
{
	if( i < 0 )
		clear( 0xFFFFFFFF );
	else
		clear();
	m_data[0] = i;
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator|=( const hugeint<N>& i )
{
	for( unsigned j=0; j<SIZE; j++ )
		m_data[j] |= i.m_data[j];
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator&=( const hugeint<N>& i )
{
	for( unsigned j=0; j<SIZE; j++ )
		m_data[j] &= i.m_data[j];
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator^=( const hugeint<N>& i )
{
	for( unsigned j=0; j<SIZE; j++ )
		m_data[j] ^= i.m_data[j];
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator|=( int i )
{
	m_data[0] |= i;
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator&=( int i )
{
	m_data[0] &= i;
	for( unsigned j=1; j<SIZE; j++ )
		m_data[j] = 0;
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator^=( int i )
{
	m_data[0] ^= i;
	for( unsigned j=1; j<SIZE; j++ )
		m_data[j] ^= 0;
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator>>=( unsigned int s )
{
	unsigned int skip = s/FRAGSIZE;
	s %= FRAGSIZE;
	unsigned j=0;
	bool sf = sign();

	if( (j+skip) < N )
	{
		m_data[j]   = m_data[j+skip] >> s;
		j++;

		while( (j+skip) < N )
		{
			if( s > 0 )
				m_data[j-1]|= m_data[j+skip] << (FRAGSIZE-s);
			m_data[j]   = m_data[j+skip] >> s;
			j++;
		}
		while( j<N )
		{
			m_data[j] = 0;
			j++;
		}
	}
	// if it was signed, me must extend the sign bit
	if( sf )
	{	// upper byte
		for( j=0; j<skip; j++ )
			m_data[N-j-1] = 0xFFFFFFFF;
		if( s > 0 )
			m_data[N-j-1] |= 0xFFFFFFFF<<(FRAGSIZE-s);
	}
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator<<=( unsigned int s )
{
	int skip = s/FRAGSIZE;
	s %= FRAGSIZE;
	int j=N-1;
	using namespace std;
	if( j >= skip )
	{
		m_data[j]   = m_data[j-skip] << s;
		j--;
		while( j>=skip )
		{
			if( s > 0 )
				m_data[j+1] |= m_data[j-skip] >> (FRAGSIZE-s);
			m_data[j]    = m_data[j-skip] << s;
			j--;
		}
		while( j>=0 )
		{
			m_data[j] = 0;
			j--;
		}
	}
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator+=( const hugeint<N>& i )
{
	//assert( !overflowOnAdd( *this, i ) );
	unsigned int carrybit = 0;
	for( unsigned j=0; j<SIZE; j++ )
	{
		// add carry from last loop
		unsigned int CF = 0;
		// WOW -> overflow if: c = a+b; if(c<a)
		m_data[j] += i.m_data[j];
		if( m_data[j] < i.m_data[j] )
			CF++;
		m_data[j] += carrybit; // add carry from last round
		carrybit = (m_data[j]<carrybit) ? CF+1:CF; // check Overflow and set carry
	}
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator-=( const hugeint<N>& i )
{
	// a - b = a + (-b)
	hugeint<N> b(i);
	b.neg();
	this->operator+=(b);
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator*=( const hugeint<N>& x )
{
	assert( !overflowOnMul( *this, x ) );

	hugeint<N> r(*this);
	hugeint<N> i(x);
	clear(0);
	int signmask = remove_sign( r, i );

/*
	hugeint<N> bitmask(1);
	for( unsigned bitpos=0; bitpos<BITSIZE; bitpos++ )
	{
		if( (r & bitmask) != 0 )
			this->operator+=(i<<bitpos);
		bitmask <<= 1;
	}
*/

	unsigned byteoff = 0;
	for( unsigned j=0; j<N; j++ )
	{
		if( r.m_data[j] != 0 ) // speedup!
		{
			unsigned bitmask = 1;
			for( unsigned bitoff=0; bitoff < sizeof(unsigned)*8; bitoff++ )
			{
				if( r.m_data[j] & bitmask ) {
					this->operator+=(i<<(byteoff+bitoff));
				}
				bitmask <<= 1;
			}
		}
		byteoff += sizeof(unsigned int)*8;
	}

//	restore_sign( r, i, signmask );
	// adjust sign of result
	// 1,2 -> -
	// 0,3 -> +
	if( signmask > 0 && signmask < 3 )
		neg();

	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator/=( const hugeint<N>& x )
{
	// check div by zero
	assert( x != 0 );
	hugeint<N> i(x);
	hugeint<N> remainder(0); // holds remainder at the end
	// save and clear signs
	int signmask = remove_sign( *this, i );
	// result = result / i;
	// remainder = result % i;
	div( *this, i, remainder );
// check and set correct sign
	if( signmask == 1 || signmask == 2 ) // only one operand was < 0
		neg();
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator%=( const hugeint<N>& x )
{
	assert( x != 0 );
	hugeint<N> result(*this);// holds quotient at the end
	hugeint<N> i(x);
	clear();
	// save and clear signs
	unsigned signmask = remove_sign( result, i );
	// result = result / i;
	// remainder = result % i;
	div( result, i, *this );
// check and set correct sign
	if( signmask & 1 ) // quotient was < 0
		neg();
	assert( m_data.size() == N );
	return *this;
}

//**********************************************************
template<unsigned int N>
hugeint<N>& hugeint<N>::operator+=( int i )
{
	assert( !overflowOnAdd( *this, hugeint<N>(i) ) );

	//std::cout << "operator+=(int);";
	unsigned int carrybit = 0;
	// WOW -> overflow if: c = a+b; if(c<a)
	unsigned CF = 0;
	m_data[0] += i;
	if( m_data[0] < i )
		CF++;
	carrybit = (m_data[0]<0) ? CF+1:CF; // check Overflow and set carry
	if( carrybit != 0 )
	{
		unsigned int mask = (i<0)? -1:0;
		for( unsigned j=1; j<SIZE; j++ )
		{
			CF = 0;
			m_data[j] += mask;
			if( m_data[j] < mask )
				CF++;
			m_data[j] += carrybit; // add carry from last round
			carrybit = (m_data[j]<carrybit) ? CF+1:CF; // check Overflow and set carry
		}
	}
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator-=( int i )
{
	// a - b = a + (-b)
	this->operator+=(-i);
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator*=( int x )
{
	hugeint<N> y(x);
	this->operator*=( y );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator/=( int x )
{
	hugeint<N> y(x);
	this->operator/=( y );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator%=( int x )
{
	hugeint<N> y(x);
	this->operator%=( y );
	return *this;
}

//**********************************************************
template<unsigned int N>
hugeint<N>& hugeint<N>::operator+=( unsigned int i )
{
	assert( !overflowOnAdd( *this, hugeint<N>(i) ) );

	//std::cout << "operator+=(unsigned int):";
	// WOW -> overflow if: c = a+b; if(c<a)
	m_data[0] += i;
	if( m_data[0] < i )
	{
		for( unsigned j=1; j<SIZE; j++ )
		{
			m_data[j]++; // add carry from last round
			if( m_data[j]>0 )
				break;
		}
	}
	assert( m_data.size() == N );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator-=( unsigned int i )
{
	hugeint<N> r(i);
	this->operator-=(r);
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator*=( unsigned int x )
{
	hugeint<N> y(x);
	this->operator*=( y );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator/=( unsigned int x )
{
	hugeint<N> y(x);
	this->operator/=( y );
	return *this;
}

template<unsigned int N>
hugeint<N>& hugeint<N>::operator%=( unsigned int x )
{
	hugeint<N> y(x);
	this->operator%=( y );
	return *this;
}


// --i
template<unsigned int N>
hugeint<N>& hugeint<N>::operator--()
{
	// a-1 = --(a-1) = -(-a+1)
	neg();
	this->operator++();
	neg();
	assert( m_data.size() == N );
	return *this;
}

// i--
template<unsigned int N>
hugeint<N> hugeint<N>::operator--(int)
{
	hugeint<N> r(*this);
	neg();
	this->operator++();
	neg();
	assert( m_data.size() == N );
	return r;
}

// ++i

template<unsigned int N>
hugeint<N>& hugeint<N>::operator++()
{
	for( unsigned j=0; j<SIZE; j++ ) {
		m_data[j]++;
		if( m_data[j] >= 1 )
			break;
	}
	assert( m_data.size() == N );
	return *this;
}

// i++
template<unsigned int N>
hugeint<N> hugeint<N>::operator++(int)
{
	hugeint<N> r(*this);
	this->operator++();
	assert( m_data.size() == N );
	return r;
}


template<unsigned int N>
bool hugeint<N>::sign() const
{
	assert( m_data.size() == N );
	return m_data.back()&(1<<(FRAGSIZE-1)); // returns sign bit
}

template<unsigned int N>
std::ostream& hugeint<N>::print( std::ostream& str, const hugeint<N>& i )
{
	static char abcd[17]="0123456789ABCDEF";
	int j=0;
	std::ios_base::fmtflags f = str.flags();

	if( f & std::ios_base::hex )
	{
		for( j=SIZE-1; j>=0; j-- )
		{
			for(int  k=0; k<FRAGSIZE; k+= 4 )
			{
				int digit = (i.m_data[j]>>(FRAGSIZE-k-4))&15;
				//if( digit != 0 )
				//	notnull = true;
				//if( notnull )
					str << abcd[ digit ];
			}
		}
		//if( !notnull ) str << "0";
		str << "h";
	}
	// binary
	else if( f & std::ios_base::oct )
	{
		str << std::dec;
		for( j=SIZE-1; j>=0; j-- )
		{
			for( int b=FRAGSIZE-1; b>=0; b-- )
			{
				str << ((i.m_data[j]>>b)&1);
			}
			if( j > 0 )
				str << " ";
		}
		str << "b";
		str << std::oct;
	}
	else // f & std::ios_base::dec
	{
		std::string number;
		hugeint<N> remain=0;
		hugeint<N> ten=10;
		hugeint<N> x(i);
		if( x.sign() )
		{
			str << "-";
			x.neg();
		}
		do {
			ten=10;
			div( x, ten, remain);
			number = abcd[remain.m_data[0]] + number;
		} while( x != 0 );
		str << number;
	}
  return str;
}

// divides unsigned numbers in result by i
// result goes into result
// remainder goes into remainder
// i undefined
template<unsigned int N>
void hugeint<N>::div( hugeint<N>& result, hugeint<N>& i, hugeint<N>& remainder )
{
	assert( i != 0 );
	assert(!result.sign());
	assert(!i.sign());
	// division by 0
	// 0/x = 0
	if( !result )
	{
		remainder = 0;
		return;
	}
	hugeint<N> mask( 0x80000000, N-1 ); // = 0x80000000000000000000000000000000

	//std::ios::fmtflags f = cout.flags();
	//cout << oct << "mask: " <<  mask << endl;

	int crossfoot=BITSIZE; // total number of used digits of the divisor "crossfoot"
	// now the divisor is shifted to left, so that the uppermost bit is set
	while( !(i & mask) )
	{
		i<<=1;
		crossfoot--;
	}
	i >>= 1;
	i.m_data[N-1] &= 0x7FFFFFFF;
	//cout <<        "i after crossfoot: " << i << endl;
	//cout << dec << "crossfoot        : " << crossfoot << oct << endl;
	//i>>=1;
	// algorithm: z / i (works only for numbers > 0)
	// 0. result = 0; remainder = z; bits=BITSIZE;
	remainder	= result; // holds remainder at the end
	result		= 0;	  // holds result at the end
	// 1. digits = crossfoot(i);
	// 2. i <<= (BITSIZE-digits),  the uppermost bit is set now!
	// already done
	// 3. if( i > remainder ) goto 6
	// 4. result |= 1;
	// 5. remainder -= i;
	// 6. result <<= 1;
	// 7. i >>= 1;
	// 8. bits--;
	// 9. if( bits >= digits ) goto 3
	int bits = BITSIZE-1;
	do {
		if( i <= remainder )
		{
			result		|= 1;
			remainder	-= i;
		}
		result <<= 1;
		i >>= 1;
		bits--;
	}	while( bits >= crossfoot );

	result >>= 1;

	//cout.flags(f);
}

// makes both numbers positive, returns bitmask, set bit for changed sign
template<unsigned int N>
int  hugeint<N>::remove_sign( hugeint<N> &a, hugeint<N> &b )
{
	int result=0;
	if( a.sign() )
	{
		result |= 1;
		a.neg();
	}
	if( b.sign() )
	{
		result |= 2;
		b.neg();
	}
	return result;
}

template<unsigned int N>
void hugeint<N>::restore_sign( hugeint<N>& a, hugeint<N>& b, int signmask )
{
	if( signmask & 1 )
		a.neg();
	if( signmask & 2 )
		b.neg();
}

/*
template<unsigned int N>
hugeint<N>::operator char() const
{
	char r = m_data[0]&255;
	return r;
}
template<unsigned int N>
hugeint<N>::operator int() const
{
	int r = m_data[0];
	return r;
}
template<unsigned int N>
hugeint<N>::operator long() const
{
	long r = m_data[0];
	return r;
}
template<unsigned int N>
hugeint<N>::operator float() const
{
	return float(m_data[0]);
}
*/
template<unsigned int N>
mtl::type::hugeint<N>::operator unsigned int() const
{
	//std::cout << "unsigned()";
	long r = m_data[0];
	for( unsigned i=1; i<N; i++ )
	{
		assert( m_data[i] == 0 );
	}
	return r;
}
/*
template<unsigned int N>
hugeint<N>::operator double() const
{
	std::cout << "WARNING: double()";
	double r=0.0;
	for( unsigned i=0; i<N; i++ )
	{
		double a = double(m_data[i]);
		a *= double(1<<(i*sizeof(int)*8));
		r += a;
	}
	return r;
}
*/
} // namespace type
} // namespace mtl

template<unsigned int N>
mtl::type::hugeint<N> operator-( const mtl::type::hugeint<N>& i )
{
	mtl::type::hugeint<N> r(i);
	r.neg();
	return r;
}

template<unsigned int N>
bool operator!( const mtl::type::hugeint<N>& i )
{
	for( unsigned j=0; j<N; j++ )
		if( i.m_data[j] != 0 )
			return false;
	return true;
}

template<unsigned int N>
mtl::type::hugeint<N> operator~( const mtl::type::hugeint<N>& x )
{
	mtl::type::hugeint<N> r(x);
	for( unsigned j=0; j<N; j++ )
		r.m_data[j] = ~m_data[j];
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator|( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r |= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator&( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r &= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator^( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r ^= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator>>( const mtl::type::hugeint<N>& x, unsigned int i )
{
	mtl::type::hugeint<N> r(x);
	r >>= i;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator<<( const mtl::type::hugeint<N>& x, unsigned int i )
{
	mtl::type::hugeint<N> r(x);
	r <<= i;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator+( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r += j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator-( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(j);
	r.neg();
	r += i;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator*( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r *= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator/( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r /= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator%( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r %= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator+( int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r += j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator-( int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(j);
	r.neg();
	r += i;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator*( int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r *= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator/( int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r /= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator%( int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r %= j;
	return r;
}
template<unsigned int N>
mtl::type::hugeint<N> operator+( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(i);
	r += j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator-( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(j);
	r.neg();
	r += i;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator*( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(i);
	r *= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator/( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(i);
	r /= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator%( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(i);
	r %= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator+( unsigned int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r += j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator-( unsigned int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(j);
	r.neg();
	r += i;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator*( unsigned int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r *= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator/( unsigned int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r /= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator%( unsigned int i, const mtl::type::hugeint<N>& j )
{
	mtl::type::hugeint<N> r(i);
	r %= j;
	return r;
}
template<unsigned int N>
mtl::type::hugeint<N> operator+( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(i);
	r += j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator-( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(j);
	r.neg();
	r += i;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator*( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(i);
	r *= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator/( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(i);
	r /= j;
	return r;
}

template<unsigned int N>
mtl::type::hugeint<N> operator%( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(i);
	r %= j;
	return r;
}


template<unsigned int N>
bool operator==( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	for( unsigned k=0; k<N; k++ )
		if( i.m_data[k] != j.m_data[k] )
			return false;
	return true;
}

template<unsigned int N>
bool operator!=( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	for( unsigned k=0; k<N; k++ )
		if( i.m_data[k] != j.m_data[k] )
			return true;
	return false;
}

template<unsigned int N>
bool operator<=( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{

	if( i.sign() && !j.sign() )
		return true;
	if( !i.sign() && j.sign() )
		return false;
	if( !i.sign() ) // both >= 0
	{
		for( int k=N-1; k>=0; k-- )
		{
			if( i.m_data[k] > j.m_data[k] ) // must be bigger
				return false;
			if( i.m_data[k] < j.m_data[k] )
				return true;
		}
		return true; // is equal!
	}
	else
	{	// both < 0
		mtl::type::hugeint<N> a(i);
		mtl::type::hugeint<N> b(j);
		a.neg();
		b.neg();
    	return a >= b;
	}
}

template<unsigned int N>
bool operator>=( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	if( !i.sign() && j.sign() )
		return true;
	if( i.sign() && !(j.sign()) )
		return false;
	if( !i.sign() ) // both >= 0
	{
		for( int k=N-1; k>=0; k-- )
		{
			if( i.m_data[k] > j.m_data[k] ) // must be bigger
				return true;
			if( i.m_data[k] < j.m_data[k] )
				return false;
		}
		return true; // is equal!
	}
	else
	{ // both < 0
		mtl::type::hugeint<N> a(i);
		mtl::type::hugeint<N> b(j);
		a.neg();
		b.neg();
    	return a<=b;
	}
}

template<unsigned int N>
bool operator< ( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	//std::cout << std::hex << i << " < " << j << "?" << std::endl;
	if( i.sign() && !(j.sign()) ) // -a < +b !
		return true;
	if( !i.sign() && j.sign() )   // +a > -b !
		return false;
	if( !i.sign() ) // both >= 0
	{
		for( int k=N-1; k>=0; k-- ) {
			if( i.m_data[k] > j.m_data[k] ) // must be bigger
				return false;
			if( i.m_data[k] < j.m_data[k] )
				return true;
		}
		return false; // is equal!
	}
	else { // both < 0
		mtl::type::hugeint<N> a=i;
		mtl::type::hugeint<N> b=j;
		a.neg();
		b.neg();
    return a>b;
	}
}

template<unsigned int N>
bool operator> ( const mtl::type::hugeint<N>& i, const mtl::type::hugeint<N>& j )
{
	if( !i.sign() && j.sign() ) // a > -b !
		return true;
	if( i.sign() && !(j.sign()) ) // -a > -b
		return false;
	if( !i.sign() ) // both >= 0
	{
		for( int k=N-1; k>=0; k-- ) {
			if( i.m_data[k] > j.m_data[k] ) // must be bigger
				return true;
			if( i.m_data[k] < j.m_data[k] ) // must be smaller
				return false;
		}
		return false; // is equal!
	}
	else { // both < 0
		mtl::type::hugeint<N> a(i);
		mtl::type::hugeint<N> b(j);
		a.neg();
		b.neg();
    return a<b;
	}
}

// int
template<unsigned int N>
bool operator==( int i, const mtl::type::hugeint<N>& j ) { return j == i; }
template<unsigned int N>
bool operator!=( int i, const mtl::type::hugeint<N>& j ) { return j != i; }
template<unsigned int N>
bool operator<=( int i, const mtl::type::hugeint<N>& j ) { return j > i; }
template<unsigned int N>
bool operator>=( int i, const mtl::type::hugeint<N>& j ) { return j < i; }
template<unsigned int N>
bool operator< ( int i, const mtl::type::hugeint<N>& j ) { return j >= i; }
template<unsigned int N>
bool operator> ( int i, const mtl::type::hugeint<N>& j ) { return j <= i; }

template<unsigned int N>
bool operator==( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(j);
	return i == r;
}

template<unsigned int N>
bool operator!=( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(j);
	return i != r;
}

template<unsigned int N>
bool operator<=( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(j);
	return i <= r;
}

template<unsigned int N>
bool operator>=( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(j);
	return i >= r;
}

template<unsigned int N>
bool operator< ( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(j);
	return i < r;
}

template<unsigned int N>
bool operator> ( const mtl::type::hugeint<N>& i, int j )
{
	mtl::type::hugeint<N> r(j);
	return i > r;
}

// unsigned
template<unsigned int N>
bool operator==( unsigned int i, const mtl::type::hugeint<N>& j ) { return j == i; }
template<unsigned int N>
bool operator!=( unsigned int i, const mtl::type::hugeint<N>& j ) { return j != i; }
template<unsigned int N>
bool operator<=( unsigned int i, const mtl::type::hugeint<N>& j ) { return j > i; }
template<unsigned int N>
bool operator>=( unsigned int i, const mtl::type::hugeint<N>& j ) { return j < i; }
template<unsigned int N>
bool operator< ( unsigned int i, const mtl::type::hugeint<N>& j ) { return j >= i; }
template<unsigned int N>
bool operator> ( unsigned int i, const mtl::type::hugeint<N>& j ) { return j <= i; }

template<unsigned int N>
bool operator==( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(j);
	return i == r;
}
template<unsigned int N>
bool operator!=( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(j);
	return i != r;
}
template<unsigned int N>
bool operator<=( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(j);
	return i <= r;
}
template<unsigned int N>
bool operator>=( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(j);
	return i >= r;
}
template<unsigned int N>
bool operator< ( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(j);
	return i < r;
}
template<unsigned int N>
bool operator> ( const mtl::type::hugeint<N>& i, unsigned int j )
{
	mtl::type::hugeint<N> r(j);
	return i > r;
}

// output
template<unsigned int N>
std::ostream& operator<<( std::ostream& str, const mtl::type::hugeint<N>& i )
{
	return mtl::type::hugeint<N>::print( str, i );
}






