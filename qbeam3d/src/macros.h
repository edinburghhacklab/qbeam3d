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

#ifndef _MACRO_H_
#define _MACRO_H_

#include <iostream>

template <class T>
std::ostream& dump( std::ostream& out, const T& cont )
{
	typename T::const_iterator iter = cont.begin();
	for( ; iter != cont.end(); ++iter )
		out << *iter << ", ";
	out << std::endl;
	return out;
}
template <class T>
std::ostream& dump_int( std::ostream& out, const T& cont )
{
	typename T::const_iterator iter = cont.begin();
	for( ; iter != cont.end(); ++iter )
		out << std::hex << int(*iter) << ", ";
	out << std::endl;
	return out;
}

template <class T>
std::ostream& dump_int( std::ostream& out, const T* cont, unsigned n )
{
	for( unsigned i=0; i<n; i++ )
		out << std::hex << int(cont[i]) << ", ";
	out << std::endl;
	return out;
}



#define MIN(X,Y) if( X > Y ) X = Y;
#define MAX(X,Y) if( X < Y ) X = Y;

#define ABS( X ) if( X < 0 ) X = -X;

#define DUP( X, N, V ) for( int i=0; i<N; i++ ) X[i]=V;
#define DUP2( X, A, B ) X[0] = A; X[1] = B;
#define DUP3( X, A, B, C ) X[0] = A; X[1] = B; X[2] = C;
#define DUP4( X, A, B, C, D ) X[0] = A; X[1] = B; X[2] = C; X[3] = D;

#endif
