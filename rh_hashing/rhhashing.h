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

	hash_node * curr_node_p = &ht[hv];
	int curr_dib = 0;
	while (curr_node_p->hash_value < curr_dib && hv < size)
	{
		//find position of deleted element.Insert it directly.
		if (curr_node_p->hash_value == -2 )
		{
			curr_node_p->hash_value = curr_dib;
			curr_node_p->val = val;
			break;
		}
		if (curr_node_p->hash_value == -1 )
		{
			curr_node_p->hash_value = curr_dib;
			curr_node_p->val = val;
			break;
		}
		curr_node_p = &ht[hv++];
		curr_dib++;
	}
	

	return hv;
}


#endif