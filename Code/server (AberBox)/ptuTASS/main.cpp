#include "PanNTilt.h"
#include "PTfunctions.h"
#include "uart.h"
#include <math.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
using namespace std;

int main(){
int choice;
cout<< "Hello world, starting app" << endl;
//PTinterface p(9600, "/dev/ttyAMA0");
PTinterface p(9600, "/dev/ttyUSB0");
p.VerifyHome();
p.GotoHome();

while(1)
	{
		cout<< "Choose command" << endl;
		cout<< "1 - Stabilize" << endl;
		cout<< "2 - Get Drift" << endl;
		cout<< "3 - Stop"<<endl;
		cout<< "4 - Go Home"<<endl;
		cout<< "5 - Pan Left"<<endl;
		cout<< "6 - Pan Right"<<endl;
		cout<< "7 - Get Current position"<<endl;
		cout<< "8 - Exit"<<endl;
		cout<< "9 - Tilt Up"<<endl;
		cout<< "10 - Tilt Down"<<endl;
		cout<< "11 - Get Limits"<<endl;
		cout<< "12 - Set Inertial gain rate"<<endl;
		cout<< "13 - Go to"<<endl;
		cout<< "14 - Calculate drift"<<endl;
		cin >> choice;
		switch(choice)
		{
			case 1:
				p.Stabilize();
				break;
			case 2:
				p.getDriftRate();
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
			case 12:
				cout<< "Enter drift rate: ";
				double panD, tiltD;
				cin>>panD;
				cin>>tiltD;
				p.setInertialRate(panD,tiltD);
				break;
			case 13:
				/*double p1,p2;
				cout<< "Enter position"<<endl;
				cin>>p1;
				cin>>p2;
				PTcoord GotoCoord(p1,p2);
				p.Goto(GotoCoord);*/
				break;
			case 14:
				p.GotoHome();
				sleep(3);
				double pan =  p.getCurrentPosition().pan;
				double tilt = p.getCurrentPosition().tilt;
				cout<< "Pan is: "<<  pan << endl;
				cout<< "Tilt is: "<< tilt << endl;
				p.Stabilize();
				sleep(15);
				double  pan2 = p.getCurrentPosition().pan;
				double tilt2 = p.getCurrentPosition().tilt;
				cout<< "Pan is: "<<  pan2 << endl;
				cout<< "Tilt is: "<< tilt2 << endl;
				double panDiff = (pan2-pan)/15*(-1)*PI/180;
				double tiltDiff = (tilt2-tilt)/15*(-1)*PI/180;
				cout<< "The difference is: "<< panDiff<<", "<<tiltDiff<<endl;
				p.GotoHome();
				sleep(3);
				p.Stabilize();
				p.setInertialRate(panDiff,tiltDiff);
				break;
		}
	}
return 0;
}