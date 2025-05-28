#ifndef THREADTABLE_H
#define THREADTABLE_H

class Thread;

#define MAX_THREADS 128

class ThreadTable {
public:
    ThreadTable();
    ~ThreadTable();

    int addThread(Thread* thread);      
    Thread* getThread(int tid);        
    void removeThread(int tid);         
    bool exists(int tid);               

private:
    Thread* table[MAX_THREADS];
};

#endif
