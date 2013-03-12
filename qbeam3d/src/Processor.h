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


#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>

#include <InstructionSet.h>

class Processor
{
public: 
	Processor();
	virtual ~Processor();

	virtual bool init();
	virtual bool reset();
	virtual bool run();
	virtual bool stop();

	virtual int status() const;

	virtual bool execute( const std::string& mnemonic, const char* p, char* r );
	virtual bool execute( const std::string& mnemonic, const int p, std::vector<char>& r );
	virtual bool execute( const std::string& mnemonic, const std::vector<char>& p, std::vector<char>& r );

	const	InstructionSet* instructionSet() const;
	bool	hasInstruction(const std::string& mnemonic) const;

	static void usleep( unsigned long usecs );
	
protected:

	InstructionSet* _instructionSet;
};

#endif
