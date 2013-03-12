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
// C++ MTL templates: algorithm
//

#ifndef MTL_ALGORITHM_H_
#define MTL_ALGORITHM_H_

#include <iostream>
#include <limits>
#include "matrix.h"

namespace mtl {
// calculates a least squares solution of an over-determined
// linear equation system A*x = b
//
// N equals to the number of unknown vars
// M equals to the number of equations
//
export template <class T, unsigned M, unsigned N>
vector<T,N> leastSquares( matrix<T,M,N> &A, vector<T,M> &b )
{
	// solve: A^T*A*v = A^T*b -> AtA*v = x
	// calc A^T*A
	matrix<T,N,N>	AtA	= A.transposed()*A;
	// calc A^T*b
	vector<T,N>		x	= A.transposed()*b;
	// solve (A^T*A)v = x;
	AtA					= AtA.solveLGS( x );
	return AtA[0];
}

export template <class T>
dyn_vector<T> leastSquares( const dyn_matrix<T>& A, const dyn_vector<T>& b )
{
	// solve: A^T*A*v = A^T*b -> AtA*v = x
	// calc A^T*A

	//	std::cout << "LEAST SQUARES: A(T)*A*v = A(T)*b" << std::endl;
	//	std::cout << "A= " << std::endl;
	//	std::cout << A << std::endl << std::endl;

	//	std::cout << "A(T)= " << std::endl;
	//	std::cout << A.transposed()  << std::endl << std::endl;
/*
		std::cout << "A(T)*A =" << std::endl;
	dyn_matrix<T>	AtA	= A.transposed()*A;
	//	std::cout << AtA << std::endl << std::endl;

		std::cout << "x = A(T)*b = " << std::endl;
	// calc A^T*b
	dyn_vector<T>	x	= A.transposed()*b;
	//	std::cout << x << std::endl << std::endl;

	// solve (A^T*A)v = x;
		std::cout << "A(T)A*v = x = " << std::endl;
	AtA					= AtA.solveLGS( x );
	//	std::cout << AtA[0] << std::endl << std::endl;
*/
		//std::cout << "A(T)*A =" << std::endl;

	dyn_matrix<T> AtA( A.cols(), A.cols() );
	for( unsigned i=0; i<AtA.rows(); i++ ) {
		for( unsigned j=i; j<AtA.cols(); j++ ) {
			T num = A[0][i]*A[0][j];
			for( unsigned k=1; k<A.rows(); k++ )
				num += A[k][i]*A[k][j];
			AtA[i][j] = num;
			if( i != j )
			AtA[j][i] = num;
		}
	}

		//std::cout << "x = A(T)*b = " << std::endl;
	// calc A^T*b
	dyn_vector<T>	x	= A.transposed()*b;

	// solve (A^T*A)v = x;
		//std::cout << "A(T)A*v = x = " << std::endl;
	AtA					= AtA.solveLGS( x );

	return AtA[0];
}

// calculate greatest common divisor
export template <class T>
T gcd( T u, T v )
{
	assert( std::numeric_limits<T>::is_integer );
	if( u%v == 0 )
		return v;
	return gcd( v, u%v );
}

export template <class FIRST_T,class SECOND_T>
class pair_first
{
public:
	pair_first() {}
	FIRST_T operator()(std::pair<FIRST_T,SECOND_T>& elem) const
	{ return elem.first; }
	const FIRST_T operator()(const std::pair<FIRST_T,SECOND_T>& elem) const
	{ return elem.first; }

private:
	pair_first& operator=(const pair_first&);
};

export template <class FIRST_T,class SECOND_T>
class pair_second
{
public:
	pair_second() {}
	SECOND_T operator()(std::pair<FIRST_T,SECOND_T>& elem) const
	{ return elem.second; }
	const SECOND_T operator()(const std::pair<FIRST_T,SECOND_T>& elem) const
	{ return elem.second; }
private:
	pair_second& operator=(const pair_second&);
};


export template <class T, class In, class Converter>
T arithMean( In beg, In end, Converter Out )
{
	T meanVal = 0.0;
	T N=0;
	while( beg != end )
	{
		meanVal += T(Out(*beg));
		++beg;
		++N;
	}
	if( N != 0 )
		meanVal /= N;
	return meanVal;
}
/*
template <class T>
T arithMean( T* beg, T* end)
{
	T meanVal = 0.0;
	T N=0;
	while( beg != end )
	{
		meanVal += *beg++;
		++N;
	}
	if( N != 0 )
		meanVal /= N;
	return meanVal;
}
*/

}; // namespace

#endif
