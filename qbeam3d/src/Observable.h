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

#ifndef OBSERVABLE_H
#define OBSERVABLE_H

class Observer;
#include <list>

class Observable
{
public:
	Observable();
	virtual ~Observable();
	
	bool addObserver( Observer* const o );
	bool removeObserver( Observer* const o );
	
protected:

	virtual void updateObservers() const;

private:

	typedef std::list<Observer*> ObPtrList;
	typedef ObPtrList::iterator  ObPtrIter;
	typedef ObPtrList::const_iterator  constObPtrIter;
	
	ObPtrList _observerList;

	Observable( const Observable& );
	Observable& operator=(const Observable&);
};

#endif
