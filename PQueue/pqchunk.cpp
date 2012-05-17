#include "pqchunk.h"
#include "genlib.h"
#include <iostream>

int const MaxElemsPerBlock = 4; // chunk size

PQueue::PQueue()
{
    head = NULL;
}

PQueue::~PQueue()
{
    while (head != NULL) {
        Cell *next = head->next;
        delete head;
        head = next;
    }
}

void PQueue::print()
{
    for (Cell *cur = head; cur != NULL; cur = cur->next)
    {
        for (int i = 0; i < cur->BlocksInUse; i++)
        {
            cout << cur->values[i] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

bool PQueue::isEmpty()
{
    return (head == NULL);  
}

int PQueue::size()
{
    int count = 0;
    for (Cell *cur = head; cur != NULL; cur = cur->next)
        count += cur->BlocksInUse;
    return count;
}

void PQueue::enqueue(int newValue)
{
    if (head == NULL)
    {
        initPQueue(newValue); // first value enqueued
        return;
    }
    
    Cell *cur, *prev, *newChunk = new Cell;
    for (prev = NULL, cur = head; cur != NULL; prev=cur, cur = cur->next)
    {
        for (int i = 0; i < cur->BlocksInUse; i++)
        {
            if (newValue > cur->values[i])
            {
                // put in this cell if there is room
                if (cur->BlocksInUse < MaxElemsPerBlock)
                {
                    shiftAdd(cur,newValue);
                    return;
                }
                // create a new Cell at the head and insert value
                else if (i == 0 && prev == NULL)
                {
                    newChunk->values = new int[MaxElemsPerBlock];
                    newChunk->values[0] = newValue;
                    newChunk->BlocksInUse = 1;
                    newChunk->next = cur;
                    head = newChunk;
                    return;
                }
                // put in previous cell if there is room
                else if (i == 0 && prev != NULL && prev->BlocksInUse < MaxElemsPerBlock)
                {
                    shiftAdd(prev,newValue);
                    return;
                }
                // current and previous cells full, must split
                else
                {
                    splitAdd(cur,newValue);
                    return;
                }
            }
        }
    }

    // insert in last Cell
    if (prev->BlocksInUse < MaxElemsPerBlock)
    {
        prev->values[prev->BlocksInUse] = newValue;
        prev->BlocksInUse++;
    }
    // create new Cell at the end of the list and add value
    else
    {
        newChunk->values = new int[MaxElemsPerBlock];
        newChunk->values[0] = newValue;
        newChunk->BlocksInUse = 1;
        newChunk->next = NULL;
        prev->next = newChunk;
    }
}

/* 
* Searches cell to find appropriate insert point.
* Shifts values to make room, then inserts value.
*/
void PQueue::shiftAdd(Cell *cell, int newValue)
{
    int index; // point of insertion
    for (index = 0; index < MaxElemsPerBlock; index++)
    {
        if (newValue > cell->values[index])
            break;
    }
    for (int i = MaxElemsPerBlock; i >= index; i--)
    {
        cell->values[i] = cell->values[i-1]; // shift values
    }
    cell->values[index] = newValue; // insert
    cell->BlocksInUse++;
}

/*
* Splits current Cell values between two Cells.
* Searches for insertion point, then adds value.
*/
void PQueue::splitAdd(Cell *cur, int newValue)
{
    Cell *newChunk = new Cell;
    newChunk->values = new int[MaxElemsPerBlock];
    for (int i = MaxElemsPerBlock/2; i < MaxElemsPerBlock; i++)
    {
        newChunk->values[i-(MaxElemsPerBlock/2)] = cur->values[i];
    }
    newChunk->BlocksInUse = MaxElemsPerBlock/2;
    cur->BlocksInUse = MaxElemsPerBlock/2;
    
    // insert into list
    newChunk->next = cur->next;
    cur->next = newChunk;

    // check older Cell for insertion point
    for (int j = 0; j < cur->BlocksInUse; j++)
    {
        if (newValue > cur->values[j])
        {
            shiftAdd(cur,newValue);
            return;
        }
    }

    // insert into new Cell
    for (int k = 0; k < newChunk->BlocksInUse; k++)
    {
        if (newValue > newChunk->values[k])
        {
            shiftAdd(newChunk,newValue);
            return;
        }        
    }
}

/*
* Initializes first Cell and enqueues first value.
*/
void PQueue::initPQueue(int newValue)
{
    Cell *newChunk = new Cell;
    newChunk->values = new int[MaxElemsPerBlock];
    newChunk->values[0] = newValue;
    newChunk->BlocksInUse = 1;
    newChunk->next = NULL;
    head = newChunk;
}

/*
* Returns and removes the largest (first) value in the list.
*/
int PQueue::dequeueMax()
{
    if (isEmpty())
        Error("Tried to dequeue max from an empty pqueue!");

    int num = head->values[0]; // value to return

    if (head->BlocksInUse > 1)
    {
        for (int i = 0; i < head->BlocksInUse-1; i++)
        {
            head->values[i] = head->values[i+1];
        }
    }

    head->BlocksInUse--;

    if (head->BlocksInUse == 0)
    {
         Cell *toBeDeleted = head;
         head = head->next;     // splice head cell out
        delete toBeDeleted;
    }

    return num;
}

int PQueue::bytesUsed()
{
    int total = sizeof(*this);
     for (Cell *cur = head; cur != NULL; cur = cur->next)
         total += sizeof(*cur);
     return total;
}

string PQueue::implementationName()
{
    return "chunk list";
}

void PQueue::printDebuggingInfo()
{
   int count = 0;

    cout << "------------------ START DEBUG INFO ------------------" << endl;
    for (Cell *cur = head; cur != NULL; cur = cur->next) {
        cout << "Cell #" << count << " (at address " << cur << ") values = ";
        for (int i = 0; i < cur->BlocksInUse; i++)
        {
            cout << cur->values[i] << " ";
        }
       cout << " next = " << cur->next << endl;
       count++;
    }
    cout << "------------------ END DEBUG INFO ------------------" << endl;
}