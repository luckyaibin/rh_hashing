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
	//��Ҫ���·�����ڴ�
	if (index>=arr->capacity)
	{
		int *old_buff_heap = arr->buff_heap;
		int old_capacity = arr->capacity;
		arr->capacity = (index+1) * 2;
		//memeory
		arr->buff_heap = (int*)malloc(sizeof(int) * arr->capacity);
		//clear memory
		memset(arr->buff_heap,0,sizeof(int) * arr->capacity);
		//֮ǰ�õ��Ƕ��ڴ棬�������ͷ�
		if (old_buff_heap)
		{
			memcpy(arr->buff_heap,old_buff_heap,sizeof(int) * old_capacity);
			free(old_buff_heap);
		}
		else//֮ǰ�õ���ջ�ڴ棬ֻ��Ҫ����
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
		//ɾ���Ĳ������һ������Ҫ�ƶ�������ɾ���������һ��������Ҫ�ƶ�
		if (index < arr->size-1)
		{
			memcpy(&arr->buff_heap[index],&arr->buff_heap[index+1],sizeof(int)*(arr->size-1 - index));
		}
	}
	else
	{
		if(value)
			*value = arr->buff_stack[index];
		//ɾ���Ĳ������һ������Ҫ�ƶ�������ɾ���������һ��������Ҫ�ƶ�
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
	int capacity;		//hashtable��bucket��С
	int treshold;		//��ֵ��С���Ǹ���load_factor_percent * capacity���������
	int element_num;//��ЧԪ�ش�С
	float load_factor_percent;
	hash_node *hn;
	dynamic_int_array different_hash_array;
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
	//������һ����λ
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

//���Ӵ�С��ԭ����factor����Ĭ��Ϊ2
int rhht_check_increase_to(hash_table *ht,float increase_factor = 1.50f)
{
	//û�ﵽ���޴�С
	if (ht->element_num < ht->treshold)
	{
		return 0;
	}

	
	//���·����ڴ�
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
	//������һ����λ����֤�����ܹ��ɹ��ڲ��룬����֮��ſ�������capacity
	if (ht->capacity - ht->treshold < 1)
	{
		ht->treshold = ht->capacity - 1;
	}
	

	//����ԭ����ֵ
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
	v |= v==0;//������hashֵ0
	return v;
}
//inline int hash_function(int v,int hash_size)
//{
//	v = v % hash_size;
//	v |= v==0;//������hashֵ0
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

//start_table_pos��ʼ��������
int rhht_insert_helper(hash_table *ht,int key,int value,int start_table_pos_instinct)
{
	int is_increased = rhht_check_increase_to(ht);
	int size = ht->capacity;
	int inserted_hash_value = hash_function(key,size);
	int table_pos = inserted_hash_value;
	//���·����˴�С�Ļ�������ʽ��start_table_pos�Ͳ������ˣ�ʧЧ��
	if ((!is_increased) && (-1!=start_table_pos_instinct))
	{
		table_pos = start_table_pos_instinct;
		//printf("start_table_pos_instinct: %d ,save steps: %d \n",start_table_pos_instinct,start_table_pos_instinct-inserted_hash_value);
	}
	while (true)
	{
		table_pos = table_pos % size;
		int table_hash = ht->hn[table_pos].hash_value;
		//û��Ԫ��
		//empty
		if (table_hash == 0)
		{
			ht->hn[table_pos].key = key;
			ht->hn[table_pos].value = value;
			ht->hn[table_pos].hash_value = inserted_hash_value;
			ht->element_num++;
			return table_pos;
		}
		//ע�⣡������������֮����Ҫtable_pos + size - table_hash ������size����Ϊ�˱�֤���ػ�����ʼ���ʱ�����õ���ֵ��Ȼ����ȷ�ģ�������ܳ��ָ�ֵ
		//��Ԫ��
		//table_pos��Զ�Ǵ��ڵ���table_hash�ģ���Ϊ����Ԫ�ض�������λ���е�
		int table_dib = (table_pos + size - table_hash & 0x7FFFFFFFU)%size;

		//table_pos��һ������ inserted_hash_value������������Ǵ��ڵģ����ǵ���hash_table����ת��hash_table��ʼ��ʱ��
		//table_pos С��inserted_hash_value
		int inserted_dib = (table_pos  + size - (inserted_hash_value & 0x7FFFFFFFU) )%size;
		//swap���൱�ڰ���Ԫ��new_node���뵽��ǰtable_pos��λ�ã��൱�ڽ���new_node��table_pos�ϵ�old_node��Ȼ���old_node���Ų��
		if(table_dib < inserted_dib)
		{
			//deleted,���ܷ�������
			if (table_hash & 0x80000000)
			{
				ht->hn[table_pos].key = key;
				ht->hn[table_pos].value = value;
				ht->hn[table_pos].hash_value = inserted_hash_value;
				ht->element_num++;
				return table_pos;
			}
			//���汻����λ�õ�ԭʼֵ
			hash_node tmp_hash_node = ht->hn[table_pos];
		
			//���뵽����
			ht->hn[table_pos].hash_value = inserted_hash_value;
			ht->hn[table_pos].key		 = key;
			ht->hn[table_pos].value		 = value;

			//�ѱ��е�ֵ�����Ҫ�����ֵ
			//��������µ�hashֵ
			inserted_hash_value = tmp_hash_node.hash_value;
			key = tmp_hash_node.key;
			value = tmp_hash_node.value;
		}
		table_pos++;
	}
}


//find ����key������
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
		}//��dibͻȻ���С����ʼ��Ӧ�е�dib��ʱ��˵���Ѿ�������ֵ�Ĳ�����
		if (dib < find_len)
		{
			return -1;
		}
		if (ht->hn[table_pos].hash_value == hash_value)
		{
			//��¼��һ��hash_value������Ҫ����ֵ��hash��λ�ã����Լӿ����
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

//ͬһ��ֵֻ����һ��,�Ҳ�����֮ǰ��ֵ
int rhht_unique_insert(hash_table *ht,int key,int value)
{
	int endpos = -1;
	int index = __rhht_find_helper(ht,key,&endpos);
	//�����ڲŲ��룬���ݷ��ص�endpos���Լӿ������ٶ�
	if (index==-1)
	{
		index = rhht_insert_helper(ht,key,value,endpos);
	}
	return index;
}

//ͬһ��ֵֻ����һ��,�Ҹ���֮ǰ���ڵ�ֵ
int rhht_unique_overwrite_insert(hash_table *ht,int key,int value)
{
	int endpos = -1;
	int index = __rhht_find_helper(ht,key,&endpos);
	//�����ڲŲ��룬���ݷ��ص�endpos���Լӿ������ٶ�
	if (index==-1)
	{
		index = rhht_insert_helper(ht,key,value,endpos);
	}
	//���ڣ�ֱ�Ӹ���
	else
	{
		ht->hn[index].key = key;
		ht->hn[index].value = value;
	}
	return index;
}



//find ����key������ findpos_output�ǲ��ҳɹ��Ͳ���ʧ��ʱ�ĵ�ǰ������shift_endpos_output��backshiftɾ��ʱ��Ҫ�ƶ����ݿ��������
//������Ҫ�ƶ� findpos_output ~ shift_endpos_output ֮��[findpos_output,shift_endpos_output)�����ݿ�,shift_endpos_output������Ϊ��
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
		}//��dibͻȻ���С����ʼ��Ӧ�е�dib��ʱ��˵���Ѿ�������ֵ�Ĳ�����
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

	//-1����ʾ�����ڣ���ôfindpos_output��������ģ�shift_endpos_output���������
	//��Ϊ-1����ʾ���ڣ�Ҫ�ҵ���Ҫ��shift�Ľ�β��λ�� findpos_output��������ģ�shift_endpos_output���������
	//TODO:����ÿ��ɾ����Ҫִ��back_shift�����ǵ�dib����ĳ��ֵ������5 ��ʱ�򣬲���Ҫshift�����ܷ�ʵ�������
	if(find_result != -1)
	{
		//��Ҫfind_result2����Ҫshiftback�Ľ���������
		int find_result2 = find_result;
		if (shift_endpos_output)
			*shift_endpos_output = find_result2;
		while (true)
		{
			//empty
			if (ht->hn[find_result2].hash_value == 0)
				break;
			//dib == 0,�п���find_result����dibΪ0�ģ�Ҫ�жϺ����
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

//find ����key������ findpos_output�ǲ��ҳɹ��Ͳ���ʧ��ʱ�ĵ�ǰ������shift_endpos_output��backshiftɾ��ʱ��Ҫ�ƶ����ݿ��������
//������Ҫ�ƶ� findpos_output ~ shift_endpos_output ֮��[findpos_output,shift_endpos_output)�����ݿ�,shift_endpos_output������Ϊ��
//
int __rhht_find_helper_for_backshift_remove_all(hash_table *ht,int key,int *findpos_output=NULL,int *shift_endpos_output=NULL)
{
	int find_result = -1;
	int size = ht->capacity;
	int hash_value = hash_function(key,size);
	int table_pos = hash_value;
	int find_len = 0;

	ht->different_hash_array

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
		}//��dibͻȻ���С����ʼ��Ӧ�е�dib��ʱ��˵���Ѿ�������ֵ�Ĳ�����
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

	//-1����ʾ�����ڣ���ôfindpos_output��������ģ�shift_endpos_output���������
	//��Ϊ-1����ʾ���ڣ�Ҫ�ҵ���Ҫ��shift�Ľ�β��λ�� findpos_output��������ģ�shift_endpos_output���������
	//TODO:����ÿ��ɾ����Ҫִ��back_shift�����ǵ�dib����ĳ��ֵ������5 ��ʱ�򣬲���Ҫshift�����ܷ�ʵ�������
	if(find_result != -1)
	{
		//��Ҫfind_result2����Ҫshiftback�Ľ���������
		int find_result2 = find_result;
		if (shift_endpos_output)
			*shift_endpos_output = find_result2;
		while (true)
		{
			//empty
			if (ht->hn[find_result2].hash_value == 0)
				break;
			//dib == 0,�п���find_result����dibΪ0�ģ�Ҫ�жϺ����
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
	//���һ�����Ϊ��
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
	//���
	array_deinit(&ht->different_hash_array);
	int pos =  __rhht_find_helper_for_backshift_remove_all(ht,key);
	pos != -1;
	return removed_num;
}

#endif

#endif