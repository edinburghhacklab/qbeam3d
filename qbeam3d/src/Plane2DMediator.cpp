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

#include "Plane2DMediator.h"
#include "Plane2DObserver.h"
#include "Plane2DPoints.h"
#include "Plane2DView.h"

Plane2DMediator::Plane2DMediator( Plane2DView* view )
: _view(view), _observer_list()
{
}

Plane2DMediator::~Plane2DMediator()
{
}

void Plane2DMediator::addPoints( Plane2DPoints* p )
{
	// suchen ob schon vorhanden
	std::list<Plane2DObserver>::iterator iter= _observer_list.begin();
	for(; iter != _observer_list.end(); ++iter )
	{
		if( p == iter->points() )
			break;
	}
	if( iter == _observer_list.end() )
	{
		_observer_list.push_back( Plane2DObserver( *this, *p ) );
		if( _view ) _view->addFunction( p );
	}
}

void Plane2DMediator::removePoints( Plane2DPoints* p )
{
	std::list<Plane2DObserver>::iterator iter= _observer_list.begin();
	for(; iter != _observer_list.end(); ++iter )
	{
		if( p == iter->points() ) {
			if( _view )
				_view->removeFunction( iter->points() );
			_observer_list.erase( iter );
			break;
		}
	}
}

void Plane2DMediator::setView( Plane2DView *view )
{
	// vom alten entfernen
	if( _view && _view != view )
	{
		std::list<Plane2DObserver>::iterator iter= _observer_list.begin();
		for(; iter != _observer_list.end(); ++iter )
			_view->removeFunction( iter->points() );
	}
	// zum neuen hinzufpgen
	_view = view;
	if( _view )
	{
		std::list<Plane2DObserver>::iterator iter= _observer_list.begin();
		for(; iter != _observer_list.end(); ++iter )
			_view->addFunction( const_cast<Plane2DPoints*>(iter->points()) );
	}
}

void Plane2DMediator::mediateChange( Plane2DPoints* /*p*/ ) const
{
	if( _view )
		_view->notifyChange();
}


