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

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    Process* getProcess();
    std::vector<Process*> getProcessesVector();
    uint32_t createNewProcess(uint32_t textSize, uint32_t dataSize, PageTable *pageTable);
    void print();
};

#endif // __MMU_H_
