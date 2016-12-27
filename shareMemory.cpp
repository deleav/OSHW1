#include <sys/types.h>  /*  These are the necessary       */
                        /*  files for using shared memory */
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>


#define SHM_MODE (SHM_W | SHM_R |  IPC_CREAT)  /* Set mode bits    */
                                               /* on shared memory */


/*  This is an example of shared memory and fork */
int main()
{
	int memid, pid;
	int *ptr;

/*  This allocates the shared memory.  The IPC_PRIVATE tells the
    system to go and get the next available memory queue.
    The SHM_MODE sets this memory queue so that the owner
    can read and write to it, and the IPC_CREAT specifies that the
    system can create the   queue if it does not exist */

	if ((memid = shmget(IPC_PRIVATE,sizeof(int),SHM_MODE)) == -1)              {
        	printf("Unsuccessful memory get \n");
		printf("Errno was %d \n",errno);
		exit(-1);
            }

	if ((pid = fork()) != 0)
          { /* This is the parent process */

	    ptr = (int *) shmat(memid,0 ,0); /* This attaches the
                                           shared memory segment
	                                     to this process  */
            *ptr = 10;
	    printf("Parent: I set the value to %d \n",*ptr);
	    sleep(10);  /*  This puts this process to sleep for
                            10 seconds */

	    printf("Parent: The value now is %d \n",*ptr);
	   }
	else
           {/* This is the child */
	    ptr = (int *)  shmat(memid,0 ,0);
	    sleep(5);
      	    printf
              ("Child:  The value I got from the shared memory  is %d.\n",*ptr);
	    *ptr = 5;
	    printf("Child: I changed the value to %d.\n",*ptr);

	    shmdt((void *)ptr);  /* This detaches the process from the
                            shared memory segment */

	    }



/*  We now have to destroy the shared memory queue.  This does not
    automatically disappear after the process completes.
    There's a system imposed limit of 100 shared
    memory queue, so be kind, and cleap up after youself.  */

	if (pid !=0)
	   if (shmctl(memid,IPC_RMID,0) < 0)
            {  /* This destroys the memory queue */
		printf("Problems destroying shared memory ID \n");
		printf("Errno was %d \n", errno);
		exit(-1);
	    }

}
