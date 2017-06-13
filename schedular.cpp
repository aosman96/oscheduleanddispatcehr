#include "shedular.h"
#include <sys/msg.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/ipc.h>
dispatcher Dispatcher;
sched schedular ;
int counter=1 ;
void chilhandler (int signum )
{
  cout <<"i entered the handler of the parent "<<endl;
int stat_loc ;
pid_t pid ;
pid = wait (&stat_loc);
 int exitcode =stat_loc>>8;


 for(int i =0 ; i<Dispatcher.runningqueue.size();i++ )
 {
  if(Dispatcher.runningqueue[i].id == exitcode)
  {
    if(Dispatcher.getcpu(Dispatcher.runningqueue[i])!=NULL)
      (Dispatcher.getcpu(Dispatcher.runningqueue[i]))->runningprocess = NULL ;

  Dispatcher.waitingsum += Dispatcher.runningqueue[i].waitingtime;
  float turnaround = counter - Dispatcher.runningqueue[i].arrivaltime + 1 ;
  Dispatcher.performDeno += (Dispatcher.runningqueue[i].waitingtime*Dispatcher.runningqueue[i].priority*0.1)/Dispatcher.runningqueue[i].totaltime;
  Dispatcher.weighted.push_back((float)(turnaround/(float)Dispatcher.runningqueue[i].totaltime));
  cout<<"Done printing process"<<Dispatcher.runningqueue[i].id<<endl;
    Dispatcher.runningqueue.erase(Dispatcher.runningqueue.begin()+i);
    break ;
  }
 }
  for(int i =0 ; i<Dispatcher.blockedqueue.size();i++ )
 {
  if(Dispatcher.blockedqueue[i].id == exitcode)
  {
     if(Dispatcher.getcpu(Dispatcher.blockedqueue[i])!=NULL)
        (Dispatcher.getcpu(Dispatcher.blockedqueue[i]))->runningprocess = NULL ;
       Dispatcher.waitingsum += Dispatcher.blockedqueue[i].waitingtime;
  float turnaround = counter - Dispatcher.blockedqueue[i].arrivaltime + 1 ;
  Dispatcher.performDeno += (Dispatcher.blockedqueue[i].waitingtime*Dispatcher.blockedqueue[i].priority*0.1)/Dispatcher.blockedqueue[i].totaltime;
  Dispatcher.weighted.push_back((float)(turnaround/(float)Dispatcher.blockedqueue[i].totaltime));
  cout<<"Done printing process"<<Dispatcher.blockedqueue[i].id<<endl;
    Dispatcher.blockedqueue.erase(Dispatcher.blockedqueue.begin()+i);
    break ;
  }
 }


 for(int i =0 ; i<schedular.readyqueue.size();i++ )
 {
  if(schedular.readyqueue[i].id == exitcode)
  {
     if(Dispatcher.getcpu(schedular.readyqueue[i])!=NULL)
      (Dispatcher.getcpu(schedular.readyqueue[i]))->runningprocess = NULL ;
           Dispatcher.waitingsum += schedular.readyqueue[i].waitingtime;
  float turnaround = counter - schedular.readyqueue[i].arrivaltime + 1 ;
  Dispatcher.performDeno += (schedular.readyqueue[i].waitingtime*schedular.readyqueue[i].priority*0.1)/schedular.readyqueue[i].totaltime;
  Dispatcher.weighted.push_back((float)(turnaround/(float)schedular.readyqueue[i].totaltime));
  cout<<"Done printing process"<<schedular.readyqueue[i].id<<endl;

    schedular.readyqueue.erase(schedular.readyqueue.begin()+i);
    break ;
  }
 }
cout <<"a process with id = has been finished "<<exitcode<<endl ;

}

void sendcounter(int counter,key_t msgqid) 
{
	messagecounter message ;
	message.counter = counter ;
	int send_val = msgsnd(msgqid, &message, sizeof(message.counter), !IPC_NOWAIT);
if(send_val == -1)
  	perror("Errror in sending counter");

}
messagebuff reciveproces( key_t msgqid)
{

  messagebuff message ;
//
  int messagesize = sizeof(struct messagebuff) - sizeof(long);
  int rec_val = msgrcv(msgqid,(void *) &message, messagesize, 0, !IPC_NOWAIT);  
 
  if(rec_val == -1)
    perror("Error in receive");
  else
    cout <<"i finished reciving process "<<endl ;
  
  return message ;

}

config reciveconfig(key_t msgqid)
{
  config t ;
  int messagesize = sizeof(t.algonum)+sizeof(t.penality)+sizeof(t.quantum)+sizeof(t.rateofdata) ;
  
  int rec_val = msgrcv(msgqid, &t, messagesize, 0, !IPC_NOWAIT);  
 if(rec_val == -1)
    perror("Error in receive");
  else
    cout <<"i finished reciving config "<<endl ;

  return t;
}



//------------------------------------------------------|--------------------------------------_|_
int main()
{
  ///////////////////////////////////////////nshel 2l varibe 2 zyda
	pid_t pid;
  key_t msgqid,msgqid2;
  key_t msgqid3 ;
  
  
  std::map<int, pid_t> mapp;
  bool generatoralive = true;
  char a[10];
  char b[10];
  
signal(SIGCHLD,chilhandler);
  msgqid =  msgget(12700, IPC_CREAT | 0644);
  msgqid2 = msgget(12701, IPC_CREAT | 0644);
//20-4
  msgqid3 = msgget(12702, IPC_CREAT | 0644);

  if(msgqid == -1)
  {	
    perror("Error in create");
    exit(-1);
  }
  printf("msgqid = %d\n", msgqid);
  printf("msgqid2 = %d\n", msgqid2);
  config configuration = reciveconfig(msgqid3);
    schedular.CSperiod = configuration.penality;
    schedular.Quantum = configuration.quantum;
    schedular.mode = configuration.algonum;
    schedular.rateofData = configuration.rateofdata;  
    	while(counter)
    	{
        
cout<<"-------------------------------------------------------------------------"<<endl;
        cout<<"Counter has reached "<<counter<<endl;
        if(generatoralive)
        {
          messagebuff message ;
          cout<<"i will send the counter" <<counter<<endl;
    		  sendcounter(counter,msgqid2);
          do{
            	message =reciveproces(msgqid);
              process temp ;
              if(message.arrivaltime == -1 )
              {
                generatoralive = false ;
                break ;
              }
               if(message.size!=0 )
              { 
              temp.changetoprocess(message);
              temp.isNew = true;
              temp.waitingtime = 0;
              temp.remainingtime = temp.totaltime;
                 if(temp.memory!=0)
                schedular.readyqueue.push_back(temp);
              }

    	       	cout <<"i reciveproces"<<endl;
              cout<<message.arrivaltime<<endl;
              cout << message.numberofprocess<<endl;

            } while( message.numberofprocess!=0);
          }

          cout<<"Finished reciveproces "<<endl ;

          std::vector<process> unblocked=schedular.checkblockedtounblock(Dispatcher);
          cout<<"Unblocked size "<<unblocked.size()<<endl;
          
          
          for(int i=0;i<unblocked.size();i++)
            schedular.putinreadyqueue(unblocked[i],Dispatcher);

  
          //std::vector<process> Blocked=schedular.checkrunningtoblock(Dispatcher);
          //cout<<"Done creating block"<<endl;
          //Dispatcher.insertInBlocked(Blocked);
          //cout<<"New IDS to block now :"<<endl;
          //for (int i = 0; i < Blocked.size(); ++i)
            //cout<<Blocked[i].id<<" "<<endl;
          //cout<<endl;
          cout<<"Will start in algorithms "<<endl ;
          cout<<"Mode is "<<configuration.algonum<<endl ;



            switch (schedular.mode)
            {
               case 1: //// SRT Algorithm
                {
                  cout<<"Will start in SRT "<<endl ;
                  cout<<"Ready queue now has "<< schedular.readyqueue.size()<< "processes"<<endl;
                  cout<<"Ready queue IDS before algorithm:"<<endl ;
                  for (int i = 0; i < schedular.readyqueue.size(); ++i)
                    cout<<schedular.readyqueue[i].id<<" ";
                  cout<<endl;

                  cout<<"Running Remaining Time before ComputeSTR:";
                  for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
                    cout<<Dispatcher.runningqueue[i].remainingtime<< " ";
                  cout<<endl;

                  cout<<"I got my running size is " <<Dispatcher.runningqueue.size()<<endl ;
                  cout<<"I got my ready size is " <<schedular.readyqueue.size()<<endl ;
                std::vector<process> SRToutput=schedular.computeSRT(Dispatcher);
                cout<<"I got "<<SRToutput.size()<<" processes and they are:" <<endl ;
                for (int i = 0; i < SRToutput.size(); ++i)
                    cout<<SRToutput[i].id<<" ";
                  cout<<endl;
                if (Dispatcher.runningqueue.size()==0)   //hna mafish context switching hay7sl 
                {
                  cout<<"Here, I got my running size is zero" <<endl ;
                  Dispatcher.runningqueue = SRToutput;
                  cout<<"Now, I got my running size = "<<Dispatcher.runningqueue.size()<<endl ;
                  if (SRToutput.size()==1)
                  {
                    cout<<"I got "<<Dispatcher.cpus.size()<< " cpus" <<endl ;
                    for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                    {
                      if (!Dispatcher.cpus[i].inCS)
                      {
                        cout<<"test"<<endl;
                        Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[0];

                        cout<<Dispatcher.cpus[i].name<<endl;
                        break;
                      }
                    }
                    if (Dispatcher.cpus[0].inCS && Dispatcher.cpus[1].inCS)
                      Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[0];  //de betet7at fe CPU1 mesh CPU0 3shan out of the 2 processes the more important to keep is in CPU0
                  }
                  else if(SRToutput.size()==2)
                  {
                    int j=0;
                    for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                    {
                      Dispatcher.cpus[i].runningprocess= &Dispatcher.runningqueue[i];
                    }
                  }
                }
                else                             //hna fe context switching 
                {
                  int tempindex;


                  //-----------------------------------------------------------------------
                  for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
                  {
                    for (int j = 0; j < SRToutput.size(); ++j)
                    {
                      if(SRToutput[j].id == Dispatcher.runningqueue[i].id)
                        {   

                          SRToutput.erase(SRToutput.begin()+j);
                        }
                      else
                        tempindex = i;
                    }
                  }
                  //--------------------------------------------------------------------------
                  cout<<"B3d el filter hwa da el output size "<<SRToutput.size()<<endl;
                  int Temp;
                  if(SRToutput.size()==2)    //ha3ml context switching le 1 aow 2  
                  {
                    for (int i = 0; i <= Dispatcher.runningqueue.size(); ++i)
                    {
                      schedular.putinreadyqueue(Dispatcher.runningqueue[i],Dispatcher);
                      //Dispatcher.runningqueue.erase(Dispatcher.runningqueue.begin()+i);
                    }
                    for (int i = 0; i < SRToutput.size(); ++i)
                      Dispatcher.runningqueue.push_back(SRToutput[i]);        //SRToutput more priority to run than the currently running process
                    for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                    {
                      if(Dispatcher.cpus[i].runningprocess != NULL && Dispatcher.cpus[i].inCS==false) //check eno mesh b NULL 3shan lw process gdeeda gat
                      {
                        Dispatcher.cpus[i].runningprocess->isSaving = true;
                        Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[i];
                        Dispatcher.cpus[i].inCS = true;
                        Dispatcher.cpus[i].CSremaining = schedular.CSperiod;
                        Temp = 1-i;
                      }
                      else
                      {
                        Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[i]; //if the cpu is holding a process and is saving sth else to start this process, it leaves it to hold on to the new process

                      }
                    }
                    Dispatcher.cpus[Temp].runningprocess = &Dispatcher.runningqueue[0];

                  }
                  //////
                  else if(SRToutput.size()==1)   // ha3ml context switching e7tmal 
                  {

                    if (Dispatcher.runningqueue.size()==1)
                    {
                      Dispatcher.runningqueue.push_back(SRToutput[0]);
                      for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                        if(Dispatcher.cpus[i].runningprocess==NULL)
                        {
                          Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[1];
                          Temp = 1-i;
                        }
                      Dispatcher.cpus[Temp].runningprocess = &Dispatcher.runningqueue[0];
                    }
                    else
                    {
                      schedular.putinreadyqueue((*(Dispatcher.runningqueue.begin()+tempindex)),Dispatcher);
                      //Dispatcher.runningqueue.erase(Dispatcher.runningqueue.begin()+tempindex);
                      Dispatcher.runningqueue.push_back(SRToutput[0]);

                      Dispatcher.getcpu(Dispatcher.runningqueue[tempindex])->runningprocess = &Dispatcher.runningqueue[1]; 
                      Dispatcher.getcpu(Dispatcher.runningqueue[tempindex])->inCS = true;
                      Dispatcher.getcpu(Dispatcher.runningqueue[tempindex])->CSremaining = schedular.CSperiod;
                      for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                        if(Dispatcher.cpus[i].runningprocess->id == Dispatcher.runningqueue[tempindex].id)
                          Temp = 1-i;
                      Dispatcher.cpus[Temp].runningprocess = &Dispatcher.runningqueue[0];
                    }
                  }
                  for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
                    {
                      if((Dispatcher.getcpu(Dispatcher.runningqueue[i]))!=NULL)
                      {
                        cout<<"CPU is "<<(Dispatcher.getcpu(Dispatcher.runningqueue[i]))->name<<endl;
                        cout<<"ID is "<<(Dispatcher.runningqueue[i].id)<<endl;
                      }
                    }  
                }
                cout<<"I finished SRT"<<endl;
                }
              break;
              //-----------------------------HPF Algorithm-----------------------------------
              case 2:
              {
                


                cout<<"Will start in HPF "<<endl ;
              if (Dispatcher.cpus[0].runningprocess==NULL || Dispatcher.cpus[1].runningprocess==NULL)
              {
                std::vector<process> HPFoutput=schedular.computeHPF(Dispatcher);


                

                if (Dispatcher.runningqueue.size()==0)   //hna mafish context switching hay7sl
                {
                  Dispatcher.runningqueue = HPFoutput;
                  if (HPFoutput.size()==1)
                  {
                    for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                    {
                      if (!Dispatcher.cpus[i].inCS)
                      {
                        Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[0];
                        break;
                      }
                    }
                    if (Dispatcher.cpus[0].inCS && Dispatcher.cpus[1].inCS)
                      Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[0];
                  }
                  else if(HPFoutput.size()==2)
                  {
                    int j=0;
                    for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                    {
                      Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[i];
                    }
                  }
                }
                else           //hna running queue feh wa7ed 3shan el condition ely fe awel el switch 
                {
                  //-----------------------------------This is the filter------------------------------
                  for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
                  {
                    for (int j = 0; j < HPFoutput.size(); ++j)
                    {

                      if(HPFoutput[j].id == Dispatcher.runningqueue[i].id)
                      {
                        HPFoutput.erase(HPFoutput.begin()+j);
                      }
                    }
                  }
                  cout <<"i have finished the filter with size  = "<<HPFoutput.size()<<endl ;
                  //--------The HPFoutput after this has no duplicates with the running queue----------
                  //Since we check that at least a CPU is free before computing the HPF, we are sure that
                  if(HPFoutput.size()!=0)
                  {
                    
                    cout<<"Running CPU IDS:";
            for (int i = 0; i < Dispatcher.cpus.size(); ++i)
              if(Dispatcher.cpus[i].runningprocess!=NULL)
                cout<<Dispatcher.cpus[i].runningprocess->id<< " ";
            cout<<endl;



                  Dispatcher.runningqueue.push_back(HPFoutput[0]);



                  int Temp;
                  for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                  {
                    if(Dispatcher.cpus[i].runningprocess==NULL)
                    {
                      Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[1];
                      Temp = 1-i;
                    }
                  }

                  Dispatcher.cpus[Temp].runningprocess = &Dispatcher.runningqueue[0];

                    cout<<"Running CPU IDS:";
                    for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                      if(Dispatcher.cpus[i].runningprocess!=NULL)
                        cout<<Dispatcher.cpus[i].runningprocess->id<< " ";
                    cout<<endl;

                    cout<<"Running Queue IDS:";
                    for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
                        cout<<Dispatcher.runningqueue[i].id<< " ";
                    cout<<endl;
                  
                }
                }
              }
              }
                break;
                //---------------------------------RR Algorithm--------------------------
              case 3: 
              {
                cout<<"Will start in RR "<<endl ;
              std::vector<process> ready = schedular.checkQuantums(counter, Dispatcher);
              cout<<"Done checking quantums with size "<<ready.size()<<endl ;
              for(int i=0;i<ready.size();i++)
                schedular.putinreadyqueue(ready[i],Dispatcher);

              cout<<"Done placing in readyqueue"<<endl ;

              std::vector<process> RRoutput=schedular.computeRR(Dispatcher);
              cout<<"Done computing RR with output size "<<RRoutput.size()<<endl ;
              if (Dispatcher.runningqueue.size()==0)
              {
                Dispatcher.runningqueue = RRoutput;    //lw fe wa2t turn this to loop on push_back
                if (RRoutput.size()==1)
                {
                  for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                  {
                    if (!Dispatcher.cpus[i].inCS)
                    {
                      Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[0];
                      Dispatcher.cpus[i].currentQuantumStart = counter;
                      break;
                    }
                  }
                  if (Dispatcher.cpus[0].inCS && Dispatcher.cpus[1].inCS)
                    Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[0];
                }
                else if(RRoutput.size()==2)
                {
                  int j=0;
                  for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                  {
                    Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[i];
                    if(!Dispatcher.cpus[i].inCS)
                      Dispatcher.cpus[i].currentQuantumStart = counter;
                  }
                }
              }

              else if (Dispatcher.runningqueue.size()==1)
              {
                if(RRoutput.size()==2)
                {
                  Dispatcher.runningqueue.push_back(RRoutput[1]);

                  int Temp;     //After using push_back, the first element's cpu pointer was lost 
                  for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                  {
                    if(Dispatcher.cpus[i].runningprocess==NULL)
                    {
                      Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[1];
                      if(!Dispatcher.cpus[i].inCS)
                        Dispatcher.cpus[i].currentQuantumStart = counter;
                      Temp = 1-i;
                    }
                  }
                  Dispatcher.cpus[Temp].runningprocess = &Dispatcher.runningqueue[0];
                }
              }
                /*for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
                {
                  for (int j = 0; j < RRoutput.size(); ++j)
                  {
                    if(RRoutput[j].id == Dispatcher.runningqueue[i].id)
                      RRoutput.erase(RRoutput.begin()+j);
                  }
                }
                for (int i = 0; i < RRoutput.size(); ++i)
                {
                  
                  Dispatcher.runningqueue.push_back(RRoutput[i]);
                  
                  for (int j = 0; j < Dispatcher.cpus.size(); ++j)
                    if (Dispatcher.cpus[j].runningprocess==NULL)
                    {
                      *(Dispatcher.cpus[j].runningprocess) = Dispatcher.runningqueue[i+1];
                    }
                }*/
              }
              break;
              //-----------------------------HPRR(Custom) Algorithm----------------------------------------
              case 4:
              {
                cout<<"Will start in HPRR "<<endl ;
              std::vector<process> ready = schedular.checkCustomQuantums(counter,Dispatcher);
              
              for(int i=0;i<ready.size();i++)
                schedular.putinreadyqueue(ready[i],Dispatcher);
              
              std::vector<process> HPRRoutput=schedular.computeHPRR(Dispatcher);

              cout<<"I got "<<HPRRoutput.size()<<" processes and they are:" <<endl ;
                for (int i = 0; i < HPRRoutput.size(); ++i)
                    cout<<HPRRoutput[i].id<<" ";
                  cout<<endl;

              bool cpus[2] = {false,false};
              for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                if(Dispatcher.cpus[i].runningprocess!=NULL)
                  cpus[i] = true;

              //---------------CPU0----------------
              if(Dispatcher.runningqueue.size()==0)
              {
                Dispatcher.runningqueue = HPRRoutput;
                if (HPRRoutput.size()==1)
                {
                  for (int i = 0; i < Dispatcher.cpus.size(); ++i)
                  {
                    if (!Dispatcher.cpus[i].inCS)
                    {
                      Dispatcher.cpus[i].runningprocess = &Dispatcher.runningqueue[0];
                      Dispatcher.cpus[i].currentQuantumStart = counter;
                      break;
                    }
                  }
                  if (Dispatcher.cpus[0].inCS && Dispatcher.cpus[1].inCS)
                    Dispatcher.cpus[0].runningprocess = &Dispatcher.runningqueue[0];
                }
                else if(HPRRoutput.size()==2)
                {
                    Dispatcher.cpus[0].runningprocess = &Dispatcher.runningqueue[0];
                    Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[1];
                    if(!Dispatcher.cpus[1].inCS)
                      Dispatcher.cpus[1].currentQuantumStart = counter;
                }
              }
              else if (Dispatcher.runningqueue.size()>=1)
              {
                int tempindex;
                //------------------------Filter here-------------------------------
                for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
                {
                  for (int j = 0; j < HPRRoutput.size(); ++j)
                  {
                    if(HPRRoutput[j].id == Dispatcher.runningqueue[i].id)
                    {
                      HPRRoutput.erase(HPRRoutput.begin()+j);
                      tempindex = i;
                     }
                  }
                }
                tempindex = 1 - tempindex;    //temp index now points to the index of the element in running queue that is not in the HPRRoutput



                if(HPRRoutput.size()==2)    //after the filter
                {
                  for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
                  {
                    schedular.putinreadyqueue(Dispatcher.runningqueue[i],Dispatcher);
                    //Dispatcher.runningqueue.erase(Dispatcher.runningqueue.begin() + i);
                  }
                  Dispatcher.runningqueue = HPRRoutput;
                  Dispatcher.cpus[0].runningprocess = &Dispatcher.runningqueue[0];
                  Dispatcher.cpus[0].inCS = true;
                  Dispatcher.cpus[0].CSremaining = schedular.CSperiod;

                  Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[1];
                  if(!Dispatcher.cpus[1].inCS)
                    Dispatcher.cpus[1].currentQuantumStart = counter;
                }

                else if(HPRRoutput.size()==1)
                {
                  if (Dispatcher.runningqueue.size()==2)
                  {
                      Dispatcher.cpus[0].runningprocess->isSaving = schedular.CSperiod;
                      schedular.putinreadyqueue(Dispatcher.runningqueue[0],Dispatcher);
                    //Dispatcher.runningqueue.erase(Dispatcher.runningqueue.begin()+tempindex);
                    //if(tempindex==0)
                    //{
                      Dispatcher.runningqueue.insert(Dispatcher.runningqueue.begin(),HPRRoutput[0]); //Push_forward :P
                      Dispatcher.cpus[0].runningprocess = &Dispatcher.runningqueue[0];
                      Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[1];
                      Dispatcher.cpus[0].inCS = true;
                      Dispatcher.cpus[0].CSremaining = schedular.CSperiod;
                    //}
                    /*else if(tempindex==1)
                    {
                      Dispatcher.runningqueue.push_back(HPRRoutput[0]);
                      Dispatcher.cpus[0].runningprocess = &Dispatcher.runningqueue[0];
                      Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[1];
                      if(!Dispatcher.cpus[1].inCS)
                        Dispatcher.cpus[1].currentQuantumStart = counter;
                    }  */
                  }
                  else
                  {
                    if(cpus[0]==true)
                    {
                      Dispatcher.runningqueue.push_back(HPRRoutput[0]); 
                      Dispatcher.cpus[0].runningprocess = &Dispatcher.runningqueue[0];
                      Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[1];
                      if(!Dispatcher.cpus[1].inCS)
                        Dispatcher.cpus[1].currentQuantumStart = counter;
                    }
                    else if(cpus[1]==true)
                    {
                      Dispatcher.runningqueue.insert(Dispatcher.runningqueue.begin(),HPRRoutput[0]); //Push_forward :P
                      Dispatcher.cpus[0].runningprocess = &Dispatcher.runningqueue[0];
                      Dispatcher.cpus[1].runningprocess = &Dispatcher.runningqueue[1];
                    } 
                  }
                }
              }
            }
              break;
            }

            cout<<"Running Remaining Time:";
            for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
              cout<<Dispatcher.runningqueue[i].remainingtime<< " ";
            cout<<endl;

            for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
            {

              (Dispatcher.getcpu(Dispatcher.runningqueue[i]))->runningprocess = &Dispatcher.runningqueue[i];
            }



            cout<<"Running CPU IDS:";
            for (int i = 0; i < Dispatcher.cpus.size(); ++i)
              if(Dispatcher.cpus[i].runningprocess!=NULL)
                cout<<Dispatcher.cpus[i].runningprocess->id<< " ";
            cout<<endl;


            cout<<"Running IDS:";
            for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
              cout<<Dispatcher.runningqueue[i].id<< " ";
            cout<<endl;

            

          
            cout<<"Blocked IDS:";
            for (int i = 0; i < Dispatcher.blockedqueue.size(); ++i)
              cout<<Dispatcher.blockedqueue[i].id<< " ";
            cout<<endl;

            cout<<"Step-1"<<endl;

             //----------Cleaning the ready queue--------------------
            for (int i = 0; i < Dispatcher.runningqueue.size(); ++i)
              Dispatcher.deletefromqueue(schedular.readyqueue,Dispatcher.runningqueue[i].id);

            cout<<"Ready IDS:";
            for (int i = 0; i < schedular.readyqueue.size(); ++i)
              cout<<schedular.readyqueue[i].id<< " ";
            cout<<endl;

            cout<<"Step0"<<endl;


          //------------Update Process Log(here for the first time it loops)---------------
            //Dispatcher.checkblockedforhistory(counter);
            cout<<"Step0.5"<<endl;
            //schedular.checkreadyforhistory(counter,Dispatcher);
            //Dispatcher.checkrunningforhistory(counter, schedular.CSperiod);
            string asd = "nth";
            for (int i = 0; i < Dispatcher.blockedqueue.size(); ++i)
            {
              if(Dispatcher.blockedqueue[i].isSaving==0)
                Dispatcher.blockedqueue[i].addtohistory(Dispatcher.blockedqueue[i].ip,counter,asd);
              else
                Dispatcher.blockedqueue[i].isSaving--;
            }
            cout<<"Step1"<<endl;

            //-----------------Increment processes in Blocked Queue----------------------------
            Dispatcher.decblockedprocesstime(mapp);

            cout<<"Step2"<<endl;
          //--------------Loop to decrement CS time and inc running processes in CPUS (if it is not switching)---------------
          for (int i = 0; i < Dispatcher.cpus.size(); ++i)
          {
            if(Dispatcher.cpus[i].inCS)
            {
              cout<<"No Incrementation"<<endl;
              Dispatcher.cpus[i].CSremaining--;
              if (Dispatcher.cpus[i].CSremaining==0)
              {

                Dispatcher.cpus[i].inCS=false;
                if(Dispatcher.cpus[i].runningprocess!=NULL)
                {
                  Dispatcher.cpus[i].currentQuantumStart = counter+1;
                  Dispatcher.cpus[i].runningprocess->addtohistory(Dispatcher.cpus[i].runningprocess->ip, counter+1, Dispatcher.cpus[i].name);
                }
              }
            }
            else if(Dispatcher.cpus[i].runningprocess!=NULL)
            { 
            
              cout<<"Incrementation"<<endl;
             // Dispatcher.cpus[i].runningprocess->addtohistory(Dispatcher.cpus[i].runningprocess->ip,counter,Dispatcher.cpus[i].name);
              Dispatcher.cpus[i].runningprocess->inc();
              
             //##
              if ( Dispatcher.cpus[i].runningprocess->isNew)
              {
                Dispatcher.cpus[i].runningprocess->isNew = false ;
                char a [10] ;
                char b[10] ;
                char c [10];
                char d [10]  ;
                char e [10]  ;
                cout <<"i am going to fork the child"<<endl;
               
                
           //cout <<"pid forked =" <<pid4 ;

           sprintf( a, " %d", counter); 
            sprintf( b, " %d", Dispatcher.cpus[i].runningprocess->id);
            cout <<"id of the proces to be execved is "<<Dispatcher.cpus[i].runningprocess->id<<endl;
            sprintf( c, " %d", Dispatcher.cpus[i].runningprocess->totaltime); 
             cout <<"totaltime of the proces to be execved is "<<Dispatcher.cpus[i].runningprocess->totaltime<<endl;
            sprintf( d, " %d", Dispatcher.cpus[i].runningprocess->priority); 
             cout <<"priority of the proces to be execved is "<<Dispatcher.cpus[i].runningprocess->priority<<endl;
            sprintf( e, " %d", Dispatcher.cpus[i].runningprocess->arrivaltime); 
             cout <<"arrivaltime of the proces to be execved is "<<Dispatcher.cpus[i].runningprocess->arrivaltime<<endl;

        
        cout <<"before exceve !!!"<<endl ;
        char *argv[]={"process",a,b,c,d,e,NULL};
           pid_t pid;
           pid =fork();
                if(pid==0)
                {
                    execve(argv[0],&argv[0],NULL);
         // exit(getpid()) 
                }
                mapp[Dispatcher.cpus[i].runningprocess->id] = pid ;
                sleep(1);
              }
             else
              {
            
                if(!(Dispatcher.cpus[i].inCS))
                {
                  Dispatcher.sendcontinue(mapp ,Dispatcher.cpus[i].runningprocess->id);
               
                   sleep(1);
             }
              }
            //##
            }
          }


         cout<<"Step3"<<endl;
          //----------Increase Waiting time in Ready--------------
          for (int i = 0; i < schedular.readyqueue.size(); ++i)
           { 
            if(schedular.readyqueue[i].isSaving==0)
              schedular.readyqueue[i].waitingtime++;
            else
              schedular.readyqueue[i].isSaving--;
                }
            cout<<"Step4"<<endl;
          //----Check for dead files to print----------
          //Dispatcher.checkblockedforhistory(counter);
          //schedular.checkreadyforhistory(counter,Dispatcher);
          //Dispatcher.checkrunningforhistory(counter, schedular.CSperiod);
          cout<<"Step5"<<endl;
          //Dispatcher.cleanUp();
          cout <<"step6" <<endl ;
          if( Dispatcher.runningqueue.size()==0 && schedular.readyqueue.size()==0 && !generatoralive)
            break;

             
         
         /* for(int i =0 ; i < Dispatcher.runningqueue.size();i++)
            {
              if ()
              {
                
                
              }
            }*/
            for (int i = 0; i < Dispatcher.cpus.size(); ++i)
            {
              if(Dispatcher.cpus[i].runningprocess!=NULL && !Dispatcher.cpus[i].inCS && !Dispatcher.cpus[i].runningprocess->isNew)
              {
                Dispatcher.cpus[i].Record(counter,Dispatcher.cpus[i].runningprocess->id);
                kill(mapp[Dispatcher.cpus[i].runningprocess->id],SIGUSR2);
                sleep(1);
              }
              
            }

            for(int i =0 ; i < Dispatcher.blockedqueue.size();i++)
            {
              if (!Dispatcher.blockedqueue[i].isNew)
              {
                kill(mapp[Dispatcher.blockedqueue[i].id],SIGUSR1);
                sleep(1);
              }
            }

              for(int i =0 ; i < schedular.readyqueue.size();i++)
            {
              if (!schedular.readyqueue[i].isNew)
              {
                cout <<"i going to send signal in a ready queue" ;
                kill(mapp[schedular.readyqueue[i].id],SIGUSR1);
              sleep(1);
              }
            }

          counter ++ ;



        	 }
       cout <<"i am sched and i finished"<<endl ;
       for (int i = 0; i < Dispatcher.cpus.size(); ++i)
         Dispatcher.cpus[i].printfile(counter);

        Dispatcher.printMetrics();
       msgctl(msgqid, IPC_RMID, (struct msqid_ds *) 0);
        msgctl(msgqid2, IPC_RMID, (struct msqid_ds *) 0);
         msgctl(msgqid3, IPC_RMID, (struct msqid_ds *) 0);

  return 0;      // }
}