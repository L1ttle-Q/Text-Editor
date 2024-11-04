#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <cstring>
#include <cstdlib>
#include <utility>

template<typename T>
class MemoryPool
{
private:
    struct Block
    {
        T data;
        Block* next;
        bool used; // The order of different kinds of type matters

        // union
        // {
        //     T data;
        //     Block* next;
        // };
        // 1. Time-based memory reuse
        // 2. Memory alignment
        //    if define separately,
        //    allocate Block type space
        //    but deallocate T type space
        Block(): next(nullptr), used(false) {}
        Block(Block* p): next(p), used(false) {}
    };
    Block* freeList;
    size_t blockSize;
    size_t blockCount;
    static const size_t MaxSingleSize = (1 << 15) - 1;
    // avoid always wasting a half of memory

public:
    MemoryPool(size_t initSize = 7):
    freeList(nullptr), blockSize(sizeof(Block)), blockCount(initSize)
    { expandPool();}

    // virtual ~MemoryPool()
    // {
    //     while (freeList)
    //     {
    //         Block *tmp = freeList->next;
    //         free(freeList);
    //         freeList = tmp;
    //     }
    //     freeList = nullptr;
    // }

    // void Debug()
    // {
    //     Block* now = freeList;
    //     for (int i = 1; i <= 8 && now; i++)
    //     {
    //         printf("freeEle: %p->%p\n", now, now->next);
    //         now = now->next;
    //     }
    // }

    void* allocate()
    {
        if (!freeList) expandPool();
        Block* tmp = freeList;
        tmp->used = true;
        freeList = freeList->next;
        return &(tmp->data);
    }

    void deallocate(void *ptr)
    {
        Block* tmp = (Block *)ptr;
        if (!tmp->used) return ; // repeated free
        tmp->next = freeList;
        tmp->used = false;
        freeList = tmp;
    }

private:
    void expandPool()
    {
        size_t size = blockSize * blockCount;
        Block* block = (Block *)malloc(size);
        if (!block)
        {
            fprintf(stderr, "Fail to allocate space!\n");
            exit(1);
        }
        for (size_t i = 0; i < blockCount - 1; i++)
            new(&block[i]) Block(&block[i + 1]);
        new(&block[blockCount - 1]) Block(nullptr);
        freeList = block;
        blockCount = blockCount << 1 | 1; // binary lifting
        blockCount &= MaxSingleSize;
    }
};

template<typename T>
class Node
{
public:
    Node *last, *next;
    T data;
    static MemoryPool< Node<T> > memoryPool;
    // general independent memorypool for each type

    Node(): last(this), next(this), data() {}
    Node(T val): next(this), last(this), data(val) {}
    Node(Node *q, Node *p): last(q), next(p), data() {}
    Node(Node *q, Node *p, T val): last(q), next(p), data(val) {}

    template<typename... Args>
    void* operator new(size_t size, Args&&... args)
    {
        void* ptr = memoryPool.allocate();
        return ::new(ptr) Node(std::forward<Args>(args)...); // placement new
    }
    void operator delete(void* ptr)
    {
        ((Node *)ptr)->~Node();
        memoryPool.deallocate(ptr);
    }
};

template<typename T>
MemoryPool< Node<T> > Node<T>::memoryPool;

template<typename T>
class LinkedList
{
protected:
    Node<T> *head, *tail;
    Node<T>* now;
    int cur;
    int length;

    friend class TextEditor;
public:
    LinkedList()
    {
        cur = 0; length = 0;
        head = new Node<T>(); tail = new Node<T>();
        head->next = tail; tail->last = head; now = head;
    }
    // need template specialization
    // LinkedList(const char *s): LinkedList()
    // {
    //     for (int i = 0; i < strlen(s); i++)
    //         AddElem(s[i]);
    // }

    class Iterator
    {
    private:
        Node<T>* current;
    public:
        Iterator(Node<T>* ptr): current(ptr) {}
        T& operator *() {return current->data;}
        Iterator operator++(int) {current = current->next; return current;}
        bool operator !=(const Iterator &b) {return current != b.current;}
    };
    Iterator begin() const {return Iterator(head);}
    Iterator end() const {return Iterator(tail);}

    int getCur() const
    {return cur;}
    int getLength() const
    {return length;}
    bool isEmpty() const
    {return head->next == tail;}

    virtual ~LinkedList()
    {
        Node<T>* del = head;
        while (del != tail)
        {
            Node<T> *tmp = del->next;
            delete del;
            del = tmp;
        }
        delete del;
    }

private:
    bool AddElem()
    {
        if (now == tail) return false;
        Node<T>* newNode = new Node<T>(now, now->next);
        now->next = newNode;
        newNode->next->last = newNode;
        now = newNode;
        cur++; length++;
        return true;
    }
    bool AddElemPrev()
    {
        if (now == head) return AddElem();
        Node<T>* newNode = new Node<T>(now->last, now);
        now->last = newNode;
        newNode->last->next = newNode;
        now = newNode;
        length++;
        return true;
    }
    bool AddElem(T t)
    {
        if (now == tail) return false;
        Node<T>* newNode = new Node<T>(now, now->next, t);
        now->next = newNode;
        newNode->next->last = newNode;
        now = newNode;
        cur++; length++;
        return true;
    }
    bool AddElemPrev(T t)
    {
        if (now == head) return AddElem(t);
        Node<T>* newNode = new Node<T>(now->last, now, t);
        now->last = newNode;
        newNode->last->next = newNode;
        now = newNode;
        length++;
        return true;
    }

    bool DeleteElem()
    {
        if (now == tail || now == head) return false;
        now->last->next = now->next;
        now->next->last = now->last;
        auto tmp = now->next;
        delete now;
        now = tmp;
        length--;
        if (now == tail) now = now->last, cur--;
        return true;
    }
    bool DeleteElem(int cnt)
    {
        bool res = true;
        while ((res=DeleteElem()) && (--cnt > 0));
        return res;
    }
    bool Move(bool dir) // 0 for last; 1 for next
                        // return whether succeed to move
    {
        if (!dir)
        {
            if (now->last != head)
            {
                now = now->last, cur--;
                return true;
            }
            return false;
        }
        else
        {
            if (now->next != tail)
            {
                now = now->next, cur++;
                return true;
            }
            return false;
        }
    }
    bool MoveTo(const int& pos)
    {
        if (pos <= 0 || pos > length) return false;
        while (cur < pos) now = now->next, cur++;
        while (cur > pos) now = now->last, cur--;
        return true;
    }
};


#endif