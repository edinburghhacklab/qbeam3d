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

#ifndef PLANE2DVIEW_H
#define PLANE2DVIEW_H

#include <qwidget.h>
#include <list>
#include <qapplication.h>

#include "Plane2DPoints.h"
//#include "UpdateEvents.h"

class QPainter;
class QPixmap;

class Plane2DView : public QWidget
{
   Q_OBJECT

public:
	Plane2DView(QWidget *parent=0, const char *name=0);
	~Plane2DView();

	void setOriginX( const int x );
	void setOriginY( const int y );

	int  originX() const { return _originX; }
	int  originY() const { return _originY; }

	void addFunction( Plane2DPoints* f );
	void removeFunction( Plane2DPoints* const f );

	void notifyChange() const;

protected:

	void calcMetrics();

	void drawGrid(QPainter& p) const;
	void drawAxis(QPainter& p) const;
	void drawPoints(QPainter& p) const;

	virtual void paintEvent( QPaintEvent *e );
	virtual void resizeEvent( QResizeEvent *e );

	// distance of to marks on axis
	double		_coordStepX;
	double		_coordStepY;

	//! position of the origin in paintSpace
	int			_originX;
	int			_originY;

	double		_scaleX;
	double		_scaleY;

	QRect		_drawRect;
	QPixmap*	_drawBuffer;
	// the function
	// y = f(x)
	std::list<Plane2DPoints*> _function_list;
};

#endif
