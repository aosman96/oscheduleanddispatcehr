#ifndef soha
#define soha
struct config
{
long mtype;
int algonum ;
int penality ;
int quantum ;
int rateofdata ;
};
//
struct messagebuff
{
 long mtype ;
 int numberofprocess ;
 int IO[100];
 int cpu[100];
 int CD[100];
 int printer[100];
 int totaltime ;
 int arrivaltime ;
 int size;
 int id ; 
 int priority ;   
};

struct messagecounter
{
  long mtype ;
  int counter ;
};
#endif