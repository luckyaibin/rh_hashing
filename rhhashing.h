#ifndef __ROBIN_HASHING_H__
#define __ROBIN_HASHING_H__
#include <memory.h>

// hash node of hash table
struct hash_node
{
	int val;
	int hash_value;
};

hash_node* create_hash_table(int size)
{
	hash_node *ht = new hash_node[size];
	memset(ht,0,sizeof(hash_node)*size);
	return ht;
}

// hash function 
int hash_function(int v,int hash_size)
{
	int v = v + v << 5;
	v = v % hash_size;
	return v;
}

int insert(hash_node * ht,int size,int val)
{
	int hv = hash_function(val,size);


	return hv;
}


#endif