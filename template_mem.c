
#include <stdio.h>
#include <string.h>
#include<strings.h>
#define NB_DISK_PAGE 5
#define PAGE_SIZE 20
#define NB_MEM_PAGE 4

#define boolean int
#define true 1
#define false 0

char Disk[NB_DISK_PAGE][PAGE_SIZE];
char Memory[NB_MEM_PAGE][PAGE_SIZE];
int PageTable[NB_DISK_PAGE];
struct {
    boolean free;
    int date;
    int npage;
} MemState[NB_MEM_PAGE];

int Date = 0;
void init();
char *memory_read(int npage);
int memory_alloc();
void page_fault(int npage);
int lru_select();

void init() {
    int i;

    // initialize Disk
    for (i=0;i<NB_DISK_PAGE;i++) {
        strcpy(Disk[i],"page");
        Disk[i][4] = '0'+i;
        Disk[i][5] = 0;
    }

    for (i=0;i<NB_DISK_PAGE;i++) {
        printf("Disk",Disk[i]);
        printf("\n");
    }


    // initialize Memory
    for (i=0;i<NB_MEM_PAGE;i++)
        MemState[i].free = true;

    for(int i=0;i<NB_MEM_PAGE;i++){
        MemState[i].date=0;
    }
    // initialize PageTable
    for (i=0;i<NB_DISK_PAGE;i++)
        PageTable[i] = -1;
}

int memory_alloc() {
    printf("memory_alloc: %d\n",0);
    int i;
    for(i=0;i<NB_MEM_PAGE;i++){
        if(MemState[i].free){
            MemState[i].free=false;
            return i;
        }
    }
    return -1;
}


char *memory_read(int npage) {
    printf("memory_read (%d)\n",npage);
    if(PageTable[npage]==-1){
        page_fault(npage);
        printf("test %d",npage);
    }
    int mp=memory_alloc();
    printf("My mp %d\n",mp);
    if(mp!=-1){
        MemState[mp].npage=npage;
        memcpy(Disk[npage],Memory[mp],PAGE_SIZE);
        PageTable[npage]=mp;
    }
    
    else{
        if(PageTable[npage]!=-1){
            MemState[PageTable[npage]].date=Date;
        }else{
            mp=lru_select();
            memcpy(Memory[mp],Disk[MemState[mp].npage],PAGE_SIZE);
            PageTable[MemState[mp].npage]=-1;
            memcpy(Disk[npage],Memory[mp],PAGE_SIZE);
            MemState[mp].npage=npage;
            PageTable[npage]=mp;
        }
    }
    Date++;
    MemState[PageTable[npage]].date=Date;
    
    
    for(int i=0;i<NB_MEM_PAGE;i++){
        printf("MemState %d %d\n",i,MemState[i].date);
    }
    return Memory[PageTable[npage]];

}


void page_fault(int npage) {
    printf("page_fault (%d)\n",npage);

}

int lru_select() {
    int minDate=Date;
    int lrupage;
    for(int i=0;i<NB_MEM_PAGE;i++){
        if(MemState[i].date<minDate){
            minDate=MemState[i].date;
            lrupage=i;
        }
    }
    //printf("Min Date is: %d \n",minDate);
    printf("lru_select: %d\n",lrupage);
    return lrupage;

}

int main(int argc, char *argv[]) {
    int i;
    printf("initialization\n");
    init();


    printf("linear access of all pages\n");
    // for (i=0;i<NB_DISK_PAGE;i++)
    //     printf("read access page %d : %s\n",i,memory_read(i));


    //printf("access pages as in lecture (1,2,3,4,1,2,5,1,2,3,4,5,2,3)\n");
    int serie[14] = {0,1,2,3,0,1,4,0,1,2,3,4,1,2};
    for (i=0;i<14;i++)
        printf("read access page %d : %s\n",serie[i],memory_read(serie[i]));
        printf("completed\n");
        printf("print memory_state\n");
    for (i=0;i<NB_MEM_PAGE;i++)
        printf("%d ",MemState[i].npage);
    printf("\n");
    printf("completed\n");
}


