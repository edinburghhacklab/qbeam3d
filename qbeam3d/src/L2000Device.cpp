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

#include "SerialPort.h"
#include "Processor.h"
#include "L2000Device.h"
#include "L2000InstructionSet.h"

#include <string>
#include <vector>

#include <iostream>
#include <iomanip>

L2000::Device::Device()
:	Processor(),
	_com()
{
	_instructionSet = new L2000::InstructionSet();
	_device = "call init()!";
}

L2000::Device::~Device()
{
	delete _instructionSet;
	_com.close();
}

bool L2000::Device::init()
{
	// Versuch /dev/ttySX X=[0..9] zu öffnen und prüft ob ein L2000 angeschlossen ist!

	_device = "/dev/ttySX";
	for( char x = 0; x<9; x++ )
	{
		_com.close();
		_device[9] = '0'+x;
		_com.init( _device, SerialPort::_19200 );
		//std::cerr << "Trying " << _device << " - " << std::endl;
		if( _com.open() )
		{
			std::vector<char>p(4,0x00);
			std::vector<char>r(0);
			r.clear();

			if( !execute( "VERSION", p, r ) )
				continue;
			Int16 ver1( &r[0], Endianess::BIG, 2 );
			r.clear();

			if( !execute( "VERSION", p, r ) )
				continue;
			Int16 ver2( &r[0], Endianess::BIG, 2 );
			r.clear();

			if( !execute( "VERSION", p, r ) )
				continue;
			Int16 ver3( &r[0], Endianess::BIG, 2 );
			r.clear();
			// Laser to pos 0, off, table off
			execute("GETIR", p, r );
			execute("LONOFF", p, r );
			execute("TONOFF", p, r );
			_com.close();
			if( ver1 == ver2 && ver2 == ver3 )
			{
				//std::cerr << "Conmark L2000 version " << std::hex <<  ver1 << std::dec  << " found!" << std::endl;
				return true;
			}
		}
		else
		{
			//std::cerr << " Couldn't open device." << std::endl;
		}
	}
	_com.close();
	return false;
}

bool L2000::Device::reset()
{
	close();
	return open();
}

bool L2000::Device::open()
{
	bool ret = true;
	if( status() != SerialPort::READY )
	{
		_com.close();
		ret = _com.open();
	}
	return ret;
}

bool L2000::Device::close()
{
	if( status() != SerialPort::CLOSED )
		return _com.close();
	return false;
}

bool L2000::Device::receiveAnswer( char *buf, int n, unsigned long timeout )
{
	//std::cerr << " - RECEIVE (" << n << "): ";
	char *p = buf;
	std::fill_n( p, n, 0xFF );
	if( status() == SerialPort::READY )
	{
		_com.setTimeout( timeout/1000, (timeout%1000)*1000 );
		if( _com.receive( buf, n ) == n ) {
			return true;
		}
	}
	//std::cerr << " FAILED ";
	return false;
}

bool L2000::Device::sendCommand( char *buf, int n, unsigned long timeout )
{
	//std::cerr << " - SEND: " << std::hex << int(buf[0]) << " " << int(buf[1]) << " " << int(buf[2]) << " " << int(buf[3]) << " " << std::dec << " - ";
	int r=0;
	if( status() == SerialPort::READY )
	{
		_com.setTimeout( timeout/1000, (timeout%1000)*1000 );
		if( (r=_com.send( buf, n )) == n )
			return true;
	}
	//std::cerr << "Couldn't write more than " << r << " bytes to device." << std::endl;
	return false;
}


int L2000::Device::status() const
{
	return _com.status();
}



