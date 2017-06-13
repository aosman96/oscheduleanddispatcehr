
//------------------------------------------------------------------------------------------------------------------------------
#include "CPU.h"
#include <math.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/ipc.h>
#ifndef HEADER3
#define HEADER3
/*
int main(int argc, char * *argv )
{
	std::vector< process> recivedprocess;
	std::vector<process> blocked;
	messagebuff processrecived ;
	key_t msgqid = atoi(argv[1]);  //da bet3 send process
 	 key_t msgqid2 = atoi(argv[2]); //da bt3 el counter 
  		int time ;
  		while(true)
  		{
  			time = recivetime(msgqid2);

  			for(int i = 0 ; i< 2 ; i++)
  			{
  			processrecived = recivedprocess(msgqid);
  			process temp ;
  			// temp.takeinp(processrecived); to be implemented
 			recivedprocess.push_back(temp);
			}
			for(int i=0;i<blocked.size();i++)
			{
				if(recivedprocess[0].id == blocked[i].id)
				{
					
				}
			}

  		}

	return 0;
}
*/
//------------------------------------------------------------------------------------>
class dispatcher
{
public:
  std::vector<process> runningqueue;
  std::vector<process> blockedqueue;
  std::vector<CPU> cpus;
  int waitingsum;
  std::vector<float> weighted;
  float performDeno;
  CPU* getcpu(process p);
  bool deletefromqueue(std::vector<process> &B ,int id);
 bool getpid(int id,  pid_t & pid );
 bool insertInBlocked(std::vector<process> temp);
 bool checkinrunningqueue(int id);
 std::vector<process> checkCustomQuantums(int counter,int Quantum, int CSperiod);
 void checkblockedforhistory(int counter);
 bool decblockedprocesstime(std::map<int, pid_t> mapp);
 void checkrunningforhistory(int counter, int CSperiod);
 void printMetrics();
 void killprocess(process& P);
 void sendstop(std::map<int, pid_t> mapp,int id);
void sendcontinue(std::map<int, pid_t> mapp, int id ) ;
 void cleanUp();
  dispatcher();
};

dispatcher::dispatcher()
{
  CPU CPU1("CPU_1");
  CPU CPU2("CPU_2");
  cpus.push_back(CPU1);
  cpus.push_back(CPU2);
  waitingsum = 0;
  performDeno = 0;
}

void dispatcher::printMetrics()
{
  float utisum = 0;
  for (int i = 0; i < cpus.size(); ++i)
    utisum += cpus[i].utilization;
  float utiavg = utisum/cpus.size();
  float performance;
  if(performDeno!=0)
    performance = utiavg + (1/performDeno);
  else
    performance = -1;

ofstream outfile;
string name  = "output/Metrics.txt";
outfile.open(name.c_str(), ios::out | ios::trunc );
if(performance == -1)
  outfile<< "Performance = INFINITY!!!!!!!!!"<< endl;
else
  outfile<< "Performance = " << performance << endl;
float weightedsum = 0;

float waitingavg = (float)((float)waitingsum/(float)weighted.size());
for (int i = 0; i < weighted.size(); ++i)
  weightedsum+=weighted[i];
float weightedavg = weightedsum/(float)weighted.size();
float numerator = 0;
for (int i = 0; i < weighted.size(); ++i)
  numerator += pow((weighted[i]-weightedavg),2.0);

float standardDev = sqrt(numerator/weighted.size());
outfile << "Avg waiting = " << waitingavg << endl;
outfile << "sum waiting = " << waitingsum << endl;
outfile << "Avg weighted turnaround = " << weightedavg << endl;
outfile << "Standard Deviation = " << standardDev << endl;
outfile<<"weighted count "<<weighted.size()<<endl;
outfile<<"weighted sum "<<weightedsum<<endl;
outfile<<"standardDev numerator "<<numerator<<endl;
outfile.close();
}

void dispatcher::killprocess(process& P)
{
  cout<<"Now printing process "<<P.id<<"..."<<endl;
  P.filterhistory();
  P.testhistory();
  P.printprocess();
  waitingsum += P.waitingtime;
  float turnaround = P.historyEnd[P.historyEnd.size()-1] - P.arrivaltime + 1 ;
  performDeno += (P.waitingtime*P.priority*0.1)/P.totaltime;
  weighted.push_back((float)(turnaround/(float)P.totaltime));
  cout<<"Done printing process"<<P.id<<endl;
}

bool dispatcher::deletefromqueue(std::vector<process> &B,int id)
{

  for (int i = 0 ; i< B.size() ; i++)
  {

    if (B[i].id == id )
    {

      B.erase(B.begin()+i) ;
      break ;
    } 
  }
}


bool getpid(std::vector<process> temp, int id,  pid_t & pid )
{

  for (int i = 0 ; i< temp.size() ; i++)
  {

    if (temp[i].id == id )
    {
      pid = temp[i].processid;
      return true;
    }
  }
  return false;
}


bool dispatcher::insertInBlocked(std::vector<process> temp)
{
  for (int i = 0 ; i< temp.size() ; i++)
  {
    blockedqueue.push_back(temp[i]);
  }
}



bool dispatcher::decblockedprocesstime(std::map<int, pid_t> mapp)
{
  for (int i = 0 ; i< this->blockedqueue.size() ; i++ )
  {
    if(this->blockedqueue[i].isSaving==0)
    {
      this->blockedqueue[i].inc();
     
      sleep(1);
    }
    else
      this->blockedqueue[i].isSaving--;
  }
  return true ;
}

 bool  dispatcher::checkinrunningqueue(int id)
 {
  for (int i = 0 ; i< runningqueue.size() ; i++)
  {

    if (runningqueue[i].id == id )
    {
      return true;
    }
  }
  return false;
 }
//
CPU* dispatcher::getcpu(process p)
{
for(int i = 0 ; i< cpus.size() ;i++)
{
if (cpus[i].runningprocess!=NULL && (cpus[i].runningprocess->id) == p.id )
  return &cpus[i] ;
}
return NULL;
}

void dispatcher::cleanUp()
{
  for (int i = 0; i < blockedqueue.size(); ++i)
    if(blockedqueue[i].remainingtime==0)
    {
      cout<<"Will kill process in blocked with ID = "<<blockedqueue[i].id<<endl;
      
    //  killprocess(blockedqueue[i]);
      blockedqueue.erase(blockedqueue.begin()+i);
      i--;
    }

  for (int i = 0; i < runningqueue.size(); ++i)
    if(runningqueue[i].remainingtime==0)
    {
      cout<<"Will kill process in running with ID = "<<runningqueue[i].id<<endl;
      //killprocess(runningqueue[i]);
      (getcpu(runningqueue[i]))->runningprocess = NULL;
      runningqueue.erase(runningqueue.begin()+i);
      i--;
    }
}

void dispatcher::checkblockedforhistory(int counter)
{
  for (int i = 0; i < blockedqueue.size(); ++i)
  {
    if(blockedqueue[i].ip==1)
      blockedqueue[i].addtohistoryStart(blockedqueue[i].ip, counter, (getcpu(blockedqueue[i]))->name);
    else if(blockedqueue[i].commands[blockedqueue[i].ip]!=blockedqueue[i].commands[blockedqueue[i].ip-1])
    {
      blockedqueue[i].addtohistoryEnd(blockedqueue[i].ip-1, counter, (getcpu(blockedqueue[i]))->name);
      blockedqueue[i].addtohistoryStart(blockedqueue[i].ip, counter, (getcpu(blockedqueue[i]))->name);
    }
  }
}

void dispatcher::checkrunningforhistory(int counter, int CSperiod)
{
  for (int i = 0; i < runningqueue.size(); ++i)
  {
    if(runningqueue[i].ip==1)
      runningqueue[i].addtohistoryStart(runningqueue[i].ip, counter, (getcpu(runningqueue[i]))->name);
    else if(runningqueue[i].remainingtime==0)
      runningqueue[i].addtohistoryEnd(runningqueue[i].ip, counter, (getcpu(runningqueue[i]))->name);
    else if((getcpu(runningqueue[i]))->CSremaining == CSperiod)
      runningqueue[i].addtohistoryStart(runningqueue[i].ip, counter+CSperiod, (getcpu(runningqueue[i]))->name);
  }
}





/*for (int j = 0; j < cpus.size(); ++j)
      {
        if (cpus[j].runningprocess!=NULL && cpus[j].runningprocess->id == runningqueue[i].id)
        {
          cpus[j].inCS = true;
          cpus[j].CSremaining = CSperiod;
          cpus[j].runningprocess = NULL;
        }
      }*/



void dispatcher::sendcontinue(std::map<int, pid_t> mapp, int id )
{
kill(mapp[id],SIGCONT);
sleep(1);
}

void dispatcher::sendstop(std::map<int, pid_t> mapp,int id)
{
kill(mapp[id],SIGSTOP);
sleep(2);

}

#endif