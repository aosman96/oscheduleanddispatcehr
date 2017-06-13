union semun 
{
    int val;                  /* value for SETVAL */
    struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
    ushort *array;            /* array for GETALL & SETALL */
    struct seminfo *__buf;    /* buffer for IPC_INFO */
    void *__pad;
};

void p(int sem)     /* equivalent to down() */
{
  struct sembuf p_op;
  
  p_op.sem_num = 0;
  p_op.sem_op = -1;
  p_op.sem_flg = !IPC_NOWAIT;
  
  if(semop(sem, &p_op, 1) == -1)
      { 
    perror("Error in p()");
    exit(-1);
      }
}

void v(int sem)   /* equivalent to up() */
{
  struct sembuf v_op;
  
  v_op.sem_num = 0;
  v_op.sem_op = 1;
  v_op.sem_flg = !IPC_NOWAIT;
  
  if(semop(sem, &v_op, 1) == -1)
      { 
    perror("Error in v()");
    exit(-1);
      }

}

int  createsharedmemory(int & shmid , int & semid)
{


  union semun Semun;
 
  
  shmid = shmget(392, 6000, IPC_CREAT|0777);
  
  if((long)shmid == -1)
    {
      perror("Error in create shm");
      return -1;
    }
  else
    printf("\nShared memory ID = %d\n", shmid);
      
  semid = semget(40, 1, 0666|IPC_CREAT);
  
  if(semid == -1)
    {
      perror("Error in create sem");
    return -1;
    }
      
  Semun.val = 1;    /* initial value of the semaphore, Binary semaphore */
  if(semctl(semid, 0, SETVAL, Semun) == -1)
    {
      perror("Error in semctl");
     
    }

}