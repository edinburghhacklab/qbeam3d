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

#ifndef _OBSERVABLE_CONTAINER_H_
#define _OBSERVABLE_CONTAINER_H_

#include "Observable.h"

#include <list>
#include <vector>

namespace Observables {
	
template<class T, class A=std::allocator<T> >
class list : public Observable
{
public:

	typedef T value_type;
	typedef A allocator_type;
	
	typedef typename A::size_type size_type;
	typedef typename std::list<T,A>::difference_type difference_type;
	
	typedef typename std::list<T,A>::iterator iterator;
	typedef typename std::list<T,A>::const_iterator const_iterator;

	typedef typename std::list<T,A>::reverse_iterator reverse_iterator;
	typedef typename std::list<T,A>::const_reverse_iterator const_reverse_iterator;

	typedef typename std::list<T,A>::pointer pointer;
	typedef typename std::list<T,A>::const_pointer const_pointer;

	typedef typename std::list<T,A>::reference reference;
	typedef typename std::list<T,A>::const_reference  const_reference;
	
	// Konstruktoren
	explicit list( const A& a = A() ) : Observable(), _list( a ) {}
	explicit list( size_type n, const T& val = T(), const A& a=A()) : Observable(), _list(n,val,a) {}
	template<class In>
	  list(In b, In e, const A& a=A() ) : Observable(), _list( b, e, a ) {}
	list(const list& ob ) : Observable(ob), _list( ob._list ) {}

	~list() {}

	list& operator=(const list& ob ) { _list = ob._list; updateObservers(); return *this; }
	
	template<class In>
	  void assign(In b, In e ) { _list.assign(b,e,a); }
	void assign( size_type n, const T& val ) { _list.assign( n, val ); updateObservers(); }

	bool empty() const { return _list.empty(); }

	allocator_type get_allocator() const { return _list.get_allocator(); }

	// Array operations

	// index zugriff langsam, aber wenigstens vorhanden
	
	reference operator[](size_type n)
	{
		iterator iter = begin();
		std::advance(iter,n);
		return *iter;
	}
	const_reference operator[](size_type n) const
	{
		iterator iter = begin();
		std::advance(iter,n);
		return *iter;
	}
	reference at(size_type n)
	{
		iterator iter = begin();
		std::advance(iter,n);
		return *iter;
	}
	const_reference at(size_type n) const
	{
		iterator iter = begin();
		std::advance(iter,n);
		return *iter;
	}

	reference front() { return _list.front(); }
	const_reference front() const { return _list.front(); }
	reference back() { return _list.back(); }
	const_reference back() const { return _list.back(); }

	iterator begin() { return _list.begin(); }
	iterator end() { return _list.end(); }
	const_iterator begin() const { return _list.begin(); }
	const_iterator end() const { return _list.end(); }
	reverse_iterator rbegin() { return _list.rbegin(); }
	reverse_iterator rend() { return _list.rend(); }
	const_reverse_iterator rbegin() const { return _list.rbegin(); }
	const_reverse_iterator rend() const { return _list.rend(); }

	// Stack operations
	void push_back( const T& val )	{
		_list.push_back(val);
		updateObservers();
	}

	void push_front( const T& val ) {
		_list.push_front(val);
		updateObservers();
	}

	void pop_back()	{
		_list.pop_back();
		updateObservers();
	}

	void pop_front() {
		_list.pop_front();
		updateObservers();
	}


	// List operations
	iterator insert(iterator pos, const T& val ) {
		return _list.insert(pos,val);
		updateObservers();
	}

	void insert(iterator pos, size_type n, const T& val ) {
		_list.insert(pos,n,val);
		updateObservers();
	}

	template<class In>
	void insert(iterator pos, In b, In e )	{
		_list.insert(pos,b,e);
		updateObservers();
	}

	iterator erase(iterator pos) {
		iterator r = _list.erase(pos);
		updateObservers();
		return r;
	}

	iterator erase(iterator b, iterator e)
	{
		iterator r = _list.erase(b,e);
		updateObservers();
		return r;
	}

	void clear() {
		_list.clear();
		updateObservers();
	}
	
	size_type max_size() const { return _list.max_size(); }
	size_type size() const { return _list.size(); }

	void remove( const T& val )
	{
		_list.remove( val );
		updateObservers();
	}

	void resize(size_type n, T val=T())
	{
		size_type s = _list.size();
		_list.resize(n,val);
		if( n < s )
			updateObservers();
	}

	// later, not needed now	
	 // splice x3
	 // merge x2
	 // reverse
	 // sort
	 // swap
	 // unique
	 
private:

	std::list<T,A> _list;
};

template<class T, class A=std::allocator<T> >
class vector : public Observable
{
public:

	typedef T value_type;
	typedef A allocator_type;

	typedef typename A::size_type size_type;
	typedef typename std::vector<T,A>::difference_type difference_type;

	typedef typename std::vector<T,A>::iterator iterator;
	typedef typename std::vector<T,A>::const_iterator const_iterator;

	typedef typename std::vector<T,A>::reverse_iterator reverse_iterator;
	typedef typename std::vector<T,A>::const_reverse_iterator const_reverse_iterator;

	typedef typename std::vector<T,A>::pointer pointer;
	typedef typename std::vector<T,A>::const_pointer const_pointer;

	typedef typename std::vector<T,A>::reference reference;
	typedef typename std::vector<T,A>::const_reference  const_reference;

	// Konstruktoren
	explicit vector( const A& a = A() ) : _vector( a ) {}
	explicit vector( size_type n, const T& val = T(), const A& a=A()) : _vector(n,val,a) {}
	template<class In>
	  vector(In b, In e, const A& a=A() ) : _vector( b, e, a ) {}
	vector(const vector& ob ) : _vector( ob._vector ) {}

	~vector() {}

	vector& operator=(const vector& obj) { _vector = ob._vector; }

	template<class In>
	  void assign(In b, In e ) { _vector.assign(b,e,a); }
	void assign( size_type n, const T& val ) { _vector.assign( n, val ); }

	bool empty() const { return _vector.empty(); }

	allocator_type get_allocator() const { return _vector.get_allocator(); }
	// Array operations

	reference operator[](size_type n) { return _vector[n]; }
	const_reference operator[](size_type n) const { return _vector[n]; }
	reference at(size_type n) { return _vector.at(n); }
	const_reference at(size_type n) const { return _vector.at(n); }

	reference front() { return _vector.front(); }
	const_reference front() const { return _vector.front(); }
	reference back() { return _vector.back(); }
	const_reference back() const { return _vector.back(); }

	iterator begin() { return _vector.begin(); }
	iterator end() { return _vector.end(); }
	const_iterator begin() const { return _vector.begin(); }
	const_iterator end() const { return _vector.end(); }
	reverse_iterator rbegin() { return _vector.rbegin(); }
	reverse_iterator rend() { return _vector.rend(); }
	const_reverse_iterator rbegin() const { return _vector.rbegin(); }
	const_reverse_iterator rend() const { return _vector.rend(); }

	// Stack operations
	void push_back( const T& val ) { _vector.push_back(val); }
//	void push_front( const T& val ) { _vector.push_front(val); }
	void pop_back() { _vector.pop_back(); }
//	void pop_front() { _vector.pop_front(); }


	// List operations
	iterator insert(iterator pos, const T& val ) { return _vector.insert(pos,val); }
	void insert(iterator pos, size_type n, const T& val ) { _vector.insert(pos,n,val); }
	template<class In>
		void insert(iterator pos, In b, In e ) { _vector.insert(pos,b,e); }
	iterator erase(iterator pos) { return _vector.erase(pos); }
	iterator erase(iterator b, iterator e) { return _vector.erase(b,e); }

	void clear() { _vector.clear(); }

	size_type max_size() const { return _vector.max_size(); }
	size_type size() const { return _vector.size(); }

	void remove( const T& val ) { _vector.remove( val ); }

	void resize(size_type n, T val=T()) { _vector.resize(m,val); }
	void reserve(size_type n, T val=T()) { _vector.reserve(m,val); }

	// later, not needed now
	 // splice x3
	 // merge x2
	 // reverse
	 // sort
	 // swap
	 // unique

private:

	std::vector<T,A> _vector;
};

}; // namespace Observables

/*
template<class T, class A=std::allocator<T> >
class list : public Observable
{
public:

	typedef T value_type;
	typedef A allocator_type;

	typedef typename A::size_type size_type;
	typedef typename std::list<T,A>::difference_type difference_type;

	typedef typename std::list<T,A>::iterator iterator;
	typedef typename std::list<T,A>::const_iterator const_iterator;

	typedef typename std::list<T,A>::reverse_iterator reverse_iterator;
	typedef typename std::list<T,A>::const_reverse_iterator const_reverse_iterator;

	typedef typename std::list<T,A>::pointer pointer;
	typedef typename std::list<T,A>::const_pointer const_pointer;

	typedef typename std::list<T,A>::reference reference;
	typedef typename std::list<T,A>::const_reference  const_reference;

	// Konstruktoren
	explicit list( const A& a = A() ) : _list( a ) {}
	explicit list( size_type n, const T& val = T(), const A& a=A()) : _list(n,val,a) {}
	template<class In>
	  list(In b, In e, const A& a=A() ) : _list( b, e, a ) {}
	list(const list& ob ) : _list( ob._list ) {}

	~list() {}

	list& operator=(const list& obj) { _list = ob._list; }

	template<class In>
	  void assign(In b, In e ) { _list.assign(b,e,a); }
	void assign( size_type n, const T& val ) { _list.assign( n, val ); }

	bool empty() const { return _list.empty(); }

	allocator_type get_allocator() const { return _list.get_allocator(); }
	// Array operations

//	reference operator[](size_type n) { return _list[n]; }
//	const_reference operator[](size_type n) const { return _list[n]; }
//	reference at(size_type n) { return _list.at(n); }
//	const_reference at(size_type n) const { return _list.at(n); }

	reference front() { return _list.front(); }
	const_reference front() const { return _list.front(); }
	reference back() { return _list.back(); }
	const_reference back() const { return _list.back(); }

	iterator begin() { return _list.begin(); }
	iterator end() { return _list.end(); }
	const_iterator begin() const { return _list.begin(); }
	const_iterator end() const { return _list.end(); }
	reverse_iterator rbegin() { return _list.rbegin(); }
	reverse_iterator rend() { return _list.rend(); }
	const_reverse_iterator rbegin() const { return _list.rbegin(); }
	const_reverse_iterator rend() const { return _list.rend(); }

	// Stack operations
	void push_back( const T& val ) { _list.push_back(val); }
	void push_front( const T& val ) { _list.push_front(val); }
	void pop_back() { _list.pop_back(); }
	void pop_front() { _list.pop_front(); }


	// List operations
	iterator insert(iterator pos, const T& val ) { return _list.insert(pos,val); }
	void insert(iterator pos, size_type n, const T& val ) { _list.insert(pos,n,val); }
	template<class In>
		void insert(iterator pos, In b, In e ) { _list.insert(pos,b,e); }
	iterator erase(iterator pos) { return _list.erase(pos); }
	iterator erase(iterator b, iterator e) { return _list.erase(b,e); }

	void clear() { _list.clear(); }

	size_type max_size() const { return _list.max_size(); }
	size_type size() const { return _list.size(); }

	void remove( const T& val ) { _list.remove( val ); }

	void resize(size_type n, T val=T()) { _list.resize(m,val); }

	// later, not needed now
	 // splice x3
	 // merge x2
	 // reverse
	 // sort
	 // swap
	 // unique

private:

	std::list<T,A> _list;
};
*/


#endif
