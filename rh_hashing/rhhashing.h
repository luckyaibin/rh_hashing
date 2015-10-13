#ifndef __ROBIN_HASHING_H__
#define __ROBIN_HASHING_H__
#include <memory.h>
#include <stdio.h>
#include <malloc.h>

#if 1

struct dynamic_int_array
{
	int size;
	int capacity;
	int buff_stack[256];
	int *buff_heap;
};

void dump_array(dynamic_int_array* arr)
{
	for (int i=0;i<arr->size;i++)
	{
		if (arr->buff_heap)
			printf("%d	",arr->buff_heap[i]);
		else
			printf("%d	",arr->buff_stack[i]);		
	}
	printf("\n\n\n");
}

int array_init(dynamic_int_array *arr)
{
	arr->size = 0;
	arr->capacity = 256;
	arr->buff_heap=NULL;
	return 0;
}

int array_deinit(dynamic_int_array *arr)
{
	arr->size = 0;
	arr->capacity = 256;
	if (arr->buff_heap)
		free(arr->buff_heap);
	arr->buff_heap=NULL;
	return 0;
}

int array_set(dynamic_int_array *arr,int index,int value)
{
	//需要重新分配堆内存
	if (index>=arr->capacity)
	{
		int *old_buff_heap = arr->buff_heap;
		int old_capacity = arr->capacity;
		arr->capacity = (index+1) * 2;
		//memeory
		arr->buff_heap = (int*)malloc(sizeof(int) * arr->capacity);
		//clear memory
		memset(arr->buff_heap,0,sizeof(int) * arr->capacity);
		//之前用的是堆内存，拷贝后释放
		if (old_buff_heap)
		{
			memcpy(arr->buff_heap,old_buff_heap,sizeof(int) * old_capacity);
			free(old_buff_heap);
		}
		else//之前用的是栈内存，只需要拷贝
		{
			memcpy(arr->buff_heap,arr->buff_stack,sizeof(int) * old_capacity);
		}
	}
	if (index >= arr->size)
	{
		arr->size++;
	}
	if (arr->buff_heap)
		arr->buff_heap[index] = value;
	else
		arr->buff_stack[index] = value;
	return 0;
}
int array_get(dynamic_int_array *arr,int index,int *value)
{
	//out of range.
	if (index >= arr->size)
		return -1;
	if (arr->buff_heap)
		*value = arr->buff_heap[index];
	else
		*value = arr->buff_stack[index];
}
int array_remove(dynamic_int_array *arr,int index,int *value)
{
	//out of range.
	if (index >= arr->size)
		return -1;
	//remove and shift
	if (arr->buff_heap)
	{
		if(value)
			*value = arr->buff_heap[index];
		//删除的不是最后一个，需要移动；否则删除的是最后一个，不需要移动
		if (index < arr->size-1)
		{
			memcpy(&arr->buff_heap[index],&arr->buff_heap[index+1],sizeof(int)*(arr->size-1 - index));
		}
	}
	else
	{
		if(value)
			*value = arr->buff_stack[index];
		//删除的不是最后一个，需要移动；否则删除的是最后一个，不需要移动
		if (index < arr->size-1)
		{
			memcpy(&arr->buff_stack[index],&arr->buff_stack[index+1],sizeof(int)*(arr->size-1 - index));
		}
	}
	arr->size--;
	return 0;
}



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
	dynamic_int_array different_hash_array;//记录下来需要shift的不同hash值的起始index，加快shift操作
};
int rhht_insert_helper(hash_table *ht,int key,int value,int start_table_pos=-1);
hash_node* create_hash_node()
{
	hash_node* hn = new hash_node;
	return hn;
}

inline void duplicate(hash_table* ht,hash_table** dest_ht)
{
	if (*dest_ht)
	{
		//free((*dest_ht)->hn);
		//free(*dest_ht);
	}
	hash_table *ht2 = (hash_table*)malloc(sizeof(hash_table));
	ht2->hn = (hash_node*)malloc(ht->capacity*sizeof(hash_node));

	memcpy(ht2,ht,sizeof(hash_table));
	memcpy(&ht2->hn[0],&ht->hn[0],ht->capacity*sizeof(hash_node));

	*dest_ht = ht2;
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

	array_init(&ht->different_hash_array);
	return ht;
}

//把 [start,end] 向左移动count个格子（覆盖start - count ~ start 之前的值
//[end - count，end]之间的清空
int __shift(hash_table* ht,int start,int end,int count)
{
	if (count<=0)
	{
		return -1;
	}
	if (start <= end)
	{
		int bytes = sizeof(ht->hn[0]) * count;
		memmove(&ht->hn[start] - bytes,&ht->hn[start],bytes);
		//清空
		memset(&ht->hn[end] - bytes,bytes,0);
	}
	else//绕过了结尾
	{
		int bytes = sizeof(ht->hn[0]) * (ht->capacity - start);
		memmove(&ht->hn[start] - bytes,&ht->hn[start],bytes);

		bytes = 
		memmove(&ht->hn[ht->capacity ,&ht-hn[0]
	}
	

	return 0;
}

int get_hash(hash_table* ht,int index)
{
	int hash_value = ht->hn[index].hash_value & 0x7FFFFFFF;
	return hash_value;
}

int get_dib(hash_table* ht,int index)
{
	int dib = (index + ht->capacity + ht->hn[index].hash_value & 0x7FFFFFFF  ) % ht->capacity;
	return dib;
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
//inline int hash_function(int v,int hash_size)
//{
//	v = v % hash_size;
//	v |= v==0;//不返回hash值0
//	return v;
//}
void dump_hash_table(hash_table * ht)
{
	static int g_dump_serial=0;
	printf(" g_dump_serial is %d\n",g_dump_serial++);
	int out_put_count = -1;
	for (int i=0;i<ht->capacity;i++)
	{
		//if (ht->hn[i].hash_value > 0)
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

//start_table_pos起始查找索引
int rhht_insert_helper(hash_table *ht,int key,int value,int start_table_pos_instinct)
{
	int is_increased = rhht_check_increase_to(ht);
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
		//有元素
		//table_pos永远是大于等于table_hash的！因为所有元素都是向后错位排列的
		int table_dib = (table_pos + size - table_hash & 0x7FFFFFFFU)%size;

		//table_pos则不一定大于 inserted_hash_value，正常情况下是大于的，但是当从hash_table最后回转到hash_table起始点时候
		//table_pos 小于inserted_hash_value
		int inserted_dib = (table_pos  + size - (inserted_hash_value & 0x7FFFFFFFU) )%size;
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

int rhht_multi_insert(hash_table *ht,int key,int value)
{
	return rhht_insert_helper(ht,key,value);
}

//同一个值只存在一个,且不覆盖之前的值
int rhht_unique_insert(hash_table *ht,int key,int value)
{
	int endpos = -1;
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
	int endpos = -1;
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



//find 是用key来查找 findpos_output是查找成功和查找失败时的当前索引。shift_endpos_output是backshift删除时需要移动数据块的索引。
//将来需要移动 findpos_output ~ shift_endpos_output 之间[findpos_output,shift_endpos_output)的数据块,shift_endpos_output则设置为空
int __rhht_find_helper_for_backshift_remove(hash_table *ht,int key,int *findpos_output=NULL,int *shift_endpos_output=NULL)
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
	//TODO:不是每次删除都要执行back_shift，而是当dib大于某个值，比如5 的时候，才需要shift，看能否实现这个？
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

//find 是用key来查找 findpos_output是查找成功和查找失败时的当前索引。shift_endpos_output是backshift删除时需要移动数据块的索引。
//将来需要移动 findpos_output ~ shift_endpos_output 之间[findpos_output,shift_endpos_output)的数据块,shift_endpos_output则设置为空
//
int __rhht_find_helper_for_backshift_remove_all(hash_table *ht,int key,int *findpos_output=NULL,int *shift_endpos_output=NULL)
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

	//标记是否找到了一个hash值的起始索引
	int curr_hash = 0;
	int curr_hash_begin_index = -1;
	int curr_hash_end_index = -1;
	//-1：表示不存在，那么findpos_output是有意义的，shift_endpos_output是无意义的
	//不为-1：表示存在，要找到需要被shift的结尾的位置 findpos_output是有意义的，shift_endpos_output是有意义的
	//TODO:不是每次删除都要执行back_shift，而是当dib大于某个值，比如5 的时候，才需要shift，看能否实现这个？
	if(find_result != -1)
	{
		//需要find_result2是需要shiftback的结束的索引
		int find_result2 = find_result;
		if (shift_endpos_output)
			*shift_endpos_output = find_result2;
		
		curr_hash = ht->hn[find_result2].hash_value & 0x7FFFFFFF;
		if(curr_hash == ht->hn[find_result2].hash_value & 0x7FFFFFFF)
		{
			//把索引存起来
			array_set(&ht->different_hash_array,ht->different_hash_array.size,find_result2);
		}
		
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

			//更新某个hash值的结束index
			if(curr_hash == ht->hn[find_result2].hash_value & 0x7FFFFFFF)
			{	
				//使用size-1是为了更新索引
				array_set(&ht->different_hash_array,ht->different_hash_array.size-1,find_result2);
			}
			else//添加某个hash值的index，用于更新
			{
				//使用size是为了添加新索引
				array_set(&ht->different_hash_array,ht->different_hash_array.size,find_result2);
				//更新当前hash值
				curr_hash = ht->hn[find_result2].hash_value & 0x7FFFFFFF;
			}
			

			find_result2++;
			find_result2 %= size;
		}
	}
	return find_result;
}

//use backshift delete
int rhht_remove_helper(hash_table *ht,int key)
{
	int shift_start,shift_end;
	int index = __rhht_find_helper(ht,key);
	if (index == -1)
		return -1;

	ht->hn[index].hash_value |= 0x80000000;//flag it as deleted
	ht->element_num--;

	return index;
}
//use backshift delete
int rhht_backshift_remove_helper(hash_table *ht,int key)
{
	int shift_start,shift_end;
	int index = __rhht_find_helper_for_backshift_remove(ht,key,&shift_start,&shift_end);
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

int rhht_remove_one(hash_table *ht,int key)
{
	return  rhht_remove_helper(ht,key);
}
int rhht_remove_one2(hash_table *ht,int key)
{
	return  rhht_backshift_remove_helper(ht,key); 
}

int rhht_remove_all(hash_table *ht,int key)
{
	int removed_num = 0;
	while (-1 != rhht_backshift_remove_helper(ht,key))
	{
		removed_num++;
	}
	return removed_num;
}

int rhht_remove_all2(hash_table *ht,int key)
{
	int removed_num = 0;
	//清空
	array_deinit(&ht->different_hash_array);

	int shift_start,shift_end;
	int pos =  __rhht_find_helper_for_backshift_remove_all(ht,key,&shift_start,&shift_end);
	if(pos != -1)
	{
		//循环向前移动
	}
	return removed_num;
}

#endif

#endif