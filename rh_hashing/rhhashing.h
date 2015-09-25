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
	int capacity;		//hashtable��bucket��С
	int treshold;		//��ֵ��С���Ǹ���load_factor_percent * capacity���������
	int element_num;//��ЧԪ�ش�С
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
	//������һ����λ
	if (ht->capacity - ht->treshold < 1)
	{
		ht->treshold = ht->capacity - 1;
	}

	ht->hn = (hash_node *)malloc(sizeof(hash_node) * initial_size);
	ht->element_num = 0;
	memset(ht->hn,0,sizeof(hash_node)*initial_size);

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

//start_table_pos��ʼ��������
int rhht_insert_helper(hash_table *ht,int key,int value,int start_table_pos_instinct)
{
	int is_increased = rhht_check_increase_to(ht);
	int size = ht->capacity;
	int inserted_hash_value = hash_function(key,size);
	int table_pos = inserted_hash_value;
	//���·����˴�С�Ļ�������ʽ��start_table_pos�Ͳ������ˣ�ʧЧ��
	if ((!is_increased) && start_table_pos_instinct)
		table_pos = start_table_pos_instinct;
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
		int inserted_dib = (table_pos  + size - (inserted_hash_value & 0x7FFFFFFFU) );
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
		}//��dibͻȻ���С����ʼ��Ӧ�е�dib��ʱ��˵���Ѿ�������ֵ�Ĳ�����
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

//ͬһ��ֵֻ����һ��,�Ҳ�����֮ǰ��ֵ
int rhht_unique_insert(hash_table *ht,int key,int value)
{
	int endpos = 0;
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
	int endpos = 0;
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
//������Ҫ�ƶ� findpos_output ~ shift_endpos_output ֮������ݿ�
int __rhht_find_helper_for_backshift_remove(hash_table *ht,int key,int *findpos_output=NULL,int *shift_endpos_output=NULL)
{
	int find_result = 0;
	int size = ht->capacity;
	int hash_value = hash_function(key,size);
	int table_pos = hash_value;
	int find_len = 0;
	
	while(0 == find_result)
	{
		int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;
		//empty
		if (ht->hn[table_pos].hash_value == 0)
		{	
			if (findpos_output)
				*findpos_output = table_pos;
			find_result = -1;
		}//��dibͻȻ���С����ʼ��Ӧ�е�dib��ʱ��˵���Ѿ�������ֵ�Ĳ�����
		else if (dib < find_len)
		{
			if (findpos_output)
				*findpos_output = table_pos;
			find_result = -1;
		}
		else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key)
		{
			if (findpos_output)
				*findpos_output = table_pos;
			find_result =  table_pos;
		}
		table_pos++;
		table_pos %= size;
		find_len++;
	}

	//-1����ʾ�����ڣ���ôfindpos_output��������ģ�shift_endpos_output���������
	//��Ϊ-1����ʾ���ڣ�Ҫ�ҵ���Ҫ��shift�Ľ�β��λ�� findpos_output��������ģ�shift_endpos_output���������
	//����ÿ��ɾ����Ҫִ��back_shift�����ǵ�dib����ĳ��ֵ������5 ��ʱ�򣬲���Ҫshift����ֹ
	if(find_result != -1)
	{
		int search_len = 0;
		while (true)
		{
			//��find_result֮��ʼ�ң���Ҫ�����ƶ�һ��λ�õĽ�������
			int end_pos = (find_result + 1 + search_len) % ht->capacity;//end_pos���ܴ�ĩβ�ػ����ʼ
			int table_hash = ht->hn[end_pos].hash_value & 0x7FFFFFFFU;
			int dib = find_result + 1 + search_len - table_hash;
			//empty
			if(table_hash  == 0)
			{
				if(shift_endpos_output)
					*shift_endpos_output = end_pos;
				break;
			}
			else//ֻ�ƶ�dib����5�Ĳ���
			{
				if(dib > 5)
				{
					if(shift_endpos_output)
						*shift_endpos_output = end_pos;
				}
				else
				{
					break;
				}
			}
			search_len++;	
		}
	}
  
}

//use backshift delete
int rhht_backshift_remove_helper(hash_table *ht,int key)
{
	int shift_start,shift_end;
	int index = __rhht_find_helper_for_backshift_remove(ht,key,&shift_start,&shift_end);
	if (index == -1)
		return -1;
	ht->hn[index].hash_value |= 0x80000000;//flag it as deleted
	ht->element_num--;
	// do shift

	return index;
}

int rhht_remove_one(hash_table *ht,int key)
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

#endif

#endif