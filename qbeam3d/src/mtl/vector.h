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

#ifndef MTL_VECTOR_H
#define MTL_VECTOR_H


/**Standardklasse für einen beliebigen vector
 */
#include <assert.h>
#include <iostream>
#include <vector>

namespace mtl
{

template <class T, unsigned N>
class vector
{
public:

  vector( T s=T(0) );
  vector( const vector<T,N>& v );
  explicit vector( const T valarr[N] );
  ~vector();

  void clear( T s=T(0) );

  inline unsigned dim() const { return N; }

  inline const T&	operator [] ( const unsigned int j ) const;
  inline T&			operator [] ( const unsigned int j );


  vector<T,N>& operator = ( const vector<T,N>& v );

  vector<T,N>& operator +=( const vector<T,N>& v );
  vector<T,N>& operator -=( const vector<T,N>& v );

  vector<T,N>  operator + ( const vector<T,N>& v ) const;
  vector<T,N>  operator - ( const vector<T,N>& v ) const;

  // with skalars
  vector<T,N>& operator *=( const T s );
  vector<T,N>& operator /=( const T s );

  vector<T,N>  operator * ( const T s ) const;
  vector<T,N>  operator / ( const T s ) const;


  vector<T,N>& 			cross( const vector<T,N>& v );
  static vector<T,N>	cross( const vector<T,N>& v1, const vector<T,N>& v2 );

  // scalar product
  T						operator*( const vector<T,N>& v ) const;
  // Betrag
  double				length() const;
  void					unify();

private:
	std::vector<T> m_data;
};

}

template <class T, unsigned N>
std::ostream& operator<<( std::ostream& str, const mtl::vector<T,N>& x );

//*****************************************************************************
// dyn_vector
//*****************************************************************************

namespace mtl {

template <class T>
class dyn_vector
{
  dyn_vector();
public:

  dyn_vector( unsigned size, T s=T(0) );
  dyn_vector( const dyn_vector<T>& v );
  explicit dyn_vector( unsigned size, const T valarr[N] );
  ~dyn_vector();

  void clear( T s=T(0) );

  inline unsigned dim() const { return m_data.size(); }

  inline const T&	operator [] ( const unsigned int j ) const;
  inline T&			operator [] ( const unsigned int j );


  dyn_vector<T>& operator = ( const dyn_vector<T>& v );

  dyn_vector<T>& operator +=( const dyn_vector<T>& v );
  dyn_vector<T>& operator -=( const dyn_vector<T>& v );

  dyn_vector<T>  operator + ( const dyn_vector<T>& v ) const;
  dyn_vector<T>  operator - ( const dyn_vector<T>& v ) const;

  // with skalars
  dyn_vector<T>& operator *=( const T s );
  dyn_vector<T>& operator /=( const T s );

  dyn_vector<T>  operator * ( const T s ) const;
  dyn_vector<T>  operator / ( const T s ) const;


  dyn_vector<T>& 			cross( const dyn_vector<T>& v );
  static dyn_vector<T>		cross( const dyn_vector<T>& v1, const dyn_vector<T>& v2 );

  // scalar product
  T						operator*( const dyn_vector<T>& v ) const;
  // Betrag
  double				length() const;
  void					unify();

private:
	std::vector<T> m_data;
};

}; // namespace mtl

template <class T>
std::ostream& operator<<( std::ostream& str, const mtl::dyn_vector<T>& x );

#include "vector.cpp"

#endif
