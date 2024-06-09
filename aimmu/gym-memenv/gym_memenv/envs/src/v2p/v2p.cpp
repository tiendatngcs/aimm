#include "v2p.hpp"
#include "pfa.hpp"
unsigned long long Physical_address = 0;//reserved 

using namespace std;

unsigned long available_frame = 0;

Page::Page(){
  valid=false;
  repl_bits=511;//ready to be replaced
  page_frame_number=0;
  _migration_times = 0;
  _last_mig_cycle = 0;
  _migration_gaps = 0;
  _migration_freq = 0;
  _migrated = false;
  _access_after_migration = 0;//reset on each migration 

  _pfa = PageFrameAllocator::GetInstance();
  _page_frame_size = _pfa->return_page_frame_size(); 
}

Page::~Page(){
}

bool Page::searchEntry(int pid, unsigned long long virAddr){
  int PT_index = (virAddr>>11) & 0x1FF;//should give us 20:12
  //cout<<"trying to match: "<<(virAddr>>11)<<" --- with: "<<VirAddr<<endl;
  if(virAddr>>11 == VirAddr && this->get_pid() == pid && valid){
    assert(valid && "Page must be valid for a page hit");
    stats_page_hit++;
    Physical_address = page_frame_number * (_page_frame_size *1024) + (virAddr & 0x7FF);
    if(_migration_times > 0){//migrated page
      if(_migrated){
        stats_numofpg_acc_after_mig++;//counting number of pages got access after migration
        _migrated = false;
      }
      _access_after_migration++;
      stats_num_of_acc_to_mig_pg++;
    }
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"PT search HIT ... will return the PA if successful\n");
    fprintf(debugfile,"The page frame number: %llu\n", page_frame_number);
    fprintf(debugfile,"The Physical Address: %llu\n", Physical_address);
#endif
    return true;
  }
  else if(valid){//page conflict !!!
    stats_page_fault_conflict++;
    VirAddr = virAddr>>11;
    _pfa->update_free_list(page_frame_number);
    assert(pid<=num_proc_submission);
    page_frame_number = _pfa->get_available_frame(pid);
    this->_pid = pid;
    Physical_address = page_frame_number * (1024 * _page_frame_size) + (virAddr & 0x7FF);
    return true;
  }
  else{
    stats_page_fault++;
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"PT search MISS\n");
#endif
    return false;
  }
}

void Page::update(
    const unsigned long long vaddr,
    const unsigned long long npf,
    const int pid,
    const bool valid) {
  VirAddr = vaddr;
  page_frame_number = npf;
  this->_pid = pid;
  this->valid = valid;
}
    
bool Page::replacable() {
  return !valid || repl_bits == 511;
}

void Page::invalidate() {
  assert(valid == true);
  valid = false;
}

void Page::update_bits(const int current_status) {
  if (repl_bits < current_status) {
    repl_bits++;
    assert(repl_bits < 512 && "*** Replacement bits exceeded ***");
  }
}

int Page::get_bits() {
  return repl_bits;
}

void Page::reset_bits() {
  repl_bits = 0;
}


unsigned long long Page::getNPF() { return page_frame_number; }

unsigned long long PageDirTab::getVictim(){
  int victim_page_index=-1;
  stats_page_replacement++;
  int lower = 0;
  int upper = 512;
  //cout<<"replacement: "<<stats_page_replacement<<endl;
#ifdef DEBUG_PRINT_V2P
  fprintf(debugfile,"page replacement is invloked ...\n");
#endif
  for(int i=0;i<512;i++){
    if (PT[i].replacable()) {
        victim_page_index = i;
        break;
    }
  }
  //victim_page_index = (rand() % (upper - lower + 1)) + lower;
  assert(victim_page_index!=-1 && "*** Invalid page index ***");
  return victim_page_index;
}

void PageDirTab::update_repl_status(int PT_index){
  int current_status = PT[PT_index].get_bits();
#ifdef DEBUG_PRINT_V2P
  fprintf(debugfile,"Update Repl Status ...\n");
#endif
  PT[PT_index].reset_bits();
#pragma omp parallel for
  for(int i=0;i<512;i++){
    PT[i].update_bits(current_status);
  }
}

PageDirTab::PageDirTab(){
  valid=false;
  PT_resized = false;
  _pfa = PageFrameAllocator::GetInstance();
  _dma = DirectMemoryAccess::GetInstance();
  _tot_frame_per_cube = _pfa->tot_num_frames_percube();
}

PageDirTab::~PageDirTab(){
}

void PageDirTab::makeEntry(int pid, unsigned long long virAddr){
  stats_page_miss_make_entry++;
  if(PT_resized!=true){
    stats_total_L3_tables_created++;
    PT.resize(512);
    PT_resized = true; 
  }
  valid = true;
  //PageFrameAllocator * pfa = PageFrameAllocator::GetInstance();
  int index;
  unsigned long long npf;
  index = (virAddr>>11) & 0x1FF;
  assert(pid<=num_proc_submission);
  npf = _pfa->get_available_frame(pid);
  if(npf==0){
    index = getVictim();
    npf = PT[index].getNPF();
  }
  //assert(npf!=0);
  PT[index].update(virAddr>>11, npf, pid/*, true*/);
  Physical_address = npf * (1024* _pfa->return_page_frame_size()) + (virAddr & 0x7FF);
  
  //cout<<"[V2P] frame: "<<npf<<" PhyAddr: "<<Physical_address<<endl;  
  /*
   * Sleeping on page miss to implement page fault penalty
   */
  sleep_process[pid] = 1;
}

bool PageDirTab::searchEntry(int pid, unsigned long long virAddr){
  bool found=false;
  int PT_index = (virAddr>>11) & 0x1FF;//should give us 20:12
  assert(PT_index<512);
#ifdef DEBUG_PRINT_V2P
  fprintf(debugfile,"Searching in PT\n");
  fprintf(debugfile,"Calling PT table from PDT searchEntry for index: %d\n",PT_index);
#endif
  if(valid==true){
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"Valid entry in PT %d\n",PT_index); 
#endif
    if(!PT[PT_index].searchEntry(pid, virAddr)){
      makeEntry(pid, virAddr);//each PT_index entry points to a 4KB page
    }
    else{
      update_repl_status(PT_index);
    }
    found=true; 
  }
  else{
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"No valid entry in PDT %d\nNeed to make an entry in PDT\n",PT_index);  
#endif
    //cout<<"calling from PDT +++++++++++ because the PDT not valid +++++++++++ for: "<<(virAddr>>11)<<endl;
    valid = true;
    makeEntry(pid, virAddr);
  }
  return found;
}

PageDirPtrTab::PageDirPtrTab(){
  valid=false;
}

PageDirPtrTab::~PageDirPtrTab(){
}

bool PageDirPtrTab::makeEntry(int pid,unsigned long long virAddr){
  stats_total_L2_tables_created++;
  int PDT_index = (virAddr>>20) & 0x1FF;//should give us 29:21
#ifdef DEBUG_PRINT_V2P
  fprintf(debugfile,"Making entry in PDPT @ %d\n",PDT_index); 
#endif
  valid = true;
  PDT.resize(512);
  PDT[PDT_index].valid = true;
  PDT[PDT_index].makeEntry(pid, virAddr); 
  return false;
}

bool PageDirPtrTab::searchEntry(int pid, unsigned long long virAddr){
  bool found=false;
  int PDT_index = (virAddr>>20) & 0x1FF;//should give us 29:21
  assert(PDT_index<512);
#ifdef DEBUG_PRINT_V2P
  fprintf(debugfile,"Searching in PDT\n"); 
  fprintf(debugfile,"Calling PDT table from PDPT searchEntry for index: %d\n",PDT_index);
#endif
  if(valid==true){
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"Valid entry in PDT %d\n",PDT_index); 
#endif
    //cout<<"++++++++++++++++ search entry ptd -----------------"<<endl;
    PDT[PDT_index].searchEntry(pid, virAddr);
    found=true; 
  }
  else{
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"No valid entry in PDPT %d\nNeed to make an entry in PDT\n",PDT_index);  
#endif
    makeEntry(pid, virAddr);
  }
  return found;
}


PageMapL4::PageMapL4(){
  valid = false;
}

PageMapL4::~PageMapL4(){
}

bool PageMapL4::makeEntry(int pid, unsigned long long virAddr){
  stats_total_L1_tables_created++;
  PDPT.resize(512);
  int PDPT_index = (virAddr>>29) & 0x1FF;//should give us 38:30
  PDPT[PDPT_index].valid = true;
  PDPT[PDPT_index].makeEntry(pid, virAddr);
  return false;
}

bool PageMapL4::searchEntry(int pid, unsigned long long virAddr){
  bool found=false;
  int PDPT_index = (virAddr>>29) & 0x1FF;//should give us 38:30
  assert(PDPT_index<512);
#ifdef DEBUG_PRINT_V2P
  fprintf(debugfile,"Calling PDPT table from PML4 searchEntry for index: %d\n",PDPT_index);
#endif
  if(PDPT[PDPT_index].valid==true){
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"Valid entry in PDPT %d\n",PDPT_index); 
#endif
    PDPT[PDPT_index].searchEntry(pid, virAddr);
    found=true; 
  }
  else{
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"No valid entry in PDPT %d\nNeed to make an entry in PDPT\n",PDPT_index);  
#endif
    PDPT[PDPT_index].makeEntry(pid, virAddr);
  }
  return found;
}

vir2phy::vir2phy(){
#ifdef DEBUG_PRINT_V2P
  cout<<"\n[V2P LOGGING] ++++++++++++++ V2P Logging in ON ++++++++++++ \n"<<endl;
#endif
  PML.resize(512);
}


vir2phy::~vir2phy(){
}

bool vir2phy::makeEntry(int pid, unsigned long long virAddr){

  stats_total_L0_tables_created++;
  int PML_index = (virAddr>>38) & 0x1FF;//should give us 47:39
#ifdef DEBUG_PRINT_V2P
  fprintf(debugfile,"Making entry in PML @ %d\n",PML_index);
#endif
  PML[PML_index].valid = true;
  PML[PML_index].makeEntry(pid, virAddr);
  return false;
}

unsigned long long vir2phy::GetPhyAddr(int pid, unsigned long long virAddr){
  //CR0 register contains the address of the level-4 page directory
  //now we have to chop the virAddr to get the bits 47:39, 9 bits
#ifdef DEBUG_PRINT_V2P
  if(global_clock > 100000){
    cout<<"\n\n\n[V2P] ++++++++++++++ V2P Logging in ON ++++++++++++ \n\n\n"<<endl;
    cout<<"*** It is going to consume all your disk space ---> exiting the program ****"<<endl;
    exit(0);
  }
#endif
  assert(pid<=num_proc_submission);

  Physical_address = 0;  
  
  stats_v2p_mem_acc++;

    int PML_index = (virAddr>>38) & 0x1FF;//should give us 47:39
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"vir2phy: va: %llu PML index: %d\n",virAddr,PML_index);
#endif
    assert(PML_index<512);

    if(PML[PML_index].valid==true){
#ifdef DEBUG_PRINT_V2P
      fprintf(debugfile, "Found a valid entry in PML level-4 for VA: %llu\n",virAddr);
#endif
      PML[PML_index].searchEntry(pid, virAddr);//have start the search chain here
    }
    else{
#ifdef DEBUG_PRINT_V2P
      fprintf(debugfile, "Entry not found in PML level-4 for VA: %llu \nmaking new entry in PML level-4 and onwards\n",virAddr);
#endif
      makeEntry(pid, virAddr);
    }
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"------------------------------------\n");  
#endif
    return Physical_address;  
}

unsigned long long vir2phy::GetPhyFrameNum(int pid, unsigned long long paddr, unsigned long long vaddr){
  return (paddr - (vaddr & 0x7FF))/4096;//frame number   
}

bool vir2phy::DoPageMigration(unsigned long virAddr, int new_cube){//the virtual address is already page number
  int PML_index = (virAddr>>(38-11)) & 0x1FF;
  bool success = false;
  if(PML[PML_index].valid!=true){
    cout<<"L4 directory missing for page num: "<<virAddr<<endl;
    assert(0 && "*** level 4 page dir not valid ***");
  }
  else{
    success = PML[PML_index].update_page_frame(virAddr, new_cube);
  }
  return success; 
}

bool PageMapL4::update_page_frame(unsigned long virAddr, int cube){
  int PDPT_index = (virAddr>>(29-11)) & 0x1FF;//virAddr is already a page number
  bool success = false;
  if(PDPT[PDPT_index].valid!=true){
    cout<<"L3 directory missing for virtual adderess: "<<virAddr<<endl;
    assert(0 && "*** level 3 page dir not valid ***");
  }
  else{
    success = PDPT[PDPT_index].update_page_frame(virAddr, cube);
  }
  return success; 
}

bool PageDirPtrTab::update_page_frame(unsigned long virAddr, int cube){
  int PDT_index = (virAddr>>(20-11)) & 0x1FF;//virAddr is already a page number
  bool success = false;
  if(PDT[PDT_index].valid!=true){
    cout<<"L2 directory missing for virtual address: "<<virAddr<<endl;
    assert(0 && "*** level 2 page dir not valid ***");
  }
  else{
    success = PDT[PDT_index].update_page_frame(virAddr, cube);
  }
  return success; 
}

bool PageDirTab::update_page_frame(unsigned long virAddr, int cube){
  int PT_index = (virAddr>>(11-11)) & 0x1FF;//virAddr is already a page number
  bool success = false;
  unsigned long old_frame = 0;
  int src = -1;
  int dest = cube;

  if(PT[PT_index].valid!=true){
    //cout<<"L1 directory missing for page num: "<<virAddr<<endl;
    //assert(0 && "*** level 1 page dir not valid ***");
  }
  else{
    int pid = PT[PT_index].get_pid();
    old_frame = PT[PT_index].get_frame_number(); 
    src = (old_frame - (old_frame% _tot_frame_per_cube))/_tot_frame_per_cube;
    //PageFrameAllocator *pfa = PageFrameAllocator::GetInstance();
    unsigned long new_frame = _pfa->get_available_frame_migration(cube);
    _pfa->update_free_list(old_frame); 
    success = PT[PT_index].update_pt(new_frame);
    PT[PT_index].migration_counts();
    assert(src!=-1 && dest!=-1);
    unsigned long migr_times = PT[PT_index].return_migration_times();
    //cout<<"[V2P] pg: "<<virAddr<<" migr: "<<migr_times<<endl; //manually checked ... it is fine
    //TODO: put an assertion for absurd page and migr_times numbers
    _dma->make_entry(pid, src, dest, virAddr/*page number*/, migr_times);//making entry in DMA
  }
  return success; 
}

void Page::migration_counts(){
  _access_after_migration = 0;//resetting on each migration 
  if(_migration_times==0){//only initialized to 0 once in the beginning 
    stats_tot_pg_migrated++;//only incremented once for the page
  }
  _migrated = true;//to track access/touched after migration
  _migration_times++;
  _migration_gaps += (global_clock - _last_mig_cycle);
  _migration_freq = (_migration_gaps*1.0) / _migration_times;  
  _last_mig_cycle = global_clock;
  //cout<<"migration counts ....."<<endl;
}


void Page::reset_migration_couters(){
  _migration_times = 0;
  _migrated = 0;
}

bool vir2phy::FreeAllPageFrames(){
 bool success = true;
 for(int PML_index=0; PML_index < 512; PML_index++){ 
  if(PML[PML_index].valid==true){
    success &= PML[PML_index].free_page_frame();
    //PML[PML_index].valid=false;
  }
  //cout<<"PML index clearing: "<<PML_index<<endl;
 }
 //PML.clear();
 return success;
}

bool PageMapL4::free_page_frame(){
 bool success = true;
 for(int PDPT_index=0; PDPT_index < 512; PDPT_index++){ 
  if(PDPT[PDPT_index].valid==true){
    success &= PDPT[PDPT_index].free_page_frame();
    //PDPT[PDPT_index].valid=false;
  }
  //cout<<"PDPT index clearing: "<<PDPT_index<<endl;
 }
 return success;
}

bool PageDirPtrTab::free_page_frame(){
 bool success = true;
 for(int PDT_index=0; PDT_index < 512; PDT_index++){ 
  if(PDT[PDT_index].valid==true){
    success &= PDT[PDT_index].free_page_frame();
    //PDT[PDT_index].valid=false;
  }
  //cout<<"PDT index clearing: "<<PDT_index<<endl;
 }
 return success;
}

bool PageDirTab::free_page_frame(){
 bool success = true;
 for(int PT_index=0; PT_index < 512; PT_index++){ 
  if(PT[PT_index].valid==true){
    unsigned long long PFN = PT[PT_index].get_frame_number();
    _pfa->update_free_list(PFN); 
    PT[PT_index].valid=false;
    PT[PT_index].reset_migration_couters();
    if(!multi_program){
      dch_mig_freq->collect(to_string(PT[PT_index].get_frame_number()));
    }
  }
  //cout<<"PT index clearing: "<<PT_index<<endl;
 }
 return success;
}
