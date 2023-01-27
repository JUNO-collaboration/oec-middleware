#ifndef FRAGMENT_RING_ARRAY_H
#define FRAGMENT_RING_ARRAY_H

#include <memory>
#include <vector>

class HOECFragment
{public:       
    std::shared_ptr<std::vector<void*>> evtsPtr;
    uint32_t l1id;
    uint32_t timeSec;
    uint32_t nanoSec;
    enum Status{
        empty,
        ready,
        late,
        inWorker,
        inExcpHandling,
        unaccurate,
        copyReturned,
        returned
    };
    HOECFragment::Status stat = Status::empty;
};

class FragmentRingArray
{
public:
    class Iterator{
    public:
        HOECFragment* ptr;
        int arrayLen;
        int locate;
        Iterator(FragmentRingArray& ringArray);
        Iterator& operator++();
        HOECFragment& operator*()  const;
        HOECFragment* operator->()  const;
    };
    friend Iterator;

    FragmentRingArray(int length = 10000);
    ~FragmentRingArray();

    int insertFrag(std::shared_ptr<std::vector<void*>> evtsPtr, uint32_t l1id, uint32_t timeSec, uint16_t nanoSec);
    bool cleanFrag(FragmentRingArray::Iterator it);
    HOECFragment& operator[](int locate);

    void snapShot();
private:
    HOECFragment* m_array;
    int m_arrayLen;

    //FragmentRingArray(){};
};

#endif

//对迭代器重载算符： ++, ->
