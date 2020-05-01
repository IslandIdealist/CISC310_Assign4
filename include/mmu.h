#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include "pagetable.h"

typedef struct Variable {
    std::string name;
    int virtual_address;
    int size;
} Variable;

typedef struct Process {
    uint32_t pid;
    int process_virtual_address;
    
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    int _max_size;
    std::vector<Process*> _processes;
    void combineFrees(int pid);

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    Process* getProcess(uint32_t pid);
    Process* getFirstProcess();
    std::vector<Process*> getProcessesVector();
    uint32_t createNewProcess(uint32_t textSize, uint32_t dataSize, PageTable *pageTable);
    void allocate( int pid, std::string name, std::string type, uint32_t quantity );   
    void print();
    void set( int pid, std::string name, std::vector<std::string>* args ){
    void free(int pid, std::string name); 

};



#endif // __MMU_H_
