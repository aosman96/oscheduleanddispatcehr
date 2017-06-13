
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <vector>
#include <stdlib.h>
#include "process.h"
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "sharedmemory.h"
#include "buffers.h"
using namespace std;

vector<process> getprocess( int counter ,vector<process> processarray , int & x)

{
  cout<<"Process Array has size = "<<processarray.size()<<endl;

  vector<process> processqueue;
cout<<"Process Queue size is "<<processqueue.size()<<endl;  
for (int i =0 ; i < processarray.size() ; i++)
{ 
if(counter == processarray[i].arrivaltime)
{
  processqueue.push_back(processarray[i]) ;
  processarray.erase(processarray.begin()+i);
  i--;
  x--;
}
}
cout<<"Process Queue size is "<<processqueue.size()<<" and its data is:"<<endl;
for (int i = 0; i < processqueue.size(); ++i)
{
  cout<<processqueue[i].id<<" "<<processqueue[i].memory<<endl;
}
return processqueue ; 
}


vector<process> Readinput(config &t)
{
 ifstream fin;
  string dir, filepath;
  vector<process> processarray ;
string a ;
  DIR *dp;
string hashtag = "#";
string config ="configuration_file.txt";
string s ="size";
string P = "priority";
string arrival = "arrival";
string cpu ="cpu" ;
  struct dirent *dirp;
  struct stat filestat;
string sec ;
int index ;
 //vector<process> processarray ;
  cout << "dir to get files of: " << flush;
  getline( cin, dir );  // gets everything the user ENTERs

  dp = opendir( dir.c_str() );
  if (dp == NULL)
    {

    cout << "Error(" << "errno" << ") opening " << dir << endl;
    return processarray;
    }

  while ((dirp = readdir( dp )))
    { 
    filepath = dir + "/" + dirp->d_name;

    // If the file is a directory (or is in some way invalid) we'll skip it 
    if (stat( filepath.c_str(), &filestat )) continue;
    if (S_ISDIR( filestat.st_mode ))         continue;

    // Endeavor to read a single number from the file and display it
    fin.open( filepath.c_str() );
int z =0;
//----------------------------------
//Loop to read the configurtion file will be here
//--------------------------------
if((dirp->d_name)==config)
{
cout <<"tt"<<endl ;
while(fin >>a)
{
if (a.find(hashtag)!=string::npos)
{
fin.ignore( 10000,'\n');
cout<<"linaaaaaaaa"<<endl;
continue ;
}
if(z==0)
{
t.algonum = atoi(a.c_str()) ;
cout <<"algonum =" <<t.algonum<<endl ;
z++ ;
}
else if(z==1)
{
t.penality = atoi(a.c_str()) ;
cout <<"penality =" <<t.penality<<endl ;
z++ ;
}
else if (z==2)
{
t.quantum = atoi(a.c_str()) ;
cout <<"quantum =" <<t.quantum<<endl ;
z++ ;
}
else if (z==3)
{

t.rateofdata = atoi(a.c_str()) ;
cout <<"rate =" <<t.rateofdata<<endl ;
z++ ;
}
}
}

else
{
process tempprocess;
index = 1;
while (fin>>a)
{
cout<<"i read : "<<a <<endl ;
//
if (a.find(hashtag)!=string::npos)
{
fin.ignore( 10000,'\n');
cout<<"linaaaaaaaa"<<endl;
continue ;
}
///-----
if (a==s)
{
string x;
fin>>x;
cout<<"i will put memory "<<x <<endl ;
 
tempprocess.memory = atoi(x.c_str()) ;
}
else if (a==P)
{
string x;
fin>>x;
cout<<"i will put priority "<<x <<endl ;
 
tempprocess.priority = atoi(x.c_str()) ;
}
else if (a==arrival)
{
string x;
fin>>x;
cout<<"i will put arrivalime "<<x <<endl ;
tempprocess.arrivaltime = atoi(x.c_str()) ;


}
else 
{

fin>>sec ;
for(int i =1 ; i<= atoi(sec.c_str()) ;i++)   //lazm tesave el adem we tezwad 3leh 
{
cout<<"i will put the  "<<a <<" with index = "<<index<<endl ;

tempprocess.commands[index]= a ;
if(a==cpu)
++tempprocess.totalcpu;
else
++tempprocess.totalio ; 
index++ ;
}
}

}
tempprocess.totaltime= index -1 ;
tempprocess.remainingtime = tempprocess.totaltime ;
 processarray.push_back(tempprocess);
}


   fin.close();
    }

  closedir( dp );
  return processarray;

}


//20-4
bool sendconfig(config t ,key_t msgqid)
{

      int messagesize = sizeof(t.algonum)+sizeof(t.penality)+sizeof(t.quantum)+sizeof(t.rateofdata) ;
         
        int send_val = msgsnd(msgqid, &t,messagesize , !IPC_NOWAIT);
	 
	if(send_val == -1)
 {   perror("Errror in send");
	  return false ;
}
	else 
	return true ;

}




//20-4






/* updated*/
bool sendmessage(std::vector<process>  processtobesend,key_t msgqid)
{
  cout <<"processtobesend has size"<< processtobesend.size()<<endl;
   messagebuff message ;
    if (processtobesend.size()==0)
    {
      message.numberofprocess=0 ;
      message.size=0;
//wlahy 3'erna hena
      int messagesize = sizeof(struct messagebuff)- sizeof(long ) ;
         message.mtype =3 ;
        int send_val = msgsnd(msgqid, &message,messagesize , !IPC_NOWAIT);
    
  if(send_val == -1)
    perror("Errror in send");
  return false ;
    }
    else
    {
      message.numberofprocess = processtobesend.size()-1;

      for(int i=0 ; i<processtobesend.size() ; i++)

      {


//wlahy 3'erna hena      
//fe t3'er henaaaaaaa
        int messagesize = sizeof(struct messagebuff)- sizeof(long ) ;
     processtobesend[i].changeittobesend(message.arrivaltime,message.totaltime, message.size , message.cpu ,message.IO,message.CD ,message.printer ,message.id  ,message.priority ) ;
           message.mtype =3 ;
                 
      int send_val = msgsnd(msgqid,(void *) &message, messagesize, !IPC_NOWAIT);
      message.numberofprocess -- ;
  if(send_val == -1)
    perror("Errror in sending process");
     }
      
    }
  
 
    return true ;
}


/////////////
int recivetime(key_t msgqid)
{
  messagecounter message ;
int c_val = msgrcv(msgqid, &message, sizeof(message.counter), 0, !IPC_NOWAIT);
cout << "i recived time "<<message.counter <<endl ;
return message.counter ;
}

int main(int argc,char * *argv )
{ 
  std::vector<process> sendprocess;
  std::vector<process> processarray;
  key_t msgqid;
  key_t msgqid2;
  key_t msgqid3;
//#20/4
	config t ;
  msgqid =  msgget(12700, IPC_CREAT | 0644);
  msgqid2 = msgget(12701, IPC_CREAT | 0644);
	msgqid3 = msgget(12702, IPC_CREAT | 0644);
  //#20/4
  int time  ;
  processarray = Readinput(t);
bool x = sendconfig(t,msgqid3);
if(x== true)
cout <<"config sent"<<endl ;
else 
cout <<"config failed"<<endl ;
int ff = processarray.size();
 while (1)
  { 
    time = recivetime(msgqid2);
    cout <<"time recived ="<<time <<endl ;
    sendprocess = getprocess(time,processarray ,ff) ;
    cout <<"i will send nowwwww" <<endl ;
    cout <<"size of processarray   "<<processarray.size()<<endl ;
    cout <<"size of sendarray  "<<sendprocess.size()<<endl ;
    sendmessage(sendprocess,msgqid);
    for (int i = 0; i < sendprocess.size(); ++i)
     { sendprocess.erase(sendprocess.begin()+i);
      i--;
     }   
  cout <<"messagesended"<<endl ;
  

if (ff==0 )
  break ;

  }

messagebuff message ;
message.size = 0 ;
message.arrivaltime = -1 ;
message.numberofprocess =0 ;
message.mtype =3;
int messagesize =sizeof(struct messagebuff)- sizeof(long ) ;
 int send_val = msgsnd(msgqid, &message, messagesize, !IPC_NOWAIT);
  if(send_val == -1)
    perror("Errror in sending process");
  time = recivetime(msgqid2);
   /* msgctl(msgqid, IPC_RMID, (struct msqid_ds *) 0);
         msgctl(msgqid2, IPC_RMID, (struct msqid_ds *) 0);
         msgctl(msgqid3, IPC_RMID, (struct msqid_ds *) 0);*/
	return 0;
}
