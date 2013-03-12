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

#ifndef _MESHTOOLS_H_
#define _MESHTOOLS_H_

#include "Mesh.h"

class MeshTools {

public:

	template<typename T, typename ValIter>
	static void newton_interpolation( ValIter pBegin, ValIter pEnd, ValIter posBegin, ValIter posEnd )
	{
		if( (distance( pBegin, pEnd )>>1) > 2 );
		{
			// copy
			std::vector<T> c(pBegin,pEnd);
			
			ValIter k = c.begin()+2; // point to 2nd point
			ValIter n = c.end()-2;   // point to last point
			while( k <= n ) // 1..n
			{
				ValIter i = n;
				while( i >= k )
				{
					unsigned kk = (k-c.begin());
					T c;
					c  = *(i+1) - *(i-1);
					c /= *i - *(i-kk);
					*(i+1) = c;
					--i;
					--i;
				}
				++k;
				++k;
			}
			
			ValIter x = posBegin;
			while( x < posEnd-1 )
			{
				T p       = *(c.end()-1);	// p = c_n
				ValIter i = c.end()-4;		// i = n-1
				while( i >= c.begin() )		// i -> 0
				{
					p*= (*x-*i);
					p+= *(i+1);
					--i;
					--i;
				}
				*(x+1) = p;
				++x;
				++x;
			}
		}
	}

	template<typename VertexHandleIter>
	static void createLid( Mesh& mesh, VertexHandleIter begin, VertexHandleIter end, bool orientation )
	{
		double x=0.0,y=0.0,z=0.0;
		int count=0;
		VertexHandleIter iter = begin;
		VertexHandleIter end2 = end;

		//std::cout << "lid: " << std::endl;

		if( *(begin) == *(end2-1) )
			--end2;

		while( iter != end2 )
		{
			Mesh::Point p = mesh.point( *iter );
			x += p[0];
			y += p[1];
			z += p[2];
			++iter;
			count++;
			//std::cout << int(p[0]) << " " << int(p[1]) << " " << int(p[2]) << std::endl;
		}
		Mesh::Point center;
		x /= count;
		y /= count;
		z /= count;
		center[0] = x;
		center[1] = y;
		center[2] = z;

		//std::cout << "center: [" << count << "] - " << int(center[0]) << " " << int(center[1]) << " " << int(center[2]) << std::endl;

		Mesh::VertexHandle cvh = mesh.add_vertex(center);

		iter = begin;
		VertexHandleIter next= iter+1;
		while( next != end )
		{
			if( orientation )
				mesh.add_face( cvh, *next, *iter );
			else
				mesh.add_face( cvh, *iter, *next );
			++iter;
			++next;
		}
	}

	template<typename VertexHandleIter>
	static void triangulate( Mesh& mesh, VertexHandleIter leftBegin, VertexHandleIter leftEnd, VertexHandleIter rightBegin, bool smoothLowerBorder, bool smoothUpperBorder )
	{
		if( leftEnd-leftBegin > 1 )
		{
			VertexHandleIter leftBorderStart = leftEnd;
			VertexHandleIter rightBorderStart = leftEnd;
			VertexHandleIter leftBorderEnd = leftEnd;
			VertexHandleIter rightBorderEnd = leftEnd;
		
			VertexHandleIter left	= leftBegin+1;
			VertexHandleIter right	= rightBegin+1;

			while( left != leftEnd )
			{
				// beware orientation: counter clockwise vertices!
				// kommen in beiden Dreiecken vor, müssen also gültig sein
				/*	 ____
				//	|\  |
				//	| \ |
				//	|__\|
				*/
				if( left->is_valid() && 
					(right-1)->is_valid() )
				{
					if( (left-1)->is_valid() ) {
						mesh.add_face(*left,*(left-1),*(right-1) );
					}
					if( right->is_valid() ) {
						mesh.add_face(*left,*(right-1),*right );
					}
				}
				/*	 ____
				//	|  /|
				//	| / |
				//	|/__|
				*/
				else if( (left-1)->is_valid() &&
						 right->is_valid() )
				{
					if( left->is_valid() ) {
						mesh.add_face(*left,*(left-1),*right);
					}
					if( (right-1)->is_valid() ) {
						mesh.add_face(*(left-1),*(right-1),*right);
					}
				}
				// collect border coords for smoothing
				if( left->is_valid() ) {
					leftBorderEnd=left;
					if( leftBorderStart == leftEnd )
						leftBorderStart = left;
				}
				if( right->is_valid() ) {
					rightBorderEnd=right;
					if( rightBorderStart == leftEnd )
						rightBorderStart = right;
				}
				++left;
				++right;
			} // while
			
			// smooth borders: lower part
			if( smoothLowerBorder &&
				leftBorderStart != leftEnd &&
				rightBorderStart != leftEnd )
			{
				int l_idx = leftBorderStart-leftBegin;
				int r_idx = rightBorderStart-rightBegin;
				int dist  = l_idx - r_idx;
				if( dist <= -2 ) // case: /
				{
					dist = -dist;
					VertexHandleIter flt_left = leftBorderStart+(dist-1);
					while( flt_left > leftBorderStart ) {
						mesh.add_face(*flt_left,*(flt_left-1),*rightBorderStart);
						--flt_left;
					}
				}
				else if( dist >= 2 ) // case: 
				{
					VertexHandleIter flt_right = rightBorderStart+(dist-1);
					while( flt_right > rightBorderStart ) {
						mesh.add_face(*leftBorderStart,*(flt_right-1),*flt_right);
						--flt_right;
					}
				}
			}
			// smooth borders: upper part
			if( smoothUpperBorder && 
				leftBorderEnd != leftEnd && 
				rightBorderEnd != leftEnd )
			{
				int l_idx = leftBorderEnd-leftBegin;
				int r_idx = rightBorderEnd-rightBegin;
				int dist  = l_idx - r_idx;
				if( dist <= -2 ) // case: /
				{
					dist = -dist;
					VertexHandleIter flt_right = rightBorderEnd-(dist-1);
					while( flt_right < rightBorderEnd ) {
						mesh.add_face(*leftBorderEnd,*flt_right,*(flt_right+1));
						++flt_right;
					}
				}
				else if( dist >= 2 ) // case:
				{
					VertexHandleIter flt_left = leftBorderEnd-(dist-1);
					while( flt_left < leftBorderEnd ) {
						mesh.add_face(*(flt_left+1),*flt_left,*rightBorderEnd);
						++flt_left;
					}
				}
			}
		} // if
	}

	/** No descriptions */
	static Mesh::Point &calcNormal( Mesh::Point &n, Mesh::Point &v0, Mesh::Point &v1, Mesh::Point &v2);

	/** No descriptions */
	static double centerMesh( Mesh &mesh );

	/** No descriptions */
	static void calcVertexNormals( Mesh &mesh );

/*
	static bool importOFF(const std::string& _filename, OpenMesh::IO::BaseImporter& _bi);
	static bool importOFF(std::istream& _is, OpenMesh::IO::BaseImporter& _bi);
*/
};

#endif
