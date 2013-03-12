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

#ifndef _SCANTHREAD_H_
#define _SCANTHREAD_H_

#include <qthread.h>

#include "Mesh.h"

#include "L2000ScanOptions.h"

class QObject;

namespace L2000 {
	class	Scanner;
}

class ScanThread : public QThread
{
public:
	ScanThread( LockableMesh& mesh, L2000::Scanner& scanner, const L2000::ScanOptions& so,  QObject* observer, unsigned int stacksize=0 );
	virtual  ~ScanThread();

	const LockableMesh& mesh() const				{ return m_mesh; }
	const L2000::ScanOptions scanOptions() const	{ return m_scanOptions; }

	void cancel() { m_cancel = true; }

protected:

	void ScanThread::addVertexCol( std::vector<Mesh::VertexHandle>& handles, std::vector<L2000::Vertex>& vertices, const Mesh::Color& c );
	
	virtual void run();

private:

	LockableMesh&		m_mesh;

	L2000::Scanner&		m_scanner;

	L2000::ScanOptions	m_scanOptions;

	bool				m_cancel;

	QObject*			m_observer;
};

#endif

