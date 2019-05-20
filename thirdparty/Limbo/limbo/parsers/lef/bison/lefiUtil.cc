
#include <limbo/parsers/lef/bison/lefiUtil.hpp>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

namespace LefParser {
	
#if 0
static char tagName[200] = "CDNDESYS";
static char tagNum[24]  = "CDNCHKSM";
#endif 

time_t lefiCalcTime() {
    // Calculate the number for the given date
    // The date is 5/1/99

    /* Used to calculate the UTC for a time bomb date in libcai.a
    ** see caiInitWork() function
    */
    struct tm	ts;

    ts.tm_sec = 0;
    ts.tm_min = 0;
    ts.tm_hour = 0;
    ts.tm_mday = 1;
    ts.tm_mon = 5;
    ts.tm_year = 1999 - 1900;
    ts.tm_wday = 0;
    ts.tm_yday = 0;
    ts.tm_isdst = 0;

/*
    printf("May 1, 1999 in UTC is %d\n", mktime(&ts));
    ts.tm_mday = 2;
    printf("May 2, 1999 in UTC is %d\n", mktime(&ts));

    printf("Right now is %d\n", time(0));
*/
    return(mktime(&ts));
}

////////////////////////////////////////////////////
//
//    lefiTimeBomb
//
////////////////////////////////////////////////////

// Check the current date against the date given
int lefiValidTime() {
/*  Take the timebomb out for now
    time_t	bombTime = lefiCalcTime();
    time_t	bombTime = 928224000;
    time_t	curTime;

    curTime = time((time_t *)NULL);
    if (curTime == -1 || curTime > bombTime)
    {
	(void)printf("The demonstration version of this code is no longer\n"
		     "available.  Please contact your Lef/Def Parser\n"
		     "software provider for up to date code.\n");
	return(0);
    }
*/
    return (1);
}

////////////////////////////////////////////////////
//
//    Check if the company is authorized to use the reader
//
////////////////////////////////////////////////////

int lefiValidUser() {
#if 0
    int j = 0, i;
    
    for (i = 0; i < (int)strlen(tagName); i++) {
        j += (int)tagName[i];
    }

    if (atoi(tagNum) == j)
        return(1);
    return (0);
#endif 
	return 1;
}

////////////////////////////////////////////////////
//
//    Return user name from tagName
//
////////////////////////////////////////////////////

#if 0
char *lefiUser() {
    char * tmpUser = tagName;
    tmpUser = tmpUser + 8;
    if (strncmp(tmpUser, "     ",5) == 0)
        return ((char*)"Cadence Design Systems");
    return (tmpUser);
}
#endif 

////////////////////////////////////////////////////
//
//    Convert the orient from integer to string
//
////////////////////////////////////////////////////
char* lefiOrientStr(int orient) {
  switch (orient) {
      case 0: return ((char*)"N");
      case 1: return ((char*)"W");
      case 2: return ((char*)"S");
      case 3: return ((char*)"E");
      case 4: return ((char*)"FN");
      case 5: return ((char*)"FW");
      case 6: return ((char*)"FS");
      case 7: return ((char*)"FE");
  };
  return ((char*)"");
}


} // namespace LefParser
