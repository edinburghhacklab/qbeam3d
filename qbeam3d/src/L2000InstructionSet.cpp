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

#include "Processor.h"

#include "L2000InstructionSet.h"
#include "L2000Device.h"

L2000::InstructionSet::~InstructionSet()
{}

L2000::InstructionSet::InstructionSet() :
	::InstructionSet()
{
  bool ok = true;
  ok = ok && addInstruction( new Instruction("NOP", NOP, 0, 0, 0) );
  ok = ok && addInstruction( new Instruction("CCD1", CCD1, 0, 4, 0, 1000 ) );
  ok = ok && addInstruction( new Instruction("ITSET1", ITSET1, 2, 2, 2, 5000) );
  ok = ok && addInstruction( new Instruction("ITSET2", ITSET2, 2, 2, 2, 5000 ) );
  ok = ok && addInstruction( new Instruction("SETBD", SETBD, 1, 0, 1, 1000 ) );
  ok = ok && addInstruction( new Instruction("CCD2", CCD2, 0, 4, 0, 1000) );
  ok = ok && addInstruction( new Instruction("IAUTO", IAUTO, 0, 4, 0, 5000 ) );
  ok = ok && addInstruction( new Instruction("LONOFF", LONOFF, 1, 0, 1) );
  ok = ok && addInstruction( new Instruction("LDOWN1", LDOWN1, 0, 0) );
  ok = ok && addInstruction( new Instruction("LUP1", LUP1, 0, 0) );
  ok = ok && addInstruction( new Instruction("TLEFT1", TLEFT1, 0, 0) );
  ok = ok && addInstruction( new Instruction("TRIGHT1", TRIGHT1, 0, 0) );
  ok = ok && addInstruction( new Instruction("WRITE", WRITE, 2, 2, 2) );
  ok = ok && addInstruction( new Instruction("READ", READ, 1, 2, 1) );
  ok = ok && addInstruction( new Instruction("WIDL", WIDL, 2, 0, 1) );
  ok = ok && addInstruction( new SequenceInstruction("REPT", REPT, 1, 0, 1) );
  ok = ok && addInstruction( new SequenceInstruction("SEQUEX", SEQUEX, 2, -1 , 2) );
  ok = ok && addInstruction( new Instruction("LDOWN", LDOWN, 0, 0, 0, 5000) );
  ok = ok && addInstruction( new Instruction("LUP", LUP, 0, 0, 0, 5000) );
  ok = ok && addInstruction( new Instruction("TLEFT", TLEFT, 0, 0, 0, 20000) );
  ok = ok && addInstruction( new Instruction("TRIGHT", TRIGHT, 0, 0, 0, 5000) );
  ok = ok && addInstruction( new Instruction("U_21", U_21, 0, 0) );
  ok = ok && addInstruction( new Instruction("CCD12F", CCD12F, 0, 4, 0, 1000) );
  ok = ok && addInstruction( new Instruction("U_23", U_23, 0, 0) );
  ok = ok && addInstruction( new Instruction("TONOFF", TONOFF, 1, 0, 1) );
  ok = ok && addInstruction( new Instruction("U_25", U_25, 0, 0) );
  ok = ok && addInstruction( new Instruction("U_26", U_26, 0, 0) );
  ok = ok && addInstruction( new Instruction("GETIR", GETIR, 2, 2, 1, 20000 ) );
  ok = ok && addInstruction( new Instruction("DELL", DELL, 1, 0, 1) );
  ok = ok && addInstruction( new Instruction("CCD1M", CCD1M, 0, 2) );
  ok = ok && addInstruction( new Instruction("CCD2M", CCD2M, 0, 2) );
  ok = ok && addInstruction( new Instruction("MIDDLE", MIDDLE, 0, 4, 0, 1000) );
  ok = ok && addInstruction( new Instruction("VERSION", VERSION, 0, 2) );
  ok = ok && addInstruction( new Instruction("ITTH", ITTH, 0, 4) );
  ok = ok && addInstruction( new Instruction("XITTH1", XITTH1, 2, 0, 2) );
  ok = ok && addInstruction( new Instruction("XITTH2", XITTH2, 2, 0, 2) );
  ok = ok && addInstruction( new SequenceInstruction("SEQ2", SEQ2, 2, -1, 2 ) );
  ok = ok && addInstruction( new Instruction("WIDT", WIDT, 2, 0, 1) );
  ok = ok && addInstruction( new Instruction("DELT", DELT, 1, 0, 1) );
  ok = ok && addInstruction( new SequenceInstruction("SEQ3", SEQ3, 2, -1, 2 ) );
  ok = ok && addInstruction( new Instruction("FIAUTO", FIAUTO, 2, 4, 1) );
  ok = ok && addInstruction( new Instruction("U_41", U_41, 0, 0) );
  ok = ok && addInstruction( new Instruction("U_42", U_42, 0, 0) );
  ok = ok && addInstruction( new Instruction("EXTVER", EXTVER, 0, 4) );
  ok = ok && addInstruction( new Instruction("FOTO", FOTO, 2, 2, 1) );
  ok = ok && addInstruction( new SequenceInstruction("LEDCHK", LEDCHK, 1, 9*2, 1, 10000 ) );
  ok = ok && addInstruction( new Instruction("CCD1ML", CCD1ML, 0, 2) );
  ok = ok && addInstruction( new Instruction("CCD2ML", CCD2ML, 0, 2) );
  ok = ok && addInstruction( new Instruction("CCDML", CCDML, 0, 4));
  ok = ok && addInstruction( new Instruction("U_49", U_49, 0, 4));
  ok = ok && addInstruction( new Instruction("CMD32", CMD32, 1, 2, 1));

}


