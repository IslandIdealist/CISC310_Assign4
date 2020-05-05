#include <iostream>
#include <sstream>
#include <string>
#include "mmu.h"
#include "pagetable.h"
#include <math.h>
#include <iterator>
#include <stdio.h> 


void printStartMessage(int page_size);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter

    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }
    else if ( (std::stoi(argv[1]) >= 1024) && (std::stoi(argv[1]) <= 32768) ) //error check to make sure input is a power of 2
    {
        int argument = std::stoi(argv[1]);
        while( (argument%2) == 0)
        {
            argument = argument/2;
        }
        if(argument != 1 || argument == 0)
        {
            fprintf(stderr, "Error: the page size must be a power of 2\n");
            return 1;
        }
    }
    else if ( (std::stoi(argv[1]) < 1024) || (std::stoi(argv[1]) > 32768) ) //error check to make sure input is between 1024 and 32768
    {
        fprintf(stderr, "Error: the page size must be a power of 2 between 1024 and 32768\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint8_t *memory = new uint8_t[67108864]; // 64 MB (64 * 1024 * 1024)

    //initialize MMU and Page Table
    Mmu *mmu = new Mmu(page_size);
    PageTable *page_table = new PageTable(page_size);
    int pid;
    int number_of_pages;

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit")
    {
        // Handle command
        // TODO: implement this!

        //splits user input at 'spaces' into an array
        std::istringstream buf(command);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> args(beg, end);

        //error checking
        if(args.size() == 0){}
        else if( args[0] == "exit")
        {
            exit(0);
        }
        else if (args[0] == "create")
        {
            if(args.size() > 3)
            {
                std::cout << "Error with command 'create': Too many arguments" << std::endl;
            }
            else if(args.size() == 1 || args.size() == 2)
            {
                std::cout << "Error with command 'create': Not enough arguments" << std::endl;
            }
            else
            {
                std::cout << mmu->createNewProcess( std::stoi(args[1]), std::stoi(args[2]), page_table ) << std::endl;
            }

        }
        else if (args[0] == "allocate")
        {
            if(args.size() > 5)
            {
                std::cout << "Error with command 'allocate': Too many arguments" << std::endl;
            }
            else if(args.size() == 1)
            {
                std::cout << "Error with command 'allocate': Not enough arguments" << std::endl;
            }

            int pid = std::stoi(args[1]); 
            std::string name = args[2];  
            std::string type = args[3]; 
            int quantity = std::stoi( args[4]);
            std::cout << mmu->allocate(pid, name, type, quantity ) << std::endl;      

        }
        else if (args[0] == "set")
        {
            if(args.size() == 1)
            {
                std::cout << "Error with command 'set': Not enough arguments" << std::endl;
            }

            int pid = std::stoi(args[1]); 
            std::string name = args[2];  

            Variable* var = mmu->getVariable(pid, name); 
            if( var == NULL ){
                printf("Failed to set get the variable to set"); 
                return 1;  
            }
            int physicalAddress = page_table->getPhysicalAddress( pid, var->virtual_address );  
            int offset = stoi(args[3]);

            for(int i = 4; i < args.size(); i++ ){

                if( var->type.compare("char") == 0 ){
                    memory[physicalAddress + offset] = args[i].c_str()[0]; 
                }
                else if( var->type.compare("short") == 0 ){
                    int value = std::stoi(args[1]); 
                    uint8_t lower = value & 0xFF;  
                    uint8_t upper = value >> 8;  
                    memory[physicalAddress + offset] = upper; 
                    memory[physicalAddress + 1 + offset] = lower; 
                }
                else if( var->type.compare("int") == 0){
                    printf("filling in memory");
                    int value = std::stoi( args[i] ); 

                    for( int i = 4; i > 0; i-- ){
                        memory[physicalAddress + i ] = (value >> 8 * i) & 0xFF;  
                    }
                }
            }



        }
        else if (args[0] == "free")
        {
            if(args.size() > 3)
            {
                std::cout << "Error with command 'free': Too many arguments" << std::endl;
            }
            else if(args.size() == 1)
            {
                std::cout << "Error with command 'free': Not enough arguments" << std::endl;
            }
            
            int pid = std::stoi(args[1]); 
            std::string name = args[2];  
            mmu->free(pid, name ); 
        }
        else if (args[0] == "terminate")
        {
            if(args.size() == 1)
            {
                std::cout << "Error with command 'terminate': Not enough arguments" << std::endl;
            }
            else
            {
                mmu->terminate( std::stoi(args[1]), page_table);

            }
        }
        else if (args[0] == "print")
        {
            if(args.size() == 1)
            {
                std::cout << "Error with command 'print': Not enough arguments" << std::endl;
            }
            else if(args.size() > 2)
            {
                std::cout << "Error with command 'print': Not enough arguments" << std::endl;
            }
            else if(args[1] == "mmu")
            {
                mmu->print();
            }
            else if(args[1] == "page")
            {
                page_table->print();
            }
            else if(args[1] == "processes"){
                //TODO: 
            }
            else {
                int pid = std::stoi(args[1].substr(0,4)); 
                std::string name = args[1].substr(5); 

                Variable* var = mmu->getVariable( pid, name ); 
                int vaddress = var->virtual_address; 
                int physicalAddress = page_table->getPhysicalAddress( pid, vaddress );  
                std::string type = var->type; 
                printf("type: %s\n", type.c_str());
                printf("name: %s\n", name.c_str()); 

                if( type.compare("char") == 0 ){
                    for(  int i = 0; i < var->size; i++ ){
                        printf("%c, ", memory[ physicalAddress + i ] );  
                    }
                }
                else if( type.compare("short") == 0 ){
                    
                    for( int i = 0; i < var->size / 2; i++ ){
                        uint8_t upper = memory[ physicalAddress ]; 
                        uint8_t lower = memory[ physicalAddress + 1 ]; 
                        short var = lower || (upper << 8 ); 
                        printf("%d, ", var); 
                    }

                }
                else if( type.compare( "int" ) == 0 ){
                    int result = 0; 
                    printf("printing int: %d\n", memory[physicalAddress]); 
                    for( int i = 0; i < var->size / 4; i++ ){
                        //OR each byte into the resulting int. 
                        for( int j = 0; j < 4; j++ ){
                            uint8_t byte = memory[ physicalAddress + i ]; 
                            result = result | (byte << i );  
                        }
                        printf("%d, ", result); 
                    }
                }
            }

        }
        else
        {
              std::cout << "Command '" << args[0] << "' not found" << std::endl;
        }
        
        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
    }

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}
