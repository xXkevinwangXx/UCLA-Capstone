#include "Sequence.h"
#include <stdlib.h>
using namespace std;


//Constructor Implementation
Sequence::Sequence(): h(nullptr), t(nullptr), seqsize(0)
{
}

Sequence::~Sequence() //Destructor Implementation
{
	for (Node* p = h; p != nullptr; p = h)
	{
		h = p->next;
		delete p;
	}
}

Sequence::Sequence(const Sequence& og) //copy constructor
{
	seqsize = og.seqsize; //this holds for all cases
	if (og.seqsize == 0)  //in case of empty sequence
	{
		h = nullptr;
		t = nullptr;
	}
	else
	{
		h = new Node; //point head to a new node
		h->prev = nullptr;
		h->next = nullptr;
		h->val = og.h->val;

		Node* p = og.h; //hand pointers
		Node* q = h;

		while (p->next != nullptr)
		{
			q->next = new Node; //create node
			q->next->prev = q;	//make sure current address is stored in next
			q = q->next; //advance pointers
			p = p->next;
			q->val = p->val; //set value
		}
		q->next = nullptr;
		t = q;
	}
}

Sequence& Sequence::operator = (const Sequence& rhs) //assignment constructor
{
	if (this != &rhs)
	{
		Sequence temp = rhs;
		swap(temp);
	}
	return *this;
}

bool Sequence::empty() const //empty implementation
{
	return (seqsize == 0);
}

int Sequence::size() const //size implementation
{
	return seqsize;
}

bool Sequence::insert(const ItemType& value) //insert implementation
{
		//empty case
		if (size() == 0)
		{
			h = new Node;
			h->prev = nullptr;
			h->next = nullptr;
			h->val = value;
			t = h;
      return true;
		}
		//head case
		else
		{
			h->prev = new Node;
			h->prev->next = h;
			h = h->prev;
			h->prev = nullptr;
			h->val = value;
      return true;
		}
}

bool Sequence::erase()	//erase function implementation
{
		//only one item
		if (seqsize == 1)
		{
			delete t;
			h = nullptr;
			t = nullptr;
		}
		//removing from end
		else
		{
			Node* temp = t;
			t = t->prev;
			t->next = nullptr;
			delete temp;
		}
		seqsize--;
		return true;
}

void Sequence::swap(Sequence& other) //swap function implementation
{
	Node* temph = h; //temporary storage of og pointers and properties
	Node* tempt = t;
	int tempsize = seqsize;

	h = other.h;	//pointer and property swap
	t = other.t;
	seqsize = other.seqsize;

	other.h = temph;
	other.t = tempt;
	other.seqsize = tempsize;
}

bool Sequence::pop(ItemType &mov)
{
  if (seqsize <1)
    return false;
  else
  {
    mov = h->val;
    Node* temp = h;
	  h = h->next;
	  h->prev = nullptr;
	  delete temp;
    seqsize--;
    return true;
  }
}