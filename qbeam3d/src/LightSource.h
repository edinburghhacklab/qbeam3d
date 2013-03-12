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

#ifndef _LIGHTSOURCE_
#define _LIGHTSOURCE_

#include <qgl.h>

#include "macros.h"
#include <list>

class LightSource {

public:

  LightSource(  const GLfloat* pos=0,
                const GLfloat* amb=0,
                const GLfloat* diff=0,
                const GLfloat* spec=0,
                const GLfloat* spot=0,
                GLfloat s_exp=0.0, GLfloat s_cut=180.0, GLfloat a_con=1.0, GLfloat a_lin=0.0, GLfloat a_quad=0.0 );
  ~LightSource();

	void setup() const;

  inline bool enabled() const         { return _enabled; }
  inline void enable( bool on=true )  { _enabled = on; }
  inline GLint id() const { return _id; }

  void setPosition( bool send=true, const GLfloat *pos=0 );
  
private:

	GLfloat position[4];

	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat spot_dir[3];
	GLfloat spot_exp;
	GLfloat spot_cut;
	GLfloat att_constant;
	GLfloat att_linear;
	GLfloat att_quadratic;

  GLint   _id;
  bool    _enabled;

  void createIdPool();
  static GLint nextId();
 
  static std::list<GLint> _idPool;
};

#endif
