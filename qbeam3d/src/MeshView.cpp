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

#include "MeshView.h"

MeshView::MeshView(const LockableMesh* mesh, QWidget *parent, const char *name )
: GLView(parent,name),
  m_meshPtr(mesh)
{
}

MeshView::~MeshView()
{
}

bool MeshView::prepareColor( float* cv, const Mesh::Color& c )
{
	double r = c[0]; r /= 255.0;
	double g = c[1]; g /= 255.0;
	double b = c[2]; b /= 255.0;
	if( cv[0] != r || cv[1] != g || cv[2] != b ) 
	{
		cv[0] = r; 
		cv[1] = g;
		cv[2] = b;
		return true;
	}
	return false;
}

void MeshView::drawMesh( DRAW_MODE d )
{
	if( m_meshPtr )
	{
		std::cout << "MeshView::drawMesh()" << std::endl;

		m_meshPtr->lock();

		Mesh::ConstFaceIter fIt(m_meshPtr->faces_begin()),
				    fEnd(m_meshPtr->faces_end());
		Mesh::ConstFaceVertexIter fvIt;
		Mesh::ConstVertexIter vIt(m_meshPtr->vertices_begin());

		float color[4] = { 1.0, 1.0, 1.0, 1.0 };
		
		switch( d )
		{
			case POINTS_ONLY:
					glBegin(GL_POINTS);
					glColor3fv(color);
					for(; vIt!=m_meshPtr->vertices_end(); ++vIt) 
					{
						if( prepareColor( color, m_meshPtr->color(vIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(vIt));
					}
					glEnd();
					break;
			case WIREFRAME:
					for (; fIt!=fEnd; ++fIt)
					{
						glBegin(GL_LINE_STRIP);
						glColor3fv(color);
						fvIt = m_meshPtr->cfv_iter(fIt.handle());
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
						++fvIt;
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
						++fvIt;
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
						glEnd();
					}
					break;
			case FACETS:
			case FLAT_FACET:
					glBegin(GL_TRIANGLES);
					glColor3fv(color);
					for (; fIt!=fEnd; ++fIt)
					{
						glNormal3fv( m_meshPtr->normal( fIt.handle() ) );
						fvIt = m_meshPtr->cfv_iter(fIt.handle());
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
						++fvIt;
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
						++fvIt;
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
					}
					glEnd();
					break;
			case SMOOTH_FACET:
					glBegin(GL_TRIANGLES);
					glColor3fv(color);
					for (; fIt!=fEnd; ++fIt)
					{
						fvIt = m_meshPtr->cfv_iter(fIt.handle());
						glNormal3fv(m_meshPtr->normal(fvIt.handle()) );
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
						++fvIt;
						glNormal3fv(m_meshPtr->normal(fvIt.handle()));
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
						++fvIt;
						glNormal3fv(m_meshPtr->normal(fvIt.handle()));
						if( prepareColor( color, m_meshPtr->color(fvIt) ) )
							glColor3fv(color);
						glVertex3dv(m_meshPtr->point(fvIt));
					}
					glEnd();
					break;
			default:
					break;
		}
		//glFlush();
		m_meshPtr->unlock();
		setDirty(false);
		//std::cout << " } MeshView::drawMesh()" << std::endl;
	}
}

void MeshView::drawScene( DRAW_MODE d )
{
	GLView::drawScene(d);
	drawMesh( d );
}
