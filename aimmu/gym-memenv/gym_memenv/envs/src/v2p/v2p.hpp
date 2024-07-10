#ifndef _V2P_H_
#define _V2P_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <map>
#include <deque>
#include <math.h>
#include <algorithm>
#include <list>
#include <numeric>
#include "../stats.hpp"
#include "../extern_vars.hpp"
#include "../dma.hpp"
#include "data_collect.hpp"

using namespace std;

//     L1       L2       L3       L4
//     2^32 |---|--->[---]--->[---]--->[---]   }
//          |---|                  --->[---]   }  7MB STACK
//          |---|                  --->[---]   }
//          |---|                  --->[---]   }
//          |---|
//          |---|
//          |---|
//          |---|
//          |---|                      L4
//          |---|                      |->[---] }
//          |---|                      |->[---] } 5MB HEAP
//          |---|    L2       L3       |->[---] }
//        0 |---|--->[---]--->[---]--->|
//                                     |->[---] }
//                                     |->[---] }
//                                     |->[---] } 12MB TEXT
//                                     |->[---] }
//                                     |->[---] }
//                                     |->[---] }
//
// Key [---] Level in Page Table
//     ---> Pointer to Lower Level Page Table
//

//the division of the 64 bits virtual address
//63:48 - bits are not used
//47:39 - index to PML level-4
//38:30 - index to page directory pointer level-3
//29:21 - index to page directory level-2
//20:12 - index to page table
//11:0  - page offset

/**
 * @brief level-1 contains pointers to the physical pages
 */
class Page {
  public:
    Page();
    ~Page();
    PageFrameAllocator *_pfa;
    bool searchEntry(int, unsigned long long);//pass the virtual address
    void update(
        const unsigned long long vaddr,
        const unsigned long long npf,
        const int pid,
        const bool valid = true);
    bool replacable();
    void invalidate();
    unsigned long long getNPF();
    void update_bits(const int current_status);
    int get_bits();
    void reset_bits();
    bool valid;
    unsigned long access_count;
    bool update_pt(unsigned long new_frame){
      page_frame_number = new_frame;
      return true;
    }
    unsigned long get_frame_number(){return page_frame_number;}
    int get_pid(){return _pid;}
    void migration_counts(); 
    void reset_migration_couters();
    double get_mig_freq(){return _migration_freq;}
    unsigned long return_migration_times(){return _migration_times;}

  private: 
    int repl_bits;//to maintain replacement status
    int _pid;
    int _page_frame_size;
    unsigned long long VirAddr;//excluding the page offset
    unsigned long long page_frame_number;//covers 4KB
    unsigned long _migration_times;//number of times the page is being migrated
    unsigned long _last_mig_cycle;//register cycle when migration is initiated
    unsigned long _migration_gaps;//gap in migrations
    double _migration_freq;//how frequently the page is being migrated
    unsigned long _int_mig;//intermediate migration
    unsigned long _access_after_migration;//reset to zero on next migration !!!
    bool _migrated;//false initially, true after migration and false again once counted
};

class PageDirTab {//level-2 contains pointers to the page table
  public:
    PageDirTab();
    ~PageDirTab();
    PageFrameAllocator *_pfa;
    DirectMemoryAccess * _dma;// = DirectMemoryAccess::GetInstance();
    bool valid;
    unsigned long _tot_frame_per_cube;
    vector<Page>PT;//page table
    bool PT_resized;
    void makeEntry(int, unsigned long long);
    bool searchEntry(int, unsigned long long);//pass the virtual address
    unsigned long long getVictim();
    void update_repl_status(int PT_index);
    bool space_in_memory();//returns true is space is avialable
    int getIndex(const unsigned long long vaddr);
    bool update_page_frame(unsigned long, int);
    bool free_page_frame();
    unsigned long count_active_pages();
};

class PageDirPtrTab {//level-3 contains pointers to the page directory
  public:
    PageDirPtrTab();
    ~PageDirPtrTab();
    bool valid;
    vector<PageDirTab>PDT;//this will hold one PDT table of 512 entries
    bool makeEntry(int, unsigned long long);
    bool searchEntry(int, unsigned long long);//pass the virtual address
    bool update_page_frame(unsigned long, int);
    bool free_page_frame();
    unsigned long count_active_pages();
};

class PageMapL4 {//level-4 contains pointers to the page directory pointer table
  public:
    PageMapL4();
    ~PageMapL4();
    bool valid;
    vector<PageDirPtrTab>PDPT;//this will hold one PDPT table of 512 entries
    bool makeEntry(int, unsigned long long);
    bool searchEntry(int, unsigned long long);//pass the virtual address
    bool update_page_frame(unsigned long, int);
    bool free_page_frame();
    unsigned long count_active_pages();
};

class vir2phy {
  public:
    vir2phy();//resize PML of size 512 and initialize to invalid
    ~vir2phy();
    vector<PageMapL4>PML;//this means here we have access to PML
    bool makeEntry(int, unsigned long long);//send only the index
    unsigned long long GetPhyAddr(int, unsigned long long);//pass the pid, and virtual address
    unsigned long long GetPhyFrameNum(int, unsigned long long, unsigned long long);//pass the pid,
    bool DoPageMigration(unsigned long vaddr, int new_cube);
    bool FreeAllPageFrames();
    unsigned long CountActivePages();
    //virtual address, and physical address  
};

#endif
