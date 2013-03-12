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

 #include "MeshTools.h"

 /** No descriptions */
Mesh::Point& MeshTools::calcNormal( Mesh::Point &n, Mesh::Point &v0, Mesh::Point &v1, Mesh::Point &v2)
{
	Mesh::Point x,y;

	x = v1 - v0;
	y = v2 - v0;
// cross product
	n[0] = x[1]*y[2] - x[2]*y[1];
	n[1] = x[2]*y[0] - x[0]*y[2];
	n[2] = x[0]*y[1] - x[1]*y[0];
// normalize

	double div = sqrt( n[0]*n[0] + n[1]*n[1] + n[2]*n[2] );
    n /= div;
	return n;
}

/** centers mesh araound origin, returns greatest distance from origin */
double MeshTools::centerMesh( Mesh &mesh )
{
		Mesh::Point v;
		double	minX=1000000.0,
				maxX=-1000000.0,
				minY=1000000.0,
				maxY=-1000000.0,
				minZ=1000000.0,
				maxZ=-1000000.0;
	// find bounding box
	for( Mesh::VertexIter vi = mesh.vertices_begin(); vi != mesh.vertices_end(); ++vi )
	{
		v = mesh.point( vi );
		MIN( minX, v[0] )	MIN( minY, v[1] )	MIN( minZ, v[2] )
		MAX( maxX, v[0] )	MAX( maxY, v[1] )	MAX( maxZ, v[2] )
	}
	// determine center add
	v[0] = -minX - ((maxX - minX)/2.0);
	v[1] = -minY - ((maxY - minY)/2.0);
	v[2] = -minZ - ((maxZ - minZ)/2.0);

	// adjust
	for( Mesh::VertexIter vi = mesh.vertices_begin(); vi != mesh.vertices_end(); ++vi )
	{
		Mesh::Point x = mesh.point( vi );
		x += v;
		mesh.set_point( vi, x );
	}

  Mesh::Point v1( maxX, maxY, maxZ );
  Mesh::Point v2( minX, minY, minZ );

  v1+=v;
  v2+=v;

  double rad1 = sqrt( v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2] );
  //double rad2 = sqrt( v2[0]*v2[0]+v2[1]*v2[1]+v2[2]*v2[2] );
  return rad1;
}
/** No descriptions */
void MeshTools::calcVertexNormals( Mesh &mesh )
{
	mesh.update_normals();
/*
 if( mesh.has_vertex_normals() )
 {
	// for each vertex...
	for( Mesh::VertexIter vi = mesh.vertices_begin(); vi != mesh.vertices_end(); ++vi )
	{
		// calc and add face normals
		Mesh::Point normal;
		// calc the resulting vertex normal
		normal[0] = normal[1] = normal[2] = 0;
		int cnt = 0;

		for( Mesh::VertexFaceIter vfi = mesh.vf_iter(vi.handle()); vfi; ++vfi )
		{
			Mesh::Point v0,v1,v2,n;
			Mesh::HalfedgeHandle h = mesh.halfedge_handle( vfi.handle() );

			v0 = mesh.point( mesh.to_vertex_handle( h ) );
			v1 = mesh.point( mesh.to_vertex_handle( mesh.next_halfedge_handle( h ) ) );
			v2 = mesh.point( mesh.to_vertex_handle( mesh.prev_halfedge_handle( h ) ) );

			n = calcNormal( n, v0,v1,v2 );
			normal += n;
			cnt++;
		}

		if( cnt > 0 ) {
//			normal /= double(cnt);
			double length = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
			normal /= length;
		}
		else { std::cerr << "Vertex does not belong to any face!" << std::endl; }

		vi->set_normal( normal );
	}

 }
*/
}
