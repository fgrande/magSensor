/*
 * magSensor.h
 *
 *  Created on: May 25, 2012
 *      Author: fgrande
 */

#ifndef MAGSENSOR_H_
#define MAGSENSOR_H_

#define PRGNAME "magSensor"
#define RELEASE "0.1"

#define EXIT_OK 0
#define EXIT_WARNING 1
#define EXIT_CRITICAL 2
#define EXIT_UNKNOWN 3

typedef struct
{
  double t;
  double h;
} thdata;

int _verbose = 0;
int _tmode = 'C';
int _mode = 'T';
double _critical = 0;
double _warning = 0;
//double _hc = 0;
//double _hw = 0;
//double _tc = 0;
//double _tw = 0;
int _boundary = 'L';
char _devicePath[64] = "";
char _zoneName[64] = "";
thdata _testdata;

int _status = 0;
char _message[255] = "";

char _logStr[255] = "";

#endif /* MAGSENSOR_H_ */
