
/*
With this program, the user gives inputs A and B, A being total time and B being the time interval.
The program then prints "Hello!" every B seconds for A seconds.
The program also counts the number of characters that are printed for each minute it runs.
*/

//This is code used from typist.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_LINE_SIZE 128
#define MAX_BUF_SIZE 1280
char buf[MAX_BUF_SIZE];

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

//create a new process
int
fork1(void)
{
  int pid;
  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

//create a pipe
void 
pipe1(int fd[2])
{
 int rc = pipe(fd);
 if(rc<0){
   panic("Fail to create a pipe.");
 }
}


//pull everything from pipe and return the size of the content
int
read1(int *fd)
{
 char ch='a';
 write(fd[1],&ch,1); //write something to the pipe so the next read will not block (because read operation is a blocking operation - if the pipe is empty, read will wait till something is put in)
 int len = read(fd[0],buf,MAX_BUF_SIZE)-1;
 return len;
}

int
main(int argc, char *argv[])
{
  //Initialize variables
  int A = 0;
  int B = 0;
  int elapsed_time = 0;

  //Checks if the user included 2 inputs
  if(argc < 3) {
      printf("Not enough inputs.\n");
      return 1;
  }
  //Checks if A is a multiple of B
  A = atoi(argv[1]);
  B = atoi(argv[2]);
  if(A % B != 0) {
      printf("A is not a multiple of B.\n");
      return 1;
  }
  //Prints A and B
  printf("A: %d, B: %d\n", A, B);

  //Code used from typist.c
  //create two pipe to share with child
  int fd1[2], fd2[2];
  pipe1(fd1); //a pipe from child to parent - child sends entered texts to parent for counting
  pipe1(fd2); //a pipe from child to parent - child lets parent stop when it has run for A seconds

  int result = fork1();//create child process
  if(result==0){
     //child process:
     close(fd1[0]);
     close(fd2[0]);
     //Run loop while the elapased time is less than A
     while(elapsed_time < A){
      char* buf = "Hello!"; //Set buffer to "Hello!"
      write(fd1[1],buf,strlen(buf)); 
      elapsed_time += B; //Increase elapsed time by B to show B seconds have passed
      sleep(B); //To have the program type "Hello!" every B seconds, sleep for B seconds
	}

  //Once loop is finished, exit child
	char ch='a';
	write(fd2[1],&ch,1);
	exit(0);    
  }
  //Code used from typist.c
  else{
     //parent process:
     while(1){
       sleep(60);

       int len=read1(fd1); 	
       printf("\nIn last minute, %d characters were entered.\n", len);

       len=read1(fd2);
       if(len>0){
	 //now to terminate
	 wait(0);
	 exit(0);
       }
     }

  }    
}
