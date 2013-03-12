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

#ifndef GLVIEW_H
#define GLVIEW_H

//== INCLUDES =================================================================

#include <OpenMesh/Core/Math/VectorT.hh>
#include <qgl.h>
#include <string>
#include <vector>
#include <iostream>
#include <qcolor.h>

#include "LightSource.h"

class QPopupMenu;

class GLView : public QGLWidget
{

  Q_OBJECT

public:

  typedef enum { INVISIBLE=0, POINTS_ONLY=1, WIREFRAME, FACETS, FLAT_FACET, SMOOTH_FACET, DRAW_MAX } DRAW_MODE;

  // Default constructor.
  GLView( QWidget* _parent=0, const char* _name=0 );

  // Destructor.
  virtual ~GLView();

  /* Sets the center and size of the whole scene.
     The _center is used as fixpoint for rotations and for adjusting
     the camera/viewer (see view_all()). */
  void setScenePos( const OpenMesh::Vec3f& center, double radius );

  /* view the whole scene: the eye point is moved far enough from the
     center so that the whole scene is visible. */
  void viewAll();

  const QColor&   fgColor() const               { return _fgColor; }
  void            setFgColor( const QColor &c ) { _fgColor=c; }
  const QColor&   bgColor() const               { return _bgColor; }
  void            setBgColor( const QColor &c ) { _bgColor=c; }

  void		enableAllLightSources();
  void 		disableAllLightSources();
  void		enableLightSource( unsigned i );
  void		disableLightSource( unsigned i );
  unsigned	addLightSource( const LightSource &ls );
  void		removeLightSource( unsigned i );
  void		setupLightSources();
  
  void		setDirty(bool f=true) { m_dirty=f; }
  bool		dirty() const { return m_dirty; }

protected:

  // draw the scene: will be called by the painGL() method.
  virtual void drawScene( DRAW_MODE d );

public slots:

  void setDrawMode( int id );

private slots:

// popup menu clicked
  void slotPopupMenuDrawMode(int id);

private:

  // initialize OpenGL states (triggered by Qt)
  void initializeGL();

  // draw the scene (triggered by Qt)
  void paintGL();

  // handle resize events (triggered by Qt)
  void resizeGL( int w, int h );

  // Qt mouse events
  virtual void mousePressEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void wheelEvent( QWheelEvent* );
  virtual void keyPressEvent( QKeyEvent* );

  // updates projection matrix
  void updateProjectionMatrix();

  // translate the scene and update modelview matrix
  void translate(const OpenMesh::Vec3f& tVec);

  // rotate the scene (around its center) and update modelview matrix
  void rotate(const OpenMesh::Vec3f& axis, double angle);

  OpenMesh::Vec3f  _center;
  double           _radius;

  // Die 4x4 Matrizen für OpenGL
  GLdouble _projectionMatrix[16],
           _modelviewMatrix[16];

  // popup menu for draw mode selection
  QPopupMenu*   _popupMenu;
  DRAW_MODE     _drawMode;
  unsigned int  _numberOfDrawModes;

  // virtual trackball: map 2D screen point to unit sphere
  bool mapToSphere(const QPoint& p, OpenMesh::Vec3f& r);

  QPoint           _lastPoint2D;
  OpenMesh::Vec3f  _lastPoint3D;
  bool             _lastPointOk;

  QColor  _fgColor;
  QColor  _bgColor;
  
  bool				m_dirty;

  std::vector<LightSource> _lightSources;
  typedef std::vector<LightSource>::iterator lightIter;
  typedef std::vector<LightSource>::const_iterator constLightIter;

};

std::ostream& operator<<(std::ostream& out, GLView::DRAW_MODE m );

#endif
