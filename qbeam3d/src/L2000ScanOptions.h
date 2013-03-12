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

#ifndef _L2000SCANOPTIONS_
#define _L2000SCANOPTIONS_

#include "Mesh.h"

namespace L2000
{

struct ScanOptions
{
	ScanOptions() :
		laserStart(20),
		laserEnd(180),
		laserStep(1.0),
		tableRange(720),
		tableStep(1.0),
		layer(2),
		scanMode(Full),
		contrastMode(Automatic),
		holeThreshold(-1),
		minBorderPoints(2),
		createLid(false),
		createBottom(false),
		smoothBorders(false),
		correctReadings(true),
		createCCD1Phantom(false),
		createCCD2Phantom(false),
		ccd1Color(255,0,0),
		ccd2Color(0,255,0),
		solidColor(128,128,128)
	{}

	typedef enum {
		Full,
		VerticalCut,
		HorizontalCut,
	} ScanMode;

	typedef enum {
		Once=0,
		PerColumn=1,
		Automatic=2,
		Calibration=3,
	} ContrastMode;

	int laserRange() const { return laserEnd-laserStart+1; }

	int		laserStart;
	int		laserEnd;
	double	laserStep;

	int		tableRange;
	double	tableStep;

	int		layer;

	ScanMode		scanMode;
	ContrastMode	contrastMode;

	int		holeThreshold;
	int		minBorderPoints;

	bool	createLid;
	bool	createBottom;

	bool	smoothBorders;
	bool	correctReadings;
	
	bool	createCCD1Phantom;
	bool	createCCD2Phantom;
	
	Mesh::Color ccd1Color;
	Mesh::Color ccd2Color;
	Mesh::Color solidColor;
};

}

#endif
