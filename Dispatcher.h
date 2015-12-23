/********************************************************

 A Compact Task Dispatcher for Embedded Systems
  Ron Kreymborg Dispatcher Implemented By Hesham Eraqi

 hesham.eraqi@gmail.com
 November 2015
 
********************************************************/

#ifndef DISPATCHER_H
#define DISPATCHER_H

/********************************************************
 Configurations
********************************************************/
#define TOTALTASKS   5    // The maximum number of tasks (max value is 255 because all id's are chosen to be char variables)

/********************************************************
 API's Declarations
********************************************************/
void InitMulti();

void DecrementDelay(void);

void QueDelay(void (*pntr)(), int delay);
void QueTask(void (*pntr)());
void ReRunMe(int delay);

void Dispatch();


#endif //#ifndef DISPATCHER_H
