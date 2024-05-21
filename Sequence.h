#ifndef SEQUENCE_H
#define SEQUENCE_H

class move
{
  public:
    int mot1;
    int mot2;
    int mot3;
    int mot4;
    int turn;
};

using ItemType = move;

class Sequence
{
public:
    Sequence();    // Create an empty sequence (i.e., one whose size() is 0).
    ~Sequence(); //destructor
    Sequence(const Sequence& og); //copy constructor
    Sequence& operator = (const Sequence& rhs); //assignment constructor
    bool empty() const;  // Return true if the sequence is empty, otherwise false.
    int size() const;    // Return the number of items in the sequence.
    bool insert(const ItemType& value);
    // Insert value into the sequence so that it becomes the item at
    // position pos.  The original item at position pos and those that
    // follow it end up at positions one greater than they were at before.
    // Return pos if 0 <= pos <= size() and the value could be
    // inserted.  (It might not be, if the sequence has a fixed capacity,
    // e.g., because it's implemented using a fixed-size array.)  Otherwise,
    // leave the sequence unchanged and return -1.  Notice that
    // if pos is equal to size(), the value is inserted at the end.

    bool erase();
    // If 0 <= pos < size(), remove the item at position pos from
    // the sequence (so that all items that followed that item end up at
    // positions one lower than they were at before), and return true.
    // Otherwise, leave the sequence unchanged and return false.

    void swap(Sequence& other);
    // Exchange the contents of this sequence with the other one.

    bool pop(ItemType &mov);


private:
    struct Node
    {
        ItemType val;
        Node* next;
        Node* prev;
    };

    Node* h;
    Node* t;
    int seqsize;
};

#endif