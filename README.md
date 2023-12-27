# nginxLearning
nginx学习
# 父子进程

~~~c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
~~~

函数作用：用于创建子进程

返回值：

​	fork的返回值会返回两次，一次是在父进程中，一次实在子进程中。在父进程中返回创建的子进程ID，在子进程中返回0。可以通过fork函数的返回值将父子进程区分开来。

在父进程中返回-1表示创建子进程失败并且设置全局的errno.

读时共享，写时复制.

父子进程关系：

区别：1.fork函数的返回值不同，父进程中：>0返回子进程的ID，子进程中：=0

  			2.pcb中一些数据不同：当前进程的id，pid,当前进程的父进程id,ppid，信号集

共同点：

某些状态下：子进程刚被创建出来，还没有执行任何的写数据操作，父子进程共享

- 用户区的数据
- 文件描述符表

父子进程对变量是

不是共享的？

- 刚创建时，是共享的，如果修改了数据，就不共享了。
- 读时共享（子进程被创建，两个进程没有做任何的写的操作），写时拷贝

## 如何避免僵尸进程

僵尸进程指的是子进程结束，父进程没有回收子进程的资源，父进程需要调用wait和waitpid函数回收子进程的资源

wait函数

~~~C
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *wstatus);
~~~

功能：等到任意一个子进程结束，如果任意一个子进程结束了，该函数会回收子进程的资源

参数：进程退出时的状态信息，传入的是一个int类型的地址，传出参数

返回值：

- 成功：返回被回收的子进程id
- 失败：-1，所有的子进程都结束，调用函数失败

调用wait函数的进程会被挂机（阻塞），直到它的一个子进程退出或者收到一个不能被忽略的信号时，如果没有子进程了，函数立刻返回-1，如果子进程都已经结束了，也会立即返回-1，

waitpid函数

~~~C
#include <sys/types.h>
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *wstatus, int options);
~~~

功能：回收指定进程号的子进程，可以设置是否阻塞

参数：

- pid:

  pid>0：某个子进程的pid

  pid = 0:回收当前进程组的所有子进程

  pid = -1：回收所有的子进程，相当于wait

  pid < -1:某个进程组的组id的绝对值，回收指定进程组中的子进程

- options:设置阻塞或者非阻塞

  0：阻塞

  WNOHANG:非阻塞

- 返回值

  大于0：返回子进程的id

  等于0 =0：options = WNOHANG,表示还有子进程

  等于-1：错误或者没有子进程了

  **无论是Wait函数还是waitpid函数一次调用也只能回收一个子进程，需要配合无限循环，根据返回值判断是否结束。**

  ~~~c
  while(1){
      printf("parent, pid = %d\n",getpid());
      sleep(3);
      int st;
      int ret = waitpid(-1,&st,WNOHANG);//设置非阻塞
  	if(ret > 0){
          //输出信息继续回收
          if(WIFEXITED(st)){
              //是不是正常退出
              printf("退出的状态码：%d\n",WEXITSTATUS(st));
          }
          if(WIFSIGNALED(st)){
              //是不是异常终止
              printf("被哪个信号终止:%d\n",WTERMSIG(st));
          }
          printf("child die, pid = %d\n",ret);
      }else{
          //暂时没有子进程终止循环
          break;
      }
  }
  ~~~

  
