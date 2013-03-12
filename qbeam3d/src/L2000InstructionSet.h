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

#ifndef L2000SET_H
#define L2000SET_H

#include <InstructionSet.h>

/**Spezifisches Instruction Set des Conmark L2000 3d Scanners
  *@author Maximilian Hrabowski
  */

class Processor;

namespace L2000 {

class Device;

class InstructionSet : public ::InstructionSet
{
public:

	typedef enum
	{
	NOP=0,  CCD1,   ITSET1, ITSET2, SETBD,  CCD2,   IAUTO,  LONOFF,
	LDOWN1, LUP1,   TLEFT1, TRIGHT1,WRITE,  READ,   WIDL,   REPT,
	SEQUEX, LDOWN,  LUP,    TLEFT,  TRIGHT, U_21,   CCD12F, U_23,
	TONOFF, U_25,   U_26,   GETIR,  DELL,   CCD1M,  CCD2M,  MIDDLE,
	VERSION,ITTH,   XITTH1, XITTH2, SEQ2,   WIDT,   DELT,   SEQ3,
	FIAUTO, U_41,   U_42,   EXTVER, FOTO,   LEDCHK, CCD1ML, CCD2ML,
	CCDML, U_49, CMD32
	} InstructionCode;


	class Instruction : public ::InstructionSet::Instruction
	{
	public:

		Instruction(const std::string& mnemonic, const InstructionCode code, int pl=0, int rl=0, int args=0, unsigned long timeout_millies=100 );
		~Instruction();

		virtual bool execute( Processor* dev, const char* p, char* r ) const;
		virtual bool execute( Processor* dev, const std::vector<char>& p, std::vector<char>& r ) const;

		unsigned int argCount() const { return _argc; }

	protected:
		  //! verify XOR checksum value
		bool verifyCheckSum(const char* buf, unsigned int cmdLength) const;
		void calcCheckSum( char* buf, unsigned int cmdLength ) const;
		// number of logical params given
		unsigned int _argc;
		// timeout int milli-secs
		unsigned long _millies;

		std::ostream& printCall( std::ostream& out, const char* params ) const;
		std::ostream& printAnswer( std::ostream& out, const char* answer ) const;
	};

	class SequenceInstruction : public Instruction
	{
	public:

		~SequenceInstruction();
		SequenceInstruction( const std::string& mnemonic, const InstructionCode code, int pl=0, int rl=0, int args=0, unsigned long timeout_millies=100 );

		virtual bool execute(Processor* dev, const char *p, char *r) const;
		virtual bool execute(Processor* dev, const std::vector<char>& p, std::vector<char>& r) const;

	protected:

		mutable unsigned int _counter;
	};


	InstructionSet();
	~InstructionSet();

  //! already decl'd in InstructionSet
  //bool execute( int idx, const char *p, char *r ) const;
  //bool execute( const std::string& mnemonic, const char *p, char *r ) const;
  //bool execute( const std::string& code, const char* p, char *r) const;
};

} // namespace

#endif
