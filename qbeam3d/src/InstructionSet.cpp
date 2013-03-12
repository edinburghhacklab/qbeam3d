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
#include <iostream>

InstructionSet::InstructionSet()
: _setMap(),
  _setVec()
{}

InstructionSet::~InstructionSet()
{
 for( constInstIter iIt = _setVec.begin(); iIt!=_setVec.end(); iIt++ )
 {
    delete *iIt;
 } 
}

bool InstructionSet::addInstruction( const Instruction *i )
{
  if( _setMap.find(i->mnemonic()) == _setMap.end() )
  {
    unsigned int idx = _setVec.size();
    _setVec.push_back( i );
    _setMap[ i->mnemonic() ] = idx;
    return true;
  }
  return false;
}

const InstructionSet::Instruction* InstructionSet::instruction( const std::string &mnemonic ) const
{
  constMnemIter mIt = _setMap.find( mnemonic );
  if( mIt != _setMap.end() )
    return _setVec[ mIt->second ];
  std::cerr << "InstructionSet::instruction(mnemonic): not found." << std::endl;
  return 0;
}

const InstructionSet::Instruction* InstructionSet::instruction( const std::vector<char>& code ) const
{
	for( constInstIter instIt = _setVec.begin(); instIt!=_setVec.end(); instIt++ )
		if( (*instIt)->code() == code )
			return *instIt;
  std::cerr << "InstructionSet::instruction(code): not found." << std::endl;
  return 0;
}

const InstructionSet::Instruction* InstructionSet::instruction( unsigned int idx ) const
{
  if( idx < _setVec.size() )
    return _setVec[idx];
  std::cerr << "InstructionSet::instruction(idx): out of bounds." << std::endl;
  return 0;
}







