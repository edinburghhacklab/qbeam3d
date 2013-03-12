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

#include "MeshGenerator.h"

#include <functional>

void MeshGenerator::addVertexCol( int rotateStep, std::vector<L2000::Vertex>& vertices,  const Mesh::Color& c )
{
	std::cout << "MeshGenerator::addVertexCol(): {";
	using namespace std;
	double rad = rotateStep*M_PI/360.0; // rotateStep*0.5*PI/180 = rotateStep*PI/360
	qDebug("# collected vertices: %d", vertices.size());
	qDebug("# handles in buffer : %d", m_currentHandles->size() );
	// invalidate current handles
	/*
	for_each( m_currentHandles->begin(),
		m_currentHandles->end(),
		mem_fun_ref(&Mesh::VertexHandle::invalidate)
	);
	*/
	//std::cout << "2: ScanThread::run() {" << std::endl;
	//m_mesh.lock();
	// add new vertices to mesh and store handle for triangulation
	//std::cout << "add to lid   : " << m_mesh.point(m_lidBorder.back()) << std::endl;
	m_lidBorder.push_back( Mesh::VertexHandle(-1) );
	bool bottomFlag	= false;
				
	for(std::vector<L2000::Vertex>::iterator viter=vertices.begin();viter != vertices.end(); ++viter )
	{
		Mesh::VertexHandle vh(-1);
		if( viter->valid() ) 
		{
			vh = m_mesh.add_vertex( Mesh::Point(	std::cos(rad)*viter->z, viter->y, std::sin(rad)*viter->z ));
			m_mesh.vertex(vh).ccd1		= viter->ccd1;
			m_mesh.vertex(vh).ccd2		= viter->ccd2;
			m_mesh.vertex(vh).laserPos	= viter->laserPos;
			m_mesh.vertex(vh).tablePos	= rotateStep;
			m_mesh.set_color(vh,c); 

			m_lidBorder.back() = vh;
			if( !bottomFlag )
			{
				m_bottomBorder.push_back( vh );
				bottomFlag=true;
				//std::cout << "add to bottom: " << m_mesh.point(vh) << std::endl;
			}
		}
		unsigned index = std::distance( vertices.begin(), viter );
		assert( index < m_currentHandles->size() );
		(*m_currentHandles)[index] = vh;
		std::cout << "Current Index: " << index << std::endl;
	}
	// triangulation
	std::cout << "triangulate(){";
	MeshTools::triangulate(	m_mesh,
				m_currentHandles->begin(),
				m_currentHandles->end(),
				m_lastHandles->begin(),
				m_options.smoothBorders || m_options.createBottom,
				m_options.smoothBorders || m_options.createLid);
	std::cout << "}";
	//m_mesh.unlock();
	//std::cout << "2 1: ScanThread::run()" << std::endl;
	// swap buffers for next run...
	swap( m_currentHandles, m_lastHandles );
	
	cout << "}";
}

void MeshGenerator::firstVertexCol( std::vector<L2000::Vertex>& vertices,  const Mesh::Color& c )
{
	using namespace std;
	// two vertex handle vectors for scanning....
	m_firstHandles.clear();
	m_vHandles1.clear();
	m_vHandles2.clear();
	m_currentHandles = &m_vHandles1;
	m_lastHandles = &m_vHandles2;

	m_bottomBorder.clear();
	m_lidBorder.clear();
	
	m_vHandles1.resize( vertices.size() );
	m_vHandles2.resize( vertices.size() );
	m_firstHandles.resize( vertices.size() );

	double rad = 0.0;
			//	qDebug("# collected vertices: %d", vertices.size());
			//	qDebug("# handles in buffer : %d", lastHandles->size() );
			// invalidate handles
	/*
	for_each(	m_lastHandles->begin(),
			m_lastHandles->end(),
			mem_fun_ref(&Mesh::VertexHandle::invalidate)
	);
	for_each(	m_firstHandles.begin(),
			m_firstHandles.end(),
			mem_fun_ref(&Mesh::VertexHandle::invalidate)
	);
	*/

	//std::cout << "1: ScanThread::run() {";
	//m_mesh.lock();
	// add new vertices to mesh and store handle for triangulation

	bool bottomFlag	= false;
	m_lidBorder.push_back( Mesh::VertexHandle(-1) );

	for(std::vector<L2000::Vertex>::iterator viter=vertices.begin();viter != vertices.end();++viter )
	{
		Mesh::VertexHandle vh(-1);
		if( viter->valid() ) 
		{
			vh = m_mesh.add_vertex( Mesh::Point(	std::cos(rad)*viter->z, viter->y, std::sin(rad)*viter->z ));
			m_mesh.vertex(vh).ccd1		= viter->ccd1;
			m_mesh.vertex(vh).ccd2		= viter->ccd2;
			m_mesh.vertex(vh).laserPos	= viter->laserPos;
			m_mesh.vertex(vh).tablePos	= 0;
			m_mesh.set_color(vh,c);
			
			// collect lid and bottom vertices
			m_lidBorder.back() = vh;
			if( !bottomFlag ) 
			{
				m_bottomBorder.push_back( vh );
				bottomFlag=true;
				//std::cout << "add to bottom: " << m_mesh.point(vh) << std::endl;
			}
		}
		unsigned index = std::distance( vertices.begin(), viter);
		assert( index < m_lastHandles->size() );
		(*m_lastHandles)[index] = vh;
		m_firstHandles[index] = vh;
		
		std::cout << "Current Index: " << index << std::endl;
	}
}

void MeshGenerator::finishMesh( bool closeMesh, bool createLid, bool createBottom )
{
	// last steps:
	// 1. triangulate between first and last row to close object
	if( closeMesh )
	{
		MeshTools::triangulate( m_mesh,
					m_firstHandles.begin(),
					m_firstHandles.end(),
					m_lastHandles->begin(),
					m_options.smoothBorders || m_options.createBottom,
					m_options.smoothBorders || m_options.createLid);
		// needed for closed lid
		m_bottomBorder.push_back( *m_bottomBorder.begin() );
		m_lidBorder.push_back( *m_lidBorder.begin() );
		//std::cout << "add to bottom: " << m_mesh.point(bottomBorder.back()) << std::endl;
		//std::cout << "add to lid   : " << m_mesh.point(lidBorder.back()) << std::endl;
	}
	if( createLid )
		MeshTools::createLid( m_mesh, m_lidBorder.begin(), m_lidBorder.end(), true );
	if( createBottom )
		MeshTools::createLid( m_mesh, m_bottomBorder.begin(), m_bottomBorder.end(), false );
}

