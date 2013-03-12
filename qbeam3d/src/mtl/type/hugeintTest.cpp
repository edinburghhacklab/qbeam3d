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

#include "hugeint.h"
#include "../util.h"

#include <iostream>
#include <iomanip>
#include <cassert>

template<unsigned int N> void hugeintTester();

int main( int argc, char* argv[] )
{
	hugeintTester<16>();
	return 0;
}

template<unsigned int N> void hugeintTester()
{
	using namespace std;
	using namespace mtl::type;

	cout << "Teste hugeint Klasse: " << mtl::typeinfo<hugeint<N> >::name() << endl;

	hugeint<N> i1;
	hugeint<N> i2(1);
	i2 <<= 32;
	hugeint<N> i3(-1);
	hugeint<N> i4(1);
	i4 <<= 32; i4--;
	hugeint<N> i5 = 2;

	i1 = -1;
	assert( i1 < 0 );

	cout << "Teste bit-Verknüpfungen:" << endl;
	cout << "i1 = " << hex << i1 << dec << ", " << i1 << endl;
	cout << "i2 = " << hex << i2 << dec << ", " << i2 <<endl;
	cout << "i3 = " << hex << i3 << dec << ", " << i3 <<endl;
	cout << "i4 = " << hex << i4 << dec << ", " << i4 <<endl;

	cout << endl << "|=, >>=, <<=" << endl << endl;
	cout << oct;
	cout << "i1 = " << i1 << endl;
	cout << "i2 = " << i2 << endl;
	cout << "16 mal: i1 |= i2, i2>>=2;"<<endl;
	for( int k=0; k<16; k++ )
	{
		i1  |= i2;
		i2 >>= 2;
	}
	cout << "i1 = " << i1 << endl;
	cout << "i2 = " << i2 << endl;
	cout << "8 mal nach links, wieder 8 mal nach rechts" << endl;
	for( int k=0; k<8; k++ )
	{
		i1 <<= 1;
		cout << "i1 = " << i1 << endl;
	}
	for( int k=0; k<8; k++ )
	{
		i1 >>= 1;
		cout << "i1 = " << i1 << endl;
	}

	i1 <<= 48;
	cout << "i1 <<= 48 : " << i1 << endl;
	i1 >>= 43;
	cout << "i1 >>= 43 : " << i1 << endl;

	i1   = 100;
	cout << "i1        = " << i1 << endl;
	i1 <<= 0;
	cout << "i1 <<= 0  : " << i1 << endl;
	i1 >>= 0;
	cout << "i1 >>= 0  : " << i1 << endl;
	i1 <<= 32;
	cout << "i1 <<= 32 : " << i1 << endl;
	i1 >>= 32;
	cout << "i1 >>= 32 : " << i1 << endl;


	cout << "| & ^ << >> ~ " << endl;
	cout << "     i1 = " << i1 << endl;
	cout << "i1 | i2 = " << (i1 | i2) << endl;
	cout << "i1 & i2 = " << (i1 & i2) << endl;
	cout << "i1 | i3 = " << (i1 | i3) << endl;
	cout << "i1 & i3 = " << (i1 & i3) << endl;
	cout << "i1 ^ i1 = " << (i1 ^ i1) << endl;
	cout << "i1 ^ i3 = " << (i1 ^ i3) << endl;

//	i1 = 1;
//	for( int k=0; k<128; k++ ) {
//		cout << "i1 = " << i1 << endl;
//		i1<<=1;
//	}

	cout << dec;
	cout << "Teste arithmetische Operatoren: " << endl << endl;

	cout << "i1 = " << hex << i1 << dec << ", " << i1 << endl;
	cout << "i2 = " << hex << i2 << dec << ", " << i2 <<endl;
	cout << "i3 = " << hex << i3 << dec << ", " << i3 <<endl;
	cout << "i4 = " << hex << i4 << dec << ", " << i4 <<endl;

	//i2.neg(); i3.neg();
	cout << hex << "-i2= " << -i2 << dec << ", " << -i2 << endl;
	cout << hex << "-i3= " << -i3 << dec << ", " << -i3 << endl;
	//i2.neg(); i3.neg();

	cout << "+ += ++ - -= --" << endl;
	cout << "i2 + i4  = " << (i2+i4) << endl;
	i2 += i4;
	cout << "i2 += i4 = " << i2 << endl;

	cout << "++i2     = " << ++i2 << endl;
	cout << "--i2     = " << --i2 << endl;
	cout << "--i2     = " << --i2 << endl;
	cout << "++i2     = " << ++i2 << endl;
	cout << "++i4     = " << ++i4 << endl;
	cout << "--i4     = " << --i4 << endl;
	cout << "--i4     = " << --i4 << endl;
	cout << "++i4     = " << ++i4 << endl;

	cout << "i4+ 1    = " << (i4+1) << endl;
	cout << "i4- 1    = " << (i4-1) << endl;

	cout << "i2       = " << hex << i2 << ", " << dec << i2 << endl;
	i2+=1;
	cout << "i2 += 1  = " << hex << i2 << ", " << dec << i2 << endl;
	i2-=1;
	cout << "i2 -= 1  = " << hex << i2 << ", " << dec << i2 << endl;

	for( int k=0; k<8; k++ )
	{
		cout << "i2++    = " << i2++ << endl;
	}
	cout << "i2      = "  << hex << i2 << ", " << dec << i2 << endl;
	i2 += unsigned(1<<31);
	cout << "i2 +=2^31= "  << hex << i2 << ", " << dec << i2 << endl;
	i2 += unsigned(1<<31);
	cout << "i2 +=2^31= "  << hex << i2 << ", " << dec << i2 << endl;

	i2 -= unsigned(1<<31);
	cout << "i2 -=2^31= "  << hex << i2 << ", " << dec << i2 << endl;
	i2 -= unsigned((1<<31)-1);
	cout << "i2-=2^31-1="  << hex << i2 << ", " << dec << i2 << endl;

	cout << "Difference --i1, i1--, wraparound: " << endl;
	i1 = 0;
	cout << "i1       = " << i1 << endl;
	for( int k=8; k>0; k-- ) {
	 cout << "i1--     = " << i1-- << endl;
	}
	cout << "AFTER i1 = " << i1 << endl;
	i1 = 0;
	cout << "i1       = " << i1 << endl;
	for( int k=8; k>0; k-- ) {
	 cout << "--i1     = " << --i1 << endl;
	}

	cout << "boolsche Vergleichsoperatoren: " << endl;
	i1 = 0;
	i2 = -1;
	i3 = 0xFFFFFFFF;
	i4 = -256;
	cout << "i1 (0)     = " << i1 << endl;
	cout << "i2 (-1)    = " << i2 << endl;
	cout << "i3 (2^32-1)= " << i3 << endl;
	cout << "i4 (-256)  = " << i4 << endl;
	cout << "i5 (2)     = " << i5 << endl;

	cout << "**************" << "== != !" << "**************" << endl;
	cout << "i1 == i1 : " << ((i1 == i1)?"true ":"WRONG") << endl;
	cout << "i1 != i1 : " << ((i1 != i1)?"WRONG":"false") << endl;
	cout << "i1 != i2 : " << ((i1 != i2)?"true ":"WRONG") << endl;
	cout << "i1 == i3 : " << ((i1 == i3)?"WRONG":"false") << endl;
	cout << "i1 != i4 : " << ((i1 != i4)?"true ":"WRONG") << endl;
	cout << "!i1      : " <<     ((!i1) ?"true ":"WRONG") << endl;
	cout << "!i2      : " <<     ((!i2) ?"WRONG":"false") << endl;

	cout << "**************" << "<, <=" << "**************" << endl;
	cout << "0  >  i1 : " << ((i1 <  0 )?"WRONG":"false") << endl;	// 0<0 F
	cout << "i1 <  i2 : " << ((i1 <  i2)?"WRONG":"false") << endl;	// 0<- F
	cout << "i1 <  i3 : " << ((i1 <  i3)?"true ":"WRONG") << endl;		// 0<+ T
	cout << "i2 <  i1 : " << ((i2 <  i1)?"true ":"WRONG") << endl;		// -<0 T
	cout << "i4 <  i2 : " << ((i4 <  i2)?"true ":"WRONG") << endl;	  // -<- T
	cout << "i2 <  i4 : " << ((i2 <  i4)?"WRONG":"false") << endl;	// -<- F
	cout << "i2 <  i5 : " << ((i2 <  i5)?"true ":"WRONG") << endl;		// -<+ T
	cout << "i5 <  i1 : " << ((i5 <  i1)?"WRONG":"false") << endl;		// +<0 F
	cout << "i3 <  i4 : " << ((i3 <  i4)?"WRONG":"false") << endl;		// +<- F
	cout << "i5 <  i3 : " << ((i5 <  i3)?"true ":"WRONG") << endl;		// +<+ T
	cout << "i3 <  i5 : " << ((i3 <  i5)?"WRONG":"false") << endl;		// +<+ F

	cout << "i1 <= 0  : " << ((i1 <= 0 )?"true ":"WRONG") << endl;		// 0<=0 T
	cout << "i1 <= i2 : " << ((i1 <= i2)?"WRONG":"false") << endl;		// 0<=- F
	cout << "i1 <= i3 : " << ((i1 <= i3)?"true ":"WRONG") << endl;		// 0<=+ T

	cout << "i2 <= 0  : " << ((i2 <= 0 )?"true ":"WRONG") << endl;		// -<=0 T
	cout << "i4 <= i2 : " << ((i4 <= i2)?"true ":"WRONG") << endl;	  // -<=- T
	cout << "i2 <= i4 : " << ((i2 <= i4)?"WRONG":"false") << endl;		// -<=- F
	cout << "i2 <= i2 : " << ((i2 <= i2)?"true ":"WRONG") << endl;		// -<=- T =
	cout << "i2 <= i5 : " << ((i2 <= i5)?"true ":"WRONG") << endl;		// -<=+ T

	cout << "i5 <= 0  : " << ((i5 <= 0 )?"WRONG":"false") << endl;		// +<=0 F
	cout << "i3 <= i4 : " << ((i3 <= i4)?"WRONG":"false") << endl;		// +<=- F
	cout << "i5 <= i3 : " << ((i5 <= i3)?"true ":"WRONG") << endl;		// +<=+ T
	cout << "i3 <= i5 : " << ((i3 <= i5)?"WRONG":"false") << endl;		// +<=+ F
	cout << "i5 <= i5 : " << ((i5 <= i5)?"true ":"WRONG") << endl;		// +<=+ T=

	cout << "**************" << ">, >=" << "**************" << endl;
	cout << "i1 >  0  : " << ((i1 >  0 )?"WRONG":"false") << endl;	// 0>0 F
	cout << "i1 >  i2 : " << ((i1 >  i2)?"true ":"WRONG") << endl;	// 0>- T
	cout << "i1 >  i3 : " << ((i1 >  i3)?"WRONG":"false") << endl;	// 0>+ F
	cout << "i2 >  0  : " << ((i2 >  0 )?"WRONG":"false") << endl;	// ->0 F
	cout << "i2 >  i4 : " << ((i2 >  i4)?"true ":"WRONG") << endl;	// ->- T
	cout << "i4 >  i2 : " << ((i4 >  i2)?"WRONG":"false") << endl;	// ->- F
	cout << "i2 >  i5 : " << ((i2 >  i5)?"WRONG":"false") << endl;	// ->+ F
	cout << "i5 >  0  : " << ((i5 >  0 )?"true ":"WRONG") << endl;	// +>0 T
	cout << "i3 >  i4 : " << ((i3 >  i4)?"true ":"WRONG") << endl;	// +>- T
	cout << "i3 >  i5 : " << ((i3 >  i5)?"true ":"WRONG") << endl;	// +>+ T
	cout << "i5 >  i3 : " << ((i5 >  i3)?"WRONG":"false") << endl;	// +>+ F

	cout << "i1 >= 0  : " << ((i1 >= 0 )?"true ":"WRONG") << endl;	// 0>=0 T
	cout << "i1 >= i2 : " << ((i1 >= i2)?"true ":"WRONG") << endl;	// 0>=- T
	cout << "i1 >= i3 : " << ((i1 >= i3)?"WRONG":"false") << endl;	// 0>=+ F
	cout << "i2 >= 0  : " << ((i2 >= 0 )?"WRONG":"false") << endl;	// ->=0 F
	cout << "i2 >= i4 : " << ((i2 >= i4)?"true ":"WRONG") << endl;	// ->=- T
	cout << "i4 >= i2 : " << ((i4 >= i2)?"WRONG":"false") << endl;	// ->=- F
	cout << "i2 >= i2 : " << ((i2 >= i2)?"true ":"WRONG") << endl;	// ->=- T =
	cout << "i2 >= i5 : " << ((i2 >= i5)?"WRONG":"false") << endl;	// ->=+ F
	cout << "i5 >= 0  : " << ((i5 >= 0 )?"true ":"WRONG") << endl;	// +>=0 T
	cout << "i3 >= i4 : " << ((i3 >= i4)?"true ":"WRONG") << endl;	// +>=- T
	cout << "i3 >= i5 : " << ((i3 >= i5)?"true ":"WRONG") << endl;	// +>=+ T
	cout << "i5 >= i3 : " << ((i5 >= i3)?"WRONG":"false") << endl;	// +>=+ F
	cout << "i3 >= i3 : " << ((i3 >= i5)?"true ":"WRONG") << endl;	// +>=+ T=

	cout << endl << "Komplexe arithmetische Operatoren" << endl << endl;

	i1 = 1;
	i2 = -1;
	i3 = unsigned(1<<31);
	i4 = -(1<<31);
	i5 = 10000;

	cout << dec;
	cout << "i1 (1)     = " << i1 << endl;
	cout << "i2 (-1)    = " << i2 << endl;
	cout << "i3 (2^31)  = " << i3 << endl;
	cout << "i4 -2^31   = " << i4 << endl;
	cout << "i5 (10000) = " << i5 << endl;

	cout << endl << "*=" << endl << endl;

	cout << "i1        " << i1 << "(" << i1.bitsUsed() << ")" << endl;
	cout << "i1 *= 1   " << (i1*=1) << "(" << i1.bitsUsed() << ")" <<endl;
	for( int bits = 1+1+1+1; bits <= N*sizeof(unsigned int)*8; bits++ )
		cout << hex << "i1 *= 2   " << (i1*=2) << dec << "(" << i1.bitsUsed() << ")" <<endl;

	cout << endl << endl;

	cout << endl << "/, % " << endl << endl;

	i1 = unsigned(~0);
	i2 = -1;
	i3 = unsigned(1<<31);
	i4 = i3; i4.neg();
	cout << dec;
	cout << "i1 (2^32-1)= " << i1 << endl;
	cout << "i2 (-1)    = " << i2 << endl;
	cout << "i3 (2^31)  = " << i3 << endl;
	cout << "i4 -2^31   = " << i4 << endl;
	cout << endl;

	cout << "i1/i2      = " << (i1/i2) << endl;
	cout << "           = " << i1.neg() << endl;
	i1.neg();
	cout << "i1%i2      = " << (i2%i2) << endl;
	cout << "i1/2       = " << (i1/2) << endl;
	cout << "           = " << (i1>>1u) << endl;
	cout << "i1/67      = " << (i1/67) << endl;
	cout << "i1%67      = " << (i1%67) << endl;

	i1 = 0x40000000;
	i2 = i1;
	cout << "i1    = " << i2 << endl;
	if( !hugeint<N>::overflowOnMul( i2, i1 ) ){
		i2*=i1;
		cout << "i1^2  = " << i2 << " (" << i2.bitsUsed() << ")" << endl;
	}
	if( !hugeint<N>::overflowOnMul( i2, i1 ) ){
		i2*=i1;
		cout << "i1^3  = " << i2 << " (" << i2.bitsUsed() << ")" << endl;
	}
	if( !hugeint<N>::overflowOnMul( i2, i1 ) ){
		i2*=i1;
		cout << "i1^4  = " << i2 << " (" << i2.bitsUsed() << ")" << endl;
	}
	if( !hugeint<N>::overflowOnMul( i2, i2 ) ){
		i2*=i2;
		cout << "i1^8  = " << i2 << " (" << i2.bitsUsed() << ")" << endl;
	}
	if( !hugeint<N>::overflowOnMul( i2, i2 ) ){
		i2*=i2;
		cout << "i1^16 = " << i2 << " (" << i2.bitsUsed() << ")" << endl;
	}


	i1 = 10011;
	i2 = 100;
	i3 = -100;
	i4 = -10011;
	i5 = 123456789;

	cout << dec;
	cout << "i1 (10011)  = " << i1 << endl;
	cout << "i2 (100)    = " << i2 << endl;
	cout << "i3 (-100)   = " << i3 << endl;
	cout << "i4 (-10011) = " << i4 << endl;
	cout << "i5          = " << i5 << endl;
	cout << "i1 / i2     = " << i1/i2 << ", " << i1%i2 << " (100,11)" << endl;
	cout << "i1 / i3     = " << i1/i3 << ", " << i1%i3 << " (-100,11)" << endl;
	cout << "i4 / i2     = " << i4/i2 << ", " << i4%i2 << " (-100,-11)" << endl;
	cout << "i4 / i3     = " << i4/i3 << ", " << i4%i3 << " (100,-11)" << endl;
	cout << "i5 / 111    = " << i5/111 << ", " << i5%111 << "(1112223,36)" << endl;
}
