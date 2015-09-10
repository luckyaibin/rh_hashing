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
		printf("\t[%d,%d,%d-%d]",hn.val,hn.hash_value,i - hn.hash_value,hn.serial_id);
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
inline int hash_function(int v,int hash_size)
{
	v = v + (v << 5);
	v = v % hash_size;
	//v = v & 0xefffffff;
	return v;
}


int insert(hash_node ht[],int size,int val)
{
	int inserted_hash_value = hash_function(val,size);
	int table_pos = inserted_hash_value;
	while (true)
	{
		table_pos = table_pos % size;
		int table_hash = ht[table_pos].hash_value;
		//没有元素
		if (table_hash<0)
		{
			//empty
			if (table_hash == -1)
			{
				ht[table_pos].val = val;
				ht[table_pos].hash_value = inserted_hash_value;
				return table_pos;
			}
			//deleted
			else if (table_hash == -2)
			{
				ht[table_pos].val = val;
				ht[table_pos].hash_value = inserted_hash_value;
				return table_pos;
			}
		}

		//注意！！！下面两处之所以要table_pos + size - table_hash ，加上size，是为了保证当回环到起始点的时候计算得到的值仍然是正确的，否则可能出现负值
		//有元素
		//table_pos永远是大于等于table_hash的！因为所有元素都是向后错位排列的
		int table_dib = (table_pos + size - table_hash)%size;

		//table_pos则不一定大于 inserted_hash_value，正常情况下是大于的，但是当从hash_table最后回转到hash_table起始点时候
		//table_pos 小于inserted_hash_value
		int inserted_dib = (table_pos  + size - inserted_hash_value) % size;
		
		/*
		hash table: 0	1	2	3	4	5	6	7	8	9
		hash value  8	1	1	1	2	4	-2	5	-1	8
		dib		   -8	0	1	2	2	1	8	2	9	1

		-8不对，其实应该是 （-8+10）%10 = 2
		*/

		//swap，相当于把新元素new_node插入到当前table_pos的位置，相当于交换new_node和table_pos上的old_node，然后把old_node向后挪动
		if(table_dib < inserted_dib)
		{
			//保存被插入位置的原始值
			hash_node tmp_hash_node = ht[table_pos];
		
			//插入到表中
			ht[table_pos].hash_value = inserted_hash_value;
			ht[table_pos].val		 = val;

			//把表中的值变成需要插入的值
			//结点变成了新的hash值
			inserted_hash_value = tmp_hash_node.hash_value;
			val = tmp_hash_node.val;
		}
		table_pos++;
	}
}

int find_index(hash_node ht[],int size,int val)
{
	int hash_value = hash_function(val,size);
	int table_pos = hash_value;
	int find_len = 0;
	while(true)
	{
		int dib = (table_pos + size - hash_value)%size;

		//empty
		if (ht[table_pos].hash_value == -1)
		{
			return -1;
		}//当dib突然变得小于起始点应有的dib的时候，说明已经是其他值的部分了，遇到的-2也不用特殊处理
		else if (dib < find_len)
		{
			return -1;
		}
		else if (ht[table_pos].hash_value == hash_value && ht[table_pos].val == val)
		{
			return table_pos;
		}
		table_pos++;
		find_len++;
	}
}
int insert_old(hash_node * ht,int size,int val)
{
	int hv = hash_function(val,size);

	hash_node *curr_insert = create_hash_node();
	curr_insert->val = val;
	curr_insert->hash_value = hv;

	hash_node * curr_entry = &ht[hv];
	int curr_insert_dib = 0;
	//当前插入结点hn距离初始位置的距离大于hashtable里的结点curr_entry时，继续往后寻找
	while (true)
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