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

#include "Plane2DView.h"

#include <qpainter.h>
#include <qpixmap.h>

#include <map>
#include <limits>
#include <iterator>

#include <iostream>

#include "UpdateEvents.h"

Plane2DView::Plane2DView(QWidget *parent, const char *name )
:	QWidget(parent,name),
	_scaleX(1.0),
	_scaleY(1.0),
	_drawBuffer(0),
	_function_list()
{
	setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding) );
	_drawRect = rect();
	_originX = _drawRect.center().x();
	_originY = _drawRect.center().y();
	_coordStepX = 10.0;
	_coordStepY = 10.0;

	if( _drawRect.width() == 0 )
		_drawRect.setWidth(10);
	if( _drawRect.height() == 0 )
		_drawRect.setHeight(10);
	_drawBuffer = new QPixmap( _drawRect.size() );
}
Plane2DView::~Plane2DView()
{
	delete _drawBuffer;
}

void Plane2DView::addFunction( Plane2DPoints* f )
{
	_function_list.push_back( f );
}
void Plane2DView::removeFunction( Plane2DPoints* const f )
{
	_function_list.remove( f );
}

void Plane2DView::calcMetrics()
{
	_originX += (width()-_drawRect.width())/2;
	_originY += (height()-_drawRect.height())/2;


	QRect boundRect(_drawRect);

	for( std::list<Plane2DPoints*>::const_iterator it=_function_list.begin();
		 it!=_function_list.end();
		 ++it )
	{
		if( *it )
		{
			(*it)->lock();
			// find X min, X max, Y min Y max
			int maxX=-10000,maxY=-10000;
			int minX=10000,minY=10000;

			Plane2DPoints::point_iterator piter = (*it)->points_begin();
			for(; piter != (*it)->points_end(); ++piter)
			{
				int X = piter->first;
				int Y = piter->second;
				if( X < minX ) minX = X;
				if( Y < minY ) minY = Y;
				if( X > maxX ) maxX = X;
				if( Y > maxY ) maxY = Y;
			}
			//double unitX = double( maxX - minX ) / double( _drawRect.width() );
			//double unitY = double( maxY - minY ) / double( _drawRect.height() );
			//(*it)->setUnitX( unitX );
			//		(*it)->setUnitY( unitY );
			(*it)->unlock();
		}
	}

	_scaleX = 1.0;
	_scaleY = 1.0;
	_originX = 5;
	_originY = _drawRect.height()-5;
}

void Plane2DView::drawPoints( QPainter& p ) const
{
	for( std::list<Plane2DPoints*>::const_iterator it=_function_list.begin();
		 it!=_function_list.end();
		 ++it )
	{
		p.setPen( (*it)->color() );
		p.moveTo(0,0);
		// plot Function
		if( *it )
		{
			(*it)->lock();

			Plane2DPoints::point_iterator piter = (*it)->points_begin();
			for(; piter != (*it)->points_end(); ++piter)
			{
				double X = piter->first;
				double Y = piter->second;
				//std::cout << "[" << int(X) << ", " << int(Y) <<  "]" << std::endl;

				X += _originX;
				Y = _originY - Y;
				X /= _scaleX;
				Y /= _scaleY;
				p.drawPoint( int(X), int(Y) );
				p.drawPoint( int(X)+1, int(Y)+1 );
				p.drawPoint( int(X)+1, int(Y) );
				p.drawPoint( int(X), int(Y)+1 );
//				p.lineTo( int(X), int(Y) );
			}
			(*it)->unlock();
		}
	}
}
void Plane2DView::drawAxis( QPainter& p ) const
{
	p.setPen( black );
	p.drawLine( _drawRect.left(), _originY, _drawRect.right(), _originY);
	p.drawLine( _originX, _drawRect.top(), _originX, _drawRect.bottom() );

	// draw x-axis coords
	for( double x = 0.0; ; x+=_coordStepX )
	{
		// round
		int X = int( (ceil(x)-x)<(x-floor(x)) ? ceil(x) : floor(x) );
		// ok
		int X1 = _originX + X;
		int X2 = _originX - X;
		if( X1 < _drawRect.right() )
			p.drawLine( X1, _originY-2, X1, _originY+2 );
		if( X2 >= _drawRect.left() )
			p.drawLine( X2, _originY-2, X2, _originY+1 );
		if( X1 >= _drawRect.right() && X2<_drawRect.left() )
			break;
	}
	// draw y-axis coords
	for( double y = 0.0; ; y+=_coordStepY )
	{
		// round
		int Y = int( (ceil(y)-y)<(y-floor(y)) ? ceil(y) : floor(y) );
		// upside down paint, for y goes up!
		int Y2 = _originY + Y;
		int Y1 = _originY - Y;
		if( Y2 < _drawRect.bottom() )
			p.drawLine( _originX-2, Y2, _originX+2, Y2 );
		if( Y1 >= _drawRect.top() )
			p.drawLine( _originX-2, Y1, _originX+2, Y1 );
		if( Y2 >= _drawRect.bottom() && Y1<_drawRect.top() )
			break;
	}
}

void Plane2DView::drawGrid( QPainter& p ) const
{
	p.setPen( Qt::lightGray );
	// draw x-axis coords
	for( double x = 0.0; ; x+=_coordStepX )
	{
		// round
		int X = int( (ceil(x)-x)<(x-floor(x)) ? ceil(x) : floor(x) );
		int X1 = _originX + X;
		int X2 = _originX - X;
		if( X1 < _drawRect.width() )
			p.drawLine( X1, _drawRect.top(), X1, _drawRect.bottom() );
		if( X2 >= 0 )
			p.drawLine( X2, _drawRect.top(), X2, _drawRect.bottom() );
		if( X1 >= _drawRect.width() && X2<0 )
			break;
	}
	// draw y-axis coords
	for( double y = 0.0; ; y+=_coordStepY )
	{
		// round
		int Y = int( (ceil(y)-y)<(y-floor(y)) ? ceil(y) : floor(y) );
		int Y1 = _originY + Y;
		int Y2 = _originY - Y;
		if( Y1 < _drawRect.bottom() )
			p.drawLine( _drawRect.left(), Y1, _drawRect.right(), Y1 );
		if( Y2 >= _drawRect.top() )
			p.drawLine( _drawRect.left(), Y2, _drawRect.right(), Y2 );
		if( Y2 < _drawRect.top() && Y1 >= _drawRect.bottom() )
			break;
	}
}

void Plane2DView::paintEvent( QPaintEvent *e )
{
	e=e;
	calcMetrics();
	QPainter p(_drawBuffer);
	p.fillRect( _drawRect, colorGroup().background() );
//	drawGrid( p );
	drawAxis( p );
	drawPoints( p );
	p.end();
	bitBlt( this, 0, 0, _drawBuffer );
}

void Plane2DView::resizeEvent( QResizeEvent *e )
{
	_drawRect = rect();
	_drawBuffer->resize( size() );
	QWidget::resizeEvent( e );
}

void Plane2DView::notifyChange() const
{
	qApp->postEvent( qApp->mainWidget(), new Points2DChangeEvent() );
}

void Plane2DView::setOriginX( const int x )
{
	_originX = x;
}
void Plane2DView::setOriginY( const int y )
{
	_originY = _drawRect.height()-y;
}
