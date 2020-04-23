#include <iostream>
#include <sstream>
#include <string>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint8_t *memory = new uint8_t[67108864]; // 64 MB (64 * 1024 * 1024)

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit") {
        // Handle command
        // TODO: implement this!

        //splits user input at 'spaces' into an array
        std::istringstream buf(command);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> args(beg, end);

        //prints out the token array
        /*for(auto& s: args)
        {
            std::cout << '"' << s << '"' << '\n';
        }*/

        //error checking
        if(args.size() == 0){}
        else if (args[0] == "create")
        {
            if(args.size() > 3)
            {
                std::cout << "Error with command 'create': Too many arguments" << std::endl;
            }
            else if(args.size() == 1)
            {
                std::cout << "Error with command 'create': Not enough arguments" << std::endl;
            }
        }
        else if (args[0] == "allocate")
        {
            if(args.size() > 4)
            {
                std::cout << "Error with command 'allocate': Too many arguments" << std::endl;
            }
            else if(args.size() == 1)
            {
                std::cout << "Error with command 'allocate': Not enough arguments" << std::endl;
            }
        }
        else if (args[0] == "set")
        {
            if(args.size() == 1)
            {
                std::cout << "Error with command 'set': Not enough arguments" << std::endl;
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
        }
        else if (args[0] == "terminate")
        {
            if(args.size() == 1)
            {
                std::cout << "Error with command 'terminate': Not enough arguments" << std::endl;
            }
        }
        else if (args[0] == "print")
        {
            if(args.size() == 1)
            {
                std::cout << "Error with command 'print': Not enough arguments" << std::endl;
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
