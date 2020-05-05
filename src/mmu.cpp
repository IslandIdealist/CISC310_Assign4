#include "mmu.h"
#include <sstream>
#include <math.h>

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
    _index = 0;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;
    proc->page = 0;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

Process* Mmu::getProcess( uint32_t pid )
{
    Process* p; 
    std::vector<Process*> vec = getProcessesVector();

    for( int i = 0; i < vec.size(); i++ )
    {
       p = vec[i];

       if( p->pid == pid )
       {
           return p; 
       }
    }

    return NULL; 
}

Process* Mmu::getFirstProcess() 
{
    return _processes.front();
}

Process* Mmu::getNextProcess()
{ 
    return _processes[_index];
    _index++;
}

std::vector<Process*> Mmu::getProcessesVector()
{
    return _processes;
}

uint32_t Mmu::createNewProcess(uint32_t textSize, uint32_t dataSize, PageTable *pageTable)
{

    int p, v;
    int pageNumber;
    int max = 65536;
	bool found = false;
    Variable *text = new Variable();
    Variable *globals = new Variable();
    Variable *stack = new Variable();
    Variable *freeSpace = new Variable();
	uint32_t process = createProcess();

    for( p = 0; p < getProcessesVector().size(); p++ )
    {
        for( v = 0; v < getProcessesVector().at(p)->variables.size(); v++ )
        {
            if( getProcessesVector().at(p)->variables.at(v)->name.compare("<FREE_SPACE>") == 0 )
            {
                text = getProcessesVector().at(p)->variables.at(v);
                found = true;
                break;
            }
        }
    }

    Process *proc = getProcess(process);

    if( found )
	{
        text->name = "<TEXT>";
        text->size = textSize;

        globals->name = "<GLOBALS>";
        globals->virtual_address = text->size;
        globals->size = dataSize;
        proc->variables.push_back(globals);

        stack->name = "<STACK>";
        stack->virtual_address = globals->size + text->size;
        stack->size = 65536;
        proc->variables.push_back(stack);

        pageNumber = (globals->size + text->size + stack->size) / pageTable->pageSize();
        for( int i = 0; i < pageNumber; i++ ) 
        {
            pageTable->addEntry(proc->pid, i);
            proc->page++;
        }

        _max_size = _max_size - (globals->size + text->size + stack->size);

        freeSpace->name = "<FREE_SPACE>";
        freeSpace->virtual_address = globals->size + text->size + stack->size;
        freeSpace->size = _max_size - (globals->size + text->size + stack->size);
        proc->variables.push_back(freeSpace);
        
    }

	return process;
}

int Mmu::allocate( int pid, std::string name, std::string type, uint32_t quantity)
{
    //allocate space needed for any specificed variables.
    Variable *var = new Variable();
    Process *proc = getProcess(pid);
    int index = 0;

    for(int i = 0; i < proc->variables.size(); i++)
    {
        if(proc->variables.at(i)->name == "<FREE_SPACE>")
        {
            index = i;
            break;
        }
    }
    int address = proc->variables.at(index)->virtual_address;
    var->name = name;
    var->virtual_address = address; 
        
    if( type.compare("int") == 0 )
    {
        var->size = 4 * quantity;
        var->type = std::string("int"); 
    }
    else if( type.compare("float") == 0 )
    {
        var->size = 4 * quantity;  
        var->type = std::string("float"); 
    }
    else if( type.compare("short") == 0 )
    {
        var->size = 2 * quantity;
        var->type = std::string("short"); 
    }
    else if( type.compare( "char" ) == 0 )
    {
        var->size = 1 * quantity;
        var->type = "char"; 
    }
    else if( type.compare("long" ) == 0  )
    {
        var->size = 8 * quantity;
        var->type = "long"; 
    }
    else if( type.compare("double") == 0 )
    {
        var->size = 8 * quantity;
        var->type = "double"; 
    }

	proc->variables.push_back(var);
    proc->variables.at(index)->virtual_address += var->size;
    return address;
}


int Mmu::terminate(uint32_t pid, PageTable *pageTable)
{
        int index = 0;
	    int pageNumber = 0;
        std::string name;
        Process* p = getProcess( pid ); 

	    for(int i = 0; i < p->variables.size(); i++)
	    {
            std::string name = p->variables[i]->name;  
            free( pid, name );
            pageTable->remove( pid, pageNumber );
	    }

	    getProcessesVector().erase(getProcessesVector().begin() + index);
	    return 0;
}

void Mmu::free(int pid, std::string name)
{
    //find the variable to free. 
    Process* p = getProcess( pid ); 
    std::vector<Variable*> vars = p->variables; 
    Variable* prev; 
    for( int i = 0; i < vars.size(); i++){
       if( vars[i]->name.compare(name) == 0){
           prev = vars[i]; 
       }
    }
    
    Variable* free = new Variable(); 
    free->name = "<FREE_SPACE>";
    free->virtual_address = prev->virtual_address;
    free->size = prev->size;
    combineFrees( pid );
}



/*
 * Combines any free space variables that are next
 * to each other within a process. 
 */
void Mmu::combineFrees(int pid )
{

    Process* p = getProcess( pid ); 
    std::vector<Variable*> vars = p->variables; 
    for( int i = 0; i < vars.size(); i++){
       if( vars[i]->name.compare("<FREE_SPACE>") == 0){
          if( vars[i+1]->name.compare("<FREE_SPACE>") == 0 ){
              
              Variable* prev = vars[i]; 
              Variable* curr = vars[i+1]; 

              Variable* v = new Variable(); 
              v->name = "<FREE_SPACE>";
              v->virtual_address = prev->virtual_address;
              v->size = prev->size + curr->size ;

              //erase the curr variable
              p->variables.erase( vars.begin() + i + 1 );
              //set new free variable. 
              p->variables[i] = v; 

              //recurse until all combinable frees are combined. 
              return combineFrees( pid );
          }
          else{
              //increment extra 1. 
              i++; 
              continue; 
          }
       }
    }
}

Variable* Mmu::getVariable(int pid,  std::string name){

    Process* p = getProcess(pid); 
    for(int j = 0; j < p->variables.size(); j++)
    {
        std::string name = p->variables[j]->name;  
        if( name.compare(name) == 0 ){
            return p->variables[j]; 
        }
    }
    return NULL; 
}


void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;

    for (i = 0; i < getProcessesVector().size(); i++)
    {
        for (j = 0; j < getProcessesVector()[i]->variables.size(); j++)
        {
            if( getProcessesVector()[i]->variables[j]->name != "<FREE_SPACE>" )
            {   
                std::cout << ' ' << getProcessesVector()[i]->pid << " |";
                std::cout << ' ' << getProcessesVector()[i]->variables[j]->name;

                for( int k = getProcessesVector()[i]->variables[j]->name.length(); k<14; k++ )
                {
                    std::cout << ' ';
                }

                printf( "|   0x%08X |", getProcessesVector()[i]->variables[j]->virtual_address );

                for( int k = std::to_string( getProcessesVector()[i]->variables[j]->size ).length(); k < 11; k++ )
                {
                    std::cout << ' ';
                }

                std::cout << getProcessesVector()[i]->variables[j]->size * 8 << std::endl;
            }
        }
    }
}
