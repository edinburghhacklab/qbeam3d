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

#include "SpecSizedInt.h"

#include "L2000InstructionSet.h"
#include "L2000Device.h"

#include <iostream>
#include <iomanip>
#include <cassert>

#include "macros.h"

namespace L2000
{

InstructionSet::Instruction::Instruction( const std::string& mnemonic, const InstructionCode code, int pl, int rl, int args, unsigned long timeout_millies ) :
	::InstructionSet::Instruction( mnemonic, (const char)code, pl, rl ),
	_argc(args),
	_millies(timeout_millies)
{}

InstructionSet::Instruction::~Instruction()
{}

bool InstructionSet::Instruction::execute( Processor* dev, const char *p, char *r ) const
{
	bool ret = false;
	printCall( std::cerr, p );

	// Ein Befehl ist immer genau 4 byte lang
	// CODE, P1, P2, XOR-CHKSUM
	// die Antwort kann auch länger sein, niemals aber länger
	// als als max( n*(X+Y) + 4 ) = 256*16+4 = 4096+4 = 4100;
	// für normale (nicht Schleifen) Befehle aber nur 8 byte
	static char buffer[4+8]; // max antwort + 4 bytes command
	Device *d = static_cast<Device*>(dev);
	// instruction codes des Scanners sind immer ein byte lang
	buffer[0] = code()[0];
	buffer[1] = paramLength()>0 ? p[0] : 0;
	buffer[2] = paramLength()>1 ? p[1] : 0;
	calcCheckSum(buffer, 4);
    // calc return length
    // 0 -> 1
    // 2 -> 4
    // 4 -> 8
    unsigned int ret_length = (returnLength()==0) ? 1 : (returnLength()<<1);
    // send

//	Processor::usleep( 1000 );

	if( d->sendCommand( buffer, 4  ) )
		if( d->receiveAnswer( buffer+4, ret_length, _millies ) )
		{
			//std::cout << std::endl;
			//dump_int( std::cout, buffer, 12 );
			//std::cout << std::endl;

			//if( ret_length > 1 )
			//	buffer[4] = buffer[0];

			if( !verifyCheckSum(buffer+4, ret_length) )
				std::cerr << "WARNING CRC failed!" << std::endl;

			{
				// Hier müssen wir insgesamt returnLength() Werte zurück liefern
				//  0: __
				//  2: __ XX XX __
				//  4: __ XX XX __ __ XX XX __ __ XX XX __
				//        0  1  2     2  3        4  5
				//        1  1  1     3  3        5  5
				//        1  2  3->   5  6        9  10
				unsigned offset=1;
				unsigned index =0;

				//std::cerr << " returns " << std::hex;

				while( index < returnLength() )
				{
					assert( offset+index < ret_length );

					//std::cerr << (unsigned(buffer[4+index+offset])&255) << " ";

					r[index] = buffer[4+offset+index];
					index++;
					if( (offset+index)&3 == 3 )
						offset+=2;
				}

				// **************
				//std::cerr << "  -------> answer was: ";
				//dump_int( std::cerr, buffer, 12 );
				//for( index = 0; index<ret_length; index++ )
				//	std::cerr << unsigned((buffer[4+index]>>4)&15) << unsigned(buffer[4+index]&15) << " ";
				// **************
				ret = true;
			}
		}
	//std::cerr << std::dec << std::endl;
	return ret;
}

bool InstructionSet::Instruction::execute( Processor* dev, const std::vector<char>& p, std::vector<char>& r) const
{
	printCall( std::cerr, (const char*)&p[0] );

	bool ret=false;
	
	// Ein Befehl ist immer genau 4 byte lang
	// CODE, P1, P2, XOR-CHKSUM
	// die Antwort kann auch länger sein, niemals aber länger
	// als als max( n*(X+Y) + 4 ) = 256*16+4 = 4096+4 = 4100;
	// für normale (nicht Schleifen) Befehle aber nur 8 byte
	static char buffer[4+8]; // max antwort + 4 bytes command
	std::fill( buffer, buffer+12, 0x00 );
	Device *d = static_cast<Device*>(dev);
	// instruction codes des Scanners sind immer ein byte lang
	buffer[0] = code()[0];
	buffer[1] = paramLength()>0 ? p[0] : 0;
	buffer[2] = paramLength()>1 ? p[1] : 0;
	calcCheckSum(buffer, 4);
    // calc return length
    // 0 -> 1
    // 2 -> 4
    // 4 -> 8
    unsigned int ret_length = (returnLength()==0) ? 1 : (returnLength()<<1);
    // send

//	Processor::usleep( 1000 );

	if( d->sendCommand( buffer, 4  ) )
		if( d->receiveAnswer( buffer+4, ret_length, _millies ) )
		{
//			if( ret_length > 1 )
//				buffer[4] = buffer[0];

			if( !verifyCheckSum(buffer+4, ret_length) )
				std::cerr << "\t\t\t\tWARNING CRC failed!" << std::endl;

			{
				// Hier müssen wir insgesamt returnLength() Werte zurück liefern
				//  0: __
				//  2: __ XX XX __
				//  4: __ XX XX __ __ XX XX __ __ XX XX __
				//        0  1  2     2  3        4  5
				//        1  1  1     3  3        5  5
				//        1  2  3->   5  6        9  10
				unsigned offset=1;
				unsigned index =0;

				//std::cerr << " returns " << std::hex;

				while( index < returnLength() )
				{
					assert( offset+index < ret_length );

					//std::cerr << (unsigned(buffer[4+index+offset])&255) << " ";

					r.push_back( buffer[4+offset+index] );
					index++;
					if( (offset+index)&3 == 3 )
						offset+=2;
				}
				//******************
				//std::cerr << "  -------> answer was: ";
				//for( index = 0; index<ret_length; index++ )
				//std::cerr << unsigned((buffer[4+index]>>4)&15) << unsigned(buffer[4+index]&15) << " ";
				//*************************************
				ret=true;
			}
		}
	//std::cerr << std::dec << std::endl;
	return ret;
}

bool InstructionSet::Instruction::verifyCheckSum(const char* buf, unsigned int cmdLength) const
{
	// 1 byte buf must be == 0x5A
	unsigned int cnt = cmdLength;

	if( cnt == 1 )
	{
		if( *buf != 0x5A )
			std::cerr << " CHECKSUM error (" << cmdLength  << ") -> " << std::hex << int(buf[0]) << std::dec <<  std::endl;
		return *buf == 0x5A;
	}

	// calc xor-checksum
	const char* p = buf;
	char  crc = 0;
	while( cnt-- > 0 )
		crc ^= *p++;

	if( crc != 0 ) {
		std::cerr << " CHECKSUM error (" << cmdLength  << ") - Sequence was:" << std::endl;
		std::cerr << std::hex;
		for( int i=0; i<cmdLength; i++ ) std::cerr << int( buf[i] ) << " ";
		std::cerr << std::dec;

	}
	return crc == 0;
}

void InstructionSet::Instruction::calcCheckSum( char* buf, unsigned int cmdLength ) const
{
	// calc xor-checksum
	char* p = buf;
	char  crc = 0;
	while( cmdLength-- > 1 )
		crc ^= *p++;
	*p =  crc;
}

std::ostream& InstructionSet::Instruction::printCall( std::ostream& out, const char* params ) const
{
	/*
	out << mnemonic() << " ";
	if( argCount() == 1 && paramLength() == 1 )
	{
		out << int(*params);
	}
	else if( argCount() == 1 && paramLength()==2 )
	{
		Int16 arg( params, Endianess::BIG, 2 );
		out << arg;
	}
	else if( argCount() == 2 && paramLength()==2 )
	{
		out << unsigned(params[0]) << ", " << unsigned(params[1]);
	}
	out << ": ";
	*/
	return out;
}

std::ostream& InstructionSet::Instruction::printAnswer( std::ostream& out, const char* answer ) const
{
	// assumed when param ist 16bit than answer too
	if( returnLength() == 1 )
	{	// single bytes
		out << int(*answer);
	}
	else if( argCount() == 2 && returnLength()==2 )
	{
		out << unsigned(answer[0]) << ", " << unsigned(answer[1]);
	}
	else if( returnLength()==2 )
	{
		Int16 arg( answer, Endianess::BIG, 2 );
		out << arg;
	}
	return out;
}


// Sequence Instruction

InstructionSet::SequenceInstruction::SequenceInstruction(const std::string& mnemonic, const InstructionCode code, int pl, int rl, int args, unsigned long timeout_millies )
:	Instruction(mnemonic, code, pl, rl, args, timeout_millies )
{}

InstructionSet::SequenceInstruction::~SequenceInstruction()
{}

bool InstructionSet::SequenceInstruction::execute( Processor* dev, const char *p, char *r) const
{
	printCall( std::cerr, p ) << std::endl;

	// Answer length = max( n*(X+Y) + 4 ) = 256*16+4 = 4096+4 = 4100;
	static char buffer[4104];
	Device *d = static_cast<Device*>(dev);
	// complete length of sequence to receive
	unsigned int sequence_length=0;
	// answer length to be returned (use data)
	unsigned int answer_length  =0;
	// sequence and answer length of single command inside loop
	unsigned int seq1_length    =0;
	unsigned int seq2_length    =0;
	unsigned int ans1_length    =0;
	unsigned int ans2_length    =0;
	// additional bytes to receive
	unsigned int suffix_length = 0;

	switch( code()[0] )
	{
		case	REPT:		// set loop count and send REPT command
				_counter = *(reinterpret_cast<const unsigned int*>(p));
				return Instruction::execute( dev, p, r );
			
		case	SEQUEX:		// Sequence 1
				{
				const ::InstructionSet::Instruction* ins1=0;
				const ::InstructionSet::Instruction* ins2=0;
				ins1 = d->instructionSet()->instruction(unsigned(p[0]));
				ins2 = d->instructionSet()->instruction(unsigned(p[1]));
				ans1_length = ins1 ? ins1->returnLength() : 0;
				seq1_length = ans1_length ? ans1_length<<1 : 1;
				ans2_length = ins2 ? ins2->returnLength() : 0;
				seq2_length = ans2_length ? ans2_length<<1 : 1;
				suffix_length	= 4;
				}
				break;
		case	SEQ2:
		case	SEQ3:
				seq1_length		= 5;
				ans1_length		= 4;
				seq2_length		= 0;
				ans2_length		= 0;
				suffix_length	= 0;
				break;
		case	LEDCHK:
				_counter		= 9;
				seq1_length		= 4;
				ans1_length		= 2;
				seq2_length		= 0;
				ans2_length		= 0;
				suffix_length	= 0;
		default:
				return false;
	}
	sequence_length = (seq1_length+seq2_length)*_counter;
	answer_length	= (ans1_length+ans2_length)*_counter;

	// commando aufbauen
	// instruction codes des Scanners sind immer ein byte lang
	buffer[0] = code()[0];
	buffer[1] = paramLength()>0 ? p[0] : 0;
	buffer[2] = paramLength()>1 ? p[1] : 0;
	calcCheckSum(buffer, 4);
	// calc return length
	// 0 -> 1
	// 2 -> 4
	// 4 -> 8
	// send
	if( d->sendCommand( buffer, 4 ) )
	{
		char *t = r;
		char *s = buffer+4;
		unsigned int cnt = sequence_length;
		while( _counter > 0 )
		{
			if( seq1_length > 0 &&
				!d->receiveAnswer( s, seq1_length, _millies  ))
				break;
			if( seq2_length > 0 &&
				!d->receiveAnswer( s+seq1_length, seq2_length, _millies  ))
				break;
			
			if( seq1_length > 0 && verifyCheckSum(s, seq1_length) )
			{
				unsigned i = 0;
				unsigned o = (seq1_length-ans1_length)>>1;
				while( i < ans1_length ) {
					*t++ = s[i+o];
					i++;
				}
				answer_length -= ans1_length;
			}
			if( seq2_length > 0 && verifyCheckSum(s+seq1_length, seq2_length) )
			{
				unsigned i = 0;
				unsigned o = seq1_length + (seq2_length-ans2_length)>>1;
				while( i < ans2_length ) {
					*t++ = s[i+o];
					i++;
				}
				answer_length -= ans2_length;
			}
			s	+= seq1_length + seq2_length;
			cnt	-= seq1_length + seq2_length;
			_counter--;
		}
		if( suffix_length > 0 )
		{
			if( d->receiveAnswer( s, suffix_length, _millies  ) )
				if( verifyCheckSum( s, suffix_length ) )
				{
					suffix_length = 0;
				}
		}
	}
	_counter = 0;
	return answer_length == 0 && suffix_length==0;
}

bool InstructionSet::SequenceInstruction::execute( Processor* dev, const std::vector<char>& p, std::vector<char>& r) const
{
	printCall( std::cerr, (const char*)&p[0] ) << std::endl;

	// Answer length = max( n*(X+Y) + 4 ) = 256*16+4 = 4096+4 = 4100;
	static char buffer[4104];
	Device *d = static_cast<Device*>(dev);
	// complete length of sequence to receive
	unsigned int sequence_length=0;
	// answer length to be returned (use data)
	unsigned int answer_length  =0;
	// sequence and answer length of single command inside loop
	unsigned int seq1_length    =0;
	unsigned int seq2_length    =0;
	unsigned int ans1_length    =0;
	unsigned int ans2_length    =0;
	// additional bytes to receive
	unsigned int suffix_length = 0;

	switch( code()[0] )
	{
		case	REPT:		// set loop count and send REPT command
				_counter = static_cast<unsigned int>(p[0]);
				return Instruction::execute( dev, p, r );

		case	SEQUEX:		// Sequence 1
				{
				const ::InstructionSet::Instruction* ins1=0;
				const ::InstructionSet::Instruction* ins2=0;
				ins1 = d->instructionSet()->instruction(unsigned(p[0]));
				ins2 = d->instructionSet()->instruction(unsigned(p[1]));
				ans1_length = ins1 ? ins1->returnLength() : 0;
				seq1_length = ans1_length ? ans1_length<<1 : 1;
				ans2_length = ins2 ? ins2->returnLength() : 0;
				seq2_length = ans2_length ? ans2_length<<1 : 1;
				suffix_length	= 4;
				}
				break;
		case	SEQ2:
		case	SEQ3:
				seq1_length		= 5;
				ans1_length		= 4;
				seq2_length		= 0;
				ans2_length		= 0;
				suffix_length	= 0;
				break;
		case	LEDCHK:
				_counter		= 9;
				seq1_length		= 4;
				ans1_length		= 2;
				seq2_length		= 0;
				ans2_length		= 0;
				suffix_length	= 0;
				break;
		default:
				return false;
	}
	sequence_length = (seq1_length+seq2_length)*_counter;
	answer_length	= (ans1_length+ans2_length)*_counter;

	// commando aufbauen
	// instruction codes des Scanners sind immer ein byte lang
	buffer[0] = code()[0];
	buffer[1] = paramLength()>0 ? p[0] : 0;
	buffer[2] = paramLength()>1 ? p[1] : 0;
	calcCheckSum(buffer, 4);
	// calc return length
	// 0 -> 1
	// 2 -> 4
	// 4 -> 8
	// send
	if( d->sendCommand( buffer, 4 ) )
	{
		char *s = buffer+4;
		unsigned int cnt = sequence_length;
		while( _counter > 0 )
		{
			if( seq1_length > 0 &&
				!d->receiveAnswer( s, seq1_length, _millies  ))
				break;
			if( seq2_length > 0 &&
				!d->receiveAnswer( s+seq1_length, seq2_length, _millies ))
				break;

			if( seq1_length > 0 && verifyCheckSum(s, seq1_length) )
			{
				unsigned i = 0;
				unsigned o = (seq1_length-ans1_length)>>1;
				while( i < ans1_length ) {
					r.push_back(s[i+o]);
					i++;
				}
				answer_length -= ans1_length;
			}
			if( seq2_length > 0 && verifyCheckSum(s+seq1_length, seq2_length) )
			{
				unsigned i = 0;
				unsigned o = seq1_length + (seq2_length-ans2_length)>>1;
				while( i < ans2_length ) {
					r.push_back(s[i+o]);
					i++;
				}
				answer_length -= ans2_length;
			}
			s	+= seq1_length + seq2_length;
			cnt	-= seq1_length + seq2_length;
			_counter--;
		}
		if( suffix_length > 0 )
		{
			if( d->receiveAnswer( s, suffix_length, _millies  ) )
				if( verifyCheckSum( s, suffix_length ) )
				{
					suffix_length=0;
				}
		}
	}
	_counter = 0;
	return answer_length == 0 && suffix_length==0;
}

} // namespace











