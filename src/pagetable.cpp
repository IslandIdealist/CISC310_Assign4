#include "pagetable.h"
#include <math.h>
#include <sstream>

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
    frameTableVector = new std::vector<bool>();
}

PageTable::~PageTable()
{
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string( pid ) + "|" + std::to_string( page_number) ;

    // Find free frame
    // TODO: implement this!
    int frameNumber = 0;
    bool foundFreeSpace = false;

    for(int i = 0; i < frameTableVector->size(); i++)
    {
        if( frameTableVector->at(i) == false )
        {
            foundFreeSpace = true;
            frameNumber = i;
            break;
        }
    }


    //something is fucked here. I think I have the right idea tho?


   // std::cout << "frameNumber: " << frameNumber << std::endl;
   // std::cout << "frameTableVectorSize: " << frameTableVector->size() << std::endl;
    
    if( foundFreeSpace )
    {
        frameTableVector->push_back( true );
        frameNumber = frameTableVector->size();
    }
    else
    {
        frameTableVector->push_back( true );
    }

    _table[entry] = frameNumber;
}

int PageTable::getPhysicalAddress(uint32_t pid, int virtual_address)
{
    // Convert virtual address to page_number and page_offset

    int page_number = virtual_address >> _page_offset_bit;
    int page_offset = virtual_address & ( (int)pow(2, _page_offset_bit) - 1 );

    std::cout << "Page number: " << page_number << std::endl;
	std::cout << "Page offset: " << page_offset << std::endl;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        address = _table[entry] * _page_size + page_offset;
    }

    return address;
}

void PageTable::print()
{
    std::vector<std::string> tableEntries;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    for( auto it = _table.begin(); it != _table.end(); it++)
    {
        tableEntries.push_back( it->first );
    }

    for( auto it = tableEntries.begin(); it != tableEntries.end(); it++ )
    {   
        std::string index = *it;

        int spaces = index.find_first_of('|');

        int page = std::stoi( index.substr( spaces + 1, index.length() - spaces ) );
        int pid = std::stoi( index.substr( 0, spaces ) );

        std::cout << pid << std::string( 3, ' ') << "| " << page << std::string( 8 - std::to_string(page).length(), ' ') << "| " << _table.find( index )->second << std::string( 8 - std::to_string(_table.find( index )->second).length(), ' ') << std:: endl;
    }
    
}
