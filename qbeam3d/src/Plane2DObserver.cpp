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

#include "Plane2DObserver.h"

Plane2DObserver::Plane2DObserver(const Plane2DMediator& m, Plane2DPoints& o)
: Observer(&o.container()), _med(m), _points(o)
{
}

Plane2DObserver::Plane2DObserver(const Plane2DObserver& ob)
: Observer(ob._observable), _med(ob._med), _points(ob._points)
{}

Plane2DObserver::~Plane2DObserver()
{}

Plane2DObserver& Plane2DObserver::operator=(const Plane2DObserver& ob)
{
	if( this != &ob )
	{
		_observable = ob._observable;
		_observable->addObserver( this );
		_points = ob._points;
	}
	return *this;
}

void Plane2DObserver::notifyChange()
{
	_med.mediateChange( &_points );
}


