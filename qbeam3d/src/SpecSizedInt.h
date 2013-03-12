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

#ifndef SPECSIZEDINT_INCLUDE
#define SPECSIZEDINT_INCLUDE

#include <iostream>
#include "Endianess.h"

/* stolen from boost */
#if !defined ct_assert

#define JOIN( X, Y ) DO_JOIN( X, Y )
#define DO_JOIN( X, Y ) DO_JOIN2(X,Y)
#define DO_JOIN2( X, Y ) X##Y

template <bool x> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };
template<int x> struct static_assert_test{};

#define ct_assert( B ) \
   typedef static_assert_test<\
      sizeof(STATIC_ASSERTION_FAILURE<(bool)( B )>)>\
         JOIN(boost_static_assert_typedef_, __LINE__)
#endif
/* boost theft end */

// defines an signed integer, with wrap around and all, capable of holding exactly
// N bits
template <unsigned int N, class T=long int>
class SpecSizedInt
{

  ct_assert( N > 0 );
  ct_assert( N <= (sizeof(T)<<3));
  
  public:

  template<unsigned int SIZE,class TYPE>
  friend std::istream& operator>>( std::istream& in, SpecSizedInt<SIZE,TYPE>& n );
  template<unsigned int SIZE,class TYPE>
  friend std::ostream& operator<<( std::ostream& out, const SpecSizedInt<SIZE,TYPE>& n );

  SpecSizedInt<N,T>()
  {
    _data     = 0;
	_wrapMask = ~((~T(0)) << N);	
  }

  // Convertion to    
  SpecSizedInt<N,T>( long e )
  {
    _data     = e;
	_wrapMask = ~((~T(0)) << N);
    wrapAround();
  }
  SpecSizedInt<N,T>( int e )
  {
    _data     = e;
	_wrapMask = ~((~T(0)) << N);
    wrapAround();
  }
  SpecSizedInt<N,T>( char e )
  {
    _data     = e;
	_wrapMask = ~((~T(0)) << N);
    wrapAround();
  }
  SpecSizedInt<N,T>( const SpecSizedInt<N,T>& e )
  {
    _data     = e._data;
	_wrapMask = ~((~T(0)) << N);
  }

  // convert from byte array preserving Endianess
  SpecSizedInt<N,T>( const char* data, Endianess::EndianType e = Endianess::which(), unsigned int len=0 )
  {
	_data     = 0;
	_wrapMask = ~((~T(0)) << N);
    convertFromByteArray( data, e, len );
  }

  void convertFromByteArray( const char* data, Endianess::EndianType e = Endianess::which(), unsigned int len=0)
  {
    const T* p = reinterpret_cast<const T*>(data);
    _data = *p;
	if( len== 0 )
		len=sizeof(T);

    if( e != Endianess::which() )
    {
	//  MEM: 3 2 1 0
	//  #1 : 3 2 1 0
	//  #2 : 2 3 1 0
	//  #3 : 1 2 3 0
	//  #4 : 0 1 2 3
		char* p1 = reinterpret_cast<char*>(&_data);
		char* p2 = p1+len-1;
		while( p1 < p2 )
		{
		   char temp = *p1;
		   *p1++ = *p2;
		   *p2-- = temp;
		}
    }
    wrapAround();
  }
  
  void storeInByteArray(char* data, Endianess::EndianType e = Endianess::which(), unsigned int len=0) const
  {
	const char* p = reinterpret_cast<const char*>(&_data);
	if( len== 0 )
		len=sizeof(T);
	// 1st: copy len lower bytes from *p to data
	unsigned int i=0;
	while( i<len )
	{
		// no byte swap at this point!
		unsigned idx = Endianess::which() == Endianess::LITTLE ? i : i+sizeof(T)-len;
		data[i] = p[idx];
		i++;
	}
	// 2nd: swap bytes if necessary
    if( e != Endianess::which() )
    {
		char* p1 = data;
		char* p2 = p1+len-1;
		while( p1 < p2 )
		{
		   char temp = *p1;
		   *p1++ = *p2;
		   *p2-- = temp;
		}
	}
  }

  // convert byte array to number, preserving endianess  
  // Operators
  template <unsigned int M>
  friend class SpecSizedInt<N,T>;
  
  template <unsigned int M>
  SpecSizedInt<N,T>& operator=(const SpecSizedInt<M,T>& e)
  {
    if( (void*)this != (void*)&e )
    {
     _data = e._data;
      if( N < M )
	     wrapAround();
    }
    return *this;
  }

  // Mathematical operations

  // prefix
  SpecSizedInt<N,T>& operator++()
  {
    ++_data;
    wrapAround();
    return *this;
  }
  // postfix
  SpecSizedInt<N,T>& operator++(int)
  {
    _data++;
    wrapAround();
    return *this;
  }
  // prefix
  SpecSizedInt<N,T>& operator--()
  {
    --_data;
    wrapAround();
    return *this;
  }
  // postfix
  SpecSizedInt<N,T>& operator--(int)
  {
    _data--;
    wrapAround();
    return *this;
  }
  
  template <unsigned int M>
  SpecSizedInt<N,T>& operator+=(const SpecSizedInt<M,T>& e)
  {
    _data += e._data;
    wrapAround();
    return *this;
  }

  template <unsigned int M>
  SpecSizedInt<N,T>& operator*=(const SpecSizedInt<M,T>& e)
  {
    _data *= e._data;
    wrapAround();
    return *this;
  }

  template <unsigned int M>
  SpecSizedInt<N,T>& operator-=(const SpecSizedInt<M,T>& e)
  {
    _data -= e._data;
    wrapAround();
    return *this;
  }

  template <unsigned int M>
  SpecSizedInt<N,T>& operator/=(const SpecSizedInt<M,T>& e)
  {
    _data /= e._data;
    return *this;
  }

  template <unsigned int M>
  const SpecSizedInt<N,T> operator+(const SpecSizedInt<M,T>& e)
  {
    SpecSizedInt<N,T> result( *this );
    result._data += e._data;
    result.wrapAround();    
    return result;
  }
  
  template <unsigned int M>
  const SpecSizedInt<N,T> operator-(const SpecSizedInt<M,T>& e)
  {
    SpecSizedInt<N,T> result( *this );
    result._data -= e._data;
    result.wrapAround();
    return result;
  }
  
  template <unsigned int M>
  const SpecSizedInt<N,T> operator/(const SpecSizedInt<M,T>& e)
  {
    SpecSizedInt<N,T> result( *this );
    result._data /= e._data;
    result.wrapAround();
    return result;
  }
  template <unsigned int M>
  const SpecSizedInt<N,T> operator*(const SpecSizedInt<M,T>& e)
  {
    SpecSizedInt<N,T> result( *this );
    result._data *= e._data;
    result.wrapAround();
    return result;
  }

  // Comparators with SpecSizedInt
  template <unsigned int M>
  bool operator==(const SpecSizedInt<M,T>& e)
  {
    return _data == e._data;
  }
  template <unsigned int M>  
  bool operator!=(const SpecSizedInt<M,T>& e)
  {
    return _data != e._data;
  }
  template <unsigned int M>
  bool operator<(const SpecSizedInt<M,T>& e)
  {
    return _data < e._data;
  }
  template <unsigned int M>
  bool operator<=(const SpecSizedInt<M,T>& e)
  {
    return _data <= e._data;
  }
  template <unsigned int M>
  bool operator>(const SpecSizedInt<N,T>& e)
  {
    return _data > e._data;
  }
  template <unsigned int M>
  bool operator>=(const SpecSizedInt<M,T>& e)
  {
    return _data >= e._data;
  }

  // Comparators with underlying data type
  bool operator==(const T e)
  {
    return _data == e;
  }
  bool operator!=(const T e)
  {
    return _data != e;
  }
  bool operator<(const T e)
  {
    return _data < e;
  }
  bool operator<=(const T e)
  {
    return _data <= e;
  }
  bool operator>(const T e)
  {
    return _data > e;
  }
  bool operator>=(const T e)
  {
    return _data >= e;
  }
  // Comparators with int
  bool operator==(const int e)
  {
    return _data == e;
  }
  bool operator!=(const int e)
  {
    return _data != e;
  }
  bool operator<(const int e)
  {
    return _data < e;
  }
  bool operator<=(const int e)
  {
    return _data <= e;
  }
  bool operator>(const int e)
  {
    return _data > e;
  }
  bool operator>=(const int e)
  {
    return _data >= e;
  }
  
  // cast operators

  operator char()   const { return char(_data);}
  operator int()    const { return int(_data); }
  operator unsigned int() const { return unsigned(_data); }
  operator long()   const { return long(_data);}
  operator float()  const { return float(_data); }
  operator double() const { return double(_data);}          

  private:

  inline void wrapAround()
  {
    if( N < (sizeof(T)<<3) )
    {
    // do wrap around
    _data &= _wrapMask;
    // if number is signed now, we must extend sign bits
    if( _data & (1<<(N-1)) ) // signed?
      _data |= ~_wrapMask;
    }
  }

  T _data;
  T _wrapMask;
};

template <unsigned int N,class T>
std::istream& operator>>( std::istream& in, SpecSizedInt<N,T>& n )
{
	in >> n._data;
	n.wrapAround();
	return in;
}

template <unsigned int N,class T>
std::ostream& operator<<( std::ostream& out, const SpecSizedInt<N,T>& n )
{
	out << n._data;
	return out;
}

typedef SpecSizedInt<32> Int32;
typedef SpecSizedInt<24> Int24;
typedef SpecSizedInt<16> Int16;
typedef SpecSizedInt<12> Int12;

#endif
