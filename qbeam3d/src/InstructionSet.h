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

#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H

/**
  *@author Maximilian Hrabowski
  */

#include <string>
#include <map>
#include <vector>

#include "SpecSizedInt.h"

class Processor;

class InstructionSet
{

public:

		class Instruction
		{
		public:
			Instruction();
			Instruction(const std::string& mnemonic, const std::vector<char>& code, int pl=0, int rl=0 );
			Instruction(const std::string& mnemonic, const char code, int pl=0, int rl=0 );
			virtual	~Instruction();

			const std::vector<char>& code() const	{ return _code; }
			const std::string& mnemonic() const		{ return _mnemonic; }

			unsigned int paramLength() const     { return _paramLength; }
			unsigned int returnLength() const    { return _returnLength; }

			virtual bool execute( Processor* cpu, const char* p, char* r ) const=0;
			virtual bool execute( Processor* cpu, const std::vector<char>& p, std::vector<char>& r ) const=0;

		protected:

			std::string       _mnemonic;
			std::vector<char> _code;

			int _paramLength;
			int _returnLength;
		};

	~InstructionSet();
	const Instruction* instruction( unsigned int idx ) const;
	const Instruction* instruction( const std::string& mnemonic ) const;
	const Instruction* instruction( const std::vector<char>& mnemonic ) const;

	bool  addInstruction( const Instruction* i );

protected:

	InstructionSet();
  
	std::map<std::string,unsigned int>	_setMap;
	std::vector<const Instruction*>		_setVec;

	typedef std::vector<const Instruction*>::iterator           instIter;
	typedef std::vector<const Instruction*>::const_iterator     constInstIter;
	typedef std::map<std::string,unsigned int>::iterator        mnemIter;
	typedef std::map<std::string,unsigned int>::const_iterator  constMnemIter;
};

std::ostream& operator<<(std::ostream& out, const InstructionSet::Instruction& i);

#endif
