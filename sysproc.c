#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "stdbool.h"

// Assignment
extern int syscall_count[29];
extern bool trace_on;
extern int total_syscall;
extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;  // Declare the ptable variable

int sum = 0;

// extern int send(int, int, void *);
// extern int recv(void *);
// extern int send_multi(int, int, void *);

#define MSGSIZE 8

struct buffer_struct {
    char buffer[MSGSIZE];
    int bufferstatus;
    int sender_id;
    struct spinlock bufferlock;
};

struct buffer_struct buffers[65];
struct spinlock sleeplock;

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Assignment

// sys_print_count()
int 
sys_print_count(void)
  {
  if(syscall_count[24] > 0){
    cprintf("sys_add %d\n",syscall_count[24]);
  }
  if(syscall_count[9] > 0){
    cprintf("sys_chdir %d\n",syscall_count[9]);
  }
  if(syscall_count[21] > 0){
    cprintf("sys_close %d\n",syscall_count[21]);
  }
  if(syscall_count[10] > 0){
    cprintf("sys_dup %d\n",syscall_count[10]);
  }
  if(syscall_count[7] > 0){
    cprintf("sys_exec %d\n",syscall_count[7]);
  }
  if(syscall_count[2] > 0){
    cprintf("sys_exit %d\n",syscall_count[2]);
  }
  if(syscall_count[1] > 0){
    cprintf("sys_fork %d\n",syscall_count[1]);
  }
  if(syscall_count[8] > 0){
    cprintf("sys_fstat %d\n",syscall_count[8]);
  }
  if(syscall_count[11] > 0){
    cprintf("sys_getpid %d\n",syscall_count[11]);
  }
  if(syscall_count[6] > 0){
    cprintf("sys_kill %d\n",syscall_count[6]);
  }
  if(syscall_count[19] > 0){
    cprintf("sys_link %d\n",syscall_count[19]);
  }
  if(syscall_count[20] > 0){
    cprintf("sys_mkdir %d\n",syscall_count[20]);
  }
  if(syscall_count[17] > 0){
    cprintf("sys_mknod %d\n",syscall_count[17]);
  }
  if(syscall_count[15] > 0){
    cprintf("sys_open %d\n",syscall_count[15]);
  }
  if(syscall_count[4] > 0){
    cprintf("sys_pipe %d\n",syscall_count[4]);
  }
  if(syscall_count[25] > 0){
    cprintf("sys_ps %d\n",syscall_count[25]);
  }
  if(syscall_count[5] > 0){
    cprintf("sys_read %d\n",syscall_count[5]);
  }
  if(syscall_count[27] > 0){
    cprintf("sys_recv %d\n",syscall_count[27]);
  }
  if(syscall_count[12] > 0){
    cprintf("sys_sbrk %d\n",syscall_count[12]);
  }
  if(syscall_count[26] > 0){
    cprintf("sys_send %d\n",syscall_count[26]);
  }
  if(syscall_count[28] > 0){
    cprintf("sys_send_multi %d\n",syscall_count[28]);
  }
  if(syscall_count[13] > 0){
    cprintf("sys_sleep %d\n",syscall_count[13]);
  }
  if(syscall_count[23] > 0){
    cprintf("sys_toggle %d\n",syscall_count[23]);
  }
  if(syscall_count[18] > 0){
    cprintf("sys_unlink %d\n",syscall_count[18]);
  }
  if(syscall_count[14] > 0){
    cprintf("sys_uptime %d\n",syscall_count[14]);
  }
  if(syscall_count[3] > 0){
    cprintf("sys_wait %d\n",syscall_count[3]);
  }
  if(syscall_count[16] > 0){
    cprintf("sys_write %d\n",syscall_count[16]);
  }
  return 0;
}

// sys_toggle()
int 
sys_toggle(void)
{
  if(trace_on == true){
    trace_on = false;
    for(int i=0;i<total_syscall;i++){
    syscall_count[i] = 0; // resetting all syscall_count to 0's
    }
  }
  else{
    trace_on = true;
  }
  return 0;
}

// sys_add()
int 
sys_add(void) {
  int num1, num2;
  if (argint(0, &num1) < 0 || argint(1, &num2) < 0) {
      return -1;  // Error in reading arguments
  }
  sum = num1 + num2;
  return sum;
}

// sys_ps()
struct proc *p;
int sys_ps(void) {
    acquire(&ptable.lock);

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state != UNUSED) {
            cprintf("pid:%d name:%s\n", p->pid, p->name);
        }
    }

    release(&ptable.lock);
    return 0;
}

// sys_send()
int
send(int s_id, int r_id, char *message) {
    acquire(&buffers[r_id].bufferlock);
    if (buffers[r_id].bufferstatus == 1) {
        release(&buffers[r_id].bufferlock);
        return 1; // Buffer is full, return error
    }
    memmove(buffers[r_id].buffer, message, MSGSIZE); // Copy message using memmove
    buffers[r_id].sender_id = s_id;
    buffers[r_id].bufferstatus = 1;
    wakeup((void *)&buffers[r_id]);
    release(&buffers[r_id].bufferlock);
    return 0;
}

int send_id, recv_id;
char *message;
int
sys_send(void){	
	if(argint(0, &send_id) < 0 || argint(1, &recv_id) < 0 || argptr(2, &message, sizeof(char*)) < 0)
		return -1;
	return send(send_id, recv_id, message);
}

// sys_recv()
int
recv(char *message) {
    acquire(&buffers[myproc()->pid].bufferlock);
    if (buffers[myproc()->pid].bufferstatus == 0) {
        release(&buffers[myproc()->pid].bufferlock);
        acquire(&sleeplock);
        sleep((void *)&buffers[myproc()->pid], &sleeplock);
        release(&sleeplock);
        acquire(&buffers[myproc()->pid].bufferlock);
    }
    memmove(message, buffers[myproc()->pid].buffer, MSGSIZE); // Copy message using memmove
    buffers[myproc()->pid].bufferstatus = 0;
    release(&buffers[myproc()->pid].bufferlock);
    return 0;
}

char *message;
int
sys_recv(void){	
	if(argptr(0, &message, sizeof(char*)) < 0)
		return -1;
	//cprintf("helo");
	return recv(message);
}

// sys_send_multi()
int s_id;
int *r_id;
char *message;
int
sys_send_multi(void){

	if(argint(0, &s_id) < 0 || argptr(1, (void *)&r_id, sizeof(int *)) < 0 || argstr(2, &message) < 0)
		return -1;

	int length = 8; //no of recv processes
	
	r_id = (int *)r_id;
	for(int i=0; i<length; i++){
		send(s_id, (int)(r_id[i]), message);
	}
	
	return 0;
}
	
