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

#ifndef PLANE2DOBSERVER_H
#define PLANE2DOBSERVER_H

#include "Observer.h"
#include "Plane2DMediator.h"
#include "Plane2DPoints.h"

class Plane2DObserver : public Observer
{
public:

	Plane2DObserver(const Plane2DMediator& m, Plane2DPoints& o);
	Plane2DObserver(const Plane2DMediator& m );
	Plane2DObserver(const Plane2DObserver& ob);
	virtual ~Plane2DObserver();

	Plane2DObserver& operator=(const Plane2DObserver& ob);

	virtual void notifyChange();

	Plane2DPoints* points() { return &_points; }
	const Plane2DPoints* points() const { return &_points; }

private:

	const Plane2DMediator&	_med;
	Plane2DPoints&			_points;
};

#endif
