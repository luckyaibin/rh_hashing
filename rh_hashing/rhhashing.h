#ifndef __ROBIN_HASHING_H__
#define __ROBIN_HASHING_H__
#include <memory.h>
#include <stdio.h>

// hash node of hash table
struct hash_node
{
	int val;
	int hash_value;
	int serial_id;
	
};
extern int g_serial_id;
hash_node* create_hash_node()
{
	hash_node* hn = new hash_node;
	hn->serial_id = ++g_serial_id;
	return hn;
}

void dump_hash_table(hash_node  ht[],int size)
{
	for (int i=0;i<size;i++)
	{
		hash_node hn = ht[i];
		printf("\t[%d,%d,%d]",hn.val,hn.hash_value,hn.serial_id);
	}
	printf("\n\n");
}

hash_node* create_hash_table(unsigned int size)
{
	hash_node *ht = new hash_node[size];
	for (unsigned i=0;i<size;i++)
	{
		hash_node * node = &ht[i];
		node->val = 0;
		node->hash_value = -1;//-1 empty, -2 deleted, 0,1,2 ... DIB（distance to initial bucket)
		node->serial_id = ++g_serial_id;
	}
	return ht;
}

// hash function 
int hash_function(int v,int hash_size)
{
	v = v + (v << 5);
	v = v % hash_size;
	//v = v & 0xefffffff;
	return v;
}


int insert(hash_node * ht,int size,int val)
{
	int hv = hash_function(val,size);
	
	hash_node *curr_insert = create_hash_node();
	curr_insert->val = val;
	curr_insert->hash_value = hv;

	hash_node * curr_entry = &ht[hv];
	int curr_insert_dib = 0;
	//当前插入结点hn距离初始位置的距离大于hashtable里的结点curr_entry时，继续往后寻找
	while ( hv < size)
	{ 
		while( hv - curr_entry->hash_value>= curr_insert_dib && hv<size)
		{
			if (curr_entry->hash_value<0)
			{
				goto find;
			}
			hv++;
			curr_entry = &ht[hv];
			curr_insert_dib++;
		}
		if (hv == size)
		{
			return NULL;
		}
		//swap curr_entry and curr_insert
		hash_node tmp = *curr_entry;
		*curr_entry = *curr_insert;
		*curr_insert = tmp;	
		curr_insert_dib = hv - curr_entry->hash_value;
	}
	find:
	//find position of deleted element.Insert it directly.
	if (curr_entry->hash_value == -2 )
	{
		*curr_entry = *curr_insert;
		//break;
	}
	//find empty position ,insert it directly.
	if (curr_entry->hash_value == -1 )
	{
		*curr_entry = *curr_insert;
		//break;
	}

	return hv;
}


#endif