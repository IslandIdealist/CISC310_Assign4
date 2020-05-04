#ifndef __PAGETABLE_H_
#define __PAGETABLE_H_

#include <iostream>
#include <string>
#include <map>
#include <vector>

class PageTable {
private:
    int _page_size;
    int _page_offset_bit;
    std::map<std::string, int> _table;
    bool* _frameTableArray;

public:
    PageTable(int page_size);
    ~PageTable();

    void addEntry(uint32_t pid, int page_number);
    int pageSize() {return _page_size;}
    int getPhysicalAddress(uint32_t pid, int virtual_address);
    void remove(uint32_t pid, int pageNumber);
    void print();
};

#endif // __PAGETABLE_H_
