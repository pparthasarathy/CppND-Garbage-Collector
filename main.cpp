// pparthas: Commented out original main.cpp test code
/*#include "gc_pointer.h"
#include "LeakTester.h"

int main()
{
    Pointer<int> p = new int(19);
    p = new int(21);
    p = new int(28);

    return 0;
}*/
// pparthas: Added main.cpp code from Smart Pointer Lab: 
// https://classroom.udacity.com/nanodegrees/nd213/parts/9d1d0711-1de3-45f3-8c2f-95e0544af31c/modules/cb09d68c-24ca-496f-bb73-b1ed3b1fa8c8/lessons/99cab5de-d9e2-4beb-b334-6d036398e0f5/concepts/d6b63e4f-6b29-4107-af40-b37a4cd25b9e
#include <memory>
#include "gc_pointer.h"
#include "LeakTester.h"

namespace custom {
    template< typename T>
    class list {
        private:
            struct node{
            T value;
            Pointer< node > prev;
            Pointer< node > next;
            node(T val, node* _prev, node* _next) : value(val), prev(_prev), next(_next) {}
            };
            node* head;
            node* tail;
        public:

        // We don't have any destructors or freeing of memory
            list(): head(NULL), tail(NULL){}
            void push_back(T);
            bool empty() const { return ( !head || !tail ); }
            void print();
    };

    template <typename T>
    void list<T>::push_back(T val){
        list::tail = new node(val, list::tail, NULL);
        if(list::tail->prev){
            list::tail->prev->next = list::tail;
        }
        if( list::empty()){
            list::head = list::tail;
        }
    }

    template< typename T> 
    void list<T>::print() { 
        node* ptr= list::head;  
        while(ptr != NULL) { 
            std::cout<< ptr->value <<" "; 
            ptr = ptr->next; 
        }
        std::cout << std::endl;
    }
}

int main() {
custom::list< int > list;

std::cout << "Add 5 numbers: " << std::endl;
for(int i = 0; i < 5; i++) {
    int temp;
    std::cout<<"Add "<< i+1<< ". element:"<< std::endl;
    std::cin >>temp;
    list.push_back(temp);
}

list.print();
return 0;
}
