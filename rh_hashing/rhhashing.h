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
};

struct hash_table
{
	int capacity;		//hashtable的bucket大小
	int treshold;		//阈值大小，是根据load_factor_percent * capacity计算出来的
	int element_num;//有效元素大小
	float load_factor_percent;
	hash_node *hn;
};
int rhht_insert_helper(hash_table *ht,int key,int value,int start_table_pos=0);
hash_node* create_hash_node()
{
	hash_node* hn = new hash_node;
	return hn;
}

hash_table* create_hash_table(unsigned int initial_size = 256,float load_factor_percent=0.9f )
{
	hash_table *ht= (hash_table*)malloc(sizeof(hash_table));
	ht->capacity = initial_size;
	ht->load_factor_percent = load_factor_percent;
	ht->treshold = int(ht->capacity * ht->load_factor_percent);
	//至少有一个空位
	if (ht->capacity - ht->treshold < 1)
	{
		ht->treshold = ht->capacity - 1;
	}

	ht->hn = (hash_node *)malloc(sizeof(hash_node) * initial_size);
	ht->element_num = 0;
	memset(ht->hn,0,sizeof(hash_node)*initial_size);

	return ht;
}

//增加大小到原来的factor倍，默认为2
int rhht_check_increase_to(hash_table *ht,float increase_factor = 1.50f)
{
	//没达到极限大小
	if (ht->element_num < ht->treshold)
	{
		return 0;
	}

	
	//重新分配内存
	int old_capacity = ht->capacity;
	hash_node* old_hn   = ht->hn;

	int new_capacity = int(ht->capacity * increase_factor);
	hash_node *hn = (hash_node*)malloc(sizeof(hash_node) * new_capacity);
	memset(hn,0,sizeof(hash_node) * new_capacity);
	ht->hn = hn;
	ht->element_num = 0;
	ht->capacity = new_capacity;
	ht->treshold = int(ht->capacity * ht->load_factor_percent);
	printf("increased from %d to %d \n",old_capacity,new_capacity);
	//至少有一个空位，保证至少能够成功在插入，操作之后才可以增加capacity
	if (ht->capacity - ht->treshold < 1)
	{
		ht->treshold = ht->capacity - 1;
	}
	

	//拷贝原来的值
	for (int i=0;i<old_capacity;i++)
	{
		if (old_hn[i].hash_value > 0)
		{
			rhht_insert_helper(ht,old_hn[i].key,old_hn[i].value);
		}
	}
	free(old_hn);
	return ht->capacity;
}
// hash function 
inline int hash_function(int v,int hash_size)
{
	v = v + (v << 5);
	v = v % hash_size;
	v |= v==0;//不返回hash值0
	return v;
}
void dump_hash_table(hash_table * ht)
{
	static int g_dump_serial=0;
	printf(" g_dump_serial is %d\n",g_dump_serial++);
	for (int i=0;i<ht->capacity;i++)
	{
		if (i%4 == 0)
		{
			printf("\n");
		}
		if (ht->hn[i].hash_value > 0)
		{
			printf("[%d]=%d,",ht->hn[i].key,ht->hn[i].value);
		}
	}
	printf("\n\n");
}

//start_table_pos起始查找索引
int rhht_insert_helper(hash_table *ht,int key,int value,int start_table_pos_instinct)
{
	int is_increased = rhht_check_increase_to(ht);
	int size = ht->capacity;
	int inserted_hash_value = hash_function(key,size);
	int table_pos = inserted_hash_value;
	//重新分配了大小的话，启发式的start_table_pos就不能用了，失效了
	if ((!is_increased) && start_table_pos_instinct)
		table_pos = start_table_pos_instinct;
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
		//注意！！！下面两处之所以要table_pos + size - table_hash ，加上size，是为了保证当回环到起始点的时候计算得到的值仍然是正确的，否则可能出现负值
		//有元素
		//table_pos永远是大于等于table_hash的！因为所有元素都是向后错位排列的
		int table_dib = (table_pos + size - table_hash & 0x7FFFFFFFU)%size;

		//table_pos则不一定大于 inserted_hash_value，正常情况下是大于的，但是当从hash_table最后回转到hash_table起始点时候
		//table_pos 小于inserted_hash_value
		int inserted_dib = (table_pos  + size - (inserted_hash_value & 0x7FFFFFFFU) );
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
int __rhht_find_helper(hash_table *ht,int key,int *endpos_output=NULL)
{
	int size = ht->capacity;
	int hash_value = hash_function(key,size);
	int table_pos = hash_value;
	int find_len = 0;
	while(true)
	{
		int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;
		//empty
		if (ht->hn[table_pos].hash_value == 0)
		{	
			if (endpos_output)
				*endpos_output = table_pos;
			return -1;
		}//当dib突然变得小于起始点应有的dib的时候，说明已经是其他值的部分了
		else if (dib < find_len)
		{
			if (endpos_output)
				*endpos_output = table_pos;
			return -1;
		}
		else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key)
		{
			if (endpos_output)
				*endpos_output = table_pos;
			return table_pos;
		}
		table_pos++;
		table_pos %= size;
		find_len++;
	}
}

int rhht_multi_insert(hash_table *ht,int key,int value)
{
	return rhht_insert_helper(ht,key,value);
}

//同一个值只存在一个,且不覆盖之前的值
int rhht_unique_insert(hash_table *ht,int key,int value)
{
	int endpos = 0;
	int index = __rhht_find_helper(ht,key,&endpos);
	//不存在才插入，根据返回的endpos可以加快插入的速度
	if (index==-1)
	{
		index = rhht_insert_helper(ht,key,value,endpos);
	}
	return index;
}

//同一个值只存在一个,且覆盖之前存在的值
int rhht_unique_overwrite_insert(hash_table *ht,int key,int value)
{
	int endpos = 0;
	int index = __rhht_find_helper(ht,key,&endpos);
	//不存在才插入，根据返回的endpos可以加快插入的速度
	if (index==-1)
	{
		index = rhht_insert_helper(ht,key,value,endpos);
	}
	//存在，直接覆盖
	else
	{
		ht->hn[index].key = key;
		ht->hn[index].value = value;
	}
	return index;
}

int rhht_remove_helper(hash_table *ht,int key)
{
	int index = __rhht_find_helper(ht,key);
	if (index == -1)
		return -1;
	ht->hn[index].hash_value |= 0x80000000;//flag it as deleted
	ht->element_num--;
	return index;
}

int rhht_remove_one(hash_table *ht,int key)
{
	return  rhht_remove_helper(ht,key);
}

int rhht_remove_all(hash_table *ht,int key)
{
	int removed_num = 0;
	while (-1 != rhht_remove_helper(ht,key))
	{
		removed_num++;
	}
	return removed_num;
}

#endif

#endif