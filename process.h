#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "buffers.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <vector>
#include <stdlib.h>
using namespace std;
#ifndef HEADER1
#define HEADER1
enum state
{
running,ready,blocked
};
//walahy da geded .............

//20-4

 class process
{
 public:
  static int count ;
  int id ;
  map<int,string> commands ;
  int totaltime;
int memory ;
int usedtime;
int arrivaltime ;
pid_t processid ;
int priority ;
int waitingtime;
process();
//process(const process &p);
process& operator=(process p);
int totalcpu;
int totalio;

int isSaving;
bool isNew ;
int remainingtime ;

int ip ;
std::vector<int> historyStart;
std::vector<int> historyEnd;
std::vector<int> history;
std::vector<string> historyName;
std::vector<string> historyNameFinal;
state status ;
void printprocess();
void addtohistoryStart(int IP, int TIME, string cpu);
void addtohistoryEnd(int IP, int TIME, string cpu);
void addtohistory(int IP, int TIME, string cpu);
void filterhistory();
void inc() ;
void changetoprocess(messagebuff message) ;
void testhistory();
void changeittobesend(int &arrivaltime , int &totaltime ,int & size  , int * cpu ,int * IO ,int * CD ,int * printer,int & id ,int & priority );
};
 int process::count = 0 ;
void process::inc()
{
ip++ ;
remainingtime-- ;
usedtime++ ;
} 

/*process::process(const process &p)
{
  int id = p.id ;
  map<int,string> commands  = p.commands;
  int totaltime = p.totaltime;
  int memory = p.memory;
  int usedtime = p.usedtime;
  int arrivaltime = p.arrivaltime;
  pid_t processid = p.processid;
  int priority = p.priority;
  int waitingtime = p.waitingtime;
  int totalcpu = p.totalcpu;
  int totalio = p.totalio;

  int isSaving = p.isSaving;
  bool isNew = p.isNew;
  int remainingtime = p.remainingtime;

  int ip = p.ip;
  std::vector<int> historyStart = p.historyStart;
  std::vector<int> historyEnd = p.historyEnd;
  std::vector<int> history = p.history;
  std::vector<string> historyName = p.historyName;
  std::vector<string> historyNameFinal = p.historyNameFinal;
}*/

process& process::operator=(process p)
{
  this->id = p.id ;
  this->commands  = p.commands;
  this->totaltime = p.totaltime;
  this->memory = p.memory;
  this->usedtime = p.usedtime;
  this->arrivaltime = p.arrivaltime;
  this->processid = p.processid;
  this->priority = p.priority;
  this->waitingtime = p.waitingtime;
  this->totalcpu = p.totalcpu;
  this->totalio = p.totalio;

  this->isSaving = p.isSaving;
  this->isNew = p.isNew;
  this->remainingtime = p.remainingtime;

  this->ip = p.ip;
  this->historyStart = p.historyStart;
  this->historyEnd = p.historyEnd;
  this->history = p.history;
  this->historyName = p.historyName;
  this->historyNameFinal = p.historyNameFinal;
}

 process::process()
{
 id = ++count ;
ip=1;
totalcpu = 0 ;
totalio = 0 ;
usedtime =0 ;
isSaving = 0;
isNew =true ;
}

void process::addtohistory(int IP,int TIME, string cpu)
{
  string c = "cpu";
  
cout <<"error is here"<<endl ;
if(IP !=1 )
  if(history.size()!=0 && (commands[IP-1]) == c   && historyName[historyName.size() - 1]!= cpu)
  {
    cout <<"error is not here"<<endl ;
    history.push_back(history[history.size()-1]);
    cout <<"error is not here4"<<endl ;
      historyName.push_back(historyName[historyName.size() - 1]);
  }
  
cout <<"error is not here2"<<endl ;
  history.push_back(TIME);
  if((commands[IP]) == c)
      historyName.push_back(cpu);
  else
      historyName.push_back(commands[IP]);

  if((commands[IP+1] != commands[IP]))
  {

    history.push_back(TIME);
    if((commands[IP]) == c)
      historyName.push_back(cpu);
    else
      historyName.push_back(commands[IP]);
  }
  

}

void process::addtohistoryStart(int IP ,int TIME, string cpu)
{
    //-----Changed here, sent it the cpu name in case it was in CPU, check the dispatcher fn called killprocess-----------
    string c = "cpu";
    historyStart.push_back(TIME);
    if((commands[IP]) == c)
        historyName.push_back(cpu);
    else
        historyName.push_back(commands[IP]);
}

void process::addtohistoryEnd(int IP ,int TIME, string cpu)
{
    //-----Changed here, sent it the cpu name in case it was in CPU, check the dispatcher fn called killprocess-----------
    string c = "cpu";
    historyEnd.push_back(TIME);
    if((commands[IP]) == c)
        historyName.push_back(cpu);
    else
        historyName.push_back(commands[IP]);
}

void process::filterhistory()
{
  historyStart.push_back(history[0]);
  for (int i = 1; i < history.size(); ++i)
  {

    if(history[i+1]-history[i]>1 || historyName[i+1]!=historyName[i])
    {
      if((i+1)!=history.size())
        historyStart.push_back(history[i+1]);
      historyEnd.push_back(history[i]);
      historyNameFinal.push_back(historyName[i]);
    }

  }
}

void process::testhistory()
{
  char x [10];
  ofstream outfile;
  sprintf( x, " %d", this->id);
  string name  = "test"+std::string(x)+".txt";
  outfile.open(name.c_str(), ios::out | ios::trunc );
  cout<<"My full history size is "<<history.size()<<endl;
  for (int i =0 ; i < history.size() ; i++ ) 
    outfile <<history[i] <<" "<<historyName[i]<<endl;
  outfile.close();
}

void process::printprocess()
{
    char x [10];
ofstream outfile;
 sprintf( x, " %d", this->id);
string name  = "process"+std::string(x)+".txt";
outfile.open(name.c_str(), ios::out | ios::trunc );
cout<<"My history size is "<<historyStart.size()<<endl;
for (int i =0 ; i < historyEnd.size() ; i++ )
{
//----------------------------Changed here, was Second,Second,First...made it First,First,Second 
outfile << "["<< historyStart[i] <<":" << historyEnd[i] <<  "]" << " " << historyNameFinal[i] <<endl; 

}
//-----------Changed here, History is now int then string--------<TIME1,ID1>
//---------------------------------------------------------------<TIME2,ID1>
//---------------------------------------------------------------<TIME1,ID2>
//---------------------------------------------------------------<TIME2,ID2> and so on
int turnaround = historyEnd[historyEnd.size()-1] - this->arrivaltime + 1;
outfile << " turnaround time =  " << turnaround << " " <<endl ;
float weightedturnaround = (float)((float)turnaround)/((float)totaltime);

outfile << " weighted turnaround time =  " << weightedturnaround << " " <<endl ;
outfile << " waiting time =  " << waitingtime << " " <<endl ;
outfile << " priority =  " << priority << " " <<endl ;
outfile << " totaltime =  " << totaltime << " " <<endl ;
outfile.close();
}

/////---->fe t3'er hennaaa
void process::changeittobesend(int &arrivaltime , int &totaltime ,int & size  , int * cpu ,int * IO ,int * CD ,int * printer,int & id ,int & priority )
{
int j=0 ;
int k =0 ;
int t = 0;
int f = 0 ;
arrivaltime = this->arrivaltime ;
size = this->memory ;
totaltime= this->totaltime ;
id =this->id;
priority = this->priority;
string c ="cpu";
string cd ="CD";
string p = "printer";
string io ="IO";
for (int i = 0; i < 100; ++i)
{
  cpu[i] = -1;
  IO[i] = -1;
  CD[i] = -1;
  printer[i] = -1;
}
for (int i =1 ;i<=totaltime ;i++)
{
 if( commands[i]==c)
 {
    cpu[j] = i ;
    j++ ;

 }
 else if (commands[i]==p)
 {
    printer[t]= i;
    t++ ;
 }
 else if (commands[i]==cd)
 {
    CD[f]= i;
    f++ ;
 }
  else if(commands[i]==io)
 {
    IO[k]= i;
    k++ ;
 }
}
cout<<"Done changing"<<endl;
}
///////de gededaaaaaa---->
void process::changetoprocess(messagebuff message)
{

this->arrivaltime = message.arrivaltime ;
this->memory = message.size ;
this->id = message.id ;
cout <<"my id is "<<this->id<<endl;
this->priority = message.priority ;
this->totaltime = message.totaltime;
for(int i=1 ; i<= totaltime;i++)
{
for(int j = 0; j<totaltime; j++){
     if(message.cpu[j] == i ){
        this->commands[i]="cpu";
         break ;
     }
     else if (message.IO[j]==i)
     {
        this->commands[i] ="IO" ;
        break ;
             }
         else if (message.CD[j]==i)
     {
        this->commands[i] ="CD";   
            break ;
          }
            else if(message.printer[j]==i)
            {

                this->commands[i] ="printer";
                break ;
            }

  

}


}

}
#endif