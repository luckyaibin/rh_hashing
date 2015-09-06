#ifndef __ROBIN_HASHING_H__
#define __ROBIN_HASHING_H__
#include <memory.h>
#include <stdio.h>

// hash node of hash table
struct hash_node
{
	int val;
	int hash_value;
};

hash_node* create_hash_table(unsigned int size)
{
	hash_node *ht = new hash_node[size];
	for (unsigned i=0;i<size;i++)
	{
		hash_node * node = &ht[i];
		node->val = 0;
		node->hash_value = -1;//-1 empty, -2 deleted, 0,1,2 ... DIB£¨distance to initial bucket)
	}
	memset(ht,0,sizeof(hash_node)*size);
	return ht;
}

// hash function 
int hash_function(int v,int hash_size)
{
	v = v*hash_size + (v << 5);
	v = v % hash_size;
	return v;
}

int insert(hash_node * ht,int size,int val)
{
	int hv = hash_function(val,size);

	if (ht[hv].hash_value )
	{
	}

	return hv;
}


#endif