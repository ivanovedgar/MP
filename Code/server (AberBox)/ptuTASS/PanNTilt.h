/***************************************************************************
 *   Copyright (C) 2010 by Bertrand Rix   *
 *   bbr0ot@pcfg6.dcs.aber.ac.uk   *
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
#include <vector>
#include <iostream>
#include "uart.h"
#include "PTfunctions.h"
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <cstdio>
#include <time.h>
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
using namespace std;

/*This is an interface for the pan and tilt platform developed for the stabilization of the platform
 through attitude computing with a panoramic camera.
  This can also be used for every application that need to control the platform. For the list of everything the platform can do, please refer to the Servo 
command set documentation.
  
*/

//Structure for Pan and Tilt coordinates
struct PTcoord
{
	double pan;
	double tilt;

	PTcoord(double _pan, double _tilt)
	:pan(_pan),tilt(_tilt)
	{
	}
};

class PTinterface
{
 public:
	PTinterface(unsigned int baud, const char *device);
	~PTinterface();
	vector<unsigned char> response;

	/**Goto pan and tilt coordinates, the values in the _GotoCoord are in degrees and
	the function make the conversion in the command set format*/
	void Goto( PTcoord _GotoCoord );

	/**GotoHome*/
	void GotoHome();

	/**Verify Home, calibration of the home position, first thing to do*/
	void VerifyHome();

	/**set Acceleration on the Pan axis, _acc is in deg/sec²*/
	void setAccelerationPan( double _acc );

	/**set Acceleration on the tilt axis, _acc is in deg/sec²*/
	void setAccelerationTilt( double _acc );

	/**set Speed on the Pan axis, _rateSpeed is in deg/sec*/
	void setDiscreteRateSpeedPan( double _rateSpeed );

	/**set Speed on the tilt axis, rateSpeed is in deg/sec*/
	void setDiscreteRateSpeedTilt( double _rateSpeed );

	/**Pan to the left until it reaches the pan limit*/
	void PanLeft();

	/**Pan to the right until it reaches the pan limit*/
	void PanRight();

	/**Tilt to the back until it reaches the tilt limit*/
	void TiltDown();

	/**Tilt to the front until it reaches the tilt limit*/
	void TiltUp();

	/**Stop all movements on the Pan axis*/
	void PanStop();

	/**Stop all movements on the tilt axis*/
	void TiltStop();

	/**turn off motors, store current position*/
	void Halt();	
	
	/**get position stored at preset numPreset (0 to 255)*/
	void getPreset(int numPreset);
	/**store current position at preset numPreset (0 to 255)*/
	void setPreset(int numPreset);
	
	/**enable or disable the vector motion mod, move to a position such that both axes arrive at the designated position at the same time*/
	void setVectorMotion(bool v);

	/**Set the position limits on the pan and tilt axis,  position are sent as degrees*/
	void setPanTiltLimits(int PanCCW, int PanCW, int TiltCCW, int TiltCW);

	/**Get the position limits on the pan and tilt axis*/
	void getPanTiltLimits();

	/**Get the current position of the pan and tilt platform*/
	PTcoord getCurrentPosition();

	/**Clear the limits and set the default values*/
	void clearSoftwareLimits();

	/**Set a proportional pan speed. 
	\param[in] p between 0 and 15, 0 = slow , 15 = max
	*/
	void setProportionalPanSpeed(int p);
	
	/**Set a proportional tilt speed.
	\param[in] p between 0 and 15, 0 = slow , 15 = max
	*/
	void setProportionalTiltSpeed(int p);

	/**
	\brief Tell PTU to stabilize at whatever position it is right now.
	 */
	void Stabilize();

	/**
	\brief Move the gimbals at a specific inertial rate. Azimuth rate (Pan) and Elevation rate (Tilt). Values are in radians/sec.
	 */
	void setDriftRate();
	
	/**
	\brief Performs initial calibration.
	\param[in] calibrationTime Time period in seconds over which calibration will be performed, if false is returned try shorter period of time,
	anything greater that 55 seconds is likely to fail.
	\return	True if calibration was successfully, false if not.
	 */
	bool initialCalibration(int calibrationTime);
	
	/**
	\brief Performs recalibration. Calculates by how much PTU drifted since the last Stabilize command 
	and adjusts according values in PTUDriftRate structure. If recalibration fails initial calibration must be preformed first.
	\return True if calibration was successfully, false if not.
	 */
	bool recalibrate();
	
private :
	int devicePointer;
	
	/**
	\brief Generic function that calculates drift rate for the supplied values. It is used to calculate PTU drift for Azimuth and Elevation.
	\param[in] beginningPosition Pan or Tilt position at the beginning. 
	\param[in] endPosition Pan or Tilt position at the end.
	\param[in] time Amount of time in seconds that elapsed while PTU was drifting.
	\param[out] result Returned result is a drift rate in radians/sec.
	 */
	double getDriftRate(double beginningPosition, double endPosition, int time);
	
	/**
	\brief This function translates supplied value in to the ASCII characters and adds them to the supplied vector, so 
	that double number 10.02 would be represented as a series of numbers 49 48 46 48 50.
	\param[in] value Double number to be translate in to the ASCII. 
	\param[out] cmd Value encoded as ASCII is added to this vector.
	 */
	void PushD(double value, vector<unsigned char>& cmd);

	//used for the beginning of each command (always the same)		
	void PushBeginning(vector<unsigned char> &cmd);

	//used to convert the rotation values in degrees in the special command format
	void PushDoubleToChaine(double value, vector<unsigned char>& chaine);

	void PushLimits(int value, vector<unsigned char>& chaine);

	int CalcChecksum(const vector<unsigned char>& chaine);
};