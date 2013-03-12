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

/***************************************************************************
                          Points2D.h  -  description

	Decorator for any STL-like container of std::pair<T,T>

	Points2D represents a set of points that could be drawn on a plane.
	The underlying representation of the points is implementation specific.
	Each display point is represented as an std::pair<T,T> when its drawn.
	The coord-transformation is done in this class
	This is told about the displayed unit size and can therefore do the right
	coordinate transform to get properly displayed

	The points are accessed only via iterators with two different aproaches

	1. iterate over all points
	2. iterate over all new points since last call

                             -------------------
 ***************************************************************************/

#ifndef _POINTS2D_H_
#define _POINTS2D_H_

/**
  *@author Maximilian Hrabowski
  */
#include <utility>
#include <list>
#include <vector>
#include <map>
#include <set>

template<class CONT,class T=int>
class Points2D
{
public:

	typedef typename std::pair<T,T> POINT;

	typedef typename CONT::value_type value_type;
	typedef typename CONT::reference reference;
	typedef typename CONT::const_reference const_reference;
	typedef typename CONT::pointer pointer;
	typedef typename CONT::const_pointer const_pointer;

	typedef CONT container_type;

	class iterator
	{
		friend class Points2D<CONT,T>;
	public:
		iterator() : _iter() {}
		iterator(const iterator& it) : _iter(it._iter) {}
		iterator& operator=(const iterator& it) { _iter = it._iter; }
		bool operator!=(const iterator& it) const	{ return _iter != it._iter; }
		bool operator==(const iterator& it) const	{ return _iter == it._iter; }
		typename CONT::reference operator*()		{ return *_iter; }
		typename CONT::pointer   operator->()		{ return &(*_iter); }
		iterator  operator++(int)	{ ++_iter; return *this; } // postfix
		iterator& operator++()		{ ++_iter; return *this; } // prefix
	private:
		iterator(const typename CONT::iterator it) : _iter(it) {}
		typename CONT::iterator	_iter;
	};
//	template<class C,class S=int>
	class const_iterator
	{
//		typedef std::pair<S,S> POINT;
		friend class Points2D<CONT,T>;
	public:
		const_iterator() : _iter() {}
		const_iterator(const const_iterator& it) : _iter(it._iter) {}
		const_iterator& operator=(const const_iterator& it) { _iter = it._iter; return *this; }
		bool operator!=(const const_iterator& it) const	{ return _iter != it._iter; }
		bool operator==(const const_iterator& it) const	{ return _iter == it._iter; }
		typename CONT::reference operator*()			{ return *_iter; }
		typename CONT::pointer   operator->()		{ return &(*_iter); }
		const_iterator  operator++(int)	{ ++_iter; return *this; } // postfix
		const_iterator& operator++()	{ ++_iter; return *this; } // prefix
	private:
		const_iterator(const typename CONT::const_iterator it) : _iter(it) {}
		typename CONT::const_iterator	_iter;
	};

	class point_iterator
	{
		friend class Points2D<CONT,T>;
	public:
		point_iterator() : _cont(0), _iter() {}
		point_iterator( const point_iterator& it ) : _cont(it._cont), _iter(it._iter) {}
		point_iterator& operator=(const point_iterator& it) { _cont = it._cont; _iter = it._iter; }
		bool operator!=(const point_iterator& it) const	{ return _iter != it._iter; }
		bool operator==(const point_iterator& it) const	{ return _iter == it._iter; }
		const POINT& operator*()
		{
			if( _cont ) {
				_current = _cont->transform( *_iter );
			}
			return _current;
		}
		const POINT* operator->()
		{
			if( _cont ) {
				_current = _cont->transform( *_iter );
				return &_current;
			}
			return 0;
		}
		point_iterator  operator++(int)	{ if( _cont ) ++_iter; return *this; } // postfix
		point_iterator& operator++()	{ if( _cont ) ++_iter; return *this; } // prefix
	private:
		point_iterator( const Points2D<CONT,T>* c,
						const typename CONT::const_iterator it)
		: _cont(c),_iter(it), _current()
		{}

		const Points2D<CONT,T>*	_cont;
		typename CONT::const_iterator	_iter;
		POINT							_current;
	};

	Points2D()
	: _points(), _unitX(1.0), _unitY(1.0)
	{}
	Points2D( const CONT& data )
	: _points(data), _unitX(1.0), _unitY(1.0)
	{}
	Points2D(const	Points2D& obj)
	: _points(obj._points), _unitX(obj._unitX), _unitX(obj._unitY)
	{}
	Points2D& operator=(const Points2D& obj)
	{
		_points = obj._points;
		_unitX = obj._unitY;
		_unitY = obj._unitY;
		return *this;
	}
	virtual ~Points2D() {}

	iterator begin()				{ return iterator(_points.begin()); }
	iterator end()					{ return iterator(_points.end()); }
	const_iterator begin() const	{ return const_iterator(_points.begin()); }
	const_iterator end() const		{ return const_iterator(_points.end()); }

	point_iterator points_begin() const { return point_iterator( this, _points.begin() ); }
	point_iterator points_end() const {	return point_iterator( this, _points.end() ); }
/*
	iterator nbegin();
	iterator nend();
	const_iterator nbegin() const;
	const_iterator nend() const;
*/
	const CONT& container() const { return _points; }
	CONT& container() { return _points; }

	void setUnit( double u ) { _unitY = _unitX = u; }
	void setUnitX( double u ) { _unitX = u; }
	void setUnitY( double u ) { _unitY = u; }
	double unit() const { return _unitX; }
	double unitX() const { return _unitY; }
	double unitY() const { return _unitX; }

	void clear() { _points.clear(); }
private:
	CONT		_points;

protected:

	virtual POINT transform( const typename CONT::value_type& val ) const
	{
		double x = val.first; x /= _unitX;
		double y = val.second; y /= _unitY;
		return POINT( T(x), T(y) );
	}

	// unit size in plot
	// pixel size
	double		_unitX;
	double		_unitY;
};

#endif
