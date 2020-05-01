#include "mmu.h"
#include <sstream>

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

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
    std::vector<Process*> vec = getProcessesVector();
    for( int i = 0; i < vec.size(); i++){
       Process* p = vec[i];
       if( p->pid == pid ){
           return p; 
       }
   }
}

Process* Mmu::getFirstProcess() 
{
    return _processes.front();
}

std::vector<Process*> Mmu::getProcessesVector()
{
    return _processes;
}


uint32_t Mmu::createNewProcess(uint32_t textSize, uint32_t dataSize, PageTable *pageTable)
{
    int p, v;
    int pageNumber;
	bool found = false;
    Variable *var = new Variable();
	uint32_t process = createProcess();

    for( p = 0; p < getProcessesVector().size(); p++ )
    {
        for( v = 0; v < getProcessesVector().at(p)->variables.size(); v++ )
        {
            if( getProcessesVector().at(p)->variables.at(v)->name.compare("<FREE_SPACE>") == 0 )
            {
                var = getProcessesVector().at(p)->variables.at(v);
                found = true;
                break;
            }
        }
    }

    //testing
    /*
    std::cout << "Before!" << std::endl;
    std::cout << getProcessesVector().size() << std::endl;
    std::cout << "After!" << std::endl;*/

    if( found )
	{
		var->name = "<TEXT>";
		var->virtual_address = getFirstProcess()->process_virtual_address;
		var->size = textSize;

		getFirstProcess()->process_virtual_address = getFirstProcess()->process_virtual_address + textSize;

		Variable *globals = new Variable();
		globals->name = "<GLOBALS>";
    	globals->virtual_address = getFirstProcess()->process_virtual_address;
    	globals->size = dataSize;
		getFirstProcess()->variables.push_back(globals);

		getFirstProcess()->process_virtual_address = getFirstProcess()->process_virtual_address + globals->size;


		Variable *stack = new Variable();
		stack->name = "<STACK>";
    	stack->virtual_address = getFirstProcess()->process_virtual_address;
    	stack->size = 65536;
		getFirstProcess()->variables.push_back(stack);

		getFirstProcess()->process_virtual_address = getFirstProcess()->process_virtual_address + stack->size;

    	Variable *freeSpace = new Variable();
    	freeSpace->name = "<FREE_SPACE>";
    	freeSpace->virtual_address = 0;
    	freeSpace->size = _max_size - getFirstProcess()->process_virtual_address;
		getFirstProcess()->variables.push_back(freeSpace);

		//Create page table ***
	}

	return process;
}

void Mmu::allocate( int pid, std::string name, std::string type, uint32_t quantity){

    //allocate space needed for any specificed variables. 
    Variable *var = new Variable();

    int address = getProcess(pid)->process_virtual_address;
        
    if( type.compare("int") == 0 ){
        var->name = name;
        var->virtual_address = address; 
        var->size = 4 * quantity;
        getProcess(pid)->process_virtual_address = address + var->size; 
    }
    else if( type.compare("short") == 0 ){
        var->name = name;
        var->virtual_address = address; 
        var->size = 2 * quantity;
        getProcess(pid)->process_virtual_address = address + var->size; 
    }
    else if( type.compare( "char" ) == 0 ){
        var->name = name;
        var->virtual_address = address; 
        var->size = 2;
        getProcess(pid)->process_virtual_address = address + var->size; 
    }
    else if( type.compare("long" ) == 0  || type.compare("double") == 0 ){
        var->name = name;
        var->virtual_address = address; 
        var->size = 8 * quantity;
        getProcess(pid)->process_virtual_address = address + var->size; 
    }
	getProcess(pid)->variables.push_back(var);
}


void Mmu::set( ){

}


void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;

    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            if( _processes[i]->variables[j]->name.compare("<FREE_SPACE>") == 0 )
		    {
			    break;
		    }

	        std::cout << _processes[i]->pid << std::string( 2, ' ') << "| " << _processes[i]->variables[j]->name << std::string( 14 - _processes[i]->variables[j]->name.length(), ' ') << "| " << _processes[i]->variables[j]->virtual_address << std::string( 13 - std::to_string(_processes[i]->variables[j]->virtual_address).length(), ' ') << "| " << std::string(11 - std::to_string(_processes[i]->variables[j]->size).length(), ' ') << _processes[i]->variables[j]->size << std:: endl;

        }

    }
}
