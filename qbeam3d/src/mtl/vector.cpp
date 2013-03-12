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

#include "vector.h"

/**
	Standardklasse für einen beliebigen vektor (Implementierung)
*/

#include <assert.h>
#include <iostream>

//*****************************************************************
// vector
//*****************************************************************

namespace mtl {

template<class T,unsigned N>
vector<T,N>::vector( T s ) :
	m_data(N,s)
{
}

template<class T, unsigned N>
vector<T,N>::vector( const vector<T,N>& v ) :
	m_data(v.m_data)
{
}

template<class T, unsigned N>
vector<T,N>::vector( const T valarr[N] ) :
	m_data(N)
{
	for( unsigned i=0; i<N; i++ )
		m_data[i] = valarr[i];
}

template<class T, unsigned N>
vector<T,N>::~vector()
{}

template<class T, unsigned N>
void vector<T,N>::clear( T s )
{
	std::fill( m_data.begin(), m_data.end(), s );
}

template<class T, unsigned N>
const T& vector<T,N>::operator [] ( const unsigned int j ) const
{
	assert( j<N );
	return m_data[j];
}

template<class T, unsigned N>
T& vector<T,N>::operator [] ( const unsigned int j )
{
	assert( j<N );
	return m_data[j];
}

// copy construktor
template<class T, unsigned N>
vector<T,N>& vector<T,N>::operator=( const vector<T,N> &v )
{
	m_data = v.m_data;
	return *this;
}

// vectoraddition x + y
template<class T, unsigned N>
vector<T,N>& vector<T,N>::operator +=( const vector<T,N>& v )
{
	for( unsigned i=0; i<N; i++ )
		m_data[i] += v[i];
	return *this;
}

template<class T, unsigned N>
vector<T,N> vector<T,N>::operator+( const vector<T,N>& v ) const
{
	vector<T,N> r(*this);
	r+=v;
	return r;
}

// vectorsubtraktion x - y
template<class T, unsigned N>
vector<T,N>& vector<T,N>::operator -=( const vector<T,N>& v )
{
	for( unsigned i=0; i<N; i++ )
		m_data[i] -= v[i];
	return *this;
}

template<class T, unsigned N>
vector<T,N> vector<T,N>::operator-( const vector<T,N>& v ) const
{
	vector<T,N> r(*this);
	r-=v;
	return r;
}

// scale
template<class T, unsigned N>
vector<T,N>& vector<T,N>::operator *=( T s )
{
	for( unsigned i=0; i<N; i++ )
		m_data[i] *= s;
	return *this;
}

template<class T, unsigned N>
vector<T,N> vector<T,N>::operator *( T s ) const
{
	vector<T,N> r(*this);
	for( unsigned i=0; i<N; i++ )
		r[i] *= s;
	return r;
}

template<class T, unsigned N>
vector<T,N>& vector<T,N>::operator /=( T s )
{
	for( unsigned i=0; i<N; i++ )
		m_data[i] /= s;
	return *this;
}

template<class T, unsigned N>
vector<T,N> vector<T,N>::operator /( T s ) const
{
	vector<T,N> r(*this);
	for( unsigned i=0; i<N; i++ )
		r[i] /= s;
	return r;
}

// vectorprodukt

template<class T, unsigned N>
vector<T,N>& vector<T,N>::cross( const vector<T,N>& v )
{
	assert( N==3 ); // nur für dim=3 definiert!

	T d0 = m_data[1]*v[2] - m_data[2]*v[1];
	T d1 = m_data[2]*v[0] - m_data[0]*v[2];
	T d2 = m_data[0]*v[1] - m_data[1]*v[0];
	m_data[0] = d0;
	m_data[1] = d1;
	m_data[2] = d2;
	return *this;
}

template<class T, unsigned N>
vector<T,N> vector<T,N>::cross( const vector<T,N> &v1, const vector<T,N> &v2 )
{
	assert( N==3 );
	vector<T,3> r(v1);
	r.cross(v2);
	return r;
}

// Skalarprodukt

template<class T, unsigned N>
T vector<T,N>::operator*( const vector<T,N>& v ) const
{
	T sp = 0;
	for( unsigned i=0; i<N; i++ )
		sp += m_data[i]*v[i];
	return sp;
}

// Betrag
template<class T, unsigned N>
double vector<T,N>::length() const
{
	T sp = (*this)*(*this);
	return sqrt( double(sp) );
}

template<class T, unsigned N>
void vector<T,N>::unify()
{
	double l = length();
	if( l != 0 )
		*this /= l;
}

} // namespace

template <class T, unsigned N>
std::ostream& operator<<( std::ostream& str, const mtl::vector<T,N>& x )
{
	str << "(";
	for( unsigned i=0; i<N-1; i++ )
		str << x[i] << ", ";
	str << x[N-1] << ")T";
	return str;
}


//***************************************************************
// dyn_vector
//***************************************************************

namespace mtl {

template<class T>
dyn_vector<T>::dyn_vector( unsigned size, T s ) :
	m_data(size,s)
{}

template<class T>
dyn_vector<T>::dyn_vector( const dyn_vector<T>& v ) :
	m_data(v.m_data)
{
}

template<class T>
dyn_vector<T>::dyn_vector( unsigned size, const T valarr[] ) :
	m_data(size)
{
	for( unsigned i=0; i<dim(); i++ )
		m_data[i] = valarr[i];
}

template<class T>
dyn_vector<T>::~dyn_vector()
{}

template<class T>
void dyn_vector<T>::clear( T s )
{
	std::fill( m_data.begin(), m_data.end(), s );
}

template<class T>
const T& dyn_vector<T>::operator [] ( const unsigned int j ) const
{
	assert( j<dim() );
	return m_data[j];
}

template<class T>
T& dyn_vector<T>::operator [] ( const unsigned int j )
{
	assert( j<dim()  );
	return m_data[j];
}

// copy construktor
template<class T>
dyn_vector<T>& dyn_vector<T>::operator=( const dyn_vector<T> &v )
{
	m_data = v.m_data;
	return *this;
}

// vectoraddition x + y
template<class T>
dyn_vector<T>& dyn_vector<T>::operator +=( const dyn_vector<T>& v )
{
	for( unsigned i=0; i<dim() ; i++ )
		m_data[i] += v[i];
	return *this;
}

template<class T>
dyn_vector<T> dyn_vector<T>::operator+( const dyn_vector<T>& v ) const
{
	dyn_vector<T> r(*this);
	r+=v;
	return r;
}

// vectorsubtraktion x - y
template<class T>
dyn_vector<T>& dyn_vector<T>::operator -=( const dyn_vector<T>& v )
{
	for( unsigned i=0; i<dim() ; i++ )
		m_data[i] -= v[i];
	return *this;
}

template<class T>
dyn_vector<T> dyn_vector<T>::operator-( const dyn_vector<T>& v ) const
{
	dyn_vector<T> r(*this);
	r-=v;
	return r;
}

// scale
template<class T>
dyn_vector<T>& dyn_vector<T>::operator *=( T s )
{
	for( unsigned i=0; i<dim() ; i++ )
		m_data[i] *= s;
	return *this;
}

template<class T>
dyn_vector<T> dyn_vector<T>::operator *( T s ) const
{
	dyn_vector<T> r(*this);
	for( unsigned i=0; i<dim() ; i++ )
		r[i] *= s;
	return r;
}

template<class T>
dyn_vector<T>& dyn_vector<T>::operator /=( T s )
{
	for( unsigned i=0; i<dim() ; i++ )
		m_data[i] /= s;
	return *this;
}

template<class T>
dyn_vector<T> dyn_vector<T>::operator /( T s ) const
{
	dyn_vector<T> r(*this);
	for( unsigned i=0; i<dim() ; i++ )
		r[i] /= s;
	return r;
}

// vectorprodukt

template<class T>
dyn_vector<T>& dyn_vector<T>::cross( const dyn_vector<T>& v )
{
	assert( dim() == 3 ); // nur für dim=3 definiert!
	assert( v.dim() == 3 );

	T d0 = m_data[1]*v[2] - m_data[2]*v[1];
	T d1 = m_data[2]*v[0] - m_data[0]*v[2];
	T d2 = m_data[0]*v[1] - m_data[1]*v[0];
	m_data[0] = d0;
	m_data[1] = d1;
	m_data[2] = d2;
	return *this;
}

template<class T>
dyn_vector<T> dyn_vector<T>::cross( const dyn_vector<T> &v1, const dyn_vector<T> &v2 )
{
	assert( dim() == 3 ); // nur für dim=3 definiert!
	assert( v.dim() == 3 );

	dyn_vector<T> r(3, v1);
	r.cross(v2);
	return r;
}

// Skalarprodukt

template<class T>
T dyn_vector<T>::operator*( const dyn_vector<T>& v ) const
{
	assert( dim() == v.dim() );
	T sp(0);
	for( unsigned i=0; i<dim() ; i++ )
		sp += m_data[i]*v[i];
	return sp;
}

// Betrag
template<class T>
double dyn_vector<T>::length() const
{
	T sp = (*this)*(*this);
	return sqrt( double(sp) );
}

template<class T>
void dyn_vector<T>::unify()
{
	double l = length();
	if( l != 0 )
		*this /= l;
}

} // namespace

template <class T>
std::ostream& operator<<( std::ostream& str, const mtl::dyn_vector<T>& x )
{
	str << "(";
	for( unsigned i=0; i<x.dim()-1; i++ )
		str << x[i] << ", ";
	str << x[x.dim()-1] << ")T";
	return str;
}

