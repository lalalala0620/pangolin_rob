#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <wiringPi.h>
#include <softServo.h>
#include <map>
#include <unistd.h>
using namespace std;



// PWMServo_IO_dict = {1:4, 2:18, 3:27, 4:10, 5:20, 6:19, 7:13, 8:6, 9:11, 10:5}
// bool moveCycle(vector<double>& now_angles, vector<double>& servo_angles, )
// {
// 	for(int i=; i< 1500; i+=50){
// 		softServoWrite (pwm_io[2],  i);
// 		softServoWrite (pwm_io[4],  i);
// 		softServoWrite (pwm_io[6],  i);
// 		softServoWrite (pwm_io[8],  i);
// 		usleep(10000);
		
// 	}
// }


int main ()
{
	cout << "1" << endl;
        map<int, int> pwm_io = {{1, 7},
                                            {2, 1},
                                            {3, 2},
                                            {4, 12},
                                            {5, 28},
                                            {6, 24},
                                            {7, 23},
                                            {8, 22},
                                            {9, 14},
                                            {10, 21}};
	if (wiringPiSetup () == -1)
	{
	fprintf (stdout, "oops: %s\n", strerror (errno)) ;
	return 1 ;
	}
	cout << "1" << endl;

	// softServoSetup (pwm_io[1], pwm_io[2], pwm_io[3], pwm_io[4], pwm_io[5], pwm_io[6], pwm_io[7], pwm_io[8]);
	softServoSetup (14, 21, 19, 20, 21, 22, 23, 24) ;

	// while (true){
	// 	cout << "1" << endl;
	// 	softServoWrite (1,  0);
	// 	delay(1000);
	// 	softServoWrite (1,  1100);
	// 	delay(1000);
	// }
	for(int i=0; i<1000; i+=50){
		softServoWrite (pwm_io[9],  i);
		softServoWrite (pwm_io[10],  i);
		softServoWrite (pwm_io[6],  i);
		softServoWrite (pwm_io[8],  i);
		delay(100);
		cout << "1" << endl;
		
	}
	for(int i=1000; i>0; i-=50){
		softServoWrite (pwm_io[9],  i);
		softServoWrite (pwm_io[10],  i);
		softServoWrite (pwm_io[6],  i);
		softServoWrite (pwm_io[8],  i);
		delay(100);
		cout << "1" << endl;
	}

	// softServoWrite (13,  2000);
/*
	softServoWrite (1, 1000) ;
	softServoWrite (2, 1100) ;
	softServoWrite (3, 1200) ;
	softServoWrite (4, 1300) ;
	softServoWrite (5, 1400) ;
	softServoWrite (6, 1500) ;
	softServoWrite (7, 2200) ;
*/

	cout << "1" << endl;
	// for (;;)
	// delay (10) ;

}