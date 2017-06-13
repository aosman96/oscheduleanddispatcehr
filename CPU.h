#include "process.h"
#include <string.h>
#ifndef HEADER2
#define HEADER2
class CPU
{
public:
	std::vector<int> historyTime;
	std::vector<string> historyName;
	std::vector<int> historyStart;
	std::vector<int> historyEnd;
	std::vector<string> historyFinalName;
	process * runningprocess ;
	float utilization ;
	string name;
	void calculateutlization(int time);
	void printfile(int time);
	int currentQuantumStart;
	bool busy;
	bool inCS;
	int CSremaining;
	void RunProcess();
	void Record(int counter, int id);
	void testHistory();
	CPU(string s);
};


CPU::CPU(string s)
{
	busy=false ;
	name = s;
	utilization=0;
	runningprocess=NULL;	
	currentQuantumStart=0;
	inCS = false;
	CSremaining = 0;
}
///////////////////////////->>>>>>>>>>>>>>>>>>>>>>>>>>>>
void CPU::calculateutlization(int time)
{

int sum = 0 ;
	
for(int i=0; i < historyStart.size(); i++)
     sum +=(historyEnd[i]-historyStart[i] + 1);

utilization = (float)((float)sum /(float)time) ;
}
 
void CPU::RunProcess()
{
	runningprocess->inc();
}

void CPU::Record(int counter,int id)
{

	char a[10];
	sprintf( a, " %d", id);
	historyTime.push_back(counter);
	string processid = "Process_" + std::string(a);
	historyName.push_back(processid);
}

void CPU::testHistory()
{
	string s = name + "test.txt";
	ofstream fileout;
	fileout.open(s.c_str(), ios::out | ios::trunc );

	for (int i =0 ; i < historyTime.size() ; i++ ) 
    	fileout <<historyTime[i] <<" "<<historyName[i]<<endl;
    fileout.close();

}

void CPU::printfile (int time)
{
	testHistory();
	string s = "output/"+name + ".txt";
	ofstream fileout;
	fileout.open(s.c_str(), ios::out | ios::trunc );

	
	int i=0,j=1;
	historyStart.push_back(historyTime[i]);
	historyFinalName.push_back(historyName[i]);
	while(i<historyTime.size())
	{
		
		if(historyTime[j]-historyTime[j-1]>1 || historyName[j]!=historyName[j-1])
		{
			historyEnd.push_back(historyTime[j-1]);
			i=j;
			historyStart.push_back(historyTime[i]);
			historyFinalName.push_back(historyName[i]);
		}
		j++;
		if(j==historyTime.size())
			break;
	}

	if(historyStart.size()!=historyEnd.size())
		historyEnd.push_back(historyTime[historyTime.size()-1]);

	for(int i=0; i < historyStart.size(); i++)
	{
	    fileout << "[" << historyStart[i] <<":" << historyEnd[i] <<"]" << " " << historyFinalName[i] <<endl;
	}

	this -> calculateutlization(time);
	fileout << "CPU Utilization = " <<this->utilization<<endl ;

   	fileout.close();

}
#endif