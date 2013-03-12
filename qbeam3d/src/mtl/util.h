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
#ifndef MTL_UTIL_H_
#define MTL_UTIL_H_

#include <string>
#include <cassert>
#include "type/hugeint.h"

namespace mtl
{

template<class T>
class typeinfo
{
public:
	static const bool is_specialized = false;
	static const std::string& name() { assert(0); }
};

class typeinfo<short int>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="short int"; return n; }
};

class typeinfo<short unsigned int>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="short unsigned int"; return n; }
};

class typeinfo<int>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="int"; return n; }
};

class typeinfo<unsigned int>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="unsigned int"; return n; }
};

class typeinfo<char>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="char"; return n; }
};

class typeinfo<unsigned char>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="unsigned char"; return n; }
};

class typeinfo<long>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="long"; return n; }
};

class typeinfo<long long>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="long long"; return n; }
};

class typeinfo<unsigned long>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="unsigned long"; return n; }
};

class typeinfo<unsigned long long>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="unsigned long long"; return n; }
};

class typeinfo<float>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="float"; return n; }
};

class typeinfo<double>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="double"; return n; }
};

class typeinfo<long double>
{
public:
	static const bool is_specialized = true;
	static const std::string& name() { static std::string n="long double"; return n; }
};

template<unsigned int N>
class typeinfo<mtl::type::hugeint<N> >
{
public:
	static const bool is_specialized = true;
	static const std::string& name()
	{
		static std::string n="hugeint<" + std::string(1,(const char)(N+'0')) + ">";
		return n;
	}
};

}
#endif
