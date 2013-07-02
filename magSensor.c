/*
 ============================================================================
 Name        : magSensor.c
 Author      : Fabio GRANDE
 Version     : 0.1
 Copyright   : 
 Description : magSensor, a Nagios plugin
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/hiddev.h>
#include <fcntl.h>
#include <syslog.h>
#include "magSensor.h"


void show_version()
{
  printf("%s rel. %s\n", PRGNAME, RELEASE);
}


void show_usage()
{
  show_version();
  printf("Usage : %s [-h] [-V] [-v] [-t <C|F>] [-m <T|H>] [-l <U|D>] [-z <Sensor name>] -p <devicePath> -c <value> -w <value>\n", PRGNAME);
  printf("\n");
  printf("        -h : Show this Help.\n");
  printf("        -V : Show Version.\n");
  printf("        -v : Fully Verbose.\n");
  printf("        -t : Temperature units (C for Celsius, F for Fahreneit). Celsius is default\n");
  printf("        -m : Mode (T for Temperature, H for Humidity). Temperature is default\n");
  printf("        -l : Limit of thresholds (U for Upper, L for Lower). Lower is default\n");
  printf("        -c : Critical Threshold\n");
  printf("        -w : Warning Threshold\n");
  printf("        -z : Sensor Name (for use in returning messages)\n");
  printf("        -p : Device Path\n");

  exit(EXIT_UNKNOWN);
}


void WLog(char pMessage[])
{
  if (_verbose == 1)
	  printf("%s\n", pMessage);
}

void loadOptions(int argc, char **argv)
{
	int _opt;
	char *cvalue = NULL;
	char *endptr;

	while ((_opt = getopt(argc, argv, "hVvt:m:c:w:l:p:z:")) != -1)
	{
		switch (_opt)
		{
		case 'h':
			show_usage();
			break;
		case 'V':
			show_version();
			exit(EXIT_UNKNOWN);
			break;
		case 'v':
			_verbose = 1;
			break;
		case 't':
			cvalue = optarg;

			if (*cvalue == 'C' || *cvalue == 'c')
				_tmode = 'C';
			else if (*cvalue == 'F' || *cvalue=='f')
				_tmode = 'F';
			else
				show_usage();
			break;
		case 'm':
			cvalue = optarg;

			if (*cvalue == 'T' || *cvalue == 't')
				_mode = 'T';
			else if (*cvalue == 'H' || *cvalue=='h')
				_mode = 'H';
			else
				show_usage();
			break;
		case 'l':
			cvalue = optarg;

			if (*cvalue == 'U' || *cvalue == 'u')
				_boundary = 'U';
			else if (*cvalue == 'L' || *cvalue=='l')
				_boundary = 'L';
			else
				show_usage();
			break;
		case 'c':
			_critical = strtod(optarg, &endptr);
			break;
		case 'w':
			_warning = strtod(optarg, &endptr);
			break;
		case 'p':
			strcpy(_devicePath, optarg);
			break;
		case 'z':
			strcpy(_zoneName, optarg);
			break;
		case '?':
			show_usage();
			break;
		default:
			show_usage();
			break;
		}
	}

	if (strlen(_devicePath) == 0)
		show_usage();

	if (_critical < _warning && _mode == 'L')
		show_usage();
	if (_warning < _critical && _mode == 'U')
		show_usage();
}

int GetData(char address[], thdata *data) {
	double hl = 0;
	double hh = 0;
	double tl = 0;
	double th = 0;

	int _status = EXIT_OK;

	//double _t, _h;
	int i, j;
	int fd;
	int ret;
	struct hiddev_report_info rinfo;
	struct hiddev_field_info finfo;
	struct hiddev_usage_ref uref;

	data->t = 0;
	data->h = 0;

	// Try to open device
	sprintf(_logStr, "Opening device at %s", address);
	WLog(_logStr);

	if ((fd = open(address, O_RDONLY)) == -1) {
		_status = EXIT_UNKNOWN;
		syslog(LOG_INFO, "%s - Error opening device...", PRGNAME);
		WLog("Error opening device.");
		strcpy(_message, "Error opening device");
	} else {
		rinfo.report_type = HID_REPORT_TYPE_INPUT;
		rinfo.report_id = HID_REPORT_ID_FIRST;
		ret = ioctl(fd, HIDIOCGREPORTINFO, &rinfo);
		if (ret == -1) {
			syslog(LOG_INFO, "%s - Error retrieving infos...", PRGNAME);
			strcpy(_message, "Error retrieving infos");
			WLog("Error retrieving infos.");
			_status = EXIT_UNKNOWN;
		} else {
			while (ret >= 0) {
				syslog(LOG_INFO, "report info: type=%u id=%u num_fields=%u",
						rinfo.report_type, rinfo.report_id, rinfo.num_fields);

				for (i = 0; i < rinfo.num_fields; i++) {
					finfo.report_id = rinfo.report_id;
					finfo.report_type = rinfo.report_type;
					finfo.field_index = i;
					ioctl(fd, HIDIOCGFIELDINFO, &finfo);

					for (j = 0; j < finfo.maxusage; j++) {
						uref.report_type = finfo.report_type;
						uref.report_id = finfo.report_id;
						uref.field_index = i;
						uref.usage_index = j;
						ioctl(fd, HIDIOCGUCODE, &uref);
						ioctl(fd, HIDIOCGUSAGE, &uref);

						switch (j) {
						case 0:
							th = uref.value;
							break;
						case 1:
							tl = uref.value;
							break;
						case 3:
							hh = uref.value;
							break;
						case 4:
							hl = uref.value;
							break;
						}

					}

				}

				rinfo.report_id |= HID_REPORT_ID_NEXT;
				ret = ioctl(fd, HIDIOCGREPORTINFO, &rinfo);
			}

			data->t = (th * 256 + tl) / 100.0;
			data->h = (hh * 256 + hl) / 100.0;

			sprintf(_logStr, "Got Data : %f (Temperature), %f (Humidity)", data->t, data->h);
			WLog(_logStr);
		}
	}

	return _status;
}

int main(int argc, char **argv)
{
	double _value;

    loadOptions(argc, argv);

    _status = GetData(_devicePath, &_testdata);

    if (_status == EXIT_OK)
    {
      if (_tmode == 'F')
      {
    	  _testdata.t = (_testdata.t * 9.00 / 5.00) + 32.00;
      }

      _value = _testdata.t;
      if (_mode == 'H')
    	  _value = _testdata.h;

      char _measure[32];
      char _unit[4];
      if (_mode == 'T')
      {
    	strcpy(_measure, "Temperature");
    	sprintf(_unit, "°%c", _tmode);
      }
      if (_mode == 'H')
      {
        strcpy(_measure, "Humidity");
      	strcpy(_unit, "%");
      }

      sprintf(_message, "%s (%3.2f %s) is in safe zone (%3.2f%s / %3.2f%s) ", _measure, _value, _unit, _warning, _unit, _critical, _unit);

      switch (_boundary)
      {
        case 'U':
          if (_value < _critical)
          {
            sprintf(_message, "%s (%3.2f %s) is less than %3.2f %s", _measure, _value, _unit, _critical, _unit);
            _status = EXIT_CRITICAL;
          }
          else if (_value < _warning)
          {
            sprintf(_message, "%s (%3.2f %s) is less than %3.2f %s", _measure, _value, _unit, _warning, _unit);
            _status = EXIT_WARNING;
  		  }
  	      break;
        case 'L':
          if (_value > _critical)
          {
            sprintf(_message, "%s (%3.2f %s) is over %3.2f %s", _measure, _value, _unit, _critical, _unit);
            _status = EXIT_CRITICAL;
          }
          else if (_value > _warning)
          {
            sprintf(_message, "%s (%3.2f) is over %3.2f %s", _measure, _value, _warning, _unit);
            _status = EXIT_WARNING;
		  }
  	      break;
      }
    }

    WLog("");
    if (strlen(_zoneName) > 0)
    	printf("[%s] - ", _zoneName);
    printf("%s|%3.2f", _message, _value);
	return _status;
}



