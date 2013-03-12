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

#ifndef MESHVIEW_H
#define MESHVIEW_H

#include <qwidget.h>

#include <GLView.h>

#include "Mesh.h"

/**
  *@author Maximilian Hrabowski
  */

class MeshView : public GLView  {
   Q_OBJECT
public:
  
	MeshView(const LockableMesh* mesh=0, QWidget *parent=0, const char *name=0);
	~MeshView();

	const LockableMesh*	mesh() const							{ return m_meshPtr; }
	void				setMesh( const LockableMesh* mesh=0 )	{ m_meshPtr = mesh; setDirty(); }

protected:

  void drawMesh( DRAW_MODE d );
   
  static bool prepareColor( float* cv, const Mesh::Color& c );
  
private:

  virtual void drawScene( DRAW_MODE d );

  const LockableMesh* m_meshPtr;
};

#endif
