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

#include "ScanThread.h"
#include "L2000Scanner.h"

#include "MeshTools.h"
#include "MeshGenerator.h"

#include <qapplication.h>
#include <qobject.h>

#include "UpdateEvents.h"


ScanThread::ScanThread( LockableMesh& mesh, L2000::Scanner& scanner, const L2000::ScanOptions& so, QObject* observer, unsigned int stacksize ) :
	QThread(stacksize),
	m_mesh(mesh),
	m_scanner(scanner),
	m_scanOptions(so),
	m_cancel(false),
	m_observer(observer)
{
}

ScanThread::~ScanThread()
{}

void ScanThread::addVertexCol(	std::vector<Mesh::VertexHandle>& /*handles*/,
				std::vector<L2000::Vertex>& /*vertices*/, 
				const Mesh::Color& /*c*/ )
{
}

void ScanThread::run()
{
	m_cancel = false;
	m_scanner.resetTable();
	m_scanner.resetLaser();
	m_scanner.setLaserOn(true);

	if( m_scanner.prepareScan( m_scanOptions ) )
	{
		if( m_cancel ) return;

		using namespace std;

		// current Scan Vertices
		vector<L2000::Vertex> vertices[3];
		vertices[0].reserve( m_scanOptions.laserRange() );
		vertices[1].reserve( m_scanOptions.laserRange() );
		vertices[2].reserve( m_scanOptions.laserRange() );
		
		MeshGenerator generator(m_mesh, m_scanOptions );
		MeshGenerator ccd1_generator(m_mesh, m_scanOptions);
		MeshGenerator ccd2_generator(m_mesh, m_scanOptions);

		double pos=0.0;
		if( m_scanOptions.tableStep < 1.0 )
		    m_scanOptions.tableStep = 1.0;

// scan the first row
		if( m_scanner.collectScanLine( vertices[0], m_scanOptions, vertices[1], vertices[2] ) )
		{
			//std::cout << vertices[0].size() << "<!=" << m_scanOptions.laserRange() << std::endl;
			assert( vertices[0].size() <= m_scanOptions.laserRange() );
			m_mesh.lock();
			generator.firstVertexCol( vertices[0], m_scanOptions.solidColor );
			if( m_scanOptions.createCCD1Phantom )
				ccd1_generator.firstVertexCol( vertices[1], m_scanOptions.ccd1Color );
			if( m_scanOptions.createCCD2Phantom )
				ccd2_generator.firstVertexCol( vertices[2], m_scanOptions.ccd2Color );
			m_mesh.update_normals();
			m_mesh.unlock();
		}
		pos+=m_scanOptions.tableStep;

		// update widgets
		if( m_observer )
			qApp->postEvent( m_observer, new MeshChangeEvent() );
		
// scan all the other rows and triangulate
		for( ; pos<m_scanOptions.tableRange; pos+=m_scanOptions.tableStep )
		{
			if( m_cancel ) 
				return;

			m_scanner.moveTableTo( int(pos) );
			vertices[0].clear();
			vertices[1].clear();
			vertices[2].clear();
			// rotate point and enter into Mesh

			if( m_scanner.collectScanLine( vertices[0], m_scanOptions, vertices[1], vertices[2] ) )
			{
				//std::cout << vertices[0].size() << "<!=" << m_scanOptions.laserRange() << std::endl;
				assert( vertices[0].size() <= m_scanOptions.laserRange() );
				m_mesh.lock();
				generator.addVertexCol( m_scanner.tablePos(), vertices[0], m_scanOptions.solidColor);
				if( m_scanOptions.createCCD1Phantom )
					ccd1_generator.addVertexCol( m_scanner.tablePos(), vertices[1], m_scanOptions.ccd1Color );
				if( m_scanOptions.createCCD2Phantom )
					ccd2_generator.addVertexCol( m_scanner.tablePos(), vertices[2], m_scanOptions.ccd2Color );
				
				m_mesh.update_normals();
				m_mesh.unlock();
			}

			// update widgets
			if( m_observer )
				qApp->postEvent( m_observer, new MeshChangeEvent() );
		}
		
		m_mesh.lock();
		generator.finishMesh( m_scanOptions.tableRange == 720, m_scanOptions.createLid, m_scanOptions.createBottom );
		if( m_scanOptions.createCCD1Phantom )
			ccd1_generator.finishMesh( m_scanOptions.tableRange == 720, m_scanOptions.createLid, m_scanOptions.createBottom );
		if( m_scanOptions.createCCD2Phantom )
			ccd2_generator.finishMesh( m_scanOptions.tableRange == 720, m_scanOptions.createLid, m_scanOptions.createBottom );
		m_mesh.update_normals();
		m_mesh.unlock();
			// update widgets
		if( m_observer )
			qApp->postEvent( m_observer, new MeshChangeEvent() );
	}
}
