/**
 * @file pool.h
 * @brief  Brief description of file.
 *
 */

#ifndef __POOL_H
#define __POOL_H

class BadItemException : public std::exception {};



template <class T, int ITEMS> class Pool {
    struct Element {
        char data[sizeof(T)];
        int nextFree;
    };
    
    Element *data;
    int firstFree;	//!< first free item index, or -1
    int entries;	//!< number of allocated slots
    
public:
    Pool() {
        data = new Element[ITEMS];
        clear();
    }
        
    
    T *getFromIdx(int idx){
        return (T*)(data+idx);
    }
    
    int getIdxFromItem(T *item){
        Element *eptr = (Element *)item;
        return eptr - data;
    }
    
    bool hasSpace() {
        return firstFree != -1;
    }

    int freeSlots() {
        return ITEMS-entries;
    }
    
    void clear()
    {
        for( int elementLoop = 0; elementLoop < ITEMS; ++elementLoop ) {
            data[ elementLoop ].nextFree = elementLoop + 1;
        }
        data[ ITEMS - 1 ].nextFree = -1;
        firstFree = 0;
        entries = 0;
    }
    
    // note that the returned item must be initialised
    T* alloc(){
        if(!hasSpace())return NULL;
        int id = firstFree;
        firstFree = data[id].nextFree;
        data[id].nextFree = -2;
        entries++;
        printf("Allocated item %d\n",id);
        dump();
        return getFromIdx(id);
    }
    
    void dump(){
        printf("Freelist: ");
        for(int i=firstFree;i>=0;i=data[i].nextFree){
            printf("%d ",i);
        }printf("\n");
        printf("Data:\n");
        for(int i=0;i<ITEMS;i++){
            printf("%d : %d\n",i,data[i].nextFree);
        }
    }
    
    void free(T *item){
        int id = getIdxFromItem(item);
        printf("Freeing item %d\n",id);
        if(id<0 || id>=ITEMS)
            throw BadItemException();
        Element *e = data+id;
        e->nextFree = firstFree;
        firstFree = id;
        entries--;
        dump();
    }
};


#endif /* __POOL_H */
