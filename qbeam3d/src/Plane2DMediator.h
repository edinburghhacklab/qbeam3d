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


#ifndef PLANE2DMEDIATOR_H
#define PLANE2DMEDIATOR_H

class Plane2DView;
class Plane2DObserver;

#include <list>

#include "Plane2DPoints.h"

class Plane2DMediator {
public: 
	Plane2DMediator(Plane2DView* view=0);
	~Plane2DMediator();

	void addPoints( Plane2DPoints* p );
	void removePoints( Plane2DPoints* p );

	void mediateChange( Plane2DPoints *p ) const;

	const Plane2DView* view() const { return _view; }
	void setView( Plane2DView* view );
	
private:

	Plane2DView* 				_view;
	std::list<Plane2DObserver>	_observer_list;
};

#endif
