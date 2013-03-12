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

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <string>
#include <iostream>

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

/**
  *@author Maximilian Hrabowski
  */

class SerialPort {
public:

	typedef enum {
	_1200=B1200,
	_2400=B2400,
	_4800=B4800,
	_9600=B9600,
	_19200=B19200,
	_38400=B38400,
	_57600=B57600,
	_115200=B115200 } BaudRate;

	enum {
	CLOSED=0,
	READY,
	READ_TIMEOUT,
	WRITE_TIMEOUT,
	IDLE
	} STATUS_CODES;

	SerialPort();
	~SerialPort();

	const std::string& name() const;

	bool init( const std::string& name, BaudRate p=_2400 );
	bool open();
	bool close();
	int  status() const;
	unsigned statusBits() const { return _statusBits; }

	int  receive( char *buf, int n ) const;
	int  send( char *buf, int n ) const;

	bool setBaud( BaudRate i );
	void setTimeout( int secs, int usecs );
	int  getBytesWaiting() const;

protected: // Protected attributes

	void child_handler(int s);

	//! current status of the device
	mutable int	_status;

	//! filename of the device e.g. "/dev/ttyS0"
	std::string _name;

	int			_timeout_secs;
	int			_timeout_usecs;

	// BaudRate to use
	BaudRate	_baudRate;

	// flags to be set by signal handler
	//volatile bool stop;

	// file descriptor for unix device IO
	int		_fd;

private:
	// real hard OS stuff
	// struct for setting up serial io params
	termios oldtio,newtio;
	// struct for setting up signal handler
	//struct sys::sigaction sa;

	mutable unsigned int _statusBits;
	void queryBits() const;

};

#endif
