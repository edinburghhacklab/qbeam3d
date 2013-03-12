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

#include "GLView.h"

#include <iostream>
#include <cmath>

#include <qnamespace.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qcursor.h>

#include <GL/glut.h>

#if !defined(M_PI)
#  define M_PI 3.141592
#endif

using namespace OpenMesh;

GLView::GLView( QWidget* _parent, const char* _name )
  : QGLWidget( _parent, _name ),
  _lightSources(),
  m_dirty(true)
{
  // qt stuff
  setBackgroundMode( NoBackground );
  setFocusPolicy(QWidget::StrongFocus);
  setAcceptDrops( true );
  setCursor(pointingHandCursor);

  // setup colors
  _fgColor = paletteForegroundColor();
  _bgColor = paletteBackgroundColor();
  
  // popup menu
  _popupMenu = new QPopupMenu(this, tr("Render Modus Menu"));
  _popupMenu->setCheckable(true);
  // init draw modes
  _popupMenu->insertItem(tr("Draw Modes"));
  _popupMenu->insertSeparator();
  _popupMenu->insertItem(tr("Punkte"), this, SLOT(slotPopupMenuDrawMode(int)), 0, POINTS_ONLY );
  _popupMenu->insertItem(tr("Netz"), this, SLOT(slotPopupMenuDrawMode(int)), 0, WIREFRAME );
  _popupMenu->insertItem(tr("Facetten"), this, SLOT(slotPopupMenuDrawMode(int)), 0, FACETS );
  _popupMenu->insertItem(tr("Flat Shading"), this, SLOT(slotPopupMenuDrawMode(int)), 0, FLAT_FACET );
  _popupMenu->insertItem(tr("Smooth Shading"), this, SLOT(slotPopupMenuDrawMode(int)), 0, SMOOTH_FACET );
  slotPopupMenuDrawMode( WIREFRAME );

    GLfloat pos[4]       = { 100.0, 100.0, 100.0, 0.0 };
    GLfloat amb[4]       = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat diff[4]      = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat spec[4]      = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat spot[3]      = { 1.0, 1.0, 1.0 };
//addLightSource( LightSource( pos, amb, diff, spec, spot, s_exp, s_cut, a_con, a_lin, a_quad ) );
    addLightSource( LightSource( pos, amb, diff, spec, spot ) );
}

GLView::~GLView()
{
}

void GLView::enableAllLightSources()
{
  for( lightIter lIt = _lightSources.begin(); lIt!=_lightSources.end(); lIt++ )
    lIt->enable( true );
}
void GLView::disableAllLightSources()
{
  for( lightIter lIt = _lightSources.begin(); lIt!=_lightSources.end(); lIt++ )
    lIt->enable( false );
}
void GLView::enableLightSource( unsigned i )
{
  for( lightIter lIt = _lightSources.begin(); lIt!=_lightSources.end(); lIt++ )
  {
    if( lIt->id() == GLint(i) )
      lIt->enable( true );
  }
}

void GLView::disableLightSource( unsigned i )
{
  for( lightIter lIt = _lightSources.begin(); lIt!=_lightSources.end(); lIt++ )
  {
    if( lIt->id() == GLint(i) )
      lIt->enable( false );
  }
}

unsigned GLView::addLightSource( const LightSource &ls )
{
  if( ls.id() != -1 )
    _lightSources.push_back( ls );
  return _lightSources.size()-1;
}

void GLView::removeLightSource( unsigned i )
{
  for( lightIter lIt = _lightSources.begin(); lIt!=_lightSources.end(); lIt++ )
  {
    if( lIt->id() == GLint(i) )
    {
      lIt->enable(false);
      _lightSources.erase( lIt );
    }
  }
}

void GLView::setupLightSources()
{
  for( constLightIter lIt = _lightSources.begin(); lIt != _lightSources.end(); lIt++ )
  {
    lIt->setup();
  }
}

void GLView::initializeGL()
{
  // OpenGL state
  glClearColor( GLdouble(_bgColor.red())/255.0,
                GLdouble(_bgColor.green())/255.0,
                GLdouble(_bgColor.blue())/255.0, 0.0);
  glDisable( GL_DITHER );
  glEnable( GL_DEPTH_TEST );

 // material
 
	GLfloat mat_a[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat mat_d[] = {0.8, 0.8, 0.8, 1.0};
	GLfloat mat_s[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat shine[] = {0.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_a);	
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_s);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);

	// lighting  
  glLoadIdentity();

  GLfloat pos1[] = { 100.0, 50.0, 0.0, 0.0};
  GLfloat col1[] = {1.0, 1.0, 1.0, 1.0};
  
  GLfloat pos2[] = { -100.0, 50.0, 0.0, 0.0};
  GLfloat col2[] = { 1.0, 1.0, 1.0, 1.0};
  
  GLfloat pos3[] = { 0.0, 150.0, 0.0, 0.0};
  GLfloat col3[] = {1.0, 1.0, 1.0, 1.0};

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0,GL_POSITION, pos1);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,  col1);
  glLightfv(GL_LIGHT0,GL_SPECULAR, col1);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1,GL_POSITION, pos2);
  glLightfv(GL_LIGHT1,GL_DIFFUSE,  col2);
  glLightfv(GL_LIGHT1,GL_SPECULAR, col2);

  glEnable(GL_LIGHT2);
  glLightfv(GL_LIGHT2,GL_POSITION, pos3);
  glLightfv(GL_LIGHT2,GL_DIFFUSE,  col3);
  glLightfv(GL_LIGHT2,GL_SPECULAR, col3);

  // Fog
  /*
  GLfloat fogColor[4] = { 0.4, 0.4, 0.5, 1.0 };
  glFogi(GL_FOG_MODE,    GL_LINEAR);
  glFogfv(GL_FOG_COLOR,  fogColor);
  glFogf(GL_FOG_DENSITY, 0.35);
  glHint(GL_FOG_HINT,    GL_DONT_CARE);
  glFogf(GL_FOG_START,    5.0f);
  glFogf(GL_FOG_END,     25.0f);
  */
  
  // scene pos and size
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glGetDoublev(GL_MODELVIEW_MATRIX, _modelviewMatrix );
  setScenePos(Vec3f(0.0, 0.0, 0.0), 1.0);

}

void  GLView::resizeGL( int _w, int _h )
{
  updateProjectionMatrix();
  glViewport(0, 0, _w, _h);
  updateGL();
}

void GLView::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixd( _projectionMatrix );
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixd( _modelviewMatrix );
  drawScene(_drawMode );
}

void GLView::drawScene( DRAW_MODE d )
{
  switch( d )
  {
    case POINTS_ONLY:
	case WIREFRAME:
	case FACETS:
            glDisable(GL_LIGHTING);
            break;
    case FLAT_FACET:
			glEnable(GL_LIGHTING);
			glShadeModel(GL_FLAT);
			break;
	case SMOOTH_FACET:
			glEnable(GL_LIGHTING);
			glShadeModel(GL_SMOOTH);
    default:
            break;
  }
}

void GLView::mousePressEvent( QMouseEvent* e )
{
  // popup menu
  if (e->button() == RightButton)
  {
    _popupMenu->exec(QCursor::pos());
  }

  else
  {
    _lastPointOk = mapToSphere( _lastPoint2D = e->pos(), _lastPoint3D );
  }
}

void GLView::mouseMoveEvent( QMouseEvent* e )
{
  QPoint newPoint2D = e->pos();

  if ( (newPoint2D.x()<0) || (newPoint2D.x()>width()) ||
       (newPoint2D.y()<0) || (newPoint2D.y()>height()) )
       return;

  // Left button: rotate around _center
  // Middle button: translate object
  // Left & middle button: zoom in/out

  float  value_y;
  Vec3f  newPoint3D;
  bool   newPoint_hitSphere = mapToSphere( newPoint2D, newPoint3D );

  float dx = newPoint2D.x() - _lastPoint2D.x();
  float dy = newPoint2D.y() - _lastPoint2D.y();

  float w  = width();
  float h  = height();

  // enable GL context
  makeCurrent();

  // move in z direction
  if ( (e->state() & LeftButton) && (e->state() & MidButton))
  {
    value_y = _radius * dy * 3.0 / h;
    translate(Vec3f(0.0, 0.0, value_y));
  }


  // move in x,y direction
  else if (e->state() & MidButton)
  {
    float z =
    -(_modelviewMatrix[ 2]*_center[0] + _modelviewMatrix[ 6]*_center[1] + _modelviewMatrix[10]*_center[2] + _modelviewMatrix[14])
    /(_modelviewMatrix[ 3]*_center[0] + _modelviewMatrix[ 7]*_center[1] + _modelviewMatrix[11]*_center[2] + _modelviewMatrix[15]);

    float fovy       = 45.0f;
    float aspect     = w / h;
    float near_plane = 0.01 * _radius;
    float top        = tan(fovy/2.0f*M_PI/180.0f) * near_plane;
    float right      = aspect*top;

    translate(Vec3f(2.0*dx/w*right/near_plane*z, -2.0*dy/h*top/near_plane*z, 0.0f));
  }

  // rotate
  else if (e->state() & LeftButton)
  {
    Vec3f axis;
    float angle;

    if(_lastPointOk)
    {
      if (newPoint_hitSphere = mapToSphere(newPoint2D, newPoint3D))
      {
      	axis = _lastPoint3D % newPoint3D;
      	float cos_angle = (_lastPoint3D | newPoint3D);
      	if ( fabs(cos_angle) < 1.0 )
      	{
      	  angle = acos( cos_angle ) * 180.0 / M_PI;
       	  angle *= 2.0;
      	}
      }
    }
    rotate( axis, angle );
  }

  // remember this point
  _lastPoint2D = newPoint2D;
  _lastPoint3D = newPoint3D;
  _lastPointOk = newPoint_hitSphere;

  // trigger redraw
  std::cout << "UpdateGL()";
  updateGL();
}

void GLView::mouseReleaseEvent( QMouseEvent* _event )
{
   _lastPointOk = false;
}

void GLView::wheelEvent(QWheelEvent* e)
{
  // Use the mouse wheel to zoom in/out
  float d = -(float)e->delta() / 120.0 * 0.2 * _radius;
  translate(Vec3f(0.0, 0.0, d));
  updateGL();
  e->accept();
}

void GLView::keyPressEvent( QKeyEvent* e )
{
   e->ignore();
}

void GLView::translate( const Vec3f& t )
{
  // Translate the object by _trans
  // Update _modelviewMatrix
  makeCurrent();
  glLoadIdentity();
  glTranslated( t[0], t[1], t[2] );
  glMultMatrixd( _modelviewMatrix );
  glGetDoublev( GL_MODELVIEW_MATRIX, _modelviewMatrix);
}

void GLView::rotate( const Vec3f& axis, double angle )
{
  // Rotate around center _center, axis _axis, by angle _angle
  // Update _modelviewMatrix

  Vec3f t( _modelviewMatrix[0]*_center[0] +
	   _modelviewMatrix[4]*_center[1] +
	   _modelviewMatrix[8]*_center[2] +
	   _modelviewMatrix[12],
	   _modelviewMatrix[1]*_center[0] +
	   _modelviewMatrix[5]*_center[1] +
	   _modelviewMatrix[9]*_center[2] +
	   _modelviewMatrix[13],
	   _modelviewMatrix[2]*_center[0] +
	   _modelviewMatrix[6]*_center[1] +
	   _modelviewMatrix[10]*_center[2] +
	   _modelviewMatrix[14] );

  makeCurrent();
  glLoadIdentity();
  glTranslatef(t[0], t[1], t[2]);
  glRotated( angle, axis[0], axis[1], axis[2]);
  glTranslatef(-t[0], -t[1], -t[2]);
  glMultMatrixd(_modelviewMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX, _modelviewMatrix);
}

bool GLView::mapToSphere( const QPoint& _v2D, Vec3f& _v3D )
{
  if ( (_v2D.x() >= 0) && (_v2D.x() <= width()) &&
       (_v2D.y() >= 0) && (_v2D.y() <= height()) )
  {
    double x            = (double)(_v2D.x() - 0.5*width())  / (double)width();
    double y            = (double)(0.5*height() - _v2D.y()) / (double)height();
    double sinx         = sin(M_PI * x * 0.5);
    double siny         = sin(M_PI * y * 0.5);
    double sinx2siny2   = sinx * sinx + siny * siny;

    _v3D[0] = sinx;
    _v3D[1] = siny;
    _v3D[2] = sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0;

    return true;
  }
  else return false;
}

void GLView::updateProjectionMatrix()
{
  makeCurrent();
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  
  gluPerspective(45.0, (GLfloat) width() / (GLfloat) height(), 0.01*_radius, 100.0*_radius);
  //
  // f/(width/height) 0 0 0
  // 0 f 0 0
  // 0 0  a b
  // 0 0 -1 0

//  GLdouble matrix[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
//  double f = cos( (45.0/2.0)*M_PI/180.0 )/sin( (45.0/2.0)*M_PI/180.0 );
//  double a = (100.01*_radius)/((0.01-100.0)*_radius);
//  double b = (2.0*100.0*0.01*_radius*_radius)/((0.01-100.0)*_radius);
//  matrix[0] = (GLdouble)f*height()/width();
//  matrix[5] = (GLdouble)f;
//  matrix[10] = (GLdouble)a;
//  matrix[11] = (GLdouble)b;
//  matrix[14] = -1.0;
//  glMultMatrixd( matrix );

  glGetDoublev( GL_PROJECTION_MATRIX, _projectionMatrix);
  glMatrixMode( GL_MODELVIEW );
}

void GLView::viewAll()
{
  translate( Vec3f(
  -(_modelviewMatrix[0]*_center[0] + _modelviewMatrix[4]*_center[1] + _modelviewMatrix[8]*_center[2] + _modelviewMatrix[12]),
  -(_modelviewMatrix[1]*_center[0] + _modelviewMatrix[5]*_center[1] + _modelviewMatrix[9]*_center[2] + _modelviewMatrix[13]),
  -(_modelviewMatrix[2]*_center[0] + _modelviewMatrix[6]*_center[1] + _modelviewMatrix[10]*_center[2] + _modelviewMatrix[14] + 3.0*_radius) ) );
}

void GLView::setScenePos( const Vec3f& cog, double radius )
{
  _center = cog;
  _radius = radius;
  updateProjectionMatrix();
  viewAll();
}

/*
void GLView::add_draw_mode(const QString& _s)
{
  ++n_draw_modes_;

  // insert in popup menu
  popup_menu_->insertItem(_s, n_draw_modes_);
  // store draw mode
  draw_mode_names_.push_back(_s);
}
*/

void GLView::setDrawMode( int id )
{
  if( id >= POINTS_ONLY && id < DRAW_MAX )
  {
    slotPopupMenuDrawMode(id);
  }
}

void GLView::slotPopupMenuDrawMode(int id)
{
  // un-check all entries
  for (int i = POINTS_ONLY; i < DRAW_MAX; i++)
    _popupMenu->setItemChecked(int(i), false);
  // save draw mode
  _drawMode = DRAW_MODE(id);
  _popupMenu->setItemChecked(id, true);

  std::cerr << "New Draw Mode: " << _drawMode << std::endl;
  
  setDirty();
}

std::ostream& operator<<(std::ostream& out, GLView::DRAW_MODE m )
{
  switch( m )
  {
    case GLView::POINTS_ONLY: out << "POINTS_ONLY"; break;
    case GLView::WIREFRAME:   out << "WIREFRAME"; break;
	case GLView::FACETS:	  out << "FACETS"; break;
    case GLView::FLAT_FACET:  out << "FLAT_FACET"; break;
    case GLView::SMOOTH_FACET:out << "SMOOTH_FACET"; break;
    default:
      out << "ILLEGAL DRAW_MODE";
  }
  return out;
}


