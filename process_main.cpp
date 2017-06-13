#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <vector>
using namespace std;
int id ;
std::vector<int> history;
int counter ;
bool block ;
int totaltime ;
int totaltimeold ;
int arrivaltime ;
int priority ;
void handler2 (int signum )
{
    
  block = false ;
	counter++;
	cout <<"My id is "<<id<<" and I got counter = "<<counter<<endl;
}

void handler3(int signum)
{
  totaltime -- ;
  block = false ;
  history.push_back(counter);
  cout <<"My id is "<<id<<" and I am running at time = "<<counter<<endl ;
  counter++;
//block = false ;


}




void printmyhistory()
{
	 char x [10];
ofstream outfile;
 sprintf( x, " %d", id);

	std::vector<int> historyStart,historyEnd;
	std::vector<string> historyNameFinal;

string cpu = "cpu";
cout <<"history total size = " << history.size()<<endl;

 int i=0,j=1;
  historyStart.push_back(history[i]);
  historyNameFinal.push_back(cpu);
  if(history.size()==1)
  {
    historyEnd.push_back(history[i]);
    return;
  }
  while(i<history.size())
  {
    
    if(history[j]-history[j-1]>1)
    {
      historyEnd.push_back(history[j-1]);
      i=j;
      historyStart.push_back(history[i]);
      historyNameFinal.push_back(cpu);
    }
    j++;
    if(j==history.size())
      break;
  }
  if(historyStart.size()!=historyEnd.size())
    historyEnd.push_back(history[history.size()-1]);




string name  = "output/process"+std::string(x)+".txt";
cout<<"----------------------------------------><------------><--------------------><------------------------------------><"<<endl ;
outfile.open(name.c_str(), ios::out | ios::trunc );
cout<<"----------------------------------------><------------><--------------------><------------------------------------><"<<endl ;
cout <<"history total size = " << historyEnd.size()<<endl;
for (int i =0 ; i < historyEnd.size() ; i++ )
{
//----------------------------Changed here, was Second,Second,First...made it First,First,Second 
outfile << "["<< historyStart[i] <<":" << historyEnd[i] <<  "]" << " " << historyNameFinal[i] <<endl; 
cout<<"----------------------------in the for loop------------><-------in the for loop-----><--------------------><------------------------------------><"<<endl ;
}
//-----------Changed here, History is now int then string--------<TIME1,ID1>
//---------------------------------------------------------------<TIME2,ID1>
//---------------------------------------------------------------<TIME1,ID2>
//---------------------------------------------------------------<TIME2,ID2> and so on
/*for (int i =0 ; i < history.size() ; i++ )
{
//----------------------------Changed here, was Second,Second,First...made it First,First,Second 
outfile << history[i]  <<endl; 
cout<<"----------------------------in the for loop------------><-------in the for loop-----><--------------------><------------------------------------><"<<endl ;
}*/
int turnaround = historyEnd[historyEnd.size()-1] - arrivaltime + 1;
outfile << " turnaround time =  " << turnaround << " " <<endl ;
float weightedturnaround = (float)((float)turnaround)/((float)totaltimeold);

outfile << " weighted turnaround time =  " << weightedturnaround << " " <<endl ;
outfile << " priority =  " << priority << " " <<endl ;
outfile << " totaltime =  " << totaltimeold << " " <<endl ;
outfile.close();
cout<<"_____________________________!______________________________!_!_!_!_!__!_!_!_!_!_!______________________"<<endl;
cout<<"i finished printing "<<endl ;

}




int main(int argc,char * *argv )
{ 
  
	block = true ;
	counter =atoi(argv[1]);
	id= atoi(argv[2]);
    totaltime = atoi(argv[3]);
    priority = atoi(argv[4]);
    arrivaltime = atoi(argv[5]);
    totaltimeold = totaltime ;
  

  signal (SIGUSR1,handler2);
  signal (SIGUSR2,handler3);
 int x=20;
  //history.push_back(counter);
 cout << "I am a new process, my ID = " <<id<<endl ;
 while (totaltime)
  { 
  	//history.push_back(counter);
    block = true ;
cout <<"I am process = "<<id <<" and my remaining time = "<<totaltime<<" at counter = "<<counter<<endl ;

    if (totaltime==0)
    	break ;
      while(block && totaltime!=0);
   
  }
  //history.push_back(counter);
  cout<<"I am process "<<id<<" and I am dead"<<endl;
  printmyhistory();
  exit(id);
 
}
