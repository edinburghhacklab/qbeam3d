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

#ifndef _MESH_H_
#define _MESH_H_

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>

#include "L2000Reading.h"

#include "macros.h"

#include <iostream>
#include <string>
#include <cmath>
#include <limits>

#include <qmutex.h>

struct MyTraits : public OpenMesh::DefaultTraits
{
/*  Default Traits contents
	typedef Vec3f Point;
	VertexTraits    {};
  	HalfedgeTraits  {};
  	EdgeTraits      {};
  	FaceTraits      {};

  	VertexAttributes   (0);
  	HalfedgeAttributes (0);
  	EdgeAttributes     (0);
  	FaceAttributes     (0);
*/
	// double values
	typedef OpenMesh::Vec3d Point;
	// we want colors and normals
	VertexAttributes( OpenMesh::Attributes::Normal |
			  OpenMesh::Attributes::Color );
	// additional data used for each vertex
	VertexTraits {
		int		laserPos;
		int		tablePos;
		int		ccd1;
		int		ccd2;
	};
	// also previous Halfedge pointers
	HalfedgeAttributes(	OpenMesh::Attributes::PrevHalfedge );
// facets also have normals
	FaceAttributes(		OpenMesh::Attributes::Normal );

};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> Mesh;

class LockableMesh : public Mesh
{
public:
	LockableMesh() : Mesh(), m_mutex() {}

	void lock() const
	{
		//std::cout << "L";
		m_mutex.lock();
	}
	void unlock() const
	{
		m_mutex.unlock();
		//std::cout << "U";
	}
	bool locked() const
	{
		return m_mutex.locked();
	}
	bool tryLock() const
	{
		return m_mutex.tryLock();
	}

private:
	LockableMesh(const LockableMesh&);
	LockableMesh& operator=(const LockableMesh&);

	mutable QMutex	m_mutex;
};

static const Mesh::Point InvalidPoint( std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN());

//static const Mesh::Point InvalidVertex;
// Functors for meshes

struct rotate_y
{
	rotate_y( double alpha ) : m_alpha(alpha) {}
	Mesh::Point operator()( const Mesh::Point& p )
	{
		Mesh::Point r;
		r[0] = std::cos( m_alpha ) * p[2];
		r[1] = p[1];
		r[2] = std::sin( m_alpha ) * p[2];
		return r;
	}
	double m_alpha;
};
// add vertices from any container
struct add_vertex
{
	add_vertex( Mesh& mesh ) : m_mesh(mesh) {}
	void operator()( const Mesh::Point& p ) { m_mesh.add_vertex( p ); }
	Mesh& m_mesh;
};

// transform a L2000::Vertex to Mesh::Vertex,
// rotate it properly regarding tablePos
// also store custom properties for later triangulation
/*
struct add_scanned_vertex
{
	add_scanned_vertex( Mesh& mesh ) :
		m_mesh(mesh)
	{}
	Mesh::VertexHandle operator()( const L2000::Vertex& v )
	{
		Mesh::VertexHandle vh = m_mesh.add_vertex( Mesh::Point(0.0, v.y, v.z) );
		m_mesh.vertex(vh).ccd1 = v.ccd1;
		m_mesh.vertex(vh).ccd2 = v.ccd2;
		m_mesh.vertex(vh).laserPos = v.laserPos;
		m_mesh.vertex(vh).tablePos = 0;
		return vh;
	}
	Mesh::VertexHandle operator()( const L2000::Vertex& v, int tablePos )
	{
		double rad = tablePos*M_PI/360.0;
		Mesh::VertexHandle vh = m_mesh.add_vertex( Mesh::Point(	std::cos(rad)*v.z,
																v.y,
																std::sin(rad)*v.z ));
		m_mesh.vertex(vh).ccd1 = v.ccd1;
		m_mesh.vertex(vh).ccd2 = v.ccd2;
		m_mesh.vertex(vh).laserPos = v.laserPos;
		m_mesh.vertex(vh).tablePos = tablePos;
		return vh;
	}
	Mesh& m_mesh;
};
*/


#endif
