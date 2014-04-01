#include "PanNTilt.h"
#include "PTfunctions.h"
#include "uart.h"

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
			}
	}

return 0;
}