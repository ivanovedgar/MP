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
#include "PanNTilt.h"

//structure for drift rate in radians per second
struct DriftRate
{
	double panDriftRate;
	double tiltDriftRate;
	int StabilizationTime;
	
	DriftRate(double _panDriftRate, double _tiltDriftRate, int _StabilizationTime)
	:panDriftRate(_panDriftRate),tiltDriftRate(_tiltDriftRate),StabilizationTime(_StabilizationTime)
	{
	}
}PTUDriftRate(0.0,0.0,-1.0);
 
PTcoord PositionBeforeStabilization(0.0,0.0);
 
PTinterface::PTinterface(unsigned int baud, const char *device)
{	
	devicePointer = uart_init(9600, device);
}

PTinterface::~PTinterface()
{
}

void PTinterface::PushBeginning(vector<unsigned char> &cmd)
{
	//push the common value of all the command send to the robot
	cmd.push_back(248);
	cmd.push_back(255);
	cmd.push_back(255);
	cmd.push_back(255);
	cmd.push_back(255);
}

void PTinterface::Goto(PTcoord _GotoCoord)
{
	vector<unsigned char> cmd;
	vector<unsigned char> result;
	PushBeginning(cmd);
	cmd.push_back(9);
	cmd.push_back(80); //P
	PushDoubleToChaine(_GotoCoord.pan,cmd);
	PushDoubleToChaine(_GotoCoord.tilt,cmd);
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);
	cout<<"\nGoing To "<<_GotoCoord.pan<<" "<<_GotoCoord.tilt<<endl;
	int valid = send(devicePointer,cmd);	
}

void PTinterface::GotoHome()
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(72);//H
	cmd.push_back(79);//O	
	cmd.push_back(149);//checksum
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);	

	if(valid == 0)
		cout<<"GotoHome OK"<<endl;
	else if(valid == -1 )
		cout<<"Nak GotoHome"<<endl;
	else if( valid == -2)
		cout<<" ? GotoHome"<<endl;
}

void PTinterface::VerifyHome()
{
	vector<unsigned char> cmd;
	vector<unsigned char> result;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(72);//H
	cmd.push_back(86);//V
	cmd.push_back(156);//checksum
	cmd.push_back(0);
	
	int valid = send(devicePointer,cmd,result);
	
}

void PTinterface::getPreset(int numPreset)
{
	vector<unsigned char> cmd;
	vector<unsigned char> result;
	PushBeginning(cmd);
	cmd.push_back(4);//payload size
	cmd.push_back(80);
	cmd.push_back(63);

	//conversion of the preset number
	int hex = numPreset/16;
	int reste = numPreset%16;	
	
	if((hex/10)==0)
		cmd.push_back(hex);
	else
		cmd.push_back(65+(hex%10));

	if((reste/10)==0)
		cmd.push_back(hex);
	else
		cmd.push_back(65+(reste%10));

	cmd.push_back(131);//checksum
	cmd.push_back(0);

	int valid = send(devicePointer,cmd,result);	
}

void PTinterface::setPreset(int numPreset)
{
	vector<unsigned char> cmd;
	vector<unsigned char> result;
	PushBeginning(cmd);
	cmd.push_back(3);//payload size
	cmd.push_back(83);	

	//conversion of the preset number
	int hex = numPreset/16;
	int reste = numPreset%16;	
	
	if((hex/10)==0)
		cmd.push_back(hex);
	else
		cmd.push_back(65+(hex%10));

	if((reste/10)==0)
		cmd.push_back(hex);
	else
		cmd.push_back(65+(reste%10));
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);	
}

/* acceleration in degree*second-² */
void PTinterface::setAccelerationPan(double _acc)
{	
	vector<unsigned char> cmd;	
	PushBeginning(cmd);
	cmd.push_back(6);
	cmd.push_back(65);// A acceleration
	cmd.push_back(80);// P pan
	PushDoubleToChaine(_acc,cmd);
	cmd.push_back(CalcChecksum(cmd));//checksum
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);
	
}

void PTinterface::setAccelerationTilt(double _acc)
{
	vector<unsigned char> cmd;	
	PushBeginning(cmd);
	cmd.push_back(6);
	cmd.push_back(65);// A acceleration
	cmd.push_back(84);// T pan
	PushDoubleToChaine(_acc,cmd);
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);

	if(valid == 0)
		cout<<"Acc change OK"<<endl;
	else if(valid == -1 )
		cout<<"Nak Acc change"<<endl;
	else if( valid == -2)
		cout<<" ? Acc change"<<endl;	
	
}

void PTinterface::setDiscreteRateSpeedPan(double _rateSpeed)
{	
	vector<unsigned char> cmd;	
	PushBeginning(cmd);
	cmd.push_back(6);
	cmd.push_back(82);// R rate
	cmd.push_back(80);// P pan
	PushDoubleToChaine(_rateSpeed,cmd);
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);
	
}

void PTinterface::setDiscreteRateSpeedTilt(double _rateSpeed)
{
	vector<unsigned char> cmd;	
	PushBeginning(cmd);
	cmd.push_back(6);
	cmd.push_back(82);// R rate
	cmd.push_back(84);// T tilt
	PushDoubleToChaine(_rateSpeed,cmd);
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);	
}

void PTinterface::PanLeft()
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(80);//P
	cmd.push_back(76);//L
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);	
	//cout<<"Pan Left"<<endl;
}

void PTinterface::PanRight()
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(80);//P
	cmd.push_back(82);//R
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);
	//cout<<"Pan Right"<<endl;	

	int valid = send(devicePointer,cmd);	
}

void PTinterface::PanStop()
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(80);//P
	cmd.push_back(83);//S
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);
	//cout<<"Pan Stop"<<endl;	
}

void PTinterface::TiltStop()
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(84);//T
	cmd.push_back(83);//S
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);
	//cout<<"Tilt Stop"<<endl;	
}

void PTinterface::TiltUp()
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(84);//T
	cmd.push_back(85);//U
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);
	//cout<<"TiltUp OK"<<endl;	
}

void PTinterface::TiltDown()
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(84);//T
	cmd.push_back(68);//D
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);
	//cout<<"TiltDown OK"<<endl;	
}

/*stores current position (where ?) and turn of motors*/
void PTinterface::Halt()
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);//payload size
	cmd.push_back(72);//H
	cmd.push_back(65);//A
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	int valid = send(devicePointer,cmd);	
}

/*this function put in the vector "chaine" the value "value" formated as described in the Pan and tilt command set documentation
  this are sent as hundredth of a degree converted as an hexadecimal value then put as an ascii code of the value*/
void PTinterface::PushDoubleToChaine(double value, vector<unsigned char>& chaine)
{
	int v = value*100;
	char *str =new char[20];
	int length = sprintf(str,"%X",v);
	int deb =0;
	
	//To add some 0 if the value is on less than 4 bytes , for 5 degrees -> 500 -> 1F4 -> 01F4
	if(v<0)
	{
		deb =4;		
		for(int i=length-4; i<4; i++)
			chaine.push_back(48);
	}
	else
		for(int i=length ; i<4; i++)
			chaine.push_back(48);
	

	for(int i=deb; i<length ; i++)
	{	
		int tmp = str[i];
		chaine.push_back(tmp);
	}
}

void PTinterface::setVectorMotion(bool v)
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(3);
	cmd.push_back(86);
	cmd.push_back(77);
	if(v)
	  cmd.push_back(49);
	else
	  cmd.push_back(48);
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	cout<<"\nChange Vector Motion mode..."<<endl;
	int valid = send(devicePointer,cmd);
}

/*Compute the checksum of the command chaine by jumping the fisrt element of chaine*/
int PTinterface::CalcChecksum(const vector<unsigned char>& chaine)
{

	int checksum=0;
	for(int i = 1 ; i< chaine.size(); i++)
		checksum+=chaine[i];

	int Q = checksum / 256;
	checksum = checksum - (Q*256);
	return checksum;
}

void PTinterface::setPanTiltLimits(int PanCCW, int PanCW, int TiltCCW, int TiltCW)
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(14);
	cmd.push_back(76);//L
	cmd.push_back(83);//S
	PushLimits(PanCCW,cmd);
	PushLimits(PanCW,cmd);
	PushLimits(TiltCCW,cmd);
	PushLimits(TiltCW,cmd);	
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);

	cout<<"\nsetting Limit.."<<endl;
	int valid = send(devicePointer,cmd);
	for(int i=0; i <cmd.size(); i++)
		cout<<(int)cmd[i]<<"   ";

}

void PTinterface::getPanTiltLimits()
{
	vector<unsigned char> cmd;
	vector<unsigned char> response;
	PushBeginning(cmd);
	cmd.push_back(2);
	cmd.push_back(76);//L
	cmd.push_back(63);//?	
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);
	
	cout<<"\ngetting limits..."<<endl;
	int valid = send(devicePointer,cmd,response);
	for(int i=0; i <response.size(); i++)
		cout<<(int)response[i]<<"   ";

}

void PTinterface::PushLimits(int value, vector<unsigned char>& chaine)
{	
	char *str =new char[20];
	int length = sprintf(str,"%X",value);
	
	int deb =0;
	if(value<0)
	{
		deb =5;	//for the negative values in hexadecimal wich begin with FFFF
		for(int i=length-5; i<3; i++)
			chaine.push_back(48);
	}
	else
		for(int i=length ; i<3; i++)
			chaine.push_back(48);
	

	for(int i=deb; i<length ; i++)
	{	
		int tmp = str[i];
		chaine.push_back(tmp);
	}
}

PTcoord PTinterface::getCurrentPosition()
{
	vector<unsigned char> cmd;
	vector<unsigned char> response;
	PTcoord pos(0,0);

	PushBeginning(cmd);
	cmd.push_back(2);
	cmd.push_back(80);//P
	cmd.push_back(63);//?	
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);
	
	cout<<"\ngetting current position..."<<endl;
	int valid = send(devicePointer,cmd,response);

	
	int t=0;
	//Pan position is in byte 6 to 9 of the response, as ASCII code of the hexadecimal value of the angle
	for(int i=6; i <10; i++)
	{
		if((int)response[i] <= 57)
			t+= ((int)response[i] - 48) * pow(16, 9 - i);
		else
			t+= ((int)response[i] - 55) * pow(16, 9 - i);
	}
	
	//A mean to check if the value is a negative one
	if((int)response[6] > 53)
		 t = t - 65536;

	//the angle are written as hundredth of a degree
	pos.pan = (float) t / 100.0;
	t = 0;

	//Tilt position is in byte 10 to 13 of the response
	for(int i=10; i <14; i++)
	{
		if((int)response[i] <= 57)
			t+= ((int)response[i] - 48) * pow(16, 13 - i);
		else
			t+= ((int)response[i] - 55) * pow(16, 13 -i);
	}

	//A mean to check if the value is a negative one
	if((int)response[10] > 53)
		 t = t - 65536;

	//the angle are written as hundredth of a degree
	pos.tilt = (float)t / 100.0;
	return pos;
}

void PTinterface::clearSoftwareLimits()
{

	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);
	cmd.push_back(76);//P
	cmd.push_back(67);//?	
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);
	
	cout<<"\nClearing limits..."<<endl;
	int valid = send(devicePointer,cmd);
}

void PTinterface::setProportionalPanSpeed(int p)
{

	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(3);
	cmd.push_back(83);//S
	cmd.push_back(80);//P
	if(p < 10)
		cmd.push_back(48+p);
	else
		cmd.push_back(55+p);	
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);
	 
	cout<<"\nSetting Pan speed at proportional value "<<p<<" ..."<<endl;
	int valid = send(devicePointer,cmd);
}

void PTinterface::setProportionalTiltSpeed(int p)
{
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(3);
	cmd.push_back(83);//S
	cmd.push_back(84);//T
	if(p < 10)
		cmd.push_back(48+p);
	else
		cmd.push_back(55+p);	
	cmd.push_back(CalcChecksum(cmd));
	cmd.push_back(0);
	 
	cout<<"\nSetting Tilt speed at proportional value "<<p<<" ..."<<endl;
	int valid = send(devicePointer,cmd);
}

void PTinterface::Stabilize(){
	PositionBeforeStabilization.pan = getCurrentPosition().pan;;
	PositionBeforeStabilization.tilt = getCurrentPosition().tilt;
	PTUDriftRate.StabilizationTime = time(0);
	
	vector<unsigned char> cmd;
	PushBeginning(cmd);
	cmd.push_back(2);// payload size
	cmd.push_back(72);//H
	cmd.push_back(73);//I
	cmd.push_back(CalcChecksum(cmd));//checksum
	cmd.push_back(0);

	send(devicePointer,cmd);
}

void PTinterface::PushD(double value, vector <unsigned char>& cmd){
    char buffer [19];  
    int length = sprintf(buffer,"%.15f",value);
    for(int i=0; i<length;i++){
        cmd.push_back(buffer[i]);
    }
}

void PTinterface::setDriftRate(){
	vector<unsigned char> cmd;
	vector<unsigned char> result;
	PushBeginning(cmd);
	cmd.push_back(39);//
	cmd.push_back(42);//*
	cmd.push_back(109);//m
	cmd.push_back(114);//r
	
	PushD(PTUDriftRate.panDriftRate, cmd);
	cmd.push_back(44);//,
	PushD(PTUDriftRate.tiltDriftRate, cmd);
	
	cmd.push_back(CalcChecksum(cmd));//checksum
	cmd.push_back(0);

	send(devicePointer,cmd);
}

bool PTinterface::initialCalibration(int calibrationTime){
	bool result = false;
	
	GotoHome();
	sleep(3);  //time for PTU to go to the home position. 
	double panAtTheBeginning =  getCurrentPosition().pan;
	double tiltAtTheBeginning = getCurrentPosition().tilt;

	Stabilize();
	sleep(calibrationTime);
	Halt();

	double  panAtTheEnd = getCurrentPosition().pan;
	double tiltAtTheEnd  = getCurrentPosition().tilt;
	
	//Below is a check to make sure that PTU didn't hit its rotation limits on Pan and Tilt axis while calibrating. 
	if(((panAtTheEnd > -39.00)&&(panAtTheEnd <39.00))&&((tiltAtTheEnd > -19.00)||(tiltAtTheEnd < 19.00))){
		PTUDriftRate.panDriftRate = getDriftRate(panAtTheBeginning,panAtTheEnd,calibrationTime);
		PTUDriftRate.tiltDriftRate = getDriftRate(tiltAtTheBeginning,tiltAtTheEnd,calibrationTime);
		result = true;
	}
	GotoHome();
	return result;
}

bool PTinterface::reCalibrate(){
	bool result = false;
	PTcoord currentPosition = getCurrentPosition();
	
	double currentPan = currentPosition.pan;
	double currentTilt = currentPosition.tilt;
	int currentTime = time(0);
	
	//Below is a check to make sure that PTU didn't hit its rotation limits on Pan and Tilt axis. 
	if(((currentPan > -39.00)&&(currentPan <39.00))&&((currentTilt > -19.00)||(currentTilt < 19.00))){
		int timeDifference = difftime(currentTime,PTUDriftRate.StabilizationTime);
		
		PTUDriftRate.panDriftRate += getDriftRate(PositionBeforeStabilization.pan,currentPan,timeDifference);
		PTUDriftRate.tiltDriftRate += getDriftRate(PositionBeforeStabilization.tilt,currentTilt,timeDifference);
		
		PositionBeforeStabilization.pan = currentPan;
		PositionBeforeStabilization.tilt = currentTilt;
		PTUDriftRate.StabilizationTime = currentTime;
		
		result = true;
	}
	return result;
}

bool PTinterface::reCalibrateWithInclinometer(double side_side, double fore_aft){
	bool result = false;
	double currentPan =  getCurrentPosition().pan;
	double currentTilt = getCurrentPosition().tilt;
	
	int currentTime = time(0);		
	int timeDifference = difftime(currentTime,PTUDriftRate.StabilizationTime);
	
	if(((currentPan > -39.00)&&(currentPan <39.00))&&((currentTilt > -19.00)||(currentTilt < 19.00))){
		int currentTime = time(0);		
		int timeDifference = difftime(currentTime,PTUDriftRate.StabilizationTime);

		PTUDriftRate.panDriftRate += getDriftRate(side_side,currentPan,timeDifference);
		PTUDriftRate.tiltDriftRate += getDriftRate(fore_aft,currentTilt,timeDifference);
		
		PTUDriftRate.StabilizationTime = currentTime;
		result = true;
	}
	return result;
}
void PTinterface::goToVerticalPosition(double side_side, double fore_aft){
	PTcoord goTo(side_side,fore_aft);
	Goto(goTo);
}

double PTinterface::getDriftRate(double beginningPosition, double endPosition, int time){
	double result = (endPosition-beginningPosition)/time; //drift rate in degrees per second
	result = result*PI/180; //drift rate converted in to the radians per second
	result = result *(-1); //change drift rate sign to the opposite one
	return result;
}