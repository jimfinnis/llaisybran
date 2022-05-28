/**
 * @file pool.h
 * @brief  Brief description of file.
 *
 */

#ifndef __POOL_H
#define __POOL_H

class BadItemException : public std::exception {};
class FreeTwiceException : public std::exception {};
class NoAllocListException : public std::exception {};

template <class T> class PoolIterator; // fwd decl for friend

template <class T> class Pool {
    friend PoolIterator<T>;
    
    struct Element {
        char data[sizeof(T)];
        Element *next,*prev;
        bool free; //!< true if item is free
    };
    
    Element *data; //!< pointer to array of elements
    Element *firstFree; //!< ptr to first free item or NULL
    Element *firstAlloc; //!< ptr to first allocated item or NULL
    
    int entries;	//!< number of allocated slots
    int maxitems;

#if POOLDEBUG
public:
#endif    
    int id(void *item){
        return (int)((Element *)item-data);
    }
    
public:
    Pool(int _maxitems) {
        maxitems = _maxitems;
        data = new Element[maxitems];
        clear();
    }
    
    // this is occasionally useful, but please don't. Returns
    // an element or NULL if that slot isn't taken.
    T* get(int i){
        Element *e = data+i;
        return e->free ? NULL : (T*)e;
    }
    
    
    
    void clear()
    {
        entries=0;
        Element *e=data;
        for(int i=0;i<maxitems;i++,e++){
            e->next = (i<maxitems-1) ? e+1 : NULL;
            e->prev = (i>0) ? e-1 : NULL;
            e->free = true;
        }
        firstFree = data;
        firstAlloc = NULL;
    }
    
    bool hasSpace() {
        return firstFree != NULL;
    }
    
    int freeSlots() {
        return maxitems-entries;
    }
    
    
    
    // note that the returned item must be initialised
    T* alloc(){
        if(!hasSpace())return NULL;
        // remove item from start of free list
        Element *e = firstFree;
        if(e->next)
            e->next->prev = NULL;
        firstFree = e->next;
        
        // add item to start of alloc list
        e->next = firstAlloc;
        e->prev = NULL;
        if(firstAlloc)
            firstAlloc->prev = e;
        firstAlloc = e;
        e->free = false;
        
        entries++;
#if POOLDEBUG
        printf("Allocated item %d\n",id(e));
        dump();
#endif        
        return (T *)e;
    }
    
    void free(T *item){
        Element *e = (Element *)item;
#if POOLDEBUG
        printf("Freeing item %d\n",id(e));
#endif
        int i = id(e);
        if(i<0 || i>=maxitems)
            throw BadItemException();
        if(e->free)
            throw FreeTwiceException();
        
        // remove from alloc list
        
        if(!firstAlloc)throw NoAllocListException();
        
        if(firstAlloc == e)
            firstAlloc = e->next;
        
        if(e->next)
            e->next->prev = e->prev;
        
        if(e->prev)
            e->prev->next = e->next;
        
        // add to free list
        
        e->next = firstFree;
        e->prev = NULL;
        if(firstFree)
            firstFree->prev = e;
        firstFree = e;
        e->free = true;
        entries--;
#if POOLDEBUG
        dump();
#endif
    }
    
#if POOLDEBUG
    void dump(){
        printf("Freelist: ");
        for(Element *e = firstFree;e;e=e->next)
            printf("%d ",id(e));
        printf("\n");
        
        printf("Alloclist: ");
        for(Element *e = firstAlloc;e;e=e->next)
            printf("%d ",id(e));
        printf("\n");
    }
#endif
    
};


// Usage: 
//   for(Mytype *c=it.first();it.cur();c=it.next()){..}
// Fewer temps than the standard pattern.

template <class T> class PoolIterator {
    Pool<T>& p;
    typename Pool<T>::Element *current;
public:
    PoolIterator(Pool<T>& _p) : p(_p) {
    }
    
    T* first(){
        current = p.firstAlloc;
        return (T*)current;
    }
    
    T* cur(){
        return (T*)current;
    }
    
    T* next(){
        current = current->next;
        return (T*)current;
    }
};


#endif /* __POOL_H */
