#include <stdio.h>
#include <unistd.h>

#include <pigpio.h>

/*
servos_demo.c
2016-10-06
Public Domain

gcc -Wall -pthread -o servos servos_demo.c -lpigpio

sudo ./servos
*/

/*
         0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
Rev.1    X  X  -  -  X  -  -  X  X  X  X  X  -  -  X  X
Rev.2    -  -  X  X  X  -  -  X  X  X  X  X  -  -  X  X
Rev.3          X  X  X  X  X  X  X  X  X  X  X  X  X  X

        16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
Rev.1    -  X  X  -  -  X  X  X  X  X  -  -  -  -  -  -
Rev.2    -  X  X  -  -  -  X  X  X  X  -  X  X  X  X  X
Rev.3    X  X  X  X  X  X  X  X  X  X  X  X  -  -  -  -

 B Rev.1: 0-1, 4, 7-11, 14-15, 17-18, 21-25
 B Rev.2: 2-4, 7-11, 14-15, 17-18, 22-25, 27-31
B+ Rev.3: 2-27
*/

typedef struct
{
   int gpio;
   int minPulse;
   int maxPulse;
   int pw;
   int pwInc;
   int connected;
} servoInf_t;


servoInf_t servoInf[]=
{
   { 0, 1000, 2000, 1500,   2, 0}, /* change 0 to 1 to enable servo */
   { 1, 1000, 2000, 1500,  -2, 0},
   { 2, 1000, 2000, 1500,   3, 0},
   { 3, 1000, 2000, 1500,  -3, 0},
   { 4, 1000, 2000, 1500,   5, 0},
   { 5, 1000, 2000, 1500,  -5, 0},
   { 6, 1000, 2000, 1500,   7, 0},
   { 7, 1000, 2000, 1500,  -7, 0},

   { 8, 1000, 2000, 1500,  11, 0},
   { 9, 1000, 2000, 1500, -11, 0},
   {10, 1000, 2000, 1500,  13, 0},
   {11, 1000, 2000, 1500, -13, 0},
   {12, 1000, 2000, 1500,  17, 0},
   {13, 1000, 2000, 1500, -17, 0},
   {14, 1000, 2000, 1500,  19, 0},
   {15, 1000, 2000, 1500, -19, 0},

   {16, 1000, 2000, 1500,  23, 0},
   {17, 1000, 2000, 1500, -23, 0},
   {18, 1000, 2000, 1500,  29, 1},
   {19, 1000, 2000, 1500, -29, 0},
   {20, 1000, 2000, 1500,  31, 0},
   {21, 1000, 2000, 1500, -31, 0},
   {22, 1000, 2000, 1500,  37, 0},
   {23, 1000, 2000, 1500, -37, 0},

   {24, 1000, 2000, 1500,  41, 0},
   {25, 1000, 2000, 1500, -41, 0},
   {26, 1000, 2000, 1500,  43, 0},
   {27, 1000, 2000, 1500, -43, 0},
   {28, 1000, 2000, 1500,  47, 0},
   {29, 1000, 2000, 1500, -47, 0},
   {30, 1000, 2000, 1500,  53, 0},
   {31, 1000, 2000, 1500, -53, 0},
};

/* forward prototype */

void servoTick(void * userdata);

int main(int argc, char *argv[])
{
   if (gpioInitialise()<0) return 1;

   /* use timer #0 to call servoTick every 100 milliseconds
      servoTick calculates the new positions */

   gpioSetTimerFuncEx(0, 100, servoTick, 0);

   while (1) sleep(1);
}

void servoTick(void * userdata)
{
    int s=18;

    if (servoInf[s].connected) /* are we using this servo? */
    {
        /* move servo by its increment */
        servoInf[s].pw += servoInf[s].pwInc;

        /* bounce back from ends */
        if ((servoInf[s].pw < servoInf[s].minPulse) ||
            (servoInf[s].pw > servoInf[s].maxPulse))
        {
        servoInf[s].pwInc = - servoInf[s].pwInc;

        servoInf[s].pw += (2 * servoInf[s].pwInc);
        }

        /* position servo */
        gpioServo(servoInf[s].gpio, servoInf[s].pw);
    }

}