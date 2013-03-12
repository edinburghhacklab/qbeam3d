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

#include "matrix.h"

namespace mtl
{

template<class T, unsigned M, unsigned N>
matrix<T,M,N>::matrix( const T d ) :
	m_data(M)
{
	unsigned s = std::min(M,N);
	for( unsigned i=0; i<s; i++ )
		m_data[i][i] = d;
}

template<class T, unsigned M, unsigned N>
matrix<T,M,N>::matrix( const matrix<T,M,N>& m ) :
	m_data( m.m_data )
{}

template<class T, unsigned M, unsigned N>
matrix<T,M,N>::matrix( const T list[M*N] ) :
	m_data(M)
{
	for( unsigned i=0; i<M; i++ )
		for( unsigned j=0; j<N; j++ )
			m_data[i][j] = list[i*N+j];
}

template<class T, unsigned M, unsigned N>
matrix<T,M,N>::~matrix()
{}

template<class T, unsigned M, unsigned N>
void matrix<T,M,N>::clear( const T v )
{
	std::fill( m_data.begin(), m_data.end(), v );
}

template<class T, unsigned M, unsigned N>
void matrix<T,M,N>::diag( const T v )
{
	unsigned i=0;
	unsigned s = std::min(M,N);
	for( ; i<s; i++ )
	{
		m_data[i].clear();
		m_data[i][i] = v;
	}
	while( i < M )
		m_data[i++].clear();
}

template<class T, unsigned M, unsigned N>
const vector<T,M> matrix<T,M,N>::col( unsigned j ) const
{
	assert(j<N);
	vector<T,M> r;
	for( unsigned i=0; i<M; i++ )
		r[i] = m_data[i][j];
	return r;
}
template<class T, unsigned M, unsigned N>
const vector<T,M>& matrix<T,M,N>::row( unsigned j ) const
{
	assert( j<M );
	return m_data[j];
}

template<class T, unsigned M, unsigned N>
const vector<T,N>& matrix<T,M,N>::matrix<T,M,N>::operator[]( unsigned int i ) const
{
	assert( i<M );
	return m_data[i];
}

template<class T, unsigned M, unsigned N>
vector<T,N>& matrix<T,M,N>::operator[]( unsigned int i )
{
	assert( i<M );
	return m_data[i];
}

// Zuweisung
template<class T, unsigned M, unsigned N>
matrix<T,M,N>& matrix<T,M,N>::operator=( const matrix<T,M,N>& A )
{
	if( this != &A )
		m_data = A.m_data;
	return *this;
}
// product with scalar
template<class T, unsigned M, unsigned N>
matrix<T,M,N>& matrix<T,M,N>::operator*=( T s )
{
	for( unsigned i=0; i<M; i++ )
		m_data[i] *= s;
	return *this;
}

template<class T, unsigned M, unsigned N>
matrix<T,M,N>  matrix<T,M,N>::operator*( T s ) const
{
	matrix<T,M,N> r(*this);
	r *= s;
	return r;
}

// matrix product
// MxN + NxM = MxM
template<class T, unsigned M, unsigned N>
matrix<T,M,M>  matrix<T,M,N>::operator* ( const matrix<T,N,M>& A ) const
{
	matrix<T,M,M> m;
	for( unsigned i=0; i<M; i++ )
		for( unsigned j=0; j<M; j++ )
		{
			m[i][j] = 0;
			for( unsigned s=0; s<N; s++ )
				m[i][j] += m_data[i][s]*A[s][j];
		}
	return m;
}

// this only works for NxN matrices
template<class T, unsigned M, unsigned N>
matrix<T,M,M>& matrix<T,M,N>::operator*=( const matrix<T,M,M>& A )
{
	matrix<T,M,M> m( *this );
	 for( unsigned i=0; i<M; i++ )
		for( unsigned j=0; j<M; j++ )
		{
			m_data[i][j] = 0;
			for( unsigned s=0; s<M; s++ )
				m_data[i][j] += m[i][s]*A[s][j];
		}
	return *this;
}

// return A*x
template<class T, unsigned M, unsigned N>
vector<T,M> matrix<T,M,N>::operator* ( const vector<T,N>& x ) const
{
	vector<T,M> r;
	for( unsigned i=0; i<M; i++ )
		r[i] = m_data[i]*x;
	return r;
}

// division by scalar
template<class T, unsigned M, unsigned N>
matrix<T,M,N>& matrix<T,M,N>::operator/=( T s )
{
	for( unsigned i=0; i<M; i++ )
		m_data[i]/=s;
	return *this;
}

template<class T, unsigned M, unsigned N>
matrix<T,M,N>  matrix<T,M,N>::operator/ ( T s ) const
{
	matrix<T,M,N> m(*this);
	m /= s;
	return m;
}

// sum
template<class T, unsigned M, unsigned N>
matrix<T,M,N>& matrix<T,M,N>::operator+=( const matrix<T,M,N> &A )
{
	for( unsigned i=0; i<M; i++ )
		m_data[i] += A[i];
	return *this;
}

template<class T, unsigned M, unsigned N>
matrix<T,M,N>  matrix<T,M,N>::operator+ ( const matrix<T,M,N> &A ) const
{
	matrix<T,M,N> m(*this);
	m += A;
	return m;
}

// diff
template<class T, unsigned M, unsigned N>
matrix<T,M,N>& matrix<T,M,N>::operator-=( const matrix<T,M,N>& A )
{
	for( unsigned i=0; i<M; i++ )
		m_data[i] -= A[i];
	return *this;
}

template<class T, unsigned M, unsigned N>
matrix<T,M,N>  matrix<T,M,N>::operator- ( const matrix<T,M,N> &A ) const
{
	matrix<T,M,N> m(*this);
	m -= A;
	return m;
}

template<class T, unsigned M, unsigned N>
matrix<T,M,N>  matrix<T,M,N>::operator-() const
{
	matrix<T,M,N> m(*this);
	for( unsigned i=0; i<M; i++ )
		for( unsigned j=0; j<N; j++ )
			m_data[i][j] = -m_data[i][j];
	return m;
}

// Transponierte
template<class T, unsigned M, unsigned N>
matrix<T,N,M>  matrix<T,M,N>::transposed() const
{
	matrix<T,N,M> m(T(0));
	for( unsigned i=0; i<M; i++ )
		for( unsigned j=0; j<N; j++ )
			m[j][i] = m_data[i][j];
	return m;
}

template<class T, unsigned M, unsigned N>
matrix<T,M,M>& matrix<T,M,N>::transpose()
{
	for( unsigned i=0; i<M; i++ )
		for( unsigned j=i+1; j<M; j++ )
			std::swap( m_data[j][i], m_data[i][j] );
	return *this;
}

// Inverse
template<class T, unsigned M, unsigned N>
matrix<T,M,M> matrix<T,M,N>::inversed() const
{
	matrix<T,M,M> m(*this);
	return m.inverse();
}

template<class T, unsigned M, unsigned N>
matrix<T,M,M>& matrix<T,M,N>::inverse()
{
 matrix<T,M,M> A(*this); // Kopie
 this->diag(1);			 // mache this zur Einheitsmatrix
 vector<T,M> v;

//	cout << "INVERSE:"<<endl;
//	cout << A << "-----------" << endl << *m << "==========" << endl;
	int s; // zu löschende Spalte
	// erster Schritt: erzeuge unter Dreiecksmatrix
	for( s=0; s<M; s++ )
	{
		if( A[s][s] == 0 )
		{
			int t=s+1;
			while( t<M )
			{
				if( A[t][s] != 0 )
				{
					A[s] += A[t];
					m_data[s] += m_data[t];
					break;
				}
				t++;
			}
			if( A[s][s] == 0 )
			{
				clear(0);
				break;
			}
//cout << A << "-----------" << endl << *m << "==========" << endl;
		}
			// 1. setze auf 1
		T c = A[s][s];
		//cout << "A[" << s << "] /= " << c << endl;
		m_data[s] /= c; A[s] /= c;
//cout << A << "-----------" << endl << *m << "==========" << endl;
		// 2. addiere zum "nullen"
		for( int t=s+1; t<M; t++ )
		{//		0 1 2 4 -> 0  1   2    4
			T x = A[t][s]; // koeffizient		0 X Y Z -> 0 X-X Y-2X Z-4X
			if( x == 0 )
				continue;
			m_data[t] -= m_data[s]*x;
			A[t] -= A[s]*x;
//cout << A << "-----------" << endl << *m << "==========" << endl;
		}
	}
// cout << "// zweiter Schritt:  erzeuge von unten nach oben diagonalmatrix" << endl;
	for( s = M-1; s>0; s-- )
	{
		if( A[s][s] == 0 )
			break; // gibt keine Inverse!
		m_data[s] /= A[s][s];
		A[s] /= A[s][s];
//cout << A << "-----------" << endl << *m << "==========" << endl;
		for( int t=s-1; t>=0; t-- )
		{//		0 1 2 4 -> 0  1   2    4
			T x = A[t][s]; // koeffizient		0 X Y Z -> 0 X-X Y-2X Z-4X
			if( x == 0 ) // wenn schon null, Zeile überspringen
				continue;
			m_data[t] -= m_data[s]*x;
			A[t] -= A[s]*x;
		}
	}
	return *this;
}

// solve an LGS with a nonsingular NxN matrix
template<class T, unsigned M, unsigned N>
matrix<T,M,M> matrix<T,M,N>::solveLGS( const vector<T,M> &y ) const
{
 //assert( N == M ); //erstmal keine über- und unterbestimmten LGS
 matrix<T,M,M> m(*this);
 vector<T,M> b(y),x(y);
 int rg = 0;

 // 0. beginne bei Spalte 0
	for( unsigned i=0,j=0; j<M; j++ )
	{
	// 1. Suche Zeile j mit Pivot Element a(ij)
		if( m[i][j] == 0 ) // nimm erste Zeile, ansonsten suche andere Zeile und addiere sie und nimm dann erste Zeile
		{
			for( unsigned k=i+1; k<M; k++ )
			{
				if( m[k][j] != 0 )
				{
					m[i] += m[k];
					b[i] += b[k];
					break;
				}
			}
			if( m[i][j] == 0 ) // nix, gefunden, ganze Spalte 0, nächste Spalte
				continue;
		}
		rg++; // rank+1;
	// 2. 'normiere' Zeile a(j)
		T a = m[i][j];
		m[i]/= a;
		b[i]/= a;
	// 3. lösche jedes a(kj) mit i!=k
		for( unsigned k=i+1; k<M; k++ )
		{
			T a_kj = m[k][j];
			if( a_kj != 0 )
			{
				m[k] -= m[i]*a_kj;
				b[k] -= b[i]*a_kj;
			}
		}
	// 4. gehe zur nächsten Zeile
		i++;
	}
//	cout << "Diese matrix mit Rang " << rg << " sollte jetzt in Dreiecksform sein: " << endl;
//	cout << *m << endl;
	// eindeutige Lösung
	if( rg == M )
	{
		for( int i=M-1; i>=0; i-- )
		{
			x[i] = b[i];
			for( unsigned k=i+1; k<M; k++ )
				x[i] -= m[i][k]*x[k];
			x[i] /= m[i][i];
		}
//		cout << "Lösungsvector: " << x << endl;
		m.clear(0);
		m[0] = x;
	}
 return m;
}

// rank
template<class T, unsigned M, unsigned N>
unsigned matrix<T,M,N>::rank() const
{
	// not yet implemented correctly
	assert( false );
	return ( N < M ) ? N : M;
}
// determinant
template<class T, unsigned M, unsigned N>
T matrix<T,M,N>::det() const
{
	assert( N==M );
	if( N > 3 )
	{ // rekursiv ausrechnen
		T x = 0;
		int v = 1;
		for( int j=0; j<N; j++ )
		{
			T a = det( 0, j, N );
			x += v * d[0][j] * a;
			v *= -1;

		}
		return x;
	}
	else
	{
		T x = 0;
		if( N == 3 ) {
			x += d[0][0]*d[1][1]*d[2][2];
			x += d[0][1]*d[1][2]*d[2][0];
			x += d[0][2]*d[1][0]*d[2][1];
			x -= d[0][2]*d[1][1]*d[2][0];
			x -= d[0][1]*d[1][0]*d[2][2];
			x -= d[0][0]*d[1][2]*d[2][1];
		}
		else if( N == 2 ) {
			x = d[0][0]*d[1][1] - d[0][1]*d[1][0];
		}
		return x;
	}
	return d[0][0];
}

// sub-determinant
template<class T, unsigned M, unsigned N>
T matrix<T,M,N>::det( unsigned int i, unsigned int j, unsigned int n ) const
{
	assert( N == M );
	assert( n <= N );
	assert( i < n );
	assert( j < n );

	T x=0;
	matrix<T,M,N> m(*this); // kopie anlegen
	// Zeile i entfernen...
	for( int _z=0; _z<(n-1); _z++ )
	{
		if( _z >= i )
		{
			m[_z] = m[_z+1];
			m[_z+1].clear( 0 );
		}
	}
	// Spalte j entfernen
	for( int _s=0; _s<(n-1); _s++ )
	{
		if( _s >= j )
			for( int k=0; k<(n-1); k++ )
			{
				m[k][_s] = m[k][_s+1];
				m[k][_s+1] = 0;
			}
	}

	if( n > 4 )
	{ // induktiv ausrechnen
		// kopie fertig
		x = 0;
		T v = 1; // -1^1+j
		for( int _j=0; _j<(n-1); _j++ )
		{
			T a = m.det( 0, _j, n-1 );
			x += v * m[0][_j] * a;
			v *= -1;
		}
	}
	else
	{
		if( n == 4 )
		{ // rule by SARRUS
			x += (*m)[0][0]*(*m)[1][1]*(*m)[2][2];
			x += (*m)[0][1]*(*m)[1][2]*(*m)[2][0];
			x += (*m)[0][2]*(*m)[1][0]*(*m)[2][1];
			x -= (*m)[0][2]*(*m)[1][1]*(*m)[2][0];
			x -= (*m)[0][1]*(*m)[1][0]*(*m)[2][2];
			x -= (*m)[0][0]*(*m)[1][2]*(*m)[2][1];
		}
		else if( N == 2 )
		{
			x = (*m)[0][0]*(*m)[1][1] - (*m)[0][1]*(*m)[1][0];
		}
		else x = (*m)[0][0];
	}
	return x;
}

} // namespace

template<class T, unsigned M, unsigned N>
std::ostream& operator <<( std::ostream& str, const mtl::matrix<T,M,N>& m )
{
	std::ios_base::fmtflags f = str.flags();
	//str.flags( std::ios_base::right | std::ios_base::dec  );
	//str.width(10);

	for( unsigned i=0; i<m.rows(); i++ )
	{
		str << "|";
		for( unsigned j=0; j<m.cols()-1; j++ )
			str << m[i][j] << " ";
		str << m[i][m.cols()-1] << "|" << endl;
	}
	str.flags(f);
	return str;
}

//***************************************************************************************************
// dyn_matrix
//***************************************************************************************************

namespace mtl
{

template<class T>
dyn_matrix<T>::dyn_matrix( unsigned rows, unsigned cols, const T d ) :
	m_data()
{
	assert( rows>0 );
	assert( cols>0 );

	m_data.reserve( rows );
	for( unsigned i=0; i<rows; i++ )
		m_data.push_back( dyn_vector<T>(cols) );
	unsigned s = std::min(rows,cols);
	for( unsigned i=0; i<s; i++ )
		m_data[i][i] = d;
}

template<class T>
dyn_matrix<T>::dyn_matrix( const dyn_matrix<T>& m ) :
	m_data( m.m_data )
{}

template<class T>
dyn_matrix<T>::dyn_matrix( unsigned rows, unsigned cols, const T list[] ) :
	m_data()
{
	assert( rows>0 );
	assert( cols>0 );

	m_data.reserve( rows );
	for( unsigned i=0; i<rows; i++ )
	{
		dyn_vector<T> v(cols);
		for( unsigned j=0; j<cols; j++ )
			v[j] = list[i*cols+j];
		m_data.push_back( v );
	}
}

template<class T>
dyn_matrix<T>::~dyn_matrix()
{}

template<class T>
void dyn_matrix<T>::clear( const T s )
{
	for( unsigned i=0; i<rows(); i++ )
		m_data[i].clear(s);
}

template<class T>
void dyn_matrix<T>::diag( const T v )
{
	unsigned i=0;
	unsigned s = std::min(rows(),cols());
	for( ; i<s; i++ )
	{
		m_data[i].clear();
		m_data[i][i] = v;
	}
	while( i < rows() )
		m_data[i++].clear();
}

template<class T>
const dyn_vector<T> dyn_matrix<T>::col( unsigned j ) const
{
	assert(j<cols());
	dyn_vector<T> r(cols());
	for( unsigned i=0; i<rows(); i++ )
		r[i] = m_data[i][j];
	return r;
}
template<class T>
const dyn_vector<T>& dyn_matrix<T>::row( unsigned j ) const
{
	assert( j<rows() );
	return m_data[j];
}

template<class T>
const dyn_vector<T>& dyn_matrix<T>::operator[]( unsigned i ) const
{
	assert( i<rows() );
	return m_data[i];
}

template<class T>
dyn_vector<T>& dyn_matrix<T>::operator[]( unsigned i )
{
	assert( i<rows() );
	return m_data[i];
}

// Zuweisung
template<class T>
dyn_matrix<T>& dyn_matrix<T>::operator=( const dyn_matrix<T>& A )
{
	if( this != &A )
		m_data = A.m_data;
	return *this;
}

// product with scalar
template<class T>
dyn_matrix<T>& dyn_matrix<T>::operator*=( T s )
{
	for( unsigned i=0; i<rows(); i++ )
		m_data[i] *= s;
	return *this;
}

template<class T>
dyn_matrix<T>  dyn_matrix<T>::operator*( T s ) const
{
	dyn_matrix<T> r(*this);
	r *= s;
	return r;
}

// matrix product
// MxN + NxM = MxM
template<class T>
dyn_matrix<T> dyn_matrix<T>::operator* ( const dyn_matrix<T>& A ) const
{
	assert( cols() == A.rows() );
	assert( rows() == A.cols() );

	dyn_matrix<T> m(rows(),rows());

	for( unsigned i=0; i<rows(); i++ )
		for( unsigned j=0; j<rows(); j++ )
		{
			m[i][j] = 0;
			for( unsigned s=0; s<cols(); s++ )
				m[i][j] += m_data[i][s]*A[s][j];
		}
	return m;
}

// this only works for NxN matrices
template<class T>
dyn_matrix<T>& dyn_matrix<T>::operator*=( const dyn_matrix<T>& A )
{
	assert( cols() == rows() );
	assert( cols() == A.cols() );
	assert( rows() == A.rows() );

	dyn_matrix<T> m( *this );
	 for( unsigned i=0; i<rows(); i++ )
		for( unsigned j=0; j<rows(); j++ )
		{
			m_data[i][j] = 0;
			for( unsigned s=0; s<rows(); s++ )
				m_data[i][j] += m[i][s]*A[s][j];
		}
	return *this;
}

// return A*x
template<class T>
dyn_vector<T> dyn_matrix<T>::operator* ( const dyn_vector<T>& x ) const
{
	assert( x.dim() == cols() );
	
	dyn_vector<T> r(rows());
	for( unsigned i=0; i<rows(); i++ )
		r[i] = m_data[i]*x;
	return r;
}

// division by scalar
template<class T>
dyn_matrix<T>& dyn_matrix<T>::operator/=( T s )
{
	for( unsigned i=0; i<rows(); i++ )
		m_data[i]/=s;
	return *this;
}

template<class T>
dyn_matrix<T>  dyn_matrix<T>::operator/ ( T s ) const
{
	dyn_matrix<T> m(*this);
	m /= s;
	return m;
}

// sum
template<class T>
dyn_matrix<T>& dyn_matrix<T>::operator+=( const dyn_matrix<T> &A )
{
	for( unsigned i=0; i<rows(); i++ )
		m_data[i] += A[i];
	return *this;
}

template<class T>
dyn_matrix<T>  dyn_matrix<T>::operator+ ( const dyn_matrix<T> &A ) const
{
	dyn_matrix<T> m(*this);
	m += A;
	return m;
}

// diff
template<class T>
dyn_matrix<T>& dyn_matrix<T>::operator-=( const dyn_matrix<T>& A )
{
	for( unsigned i=0; i<rows(); i++ )
		m_data[i] -= A[i];
	return *this;
}

template<class T>
dyn_matrix<T>  dyn_matrix<T>::operator- ( const dyn_matrix<T> &A ) const
{
	dyn_matrix<T> m(*this);
	m -= A;
	return m;
}

template<class T>
dyn_matrix<T>  dyn_matrix<T>::operator- () const
{
	dyn_matrix<T> m(*this);
	for( unsigned i=0; i<rows(); i++ )
		for( unsigned j=0; j<cols(); j++ )
			m_data[i][j] = -m_data[i][j];
	return m;
}


// Transponierte
template<class T>
dyn_matrix<T> dyn_matrix<T>::transposed() const
{
	dyn_matrix<T> m(cols(),rows());
	for( unsigned i=0; i<rows(); i++ )
		for( unsigned j=0; j<cols(); j++ )
			m[j][i] = m_data[i][j];
	return m;
}

template<class T>
dyn_matrix<T>& dyn_matrix<T>::transpose()
{
	*this = this->transposed();
	return *this;
}

// Inverse
template<class T>
dyn_matrix<T> dyn_matrix<T>::inversed() const
{
	dyn_matrix<T> m(*this);
	return m.inverse();
}

template<class T>
dyn_matrix<T>& dyn_matrix<T>::inverse()
{
 dyn_matrix<T> A(*this); // Kopie
 this->diag(1);			 // mache this zur Einheitsmatrix
 dyn_vector<T> v(rows());

//	cout << "INVERSE:"<<endl;
//	cout << A << "-----------" << endl << *m << "==========" << endl;
	int s; // zu löschende Spalte
	// erster Schritt: erzeuge unter Dreiecksmatrix
	for( s=0; s<rows(); s++ )
	{
		if( A[s][s] == 0 )
		{
			int t=s+1;
			while( t<rows() )
			{
				if( A[t][s] != 0 )
				{
					A[s] += A[t];
					m_data[s] += m_data[t];
					break;
				}
				t++;
			}
			if( A[s][s] == 0 )
			{
				clear(0);
				break;
			}
//cout << A << "-----------" << endl << *m << "==========" << endl;
		}
			// 1. setze auf 1
		T c = A[s][s];
		cout << "A[" << s << "] /= " << c << endl;
		m_data[s] /= c; A[s] /= c;
//cout << A << "-----------" << endl << *m << "==========" << endl;
		// 2. addiere zum "nullen"
		for( int t=s+1; t<rows(); t++ )
		{//		0 1 2 4 -> 0  1   2    4
			T x = A[t][s]; // koeffizient		0 X Y Z -> 0 X-X Y-2X Z-4X
			if( x == 0 )
				continue;
			m_data[t] -= m_data[s]*x;
			A[t] -= A[s]*x;
//cout << A << "-----------" << endl << *m << "==========" << endl;
		}
	}
// cout << "// zweiter Schritt:  erzeuge von unten nach oben diagonalmatrix" << endl;
	for( s = rows()-1; s>0; s-- )
	{
		if( A[s][s] == 0 )
			break; // gibt keine Inverse!
		m_data[s] /= A[s][s];
		A[s] /= A[s][s];
//cout << A << "-----------" << endl << *m << "==========" << endl;
		for( int t=s-1; t>=0; t-- )
		{//		0 1 2 4 -> 0  1   2    4
			T x = A[t][s]; // koeffizient		0 X Y Z -> 0 X-X Y-2X Z-4X
			if( x == 0 ) // wenn schon null, Zeile überspringen
				continue;
			m_data[t] -= m_data[s]*x;
			A[t] -= A[s]*x;
		}
	}
	return *this;
}

// solve an LGS with a nonsingular NxN matrix
template<class T>
dyn_matrix<T> dyn_matrix<T>::solveLGS( const dyn_vector<T>& y ) const
{
 assert( cols()== rows() ); //erstmal keine über- und unterbestimmten LGS
 assert( y.dim() == rows() );
 dyn_matrix<T> m(*this);
 dyn_vector<T> b(y),x(y);
 unsigned rg = 0;

 // 0. beginne bei Spalte 0
	for( unsigned i=0,j=0; j<rows(); j++ )
	{
	// 1. Suche Zeile j mit Pivot Element a(ij)
		if( m[i][j] == 0 ) // nimm erste Zeile, ansonsten suche andere Zeile und addiere sie und nimm dann erste Zeile
		{
			for( unsigned k=i+1; k<rows(); k++ )
			{
				if( m[k][j] != 0 )
				{
					m[i] += m[k];
					b[i] += b[k];
					break;
				}
			}
			if( m[i][j] == 0 ) // nix, gefunden, ganze Spalte 0, nächste Spalte
				continue;
		}
		rg++; // rank+1;
	// 2. 'normiere' Zeile a(j)
		T a = m[i][j];
		m[i]/= a;
		b[i]/= a;
	// 3. lösche jedes a(kj) mit i!=k
		for( unsigned k=i+1; k<rows(); k++ )
		{
			T a_kj = m[k][j];
			if( a_kj != 0 )
			{
				m[k] -= m[i]*a_kj;
				b[k] -= b[i]*a_kj;
			}
		}
	// 4. gehe zur nächsten Zeile
		i++;
	}
//	cout << "Diese matrix mit Rang " << rg << " sollte jetzt in Dreiecksform sein: " << endl;
//	cout << *m << endl;
	// eindeutige Lösung
	if( rg == rows() )
	{
		for( int i=rows()-1; i>=0; i-- )
		{
			x[i] = b[i];
			for( unsigned k=i+1; k<rows(); k++ )
				x[i] -= m[i][k]*x[k];
			x[i] /= m[i][i];
		}
//		cout << "Lösungsvector: " << x << endl;
		m.clear(T(0));
		m[0] = x;
	}
 return m;
}

// rank
template<class T>
unsigned dyn_matrix<T>::rank() const
{
	// not yet implemented correctly
	assert( false );
	return std::min( cols(), rows() );
}

// determinant
template<class T>
T dyn_matrix<T>::det() const
{
	assert( cols()==rows() );
	if( rows() > 3 )
	{ // rekursiv ausrechnen
		T x = 0;
		int v = 1;
		for( int j=0; j<rows(); j++ )
		{
			T a = det( 0, j, rows() );
			x += v * d[0][j] * a;
			v *= -1;

		}
		return x;
	}
	else
	{
		T x = 0;
		if( cols() == 3 ) {
			x += d[0][0]*d[1][1]*d[2][2];
			x += d[0][1]*d[1][2]*d[2][0];
			x += d[0][2]*d[1][0]*d[2][1];
			x -= d[0][2]*d[1][1]*d[2][0];
			x -= d[0][1]*d[1][0]*d[2][2];
			x -= d[0][0]*d[1][2]*d[2][1];
		}
		else if( cols() == 2 ) {
			x = d[0][0]*d[1][1] - d[0][1]*d[1][0];
		}
		return x;
	}
	return d[0][0];
}

// sub-determinant
template<class T>
T dyn_matrix<T>::det( unsigned int i, unsigned int j, unsigned int n ) const
{
	assert( cols() == rows() );
	assert( n <= cols() );
	assert( i < n );
	assert( j < n );

	T x=0;
	dyn_matrix<T> m(*this); // kopie anlegen
	// Zeile i entfernen...
	for( int _z=0; _z<(n-1); _z++ )
	{
		if( _z >= i )
		{
			m[_z] = m[_z+1];
			m[_z+1].clear( 0 );
		}
	}
	// Spalte j entfernen
	for( int _s=0; _s<(n-1); _s++ )
	{
		if( _s >= j )
			for( int k=0; k<(n-1); k++ )
			{
				m[k][_s] = m[k][_s+1];
				m[k][_s+1] = 0;
			}
	}

	if( n > 4 )
	{ // induktiv ausrechnen
		// kopie fertig
		x = 0;
		T v = 1; // -1^1+j
		for( int _j=0; _j<(n-1); _j++ )
		{
			T a = m.det( 0, _j, n-1 );
			x += v * m[0][_j] * a;
			v *= -1;
		}
	}
	else
	{
		if( n == 4 )
		{ // rule by SARRUS
			x += (*m)[0][0]*(*m)[1][1]*(*m)[2][2];
			x += (*m)[0][1]*(*m)[1][2]*(*m)[2][0];
			x += (*m)[0][2]*(*m)[1][0]*(*m)[2][1];
			x -= (*m)[0][2]*(*m)[1][1]*(*m)[2][0];
			x -= (*m)[0][1]*(*m)[1][0]*(*m)[2][2];
			x -= (*m)[0][0]*(*m)[1][2]*(*m)[2][1];
		}
		else if( N == 2 )
		{
			x = (*m)[0][0]*(*m)[1][1] - (*m)[0][1]*(*m)[1][0];
		}
		else x = (*m)[0][0];
	}
	return x;
}

} // namespace

template<class T>
std::ostream& operator <<( std::ostream& str, const mtl::dyn_matrix<T>& m )
{
	std::ios_base::fmtflags f = str.flags();
	//str.flags( std::ios_base::right | std::ios_base::dec  );
	//str.width(10);

	for( unsigned i=0; i<m.rows(); i++ )
	{
		str << "|";
		for( unsigned j=0; j<m.cols()-1; j++ )
			str << m[i][j] << " ";
		str << m[i][m.cols()-1] << "|" << std::endl;
	}
	str.flags(f);
	return str;
}

