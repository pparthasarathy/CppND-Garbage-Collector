#ifndef _GC_POINTER_H_
#define _GC_POINTER_H_

#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <class T, int size = 0>
class Pointer
{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise.
    */
    bool isArray;
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    unsigned arraySize; // size of the array
    static bool first; // true when first Pointer is created
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
    Pointer()
    {
        Pointer(NULL);
    }
    Pointer(T*); //pparthas: defined below outside the class scope
    // pparthas: Copy constructor defined using above constructor
    Pointer(const Pointer &inObj): Pointer(inObj.addr) {}
    // Destructor for Pointer.
    ~Pointer(); //pparthas: defined below outside the class scope
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect(); //pparthas: defined below outside the class scope
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t); //pparthas: defined below outside the class scope
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv); //pparthas: defined below outside the class scope
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*() { return *addr; }
    // Return the address being pointed to.
    T *operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i){ return addr[i];}
    // Conversion function to T *.
    operator T *() { return addr; }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t)
{
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    // pparthas: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab
	if(t!=nullptr)
	{
		addr = t;
		if(size > 0)
		{
    		isArray = true;
		}
    	arraySize = size;
	}
    auto p_iter = findPtrInfo(t);
    if(p_iter == refContainer.end())
	{
        PtrDetails<T> newptrDetails;
    	newptrDetails.refcount = 1;
    	newptrDetails.memPtr = t;
    	newptrDetails.isArray = isArray;
    	newptrDetails.arraySize = arraySize;
        refContainer.push_back(newptrDetails);
    }
	else
	{
        p_iter->refcount++;
    }
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer(){

    // pparthas: Pointer destructor
    // Lab: New and Delete Project Lab
	auto p_iter = findPtrInfo(addr);
    if(p_iter != refContainer.end())
	{	
    	if(p_iter->refcount > 0) p_iter->refcount--;
    	if(p_iter->refcount == 0) collect();
	}
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){

    // pparthas: Implement collect function
    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor
    bool freedMem = false;
    typename std::list<PtrDetails<T> >::iterator p_iter;
	
    do
	{
        // Scan refContainer looking for unreferenced pointers.
        for(p_iter = refContainer.begin(); p_iter != refContainer.end(); p_iter++)
		{
            // if in use, skip over
            if(p_iter->refcount > 0) continue;

            refContainer.remove(*p_iter);
            //refContainer.erase(p_iter--);
            freedMem = true;

            if(p_iter->memPtr)
			{
                if(p_iter->isArray) delete[] p_iter->memPtr;
                else delete p_iter->memPtr;                
            }
            // Restart the search
            break;
        }
    } while (p_iter != refContainer.end());
    
    return freedMem;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t)
{
    // pparthas: Implement operator=
    // LAB: Smart Pointer Project Lab
    auto p_iter = findPtrInfo(addr);
    p_iter->refcount--;
    if(p_iter->refcount == 0) collect();
    p_iter = findPtrInfo(t);
    if(p_iter == refContainer.end())
	{
        PtrDetails<T> newPtrDetails;
		newPtrDetails.refcount = 1;
        if(size > 0)
		{
            newPtrDetails.isArray = true;
        }
		newPtrDetails.arraySize = size;
        newPtrDetails.memPtr = t;
        refContainer.push_back(newPtrDetails);
    }
	else
	{
        p_iter->refcount++;
    }
    if(size > 0)
	{
        isArray = true;
        arraySize = size;
    }
	addr = t;
    return addr;
}

// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv)
{
    // pparthas: Implement operator==
    // LAB: Smart Pointer Project Lab
    if(*this == rv) return *this;
    auto p_iter = findPtrInfo(this->addr);
    p_iter->refcount--;
    if(p_iter->refcount == 0) collect();
    p_iter = findPtrInfo(rv.addr);
    p_iter->refcount++;
	this->addr = rv.addr;
    this->isArray = rv.isArray;
    this->arraySize = rv.arraySize;
    return *this;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}

#endif  // _GC_POINTER_H_
