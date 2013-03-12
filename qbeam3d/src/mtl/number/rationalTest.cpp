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

#include "rational.h"
#include <../type/hugeint.h>
#include "../util.h"

#include <iostream>
#include <iomanip>
#include <cassert>

template <class T> void rationalTester();

int main( int argc, char* argv[] )
{
	//rationalTester<int>();
	//rationalTester<long>();
	//rationalTester<long long>();
	rationalTester<mtl::type::hugeint<2> >();

	return 0;
}

template<class T> void rationalTester()
{
	using namespace mtl::number;

	assert( mtl::typeinfo<T>::is_specialized );

	std::cout << "Testing rational<" << mtl::typeinfo<T>::name() << ">" << std::endl;

	std::cout << std::dec;
	rational<T> r1(1,2);
	std::cout << "r1 = " << r1 << std::endl;

	rational<T> r2(0.5);
	std::cout << "r2 = " << r2 << std::endl;

	rational<T> r3(2);
	std::cout << "r3 = " << r3 << std::endl;

	rational<T> r4(-1);
	std::cout << "r4 = " << r4 << std::endl;

	rational<T> r5(3,10);
	std::cout << "r5 = " << r5 << std::endl;

	rational<T> r6(-65.0);
	std::cout << "r6 = " << r6 << std::endl;

	std::cout << "r1 == 0.5" << std::endl;	assert( r1 == 0.5 );
	std::cout << "0.5 == r2" << std::endl;	assert( 0.5 == r2 );
	std::cout << "r3 == 2  " << std::endl;	assert( r3 == 2 );
	std::cout << "-1 == r4 " << std::endl;	assert( -1 == r4 );
	std::cout << "r1 == r2 " << std::endl;	assert( r1 == r2 );
	std::cout << "r1 <= r2 " << std::endl;	assert( r1 <= r2 );
	std::cout << "r1 >= r2 " << std::endl;	assert( r1 >= r2 );
	std::cout << "r4 <  r1 " << std::endl;	assert( r4 < r1 );
	std::cout << "r4 <  r2 " << std::endl;	assert( r4 < r2 );
	std::cout << "r4 <  r3 " << std::endl;	assert( r4 < r3 );

	r4 += r1+r2;
	assert( r4 == 0 );
	std::cout << "r4 + r1 + r2 = " << r4 << std::endl;

	r1 += T(1);
	r2 += 0.2;
	r3 -= 0.25;
	r4 += T(10);
	r5 += rational<T>(7,10);

	std::cout << "r1 + 1   = " << r1 <<std::endl;
	std::cout << "r2 + 0.2 = " << r2 <<std::endl;
	std::cout << "r3 - 0.25= " << r3 <<std::endl;
	std::cout << "r4 + 10  = " << r4 <<std::endl;
	std::cout << "r5 + 0.7 = " << r5 <<std::endl;

	std::cout << "3/5 * 15/9  = " << rational<T>(3,5)*rational<T>(15,9) << std::endl;
	std::cout << "1/2 * 7/3   = " << rational<T>(1,2)*rational<T>(7,3) << std::endl;
	std::cout << "-12/13 * 1/7 = " << rational<T>(12,-13)*rational<T>(-1,7) << std::endl;
	std::cout << "11/7 * 1/5   = " << rational<T>(11,7)*rational<T>(2,5) << std::endl;

	std::cout << "3/5 / 15/9  = " << rational<T>(3,5)/rational<T>(15,9) << std::endl;
	std::cout << "1/2 / 7/3   = " << rational<T>(1,2)/rational<T>(7,3) << std::endl;
	std::cout << "-12/13 / -1/7 = " << rational<T>(12,-13)/rational<T>(-1,7) << std::endl;
	std::cout << "11/7 / 2/5   = " << rational<T>(11,7)/rational<T>(2,5) << std::endl;

	r1.set( 123456789, 123456 );
	r2.set( -1, 2 );
	r3.set( 1234, 123456789 );

	rational<T>::setPrintAsFloat(false);	std::cout << r1 << " = ";
	rational<T>::setPrintAsFloat(true);		std::cout << r1 << std::endl;
	rational<T>::setPrintAsFloat(false);	std::cout << r2 << " = ";
	rational<T>::setPrintAsFloat(true);		std::cout << r2 << std::endl;
	rational<T>::setPrintAsFloat(false);	std::cout << r3 << " = ";
	rational<T>::setPrintAsFloat(true);		std::cout << r3 << std::endl;

}
