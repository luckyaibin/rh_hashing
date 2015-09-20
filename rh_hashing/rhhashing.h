#ifndef __ROBIN_HASHING_H__
#define __ROBIN_HASHING_H__
#include <memory.h>
#include <stdio.h>
#include <malloc.h>
#if 1
// hash node of hash table
struct hash_node
{
	int key;
	int value;
	int hash_value;
	int serial_id;
};

struct hash_table
{
	int initial_size;
	int size;		//hashtable的bucket大小
	int element_num;//有效元素大小
	float load_factor_percent;
	hash_node hn[256];
};
extern int g_serial_id;
hash_node* create_hash_node()
{
	hash_node* hn = new hash_node;
	hn->serial_id = ++g_serial_id;
	return hn;
}


hash_table* create_hash_table(unsigned int initial_size = 256,float load_factor_percent=0.9f )
{
	hash_table *ht= (hash_table*)malloc(sizeof(hash_table)+initial_size);
	ht->size = initial_size;
	ht->element_num = 0;
	ht->load_factor_percent = load_factor_percent;

	memset(ht->hn,0,sizeof(hash_node)*initial_size);
	return ht;
}
// hash function 
inline int hash_function(int v,int hash_size)
{
	v = v + (v << 5);
	v = v % hash_size;
	v |= v==0;
	//v = v & 0xefffffff;
	return v;
}
void dump_hash_table(hash_table * ht)
{
	for (int i=0;i<ht->size;i++)
	{
		if (i%4 == 0)
		{
			printf("\n");
		}
		if (ht->hn[i].hash_value > 0)
		{
			printf("[%d]=%d,",ht->hn[i].key,ht->hn[i].value);
		}
		/*if (ht->hn[i].hash_value > 0)
		{
		printf("\t%d[%d,%d]%d,",i,ht->hn[i].hash_value,ht->hn[i].key,(ht->size + i - hash_function(ht->hn[i].key,ht->size))%ht->size);
		}*/
		
		//printf("\t[%d,%d,(%d)]",ht->hn[i].key,ht->hn[i].hash_value,i - ht->hn[i].hash_value);
		//printf("\t[%d,%d,%d-%d]",ht->hn[i].key,ht->hn[i].hash_value,i - ht->hn[i].hash_value,ht->hn[i].serial_id);
	}
	printf("\n\n");
}




//start_table_pos起始查找索引
int rhht_insert(hash_table *ht,int key,int value,int start_table_pos=0)
{
	int size = ht->size;
	int inserted_hash_value = hash_function(key,size);
	int table_pos = inserted_hash_value;
	if (start_table_pos)
		table_pos = start_table_pos;
	while (true)
	{
		table_pos = table_pos % size;
		int table_hash = ht->hn[table_pos].hash_value;
		//没有元素
			//empty
			if (table_hash == 0)
			{
				ht->hn[table_pos].key = key;
				ht->hn[table_pos].value = value;
				ht->hn[table_pos].hash_value = inserted_hash_value;
				ht->element_num++;
				return table_pos;
			}
			////deleted
			//else if (table_hash == -2)
			//{
			//	ht->hn[table_pos].key = key;
			//	ht->hn[table_pos].hash_value = inserted_hash_value;
			//	ht->element_num++;
			//	return table_pos;
			//}

		//注意！！！下面两处之所以要table_pos + size - table_hash ，加上size，是为了保证当回环到起始点的时候计算得到的值仍然是正确的，否则可能出现负值
		//有元素
		//table_pos永远是大于等于table_hash的！因为所有元素都是向后错位排列的
		int table_dib = (table_pos + size - table_hash & 0x7FFFFFFFU)%size;

		//table_pos则不一定大于 inserted_hash_value，正常情况下是大于的，但是当从hash_table最后回转到hash_table起始点时候
		//table_pos 小于inserted_hash_value
		int inserted_dib = (table_pos  + size - (inserted_hash_value & 0x7FFFFFFFU) ) % size;
		
		/*
		hash table: 0	1	2	3	4	5	6	7	8	9
		hash value  8	1	1	1	2	4	-2	5	-1	8
		table_dib   -8	0	1	2	2	1	8	2	9	1

		-8不对，其实应该是 （-8+10）%10 = 2
		*/

		//swap，相当于把新元素new_node插入到当前table_pos的位置，相当于交换new_node和table_pos上的old_node，然后把old_node向后挪动
		if(table_dib < inserted_dib)
		{
			//deleted,不能放在上面
			if (table_hash & 0x80000000)
			{
				ht->hn[table_pos].key = key;
				ht->hn[table_pos].value = value;
				ht->hn[table_pos].hash_value = inserted_hash_value;
				ht->element_num++;
				return table_pos;
			}
			//保存被插入位置的原始值
			hash_node tmp_hash_node = ht->hn[table_pos];
		
			//插入到表中
			ht->hn[table_pos].hash_value = inserted_hash_value;
			ht->hn[table_pos].key		 = key;
			ht->hn[table_pos].value		 = value;

			//把表中的值变成需要插入的值
			//结点变成了新的hash值
			inserted_hash_value = tmp_hash_node.hash_value;
			key = tmp_hash_node.key;
			value = tmp_hash_node.value;
		}
		table_pos++;
	}
}


//find 是用key来查找
int __rhht_find(hash_table *ht,int key,int *endpos=NULL)
{
	int size = ht->size;
	int hash_value = hash_function(key,size);
	int table_pos = hash_value;
	int find_len = 0;
	while(true)
	{
		int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;

		//empty
		if (ht->hn[table_pos].hash_value == 0)
		{	
			if (endpos)
				*endpos = table_pos;
			return -1;
		}//当dib突然变得小于起始点应有的dib的时候，说明已经是其他值的部分了，遇到的-2也不用特殊处理
		else if (dib < find_len)
		{
			if (endpos)
				*endpos = table_pos;
			return -1;
		}
		else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key)
		{
			if (endpos)
				*endpos = table_pos;
			return table_pos;
		}
		table_pos++;
		find_len++;
	}
}
//同一个值只存在一个,且不覆盖之前的值
int rhht_unique_insert(hash_table *ht,int val)
{
	int endpos = 0;
	int index = __rhht_find(ht,val,&endpos);
	//不存在才插入，根据返回的endpos可以加快插入的速度
	if (index==-1)
	{
		index = rhht_insert(ht,val,endpos);
		ht->size++;
	}
	return index;

}

//同一个值只存在一个,且覆盖之前存在的值
int rhht_unique_overwrite_insert(hash_table *ht,int val)
{
	int endpos = 0;
	int index = __rhht_find(ht,val,&endpos);
	//不存在才插入，根据返回的endpos可以加快插入的速度
	if (index==-1)
	{
		index = rhht_insert(ht,val,endpos);
		ht->size++;
	}
	//存在，直接覆盖
	else
	{
		ht->hn[index].val = val
	}
	return index;

}
//同一个值只存在一个,且不覆盖之前的值
int rhht_unique_insert(hash_table *ht,int key,int value)
{
	int endpos = 0;
	int index = __rhht_find(ht,key,&endpos);
	//不存在才插入，根据返回的endpos可以加快插入的速度
	if (index==-1)
	{
		index = rhht_insert(ht,key,value,endpos);
		ht->element_num++;
	}
	return index;

}

//同一个值只存在一个,且覆盖之前存在的值
int rhht_unique_overwrite_insert(hash_table *ht,int key,int value)
{
	int endpos = 0;
	int index = __rhht_find(ht,key,&endpos);
	//不存在才插入，根据返回的endpos可以加快插入的速度
	if (index==-1)
	{
		index = rhht_insert(ht,key,value,endpos);
		ht->element_num++;
	}
	//存在，直接覆盖
	else
	{
		ht->hn[index].key = key;
		ht->hn[index].value = value;
	}
	return index;

}

int rhht_remove(hash_table *ht,int key)
{
	int index = __rhht_find(ht,key);
	if (index == -1)
	{
		return -1;
	}
	ht->hn[index].hash_value |= 0x80000000;//flag it as deleted
	ht->element_num--;
	return index;
}

#endif

#endif