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

#include "InstructionSet.h"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

std::ostream& operator<<(std::ostream& out, const InstructionSet::Instruction& i)
{
  out << i.mnemonic() << " [";

  out << ios::hex;
  for( unsigned int idx=0; idx < i.code().size(); idx++ )
    out << int(i.code()[idx]);
  out << ios::dec;
  out << "]: (" << i.paramLength() << ")->(" << i.returnLength() << ")";
  return out;
}

InstructionSet::Instruction::Instruction()
: _mnemonic(),
  _code(),
  _paramLength(0),
  _returnLength(0)
{}

InstructionSet::Instruction::Instruction(const std::string& mnemonic, const char code, int pl, int rl)
: _mnemonic(mnemonic),
  _code(1,code),
  _paramLength(pl),
  _returnLength(rl)
{}

InstructionSet::Instruction::Instruction(const std::string& mnemonic, const std::vector<char>& code, int pl, int rl)
: _mnemonic(mnemonic),
  _code(code),
  _paramLength(pl),
  _returnLength(rl)
{}

InstructionSet::Instruction::~Instruction()
{}


