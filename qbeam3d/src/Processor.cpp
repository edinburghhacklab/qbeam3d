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

#include <Processor.h>
#include <InstructionSet.h>

#include <string>
#include <vector>

#include <unistd.h>

Processor::Processor()
: _instructionSet(0)
{}

Processor::~Processor() {}

bool Processor::init()
{ return true; }

bool Processor::reset()
{ return true; }

bool Processor::run()
{ return true; }

bool Processor::stop()
{ return true; }

int  Processor::status() const
{ return 0; }

void Processor::usleep( unsigned long usecs )
{
	unsigned wt = ::usleep( usecs );

	if( wt == 0 )
	    ::usleep( 10000 );
}

bool Processor::execute( const std::string& mnemonic, const char* p, char* r )
{
	const InstructionSet::Instruction* ins = _instructionSet->instruction( mnemonic );
	if( ins )
		return ins->execute( this, p, r );
	return false;
}

bool Processor::execute( const std::string& mnemonic, const int p, std::vector<char>& r )
{
	std::vector<char> params(sizeof(int));
	for( unsigned i=0; i<params.size(); i++ )
		params[i] = (p >> (8*i))&255;
	return execute( mnemonic, params, r );
}

bool Processor::execute( const std::string& mnemonic, const std::vector<char>& p, std::vector<char>& r )
{
	const InstructionSet::Instruction* ins = _instructionSet->instruction( mnemonic );
	if( ins )
		return ins->execute( this, p, r );
	return false;
}

const InstructionSet* Processor::instructionSet() const
{
	return _instructionSet;
}
bool Processor::hasInstruction(const std::string& mnemonic) const
{
	return _instructionSet->instruction( mnemonic ) != 0;
}
	
