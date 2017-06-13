#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <vector>
#include <stdlib.h>
#include "process.h"
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "sharedmemory.h"
#include "dispatcher.h"
#include "buffers.h"
using namespace std;
#ifndef HEADER4
#define HEADER4
class sched 
{
	public:
	   	int n ;
	    vector<process> readyqueue ;
      int mode;
      bool putinreadyqueue(process B,dispatcher& D);
	    vector<process>::iterator ri;
    	sched();
      int Quantum;
      int CSperiod;
      int rateofData;
      void checkreadyforhistory(int counter,dispatcher& D);
      std::vector<process> computeSRT(dispatcher& D) ;
      std::vector<process> computeHPF(dispatcher& D) ;
      std::vector<process> computeRR(dispatcher& D) ;
      std::vector<process> computeHPRR(dispatcher& D) ;
      std::vector<process> checkrunningtoblock(dispatcher& D);
      std::vector<process> checkblockedtounblock(dispatcher &D);
      std::vector<process> checkQuantums(int counter,dispatcher& D);
      std::vector<process> checkCustomQuantums(int counter, dispatcher& D);
};



std::vector<process> sched::checkCustomQuantums(int counter, dispatcher& D)
{
  int readysize = readyqueue.size();
  std::vector<process> returned;
  if (((counter - D.cpus[1].currentQuantumStart)/Quantum) == 1 && D.cpus[1].runningprocess!=NULL && D.cpus[1].inCS==false)
  {
    cout<<"I will remove the RR process in HPRR"<<endl;
    if(readysize==0)
        D.cpus[1].currentQuantumStart = counter ;
    D.cpus[1].inCS = true;
    D.cpus[1].CSremaining = CSperiod;
    (*(D.cpus[1].runningprocess)).isSaving = CSperiod;
    returned.push_back(*(D.cpus[1].runningprocess));
    D.cpus[1].runningprocess = NULL;
    D.runningqueue.erase(D.runningqueue.begin()+1);
  }
return returned ;
}



std::vector<process> sched::checkQuantums(int counter,dispatcher& D)
{
 int readysize = readyqueue.size();
  std::vector<process> returned;
  if(readysize==1 && ((counter - D.cpus[0].currentQuantumStart)/Quantum) == 1 && ((counter - D.cpus[1].currentQuantumStart)/Quantum) == 1 && D.cpus[0].runningprocess!=NULL && D.cpus[0].inCS==false && D.cpus[1].runningprocess!=NULL && D.cpus[1].inCS==false)
  {
    cout<<D.cpus[0].name<<" has ended its quantum"<<endl;
    cout<<"I am in First loop"<<endl;
    D.cpus[0].inCS = true;
    D.cpus[0].CSremaining = CSperiod;
    (*(D.cpus[0].runningprocess)).isSaving = CSperiod;
    returned.push_back(*(D.cpus[0].runningprocess));
    
    for(int j=0;j<D.runningqueue.size(); j++)
    {
      if(D.runningqueue[j].id == D.cpus[0].runningprocess->id)
      {
        D.cpus[0].runningprocess = NULL;

        D.runningqueue.erase(D.runningqueue.begin()+j);
        j--;
      }
    }
    D.cpus[1].currentQuantumStart = counter ;
    return returned;
  }    
  for (int i = 0 ; i< D.cpus.size() ; i++)
  {
    if (((counter - D.cpus[i].currentQuantumStart)/Quantum) == 1 && D.cpus[i].runningprocess!=NULL && D.cpus[i].inCS==false)
    {
      if(readysize==0)
        D.cpus[i].currentQuantumStart = counter ;
      else
      {

        cout<<D.cpus[i].name<<" has ended its quantum"<<endl;
        cout<<"I am in second loop"<<endl;
        D.cpus[i].inCS = true;
        D.cpus[i].CSremaining = CSperiod;
        (*(D.cpus[i].runningprocess)).isSaving = CSperiod;
        returned.push_back(*(D.cpus[i].runningprocess));
        
        for(int j=0;j<D.runningqueue.size(); j++)
        {
          cout<<"I am in for loop"<<endl;
          if(D.runningqueue[j].id == D.cpus[i].runningprocess->id)
          {

            D.cpus[i].runningprocess = NULL;

            D.runningqueue.erase(D.runningqueue.begin()+j);
            break;
          }
        }
        cout<<"I am done with for loop"<<endl;
      }
    }
  }
return returned ;
}

















std::vector<process> sched::checkblockedtounblock(dispatcher &D)
{
  string c = "cpu";
  std::vector<process> returned;
  for (int i = 0 ; i< D.blockedqueue.size() ; i++ )
  {
    if (D.blockedqueue[i].commands[D.blockedqueue[i].ip] == c)
    {
      returned.push_back(D.blockedqueue[i]);
      cout <<" i am remove process with id " <<D.blockedqueue[i].id <<endl ;
      D.blockedqueue.erase(D.blockedqueue.begin()+i) ;
      i-- ;
    }
  }
  return returned ;

}







std::vector<process> sched::checkrunningtoblock(dispatcher& D)
{
  string c = "cpu";
  std::vector<process> returned;
  for (int i = 0 ; i< D.runningqueue.size() ; i++ )
  {
    if (D.runningqueue[i].commands[D.runningqueue[i].ip] != c)
    {
      cout<<"This process is going to be blocked -> "<<D.runningqueue[i].id<<endl;
      D.runningqueue[i].isSaving = CSperiod;
      //cout<<"Error is here"<<endl;
      
      //cout<<"Error is not here"<<endl;
      //if((getcpu(runningqueue[i]))==NULL)
        //cout<<"No process found on CPU"<<endl;
      //cout<<"Error1"<<endl;
      (D.getcpu(D.runningqueue[i]))->inCS = true;
      //cout<<"Error2"<<endl;
      (D.getcpu(D.runningqueue[i]))->CSremaining = CSperiod;
      //cout<<"Error3"<<endl;
      (D.getcpu(D.runningqueue[i]))->runningprocess = NULL;
      cout<<"Done freeing CPU"<<endl;
      returned.push_back(D.runningqueue[i]);
      D.runningqueue.erase(D.runningqueue.begin()+i);
      i--;
    }
  }
return returned ;
}












void sched::checkreadyforhistory(int counter, dispatcher& D)
{
  for (int i = 0; i < readyqueue.size(); ++i)
  {
    cout<<"Printing History of ID = "<<readyqueue[i].id<<endl;
    if(readyqueue[i].ip == 1) continue;
    if(readyqueue[i].isSaving==CSperiod)
      readyqueue[i].addtohistoryEnd(readyqueue[i].ip, counter, (D.getcpu(readyqueue[i]))->name);
    else if(readyqueue[i].commands[readyqueue[i].ip]!=readyqueue[i].commands[readyqueue[i].ip-1])
      readyqueue[i].addtohistoryEnd(readyqueue[i].ip-1, counter, (D.getcpu(readyqueue[i]))->name);
  }
}




std::vector<process> sched::computeRR(dispatcher& D) 
{
    cout<<"Will start RR computation..."<<endl;
    std::vector<process> combinedqueue = D.runningqueue;
    combinedqueue.insert(combinedqueue.end(),readyqueue.begin(),readyqueue.end());
    cout<<"combinedqueue size is "<<combinedqueue.size()<<endl ;
    std::vector<process> temp;
    if (combinedqueue.size()>=2)
    {
      temp.push_back(combinedqueue[0]);
      temp.push_back(combinedqueue[1]);
    }
    else if(combinedqueue.size()==1)
    {
      temp.push_back(combinedqueue[0]);
    }
    
    return temp;

}


//-------------------------------------------|-----------------------------------
std::vector<process> sched::computeHPF(dispatcher& D)
 {
  cout<<"Will start HPF computation..."<<endl;
  std::vector<process> combinedqueue;
  if(readyqueue.size()>0)
    combinedqueue.insert(combinedqueue.end(),readyqueue.begin(),readyqueue.end());
  //cout<<"Error is here"<<endl;
  if(D.runningqueue.size()>0)
    combinedqueue.insert(combinedqueue.end(),D.runningqueue.rbegin(),D.runningqueue.rend());
  //cout<<"Error is not here"<<endl;
  std::vector<process> temp;
    if (combinedqueue.size() !=0)
    {
          int prio1= 0;
          int prio2= 0;
          int max =combinedqueue[prio1].priority ;
          
          for(int i = 1; i<combinedqueue.size();i++)
          {
            
                if(combinedqueue[i].priority >= max)
                {
                  max = combinedqueue[i].priority;
                  prio1 = i;
                }
         }

          //cout <<"Will not erase now " <<combinedqueue[min1].id<<" "<<min1<<endl;
          temp.insert(temp.end(),combinedqueue[prio1]);
          //cout <<"Will erase now" <<endl;
          combinedqueue.erase(combinedqueue.begin()+prio1);
          cout<<"prio1 =" <<max <<endl;
          if(combinedqueue.size()!=0)
          {
            max = combinedqueue[prio2].priority;
            for(int i = 1; i<combinedqueue.size();i++)
            {
              
              if(combinedqueue[i].priority >= max)
              {
                max = combinedqueue[i].priority;
                prio2 = i ;
              }
            }
            cout<<"prio2 =" <<max <<endl; 
            temp.push_back(combinedqueue[prio2]);
          }
  }

   cout <<"tesssss"<<endl;
    return temp;
}


//-------------------------------------------|----------------------------------------


std::vector<process> sched::computeHPRR(dispatcher& D) //Highest Priority Round Robin
 {
  /*cout<<"ready queue size is  before everything "<<readyqueue.size()<<endl;
  for (int i = 0; i < readyqueue.size(); ++i)
    cout<<readyqueue[i].id<<"  ";
  cout<<endl;*/
  cout<<"Will start HPRR computation..."<<endl;
  std::vector<process> combinedqueue;
  //combinedqueue.insert(combinedqueue.begin(),D.runningqueue[0]);
 // combinedqueue.insert(combinedqueue.end(),readyqueue.begin(),readyqueue.end());
  /*cout<<"Combined queue size is before pushback "<<combinedqueue.size()<<endl;
  for (int i = 0; i < combinedqueue.size(); ++i)
    cout<<combinedqueue[i].id<<"  ";
  cout<<endl;*/
  /*for (int i=0; i<readyqueue.size();i++)
  {
    combinedqueue.push_back(readyqueue[i]);
  }*/
  /*cout<<"Combined queue size is "<<combinedqueue.size()<<endl;
  for (int i = 0; i < combinedqueue.size(); ++i)
    cout<<combinedqueue[i].id<<"  ";
  cout<<endl;*/
  std::vector<process> temp;
  if(readyqueue.size()+D.runningqueue.size()==0)
    return temp;

        if(readyqueue.size()==0)
          return D.runningqueue;
        int op1=0;
        int max =readyqueue[op1].priority ;
        for(int i = 1; i<readyqueue.size();i++)
        {
            if(readyqueue[i].priority > max)
            {
              if(readyqueue[i].isSaving)
                continue;

             /* if (D.cpus[1].runningprocess != NULL && readyqueue[i].id == D.cpus[1].runningprocess->id)    // cpu0 -> highest pirority cpu1 -> RR // v[0]->hp v[1]->RR // v[0] != v[1] 
                continue;*/

                max = readyqueue[i].priority;
                op1 = i;
            }
        }
        if (D.runningqueue.size()!=0)
        {
          if (max>D.runningqueue[0].priority)
            temp.push_back(readyqueue[op1]);
          else
            temp.push_back(D.runningqueue[0]);
        }
        else
          temp.push_back(readyqueue[op1]);

        if(D.runningqueue.size()==2)
          combinedqueue.push_back(D.runningqueue[1]);
        
        combinedqueue.insert(combinedqueue.end(),readyqueue.begin(),readyqueue.end());

        if (combinedqueue.size()>=2)
          temp.push_back(combinedqueue[0]);
    return temp ;
}


//-------------------------------------------|----------------------------------------



 std::vector<process> sched::computeSRT(dispatcher &D)
 { 
  cout<<"Will start SRT computation..."<<endl;
  std::vector<process> combinedqueue;
  if(readyqueue.size()>0)
    combinedqueue.insert(combinedqueue.end(),readyqueue.begin(),readyqueue.end());
  //cout<<"Error is here"<<endl;
  if(D.runningqueue.size()>0)
    combinedqueue.insert(combinedqueue.end(),D.runningqueue.rbegin(),D.runningqueue.rend());
  //cout<<"Error is not here"<<endl;
  std::vector<process> temp;    
  if (combinedqueue.size()!=0)
  {
          int min1= 0;
          int min2= 0;
          cout <<"combinedqueue size is " << combinedqueue.size()<<endl ; // 1   3        4
          cout <<"combinedqueue remaining[0] = " << combinedqueue[0].remainingtime<<endl ;
          int min =combinedqueue[min1].remainingtime ;            //20  20       8
          cout <<"initial minimum is "<<min<<endl;
          for(int i = 1; i<combinedqueue.size();i++)
          {
            
            if(combinedqueue[i].remainingtime <= min)
            {
              min = combinedqueue[i].remainingtime;
              min1 = i ;
            }
          }

          cout <<"Will not erase now " <<combinedqueue[min1].id<<" "<<min1<<endl;
          temp.insert(temp.end(),combinedqueue[min1]);
          cout <<"Will erase now" <<endl;
          combinedqueue.erase(combinedqueue.begin()+min1);
          cout<<"min1 =" <<min <<endl;
          if(combinedqueue.size()!=0)
          {
            min = combinedqueue[min2].remainingtime;
            for(int i = 1; i<combinedqueue.size();i++)
            {
              
              if(combinedqueue[i].remainingtime <= min)
              {
                min = combinedqueue[i].remainingtime;
                min2 = i ;
              }
            }
            cout<<"min2 =" <<min <<endl; 
            temp.push_back(combinedqueue[min2]);
          }
  }

   cout <<"tesssss"<<endl;
    return temp;
}

//---------------------------------------|-----------------------------------------

sched::sched()
{
	ri = readyqueue.begin();
}
//-------------------------------------|------------------------------------------------
bool sched::putinreadyqueue(process B, dispatcher& D)
{
  if(B.memory!=0 )
 {
  string c = "cpu";
  this->readyqueue.push_back(B);
  //D.sendstop(mapp,B.id);
  if(B.remainingtime!=B.totaltime && B.commands[B.ip-1] == c)
    B.isSaving = CSperiod;
  D.deletefromqueue(D.runningqueue,B.id);
  return true ;
}
return false ;
}
#endif