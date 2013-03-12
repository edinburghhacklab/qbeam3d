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

#ifndef MTL_MATRIX_H
#define MTL_MATRIX_H


/**		MTL matrix templates

		matrix<>
		row_matrix<>
		col_matrix<>
		dyn_matrix<>

  */
#include <vector>
#include "vector.h"

namespace mtl {

// matrix template, both compile dimensions
template <class T,unsigned M,unsigned N>
class matrix
{
public:
	matrix( const T d=T(0) ); // MxN diagonol set to d
	matrix( const matrix<T,M,N>& m ); // MxN
	explicit matrix( const T list[M*N] ); // MxN
	~matrix();

	void clear( const T v );
	void diag( const T v );

	inline unsigned rows() const { return M; }
	inline unsigned cols() const { return N; }

	inline const vector<T,M>  col( unsigned j ) const;
	inline const vector<T,M>& row( unsigned j ) const;

	inline const vector<T,N>& operator[]( unsigned int i ) const;
	inline       vector<T,N>& operator[]( unsigned int i );

	// Zuweisung
	matrix<T,M,N>& operator=( const matrix<T,M,N>& A );

	// skalar product
	matrix<T,M,N>& operator*=( T s );
	matrix<T,M,N>  operator*( T s ) const;

	// matrix product
	// MxN + NxM = MxM
	matrix<T,M,M>  operator* ( const matrix<T,N,M>& A ) const;
	// this only works for NxN matrices
	matrix<T,M,M>& operator*=( const matrix<T,M,M>& A );

	// return A*x
	mtl::vector<T,M> operator* ( const mtl::vector<T,N>& x ) const;

	// division by scalar
	matrix<T,M,N>& operator/=( T s );
	matrix<T,M,N>  operator/ ( T s ) const;

	// sum
	matrix<T,M,N>& operator+=( const matrix<T,M,N> &A );
	matrix<T,M,N>  operator+ ( const matrix<T,M,N> &A ) const;

	// diff
	matrix<T,M,N>& operator-=( const matrix<T,M,N>& A );
	matrix<T,M,N>  operator- ( const matrix<T,M,N> &A ) const;
	matrix<T,M,N>  operator- () const;

	// Transponierte
	matrix<T,N,M>  transposed() const;
	matrix<T,M,M>& transpose();
	// Inverse
	matrix<T,M,M>  inversed() const;
	matrix<T,M,M>& inverse();

	// solve an LGS with a nonsingular MxM matrix
	matrix<T,M,M>  solveLGS( const vector<T,M> &x ) const;
	// rank
	unsigned rank() const;
	// determinant
	T det() const;

// Euklid Norm
// Maximum Norm
// Frobenius Norm

private:

// berechnet die Unterdeterminante der matrix ohne Zeile i und Spalte j
	T det( unsigned int i, unsigned int j, unsigned int n ) const;

private:
	std::vector< mtl::vector<T,N> > m_data; // zeilen...
};

/*
// row_row_matrix template, compile time width, runtime height
template <class T,unsigned N>
class row_matrix
{
public:
	row_matrix( const T d=T(0) ); // MxN diagonol set to d
	row_matrix( const row_matrix<T,N>& m ); // MxN
	explicit row_matrix( const T list[M*N] ); // MxN
	~row_matrix();

	void clear( const T v );
	void diag( const T v );

	inline unsigned rows() const { return M; }
	inline unsigned cols() const { return N; }

	inline const vector<T,M>  col( unsigned j ) const;
	inline const vector<T,M>& row( unsigned j ) const;

	inline const vector<T,N>& operator[]( unsigned int i ) const;
	inline       vector<T,N>& operator[]( unsigned int i );

	// Zuweisung
	row_matrix<T,N>& operator=( const row_matrix<T,N>& A );

	// skalar product
	row_matrix<T,N>& operator*=( T s );
	row_matrix<T,N>  operator*( T s ) const;

	// row_matrix product
	// MxN + NxM = MxM
	row_matrix<T,N>  operator* ( const row_matrix<T,N>& A ) const;
	// this only works for NxN matrices
	row_matrix<T,N>& operator*=( const row_matrix<T,N>& A );

	// return A*x
	mtl::vector<T,M> operator* ( const mtl::vector<T,N>& x ) const;

	// division by scalar
	row_matrix<T,N>& operator/=( T s );
	row_matrix<T,N>  operator/ ( T s ) const;

	// sum
	row_matrix<T,N>& operator+=( const row_matrix<T,N> &A );
	row_matrix<T,N>  operator+ ( const row_matrix<T,N> &A ) const;

	// diff
	row_matrix<T,N>& operator-=( const row_matrix<T,N>& A );
	row_matrix<T,N>  operator- ( const row_matrix<T,N> &A ) const;

	// Transponierte
	row_matrix<T,N>  transposed() const;
	row_matrix<T,N>& transpose();
	// Inverse
	row_matrix<T,N>  inversed() const;
	row_matrix<T,N>& inverse();

	// solve an LGS with a nonsingular MxM row_matrix
	row_matrix<T,N>  solveLGS( const vector<T,M> &x ) const;
	// rank
	unsigned rank() const;
	// determinant
	T det() const;

// Euklid Norm
// Maximum Norm
// Frobenius Norm

private:

// berechnet die Unterdeterminante der row_matrix ohne Zeile i und Spalte j
	T det( unsigned int i, unsigned int j, unsigned int n ) const;

private:
	std::vector< mtl::vector<T,N> > m_data; // zeilen...
};

// col_matrix template, compile time height, runtime width
template <class T,unsigned N>
class col_matrix
{
public:
	col_matrix( const T d=T(0) ); // MxN diagonol set to d
	col_matrix( const col_matrix<T,M,N>& m ); // MxN
	explicit col_matrix( const T list[M*N] ); // MxN
	~col_matrix();

	void clear( const T v );
	void diag( const T v );

	inline unsigned rows() const { return M; }
	inline unsigned cols() const { return N; }

	inline const vector<T,M>  col( unsigned j ) const;
	inline const vector<T,M>& row( unsigned j ) const;

	inline const vector<T,N>& operator[]( unsigned int i ) const;
	inline       vector<T,N>& operator[]( unsigned int i );

	// Zuweisung
	col_matrix<T,M,N>& operator=( const col_matrix<T,M,N>& A );

	// skalar product
	col_matrix<T,M,N>& operator*=( T s );
	col_matrix<T,M,N>  operator*( T s ) const;

	// col_matrix product
	// MxN + NxM = MxM
	col_matrix<T,M,M>  operator* ( const col_matrix<T,N,M>& A ) const;
	// this only works for NxN matrices
	col_matrix<T,M,M>& operator*=( const col_matrix<T,M,M>& A );

	// return A*x
	mtl::vector<T,M> operator* ( const mtl::vector<T,N>& x ) const;

	// division by scalar
	col_matrix<T,M,N>& operator/=( T s );
	col_matrix<T,M,N>  operator/ ( T s ) const;

	// sum
	col_matrix<T,M,N>& operator+=( const col_matrix<T,M,N> &A );
	col_matrix<T,M,N>  operator+ ( const col_matrix<T,M,N> &A ) const;

	// diff
	col_matrix<T,M,N>& operator-=( const col_matrix<T,M,N>& A );
	col_matrix<T,M,N>  operator- ( const col_matrix<T,M,N> &A ) const;

	// Transponierte
	col_matrix<T,N,M>  transposed() const;
	col_matrix<T,M,M>& transpose();
	// Inverse
	col_matrix<T,M,M>  inversed() const;
	col_matrix<T,M,M>& inverse();

	// solve an LGS with a nonsingular MxM col_matrix
	col_matrix<T,M,M>  solveLGS( const vector<T,M> &x ) const;
	// rank
	unsigned rank() const;
	// determinant
	T det() const;

// Euklid Norm
// Maximum Norm
// Frobenius Norm

private:

// berechnet die Unterdeterminante der col_matrix ohne Zeile i und Spalte j
	T det( unsigned int i, unsigned int j, unsigned int n ) const;

private:
	std::vector< mtl::vector<T,N> > m_data; // zeilen...
};
*/

// dyn_matrix template, runtime height and width
template <class T>
class dyn_matrix
{
	dyn_matrix();
public:
	dyn_matrix( unsigned rows, unsigned cols, const T d=T(0) ); // MxN diagonol set to d
	dyn_matrix( const dyn_matrix<T>& m ); // MxN
	explicit dyn_matrix( unsigned rows, unsigned cols, const T list[] ); // MxN
	~dyn_matrix();

	void clear( const T v );
	void diag( const T v );

	inline unsigned rows() const { return m_data.size(); }
	inline unsigned cols() const { return m_data[0].dim(); }

	inline const dyn_vector<T>  col( unsigned j ) const;
	inline const dyn_vector<T>& row( unsigned j ) const;

	inline const dyn_vector<T>& operator[]( unsigned int i ) const;
	inline       dyn_vector<T>& operator[]( unsigned int i );

	// Zuweisung
	dyn_matrix<T>& operator=( const dyn_matrix<T>& A );

	// skalar product
	dyn_matrix<T>& operator*=( T s );
	dyn_matrix<T>  operator*( T s ) const;

	// dyn_matrix product
	// MxN + NxM = MxM
	dyn_matrix<T>  operator* ( const dyn_matrix<T>& A ) const;
	// this only works for NxN matrices
	dyn_matrix<T>& operator*=( const dyn_matrix<T>& A );

	// return A*x
	dyn_vector<T> operator* ( const dyn_vector<T>& x ) const;

	// division by scalar
	dyn_matrix<T>& operator/=( T s );
	dyn_matrix<T>  operator/ ( T s ) const;

	// sum
	dyn_matrix<T>& operator+=( const dyn_matrix<T> &A );
	dyn_matrix<T>  operator+ ( const dyn_matrix<T> &A ) const;

	// diff
	dyn_matrix<T>& operator-=( const dyn_matrix<T>& A );
	dyn_matrix<T>  operator- ( const dyn_matrix<T> &A ) const;
	dyn_matrix<T>  operator- () const;

	// Transponierte
	dyn_matrix<T>  transposed() const;
	dyn_matrix<T>& transpose();
	// Inverse
	dyn_matrix<T>  inversed() const;
	dyn_matrix<T>& inverse();

	// solve an LGS with a nonsingular MxM dyn_matrix
	dyn_matrix<T>  solveLGS( const dyn_vector<T> &x ) const;
	// rank
	unsigned rank() const;
	// determinant
	T det() const;

// Euklid Norm
// Maximum Norm
// Frobenius Norm

private:

// berechnet die Unterdeterminante der dyn_matrix ohne Zeile i und Spalte j
	T det( unsigned int i, unsigned int j, unsigned int n ) const;

private:
	std::vector< mtl::dyn_vector<T> > m_data; // zeilen...
};

} // namespace mtl

template <class T,unsigned M, unsigned N>
std::ostream& operator <<( std::ostream& str, const mtl::matrix<T,M,N>& m );
/*
template <class T, unsigned N>
std::ostream& operator <<( std::ostream& str, const mtl::row_matrix<T,N>& m );
template <class T, unsigned Mned N>
std::ostream& operator <<( std::ostream& str, const mtl::col_matrix<T,M,N>& m );
*/
template <class T>
std::ostream& operator <<( std::ostream& str, const mtl::dyn_matrix<T>& m );

#include "matrix.cpp"

#endif

