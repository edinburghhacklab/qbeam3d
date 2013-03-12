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

#include "Observable.h"
#include "Observer.h"

#include <list>
#include <functional>
#include <algorithm>

Observable::Observable()
: _observerList()
{}

Observable::~Observable()
{
	std::for_each(	_observerList.begin(),
					_observerList.end(),
					std::mem_fun(&Observer::observableDestroyed) );
}

bool Observable::addObserver( Observer* const o )
{
	if( o )
	{
		ObPtrIter oiter = std::find( _observerList.begin(), _observerList.end(), o );
		if( oiter == _observerList.end() )
			_observerList.push_back( o );
	}
	return o != 0;
}

bool Observable::removeObserver( Observer* const o )
{
	if( o )
	{
 		_observerList.remove(o);
		return true;
	}
	return false;
}

void Observable::updateObservers() const
{
	std::for_each(	_observerList.begin(),
					_observerList.end(),
					std::mem_fun(&Observer::notifyChange) );
}



