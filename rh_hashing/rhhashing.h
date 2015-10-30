﻿#if 0

#ifndef __ROBIN_HASHING_H__
#define __ROBIN_HASHING_H__
#include <memory.h>
#include <stdio.h>
#include <malloc.h>



#ifndef min
#define min(a,b) ((a)<=(b)?a:b)
#endif

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
	int element_num;	//有效元素大小
	float load_factor_percent;//负载因子：当element_num/capacity大于它是，需要增加内存了
	hash_node *hn;
};
int __rhht_insert_helper(hash_table *ht,int key,int value,int start_table_pos=-1);

//创建一个hashtable
//initial_size 初始的capacity大小，默认为256
//load_factor_percent,负载因子，默认0.9
hash_table* create_hash_table(unsigned int initial_size = 256,float load_factor_percent=0.9f )
{
	hash_table *ht= (hash_table*)malloc(sizeof(hash_table));
	ht->capacity = initial_size;
	ht->load_factor_percent = load_factor_percent;
	ht->treshold = int(ht->capacity * ht->load_factor_percent);
	//至少要有2个空白，保证插入之后还留有一个空白的位置，否则如果load_factor_percent很接近1.0f,那么hashtable塞满了同一个值，__rhht_find_helper将死循环
	if (ht->capacity - ht->treshold < 2)
	{
		ht->treshold = ht->capacity - 2;
	}
	ht->hn = (hash_node *)malloc(sizeof(hash_node) * initial_size);
	ht->element_num = 0;
	memset(ht->hn,0,sizeof(hash_node)*initial_size);
	return ht;
}

int __get_hash(hash_table* ht,int index)
{
	int hash_value = ht->hn[index].hash_value & 0x7FFFFFFF;
	return hash_value;
}

int __get_dib(hash_table* ht,int index)
{
	int dib = (index + ht->capacity + ht->hn[index].hash_value & 0x7FFFFFFF  ) % ht->capacity;
	return dib;
}

//检查当前元素数量，是否达到了阈值，达到了则增加大小到原来的factor倍，默认为1.5
int __rhht_check_increase_to(hash_table *ht,float increase_factor = 1.50f)
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
	//至少要有2个空白，保证插入之后还留有一个空白的位置，否则如果load_factor_percent很接近1.0f,那么hashtable塞满了同一个值，__rhht_find_helper将死循环
	if (ht->capacity - ht->treshold < 2)
	{
		ht->treshold = ht->capacity - 2;
	}
	//拷贝原来的值
	for (int i=0;i<old_capacity;i++)
	{
		if (old_hn[i].hash_value > 0)
		{
			__rhht_insert_helper(ht,old_hn[i].key,old_hn[i].value);
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
	v |= v==0;//绝对不能不返回hash值0
	return v;
}

void dump_hash_table(hash_table * ht)
{
	static int g_dump_serial=0;
	printf(" g_dump_serial is %d\n",g_dump_serial++);
	int out_put_count = -1;
	for (int i=0;i<ht->capacity;i++)
	{
		if (ht->hn[i].hash_value > 0)
		{
			printf("[index%d:hash_value%d,dib%d~key%d,value%d]	\n",i,ht->hn[i].hash_value,(ht->capacity + i - ht->hn[i].hash_value) % ht->capacity,ht->hn[i].key,ht->hn[i].value);
			out_put_count++;
		}
		if (out_put_count%4 == 3)
		{
			//printf("\n");
		}
	}
	printf("\n\n");
}

//向hashtable中插入一个值，不管里面是否已经存在这个值
//start_table_pos_instinct是函数开始查找索引的，用来加快查找速度
//（start_table_pos_instinct在rhht_unique_insert和rhht_unique_overrite_insert中使用，这两个函数会给start_table_pos_instinct赋值，
//也就是key的哈希值在哈希表中的真实位置，从而不用再从key对应的hashvalue的位置开始位置遍历到真实位置)
int __rhht_insert_helper(hash_table *ht,int key,int value,int start_table_pos_instinct)
{
	int is_increased = __rhht_check_increase_to(ht);
	int size = ht->capacity;
	int inserted_hash_value = hash_function(key,size);
	int table_pos = inserted_hash_value;
	//重新分配了大小的话，启发式的start_table_pos就不能用了，失效了
	if ((!is_increased) && (-1!=start_table_pos_instinct))
	{
		table_pos = start_table_pos_instinct;
		//printf("start_table_pos_instinct: %d ,save steps: %d \n",start_table_pos_instinct,start_table_pos_instinct-inserted_hash_value);
	}
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
		//table_pos永远是大于等于table_hash的！因为所有元素都是向后错位排列的
		int table_dib = (table_pos + size - table_hash & 0x7FFFFFFFU)%size;

		//table_pos则不一定大于 inserted_hash_value，正常情况下是大于的，但是当从hash_table最后回转到hash_table起始点时候
		//table_pos 小于inserted_hash_value
		int inserted_dib = (table_pos  + size - (inserted_hash_value & 0x7FFFFFFFU) )%size;
		//swap，相当于把新元素new_node插入到当前table_pos的位置，相当于交换new_node和table_pos上的old_node，然后把old_node向后挪动
		//这样old_node离它的初始位置就变远了1个位置，相当于new_node抢夺了old_node的“好位置”，也就是robinhood hashing名字的由来
		if(table_dib < inserted_dib)
		{
			//deleted
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


//find 是用key来查找 hashtable是否存在key，如果存在则返回这个key所在的位置，不存在返回-1
//insertpos_output是key对应有相同hashvalue的第一个位置，不能直接用key的位置加快插入，因为不同的key可能有相同的hashvalue
int __rhht_find_helper(hash_table *ht,int key,int *insertpos_output=NULL)
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
			return -1;
		}//当dib突然变得小于起始点应有的dib的时候，说明已经是其他值的部分了
		if (dib < find_len)
		{
			return -1;
		}
		if (ht->hn[table_pos].hash_value == hash_value)
		{
			//记录第一个hash_value等于需要查找值的hash的位置，可以加快插入
			if (insertpos_output && (*insertpos_output == -1) )
				*insertpos_output = table_pos;
			if (ht->hn[table_pos].key == key)
			{
				return table_pos;
			}
		}
		
		table_pos++;
		table_pos %= size;
		find_len++;
	}
}

//可以插入重复的key值
int rhht_multi_insert(hash_table *ht,int key,int value)
{
	return __rhht_insert_helper(ht,key,value);
}

//同一个值只存在一个,如果之前存在这个值不进行插入而是直接返回，不会覆盖之前的值
int rhht_unique_insert(hash_table *ht,int key,int value)
{
	int endpos = -1;
	int index = __rhht_find_helper(ht,key,&endpos);
	//不存在才插入，根据返回的endpos可以加快插入的速度
	if (index==-1)
	{
		index = __rhht_insert_helper(ht,key,value,endpos);
	}
	return index;
}

//同一个值只存在一个,如果不存在直接插入，如果已经存在，则覆盖之前存在的值
int rhht_unique_overwrite_insert(hash_table *ht,int key,int value)
{
	int endpos = -1;
	int index = __rhht_find_helper(ht,key,&endpos);
	//不存在才插入，根据返回的endpos可以加快插入的速度
	if (index==-1)
	{
		index = __rhht_insert_helper(ht,key,value,endpos);
	}
	//存在，直接覆盖
	else
	{
		ht->hn[index].key = key;
		ht->hn[index].value = value;
	}
	return index;
}


//没有使用backshift delete
int __rhht_remove_one_helper(hash_table *ht,int key)
{
	int shift_start,shift_end;
	int index = __rhht_find_helper(ht,key);
	if (index == -1)
		return -1;

	ht->hn[index].hash_value |= 0x80000000;//flag it as deleted
	ht->element_num--;

	return index;
}

//key来查找,
//findpos_output是查找成功和查找失败时的当前索引。
//shift_endpos_output是backshift删除时需要移动数据块的索引。
//将来需要移动 findpos_output ~ shift_endpos_output 之间[findpos_output,shift_endpos_output)的数据块,shift_endpos_output则设置为空
//这个函数找到key对应的元素位置和需要左移的数据范围（左移并删除一个元素）
int __rhht_find_helper_bs_move_range(hash_table *ht,int key,int *findpos_output=NULL,int *shift_endpos_output=NULL)
{
	int find_result = -1;
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
			if (findpos_output)
				*findpos_output = table_pos;
			find_result = -1;
			break;
		}//当dib突然变得小于起始点应有的dib的时候，说明已经是其他值的部分了
		else if (dib < find_len)
		{
			if (findpos_output)
				*findpos_output = table_pos;
			find_result = -1;
			break;
		}
		else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key)
		{
			if (findpos_output)
				*findpos_output = table_pos;
			find_result =  table_pos;
			break;
		}
		table_pos++;
		table_pos %= size;
		find_len++;
	}

	//-1：表示不存在，那么findpos_output是有意义的，shift_endpos_output是无意义的
	//不为-1：表示存在，要找到需要被shift的结尾的位置 findpos_output是有意义的，shift_endpos_output是有意义的
	if(find_result != -1)
	{
		//需要find_result2是需要shiftback的结束的索引
		int find_result2 = find_result;
		if (shift_endpos_output)
			*shift_endpos_output = find_result2;
		while (true)
		{
			//empty
			if (ht->hn[find_result2].hash_value == 0)
				break;
			//dib == 0,有可能find_result就是dib为0的，要判断后面的
			int dib = (find_result2 + size - ht->hn[find_result2].hash_value & 0x7FFFFFFF) % size;
			if ( (find_result2!=find_result) &&  (dib == 0) )
				break;
			if (shift_endpos_output)
				*shift_endpos_output = find_result2;
			find_result2++;
			find_result2 %= size;
		}
	}
	return find_result;
}
//先找出需要移动的起始索引，然后再移动
int __rhht_remove_one_helper_bs(hash_table *ht,int key)
{
	int shift_start,shift_end;
	int index = __rhht_find_helper_bs_move_range(ht,key,&shift_start,&shift_end);
	if (index == -1)
		return -1;

	//do the shift
	int size = ht->capacity;
	int shift_index = shift_start;
	int shift_count = 0;
	while( shift_index != shift_end)
	{
		shift_count ++;
		ht->hn[shift_index] = ht->hn[ (shift_index+1) % size];
		shift_index = (shift_index+1)%size;
	}
	if (shift_count>0)
	{
		//printf("shifted count : %d \n",shift_count);
	}
	//最后一个标记为空
	ht->hn[shift_end].hash_value = 0x00000000;//flag it as empty
	ht->element_num--;

	return index;
}

//查找并删除key这个值，并且在查找过程中直接shift相应的元素
//remove_count返回删除的数量，
//remove_all 为0：不删除所有相同的值。不为0：要删除所有相同的值
int __rhht_backshift_remove_on_fly(hash_table *ht,int key,int* remove_count=NULL,int remove_all=0)
{
	int find_result = -1;
	int size = ht->capacity;
	int hash_value = hash_function(key,size);
	int table_pos = hash_value;
	int find_len = 0;
	if (remove_count)
	{
		*remove_count = 0;
	}
	while(true)
	{
		int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;
		//empty
		if (ht->hn[table_pos].hash_value == 0)
		{	
			find_result = -1;
			break;
		}//当dib突然变得小于起始点应有的dib的时候，说明已经是其他值的部分了
		else if (dib < find_len)
		{
			find_result = -1;
			break;
		}
		else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key)
		{
			find_result =  table_pos;
			break;
		}
		table_pos++;
		table_pos %= size;
		find_len++;
	}
	//
	//-1：表示不存在
	if(find_result != -1)
	{
		//先直接删掉，不管后面的shift是否覆盖了他
		ht->hn[find_result].hash_value = 0;
		if (remove_count)
			*remove_count += 1;
		//从下一个开始shift
		int shift_index = (find_result+1)%size;
		int need_shift_left_count = 1;//当前shift_index所在元素需要向左移动的数量
		while (true)
		{
			//empty
			if (ht->hn[shift_index].hash_value == 0)
				break;
			//dib == 0,有可能find_result就是dib为0的，要判断后面的
			int dib = (shift_index + size - ht->hn[shift_index].hash_value & 0x7FFFFFFF) % size;
			if ( (shift_index!=find_result) &&  (dib == 0) )
				break;

			//相同的key，如果需要全部被删掉,标记为空，把需要向左移动的数量增加1，然后判断下一个元素
			if (remove_all && ht->hn[shift_index].key == key)
			{
				ht->hn[shift_index].hash_value = 0;
				need_shift_left_count++;
				if (remove_count)
					*remove_count += 1;
			}
			else//向左移动
			{
				//do the shift
				need_shift_left_count = min( dib,need_shift_left_count);
				if (need_shift_left_count>0)
				{
					int index = (shift_index + size - need_shift_left_count)%size;
					ht->hn[index] = ht->hn[shift_index];
					ht->hn[shift_index].hash_value = 0;
				}
			}
			shift_index++;
			shift_index %= size;
		}
	}
	return find_result;
}

int __rhht_remove_one_or_all_helper_bs(hash_table *ht,int key,int is_remove_all)
{
	int remove_count=0;
	int index = __rhht_backshift_remove_on_fly(ht,key,&remove_count,is_remove_all);
	if (index == -1)
		return -1;
	ht->element_num -= remove_count;
	return remove_count;
}


// not back shift delete,just mark it as deleted
//标记删除法
int rhht_remove_one(hash_table *ht,int key)
{
	int removed_count = __rhht_remove_one_helper(ht,key);
	if (-1==removed_count)
		return 0;
	return 1;
}

//back shift delete 删除
int rhht_remove_one_bs(hash_table *ht,int key)
{
	int removed_count =  __rhht_remove_one_or_all_helper_bs(ht,key,0); 
	return removed_count;
}

//back shift delete 的一个比较慢的算法
int rhht_remove_one_bs_slow(hash_table *ht,int key)
{
	int removed_count = __rhht_remove_one_helper_bs(ht,key); 
	if (-1==removed_count)
		return 0;
	return 1;
}

//标记法全部删除
int rhht_remove_all(hash_table *ht,int key)
{
	int removed_num = 0;
	int removed_index = __rhht_remove_one_helper(ht,key);
	while (-1 != removed_index)
	{
		++removed_num;
		removed_index = __rhht_remove_one_helper(ht,key);
	}
	return removed_num;
}

//backshift 删除法
int rhht_remove_all_bs(hash_table *ht,int key)
{
	int removed_num = __rhht_remove_one_or_all_helper_bs(ht,key,1);
	return removed_num;
}

//backshift 删除法中的一个比较慢的方式
int rhht_remove_all_bs_slow(hash_table *ht,int key)
{
	int removed_num = 0;	 
	int removed_index = __rhht_remove_one_helper_bs(ht,key);
	if(-1 != removed_index)
	{
		++removed_num;
		removed_index = __rhht_remove_one_helper_bs(ht,key);
	}
	return removed_num;
}

#endif

#endif