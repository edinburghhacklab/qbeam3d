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

#ifndef _MESH_GENERATOR_H_
#define _MESH_GENERATOR_H_

#include <vector>

#include "Mesh.h"
#include "MeshTools.h"
#include "L2000ScanOptions.h"

class MeshGenerator 
{
public:
	MeshGenerator( Mesh& mesh, const L2000::ScanOptions& o ) :
		m_firstHandles(),
		m_vHandles1(),
		m_vHandles2(),
		m_currentHandles(&m_vHandles1),
		m_lastHandles(&m_vHandles2),
		m_bottomBorder(),
		m_lidBorder(),
		m_options(o),
		m_mesh(mesh)
	{}
	
	void firstVertexCol( std::vector<L2000::Vertex>& vertices, const Mesh::Color& c );
	void addVertexCol( int rotateStep, std::vector<L2000::Vertex>& vertices, const Mesh::Color& c );
	void finishMesh( bool closeMesh, bool createLid, bool createBottom );
	
private:

	std::vector<Mesh::VertexHandle>  m_firstHandles;
	std::vector<Mesh::VertexHandle>  m_vHandles1;
	std::vector<Mesh::VertexHandle>  m_vHandles2;
	std::vector<Mesh::VertexHandle>* m_currentHandles;
	std::vector<Mesh::VertexHandle>* m_lastHandles;

	std::vector<Mesh::VertexHandle>  m_bottomBorder;
	std::vector<Mesh::VertexHandle>  m_lidBorder;

	L2000::ScanOptions		m_options;
	Mesh&				m_mesh;
};

#endif

