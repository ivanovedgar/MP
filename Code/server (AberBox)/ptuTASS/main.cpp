#include "PanNTilt.h"
#include "PTfunctions.h"
#include "uart.h"
#include <math.h>


using namespace std;

int main(){
int choice;
//PTinterface p(9600, "/dev/ttyAMA0");
PTinterface p(9600, "/dev/ttyUSB0");
p.VerifyHome();
p.GotoHome();

while(1)
	{
		cout<< endl<< "Choose command" << endl;
		cout<< "1 - Stabilize" << endl;
		cout<< "3 - Stop"<<endl;
		cout<< "4 - Go Home"<<endl;
		cout<< "5 - Pan Left"<<endl;
		cout<< "6 - Pan Right"<<endl;
		cout<< "7 - Get Current position"<<endl;
		cout<< "8 - Exit"<<endl;
		cout<< "9 - Tilt Up"<<endl;
		cout<< "10 - Tilt Down"<<endl;
		cout<< "13 - Initial Calibration"<<endl;
		cout<< "14 - ReCalibrate"<<endl;
		cout<< "15 - Get pan tilt limits"<<endl;
		cin >> choice;
		switch(choice)
		{
			case 1:
				p.Stabilize();
				p.setDriftRate();
				break;
			case 3:
				p.PanStop();
				break;
			case 4:
				p.GotoHome();
				break;
			case 5:
				p.PanLeft();
				break;
			case 6:
				p.PanRight();
				break;
			case 7:
				cout<< "Pan is: "<< p.getCurrentPosition().pan << endl;
				cout<< "Tilt is: "<< p.getCurrentPosition().tilt << endl;
				break;
			case 8:
				return 0;
				break;
			case 9:
				p.TiltUp();
				break;
			case 10:
				p.TiltDown();
				break;
			case 11:
				p.getPanTiltLimits();
				break;
			case 13:
				int time;
				cout<<"Enter calibration time in seconds:"<<endl;
				cin >> time;
				bool re;
				re = p.initialCalibration(time);
				cout<<endl<<"The result is: "<<re<<endl;
				break;
			case 14:
				p.recalibrate();
				p.setDriftRate();
				break;
			case 15:
				p.getPanTiltLimits();
				break;
		}
	}
return 0;
}