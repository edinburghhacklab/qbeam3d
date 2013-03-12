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

#include "macros.h"
#include "SerialPort.h"

SerialPort::SerialPort()
:	_status(CLOSED),
	_name("not yet tried"),
	_timeout_secs(1),
	_timeout_usecs(0),
	_baudRate(_19200),
	_fd(-1),
	_statusBits(0)
{
}

SerialPort::~SerialPort()
{
	close();
}

void SerialPort::child_handler(int s)
{
	s=s;
}

bool SerialPort::init( const std::string& name, BaudRate p )
{

//  Set bps rate and hardware flow control and 8n1 (8bit,no parity,1 stopbit).
//  Also don't hangup automatically and ignore modem status.
//  Finally enable receiving characters.

	_name		= name;
	_baudRate	= p;
	return true;
}

bool SerialPort::open()
{
	int status=0;
//  Open modem device for reading and writing and not as controlling tty
//  because we don't want to get killed if linenoise sends CTRL-C.

	_fd = ::open(_name.c_str(), O_RDWR | O_NOCTTY  );
	if (_fd <0) {
		std::cerr << "Error while opening device: " << _name << std::endl;
		_status = CLOSED;
		return false;
	}
// save com port settings
	tcgetattr(_fd,&oldtio);

// setup serial for scanner

	bzero(&newtio, sizeof(newtio));

	// set baudrate, 8 bit, no parity, 1 stop bit,
	newtio.c_cflag =	_baudRate |// Baudrate
						CLOCAL |	// no modem control
						CREAD |	// enable receiving chars
//						CRTSCTS |	// hardware flow control
						CS8;		// 8N1

	newtio.c_iflag =	IGNPAR;	// ignore bytes with parity errors

//	newtio.c_iflag |= IXON | IXOFF | IXANY; // software flow control
//	newtio.c_iflag &= ~(IXON | IXOFF | IXANY); // hardware flow control

	newtio.c_oflag = 0;			// raw output mode

//	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw mode

	newtio.c_lflag = 0;			// set input mode raw(non-canonical, no echo,...)

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

// set control characters, EOF=0, ERR=0, BRK=0, EVT=0, XON=11, XOFF=13
	newtio.c_cc[VSTART] = 11;
	newtio.c_cc[VSTOP] = 13;
	newtio.c_cc[VEOF]=0;
	newtio.c_cc[VEOL]=0;
	newtio.c_cc[VEOL2]=0;
	newtio.c_cc[VQUIT]=0;
	newtio.c_cc[VINTR]=0;
	newtio.c_cc[VERASE]=0;
	newtio.c_cc[VKILL]=0;
	newtio.c_cc[VMIN]=0;
	newtio.c_cc[VTIME]=1000; // wait one half a second for data

	tcflush( _fd, TCIFLUSH);
	tcsetattr( _fd, TCSANOW, &newtio);
	tcgetattr( _fd, &newtio );

// clear rts
	ioctl(_fd, TIOCMGET, &status);
	status &= ~TIOCM_RTS;
	ioctl(_fd, TIOCMSET, &status);
// clear dtr
	ioctl(_fd, TIOCMGET, &status);
	status &= ~TIOCM_DTR;
	ioctl(_fd, TIOCMSET, &status);

	_status = READY;
	queryBits();
	return true;
}

void SerialPort::queryBits() const
{
	if( _fd > 0 )
		ioctl(_fd, TIOCMGET, &_statusBits);

	//std::cerr << std::endl << "MODEM BITS: 0x" << std::hex << _statusBits << std::dec << std::endl;
}

/** No descriptions */
bool SerialPort::close()
{
 if( _fd > 0 )
 {
	tcsetattr(_fd,TCSANOW,&oldtio);
	::close(_fd);
	_status = CLOSED;
	return true;
 }
 return false;
}

//! Receive bytes from COM Port
/** Tries to read bytes from COM Port with timeout
	\param buf Buffer to write bytes to
	\param n Number of bytes to read
	\param block continueous try to read the data
*/
int SerialPort::receive( char *buf, int n ) const
{
	int cnt = 0;
	int r = 0;

	while( cnt < n )
	{
		timeval tv;
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(_fd, &readfds );
		tv.tv_sec = _timeout_secs;
		tv.tv_usec = _timeout_usecs;

		unsigned long timeout_millies = _timeout_secs*1000+_timeout_usecs/1000;

//		std::cerr << "Waiting for data: " << n-cnt << " bytes, timeout " << timeout_millies << " ms - ";
//		std::cerr.flush();
//		struct timeval before,after;
//		int ok = sys::gettimeofday( &before, 0 );

		r = select( _fd+1, &readfds, NULL, NULL, &tv );
		if( r < 0 )
		{
			std::cerr << "select() FAILED on read!" << std::endl;
			break;
		}
		else if( r == 0 )
		{
			_status = READ_TIMEOUT;
//			ok += sys::gettimeofday( &after, 0 );
//			timeout_millies = ((after.tv_sec - before.tv_sec) * 1000) + ((after.tv_usec - before.tv_usec ) / 1000);
			std::cerr << "READ timeout after " << timeout_millies << " ms" << std::endl;
			break;
		}
		else if( FD_ISSET( _fd, &readfds ) )
		{
			r = ::read( _fd, &buf[cnt], (n-cnt) );
			//std::cout << "READ RETURNS: " << r << std::endl;

			if( r < 0 )
			{
				perror("Error on READ");
			}
			else {
				cnt	 += r;
				if( cnt != n )
				{
					std::cout << "BYTES READ SO  FAR: ";
					dump_int( std::cout, buf, n );
					std::cout << std::endl;
				}
			}
//			ok += sys::gettimeofday( &after, 0 );
//			timeout_millies = ((after.tv_sec - before.tv_sec) * 1000) + ((after.tv_usec - before.tv_usec ) / 1000);
//			std::cerr << r << " bytes read after " << timeout_millies << " ms" << std::endl;
		}
	}
	queryBits();
	return cnt;
}

int SerialPort::send( char *buf, int n ) const
{
/*
	int i = sys::write( _fd, buf, n );
	if( i == -1 ) {
		_status = WRITE_TIMEOUT;
		i = 0;
		perror("Serialport");
	}
	return i;
*/
	int cnt = 0;

	while( cnt < n )
	{
		//cout << "."
		int r = 0;

		timeval tv;
		fd_set writefds;
		FD_ZERO(&writefds);
		FD_SET(_fd, &writefds );
		tv.tv_sec = _timeout_secs;
		tv.tv_usec = _timeout_usecs;

		unsigned long timeout_millies = _timeout_secs*1000+_timeout_usecs/1000;

//		std::cerr << "Waiting for data: " << n-cnt << " bytes, timeout " << timeout_millies << " ms - ";
//		std::cerr.flush();
//		struct timeval before,after;
//		int ok = sys::gettimeofday( &before, 0 );

		r = select( _fd+1, NULL, &writefds, NULL, &tv );
		if( r < 0 )
		{
			std::cerr << "select() FAILED on write!" << std::endl;
			break;
		}
		else if( r == 0 )
		{
			_status = WRITE_TIMEOUT;
//			ok += sys::gettimeofday( &after, 0 );
//			timeout_millies = ((after.tv_sec - before.tv_sec) * 1000) + ((after.tv_usec - before.tv_usec ) / 1000);
			std::cerr << "WRITE timeout after " << timeout_millies << " ms" << std::endl;
			break;
		}
		if( FD_ISSET(_fd, &writefds ) )
		{
			r = write( _fd, &buf[cnt], (n-cnt) );
			//std::cout << "WRITE returns: " << r << std::endl;
			if( r < 0 )
			{
				perror("Error on write");
				break;
			}
			else {
				cnt	 += r;
				if( r != n )
				{
					std::cout << "ONLY " << r << " BYTES WRITTEN" << std::endl;
				}
			}
//			ok += sys::gettimeofday( &after, 0 );
//			timeout_millies = ((after.tv_sec - before.tv_sec) * 1000) + ((after.tv_usec - before.tv_usec ) / 1000);
//			std::cerr << r << " bytes read after " << timeout_millies << " ms" << std::endl;
		}
	}
	queryBits();
	return cnt;
}

const std::string& SerialPort::name() const
{
	return _name;
}

int SerialPort::status() const
{
	return _status;
}

bool SerialPort::setBaud( BaudRate i )
{
	if( _fd > 0 )
	{
		tcgetattr(_fd,&newtio);
		cfsetispeed( &newtio, i );
		cfsetospeed( &newtio, i );
		tcsetattr(_fd, TCSANOW, &newtio);
	}
	return true;
}


void SerialPort::setTimeout( int secs, int usecs )
{
	_timeout_secs = secs;
	_timeout_usecs = usecs;
/*
	if( _fd >= 0 ) {
		tcgetattr( _fd, &newtio );
		newtio.c_cc[VTIME]=dezi;
		tcsetattr( _fd, TCSANOW,&newtio );
	}
	return true;
*/
}

int SerialPort::getBytesWaiting() const
{
	int n=0;
	if( _fd >= 0 )
	{
		ioctl( _fd, FIONREAD, &n );
	}
	return n;
}


/*
// setup serial for scanner
// set baud rate
	tcgetattr(_fd,&newtio);
	cfsetispeed( &newtio, _baudRate );
	cfsetospeed( &newtio, _baudRate );
	tcsetattr(_fd, TCSANOW, &newtio);
// clear rts
	ioctl(_fd, TIOCMGET, &status);
	status &= ~TIOCM_RTS;
	ioctl(_fd, TIOCMSET, &status);
// clear dtr
	ioctl(_fd, TIOCMGET, &status);
	status &= ~TIOCM_DTR;
	ioctl(_fd, TIOCMSET, &status);
//	sys::ioctl( fd, TIOCM_DTR, TIOCMBIC ); // clear DTR
// set line control to 8,1,n
	newtio.c_cflag = CREAD | CLOCAL;
	newtio.c_cflag &= ~PARENB;
	newtio.c_cflag &= ~CSTOPB;
	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag |= CS8;
//	newtio.c_cflag |= CRTSCTS;
	tcsetattr(_fd, TCSANOW, &newtio);
// set control characters, EOF=0, ERR=0, BRK=0, EVT=0, XON=11, XOFF=13
	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw mode

//	newtio.c_iflag |= IXON | IXOFF | IXANY; // software flow control
	newtio.c_iflag &= ~(IXON | IXOFF | IXANY); // hardware flow control

	newtio.c_oflag &= ~OPOST; // raw mode for output
	newtio.c_cc[VSTART] = 0;
	newtio.c_cc[VSTOP] = 0;
	newtio.c_cc[VEOF]=0;
	newtio.c_cc[VEOL]=0;
	newtio.c_cc[VEOL2]=0;
	newtio.c_cc[VQUIT]=0;
	newtio.c_cc[VINTR]=0;
	newtio.c_cc[VERASE]=0;
	newtio.c_cc[VKILL]=0;
// set flow control -> shake=0, replace=0, xonlimit=2048, xofflimit=512
	tcsetattr(_fd, TCSANOW, &newtio);
// set timeouts rI=0, rM=100, rc=100000, wm=0, wc=0
	newtio.c_cc[VMIN]=0;
	newtio.c_cc[VTIME]=1000; // wait one half a second for data
	tcsetattr(_fd, TCSANOW, &newtio);
// set timeouts ri=0, rm=100, rc=50, wm=0, wc=0
//	newtio.c_cc[VMIN]=0;
//	newtio.c_cc[VTIME]=5; //  100*4 = 400 + 50 = 450 / 100 = 4.5
//	tcsetattr(_fd, TCSANOW, &newtio);

//	| CRTSCTS // hardware flow control
//	| CS8.//CSIZE  character  size mask.  Values are CS5, CS6, CS7, or
//	| CSTOPB // set two stop bits, rather than one.
//	| CREAD //  enable receiver.
//	| PARENB // enable  parity  generation  on  output  and  parity checking for input.
//	| PARODD // parity for input and output is odd.
//	| HUPCL // lower modem control lines after last process closes the device (hang up).
//	| CLOCAL // ignore modem control lines
//	| CIBAUD // mask for input speeds (not used).

//	newtio.c_iflag = IGNPAR; // Ignore bytes with parity errors and make terminal raw and dumb.
//	newtio.c_oflag = 0; // Raw output.
//	newtio.c_lflag = 0; // Don't echo characters because if you connect to a host it or your
// modem will echo characters for you. Don't generate signals.
*/




