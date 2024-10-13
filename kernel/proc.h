// Saved registers for kernel context switches.
//该结构体用于在上下文切换时保存一些寄存器的状态
struct context {
  uint64 ra;//return address register,存储函数调用的返回地址
  uint64 sp;//stack pointer,指向当前进程的栈顶

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.指向CPU正在运行的进程，如果CPU空闲，则指向NULL
  struct context context;     // swtch() here to enter scheduler().保存CPU的上下文，即寄存器的状态，需要时可使用swtch进行上下文切换
  int noff;                   // Depth of push_off() nesting.保存push_off函数的嵌套深度
  int intena;                 // Were interrupts enabled before push_off()?检查在调用push_off之前中断是否启用
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.
//上面的注释解释了trampoline.S内的trap处理机制
//下面的结构体用于在处理异常时保存状态
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;//自旋锁，保护进程数据并发访问

  // p->lock must be held when using these:
  enum procstate state;        // Process state进程的状态，例如就绪，运行等
  void *chan;                  // If non-zero, sleeping on chan如果非零，进程在该地址上休眠
  int killed;                  // If non-zero, have been killed如果非零，进程已经被终止
  int xstate;                  // Exit status to be returned to parent's wait进程的退出状态位，会在父进程的wait中返回
  int pid;                     // Process ID进程ID

  // wait_lock must be held when using this: 使用wait_lock时必须有以下成员
  struct proc *parent;         // Parent process父进程的指针

  // these are private to the process, so p->lock need not be held.
  //这些是进程的私有成员，不需要持有lock
  uint64 kstack;               // Virtual address of kernel stack内核栈的虚拟地址
  uint64 sz;                   // Size of process memory (bytes)进程内存的大小，单位为字节
  pagetable_t pagetable;       // User page table用户页表，用于管理进程的虚拟内存
  struct trapframe *trapframe; // data page for trampoline.S保存进程的trapframe,用于上下文切换和异常处理
  struct context context;      // swtch() here to run process保存进程的上下文，用于上下文切换
  struct file *ofile[NOFILE];  // Open files存储该进程打开的文件
  struct inode *cwd;           // Current directory当前工作目录的指针，表示进程的当前工作目录
  char name[16];               // Process name (debugging)进程名称，调试用的
};
