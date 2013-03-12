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

#include <LightSource.h>
#include <macros.h>
#include <qgl.h>

std::list<GLint> LightSource::_idPool = std::list<GLint>();

LightSource::LightSource(  const GLfloat* pos,
                const GLfloat* amb,
                const GLfloat* diff,
                const GLfloat* spec,
                const GLfloat* spot,
                GLfloat s_exp, GLfloat s_cut, GLfloat a_con, GLfloat a_lin, GLfloat a_quad )
  {
    createIdPool();
		DUP4( position, 0.0, 0.0, 1.0, 0.0 )
		DUP4( ambient,  0.0, 0.0, 0.0, 1.0 )
		DUP4( diffuse,  0.0, 0.0, 0.0, 1.0 )
		DUP4( specular, 0.0, 0.0, 0.0, 1.0 )
		DUP3( spot_dir, 0.0, 0.0, -1.0 )

    if( pos ) { DUP4( position, pos[0], pos[1], pos[2], pos[3] ) }
    if( amb ) { DUP4( ambient, amb[0], amb[1], amb[2], amb[4] ) }
    if( diff) { DUP4( diffuse,  diff[0], diff[1], diff[2], diff[3] ) }
    if( spec) { DUP4( specular, spec[0], spec[1], spec[2], spec[3] ) }
    if( spot) { DUP3( spot_dir, spot[0], spot[1], spot[2] ) }
		spot_exp = s_exp;
		spot_cut = s_cut;
		att_constant = a_con;
		att_linear = a_lin;
		att_quadratic = a_quad;
    _enabled = true;
    _id = nextId();
}

LightSource::~LightSource()
{
  _idPool.push_back( _id );
}

void LightSource::setup() const
{
    if( enabled() && _id != -1 )
    {
//		std::cerr << "setup light " << light << " =?= " <<  GL_LIGHT0 << std::endl;
//		std::cerr << "GL_AMBIENT  = " << ambient[0] << ", " << ambient[1] << ", " << ambient[2] << ", " << ambient[3] << std::endl;
//		std::cerr << "GL_DIFFUSE  = " << diffuse[0] << ", " << diffuse[1] << ", " << diffuse[2] << ", " << diffuse[3] << std::endl;
//		std::cerr << "GL_SPECULAR = " << specular[0] << ", " << specular[1] << ", " << specular[2] << ", " << specular[3] << std::endl;
//		std::cerr << "GL_POSITION = " << position[0] << ", " << position[1] << ", " << position[2] << ", " << position[3] << std::endl;
      glEnable( _id );
  		glLightfv( _id, GL_AMBIENT, ambient );
  		glLightfv( _id, GL_DIFFUSE, diffuse );
  		glLightfv( _id, GL_SPECULAR, specular );
  		glLightfv( _id, GL_POSITION, position );

  		glLightf( _id, GL_CONSTANT_ATTENUATION, att_constant );
  		glLightf( _id, GL_LINEAR_ATTENUATION, att_linear );
	  	glLightf( _id, GL_QUADRATIC_ATTENUATION, att_quadratic );

  		glLightf( _id, GL_SPOT_CUTOFF, spot_cut );
	  	glLightfv( _id, GL_SPOT_DIRECTION, spot_dir );
		  glLightf( _id, GL_SPOT_EXPONENT, spot_exp );
    }
}

void LightSource::setPosition( bool send, const GLfloat *pos )
{
  if( pos ) { DUP4( position, pos[0], pos[1], pos[2], pos[3] ) }
  if( send && _id != -1 ) glLightfv( _id, GL_POSITION, position );
}

void LightSource::createIdPool()
{
  static bool filled = false;
  if( !filled )
  {
      _idPool.push_back( GL_LIGHT0 );
      _idPool.push_back( GL_LIGHT1 );
      _idPool.push_back( GL_LIGHT2 );
      _idPool.push_back( GL_LIGHT3 );
      _idPool.push_back( GL_LIGHT4 );
      _idPool.push_back( GL_LIGHT5 );
      _idPool.push_back( GL_LIGHT6 );
      _idPool.push_back( GL_LIGHT7 );
      filled = true;
  }
}

GLint LightSource::nextId()
{
    GLint id;
    if( !_idPool.empty() )
    {
      id = _idPool.front();
      _idPool.pop_front();
    }
    else { id = -1; }
    return id;
}
