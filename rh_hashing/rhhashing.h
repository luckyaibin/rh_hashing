#ifndef __ROBIN_HASHING_H__
#define __ROBIN_HASHING_H__
#include <memory.h>
#include <stdio.h>
#include <malloc.h>
#if 1
// hash node of hash table
struct hash_node
{
	int val;
	int hash_value;
	int serial_id;
};

struct hash_table
{
	int initial_size;
	int size;		//hashtable��bucket��С
	int element_num;//��ЧԪ�ش�С
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
			printf("\t%d,",ht->hn[i].val);
		}
		/*if (ht->hn[i].hash_value > 0)
		{
		printf("\t%d[%d,%d]%d,",i,ht->hn[i].hash_value,ht->hn[i].val,(ht->size + i - hash_function(ht->hn[i].val,ht->size))%ht->size);
		}*/
		
		//printf("\t[%d,%d,(%d)]",ht->hn[i].val,ht->hn[i].hash_value,i - ht->hn[i].hash_value);
		//printf("\t[%d,%d,%d-%d]",ht->hn[i].val,ht->hn[i].hash_value,i - ht->hn[i].hash_value,ht->hn[i].serial_id);
	}
	printf("\n\n");
}




//start_table_pos��ʼ��������
int rhht_insert(hash_table *ht,int val,int start_table_pos=0)
{
	int size = ht->size;
	int inserted_hash_value = hash_function(val,size);
	int table_pos = inserted_hash_value;
	if (start_table_pos)
		table_pos = start_table_pos;
	while (true)
	{
		table_pos = table_pos % size;
		int table_hash = ht->hn[table_pos].hash_value;
		//û��Ԫ��
			//empty
			if (table_hash == 0)
			{
				ht->hn[table_pos].val = val;
				ht->hn[table_pos].hash_value = inserted_hash_value;
				ht->element_num++;
				return table_pos;
			}
			////deleted
			//else if (table_hash == -2)
			//{
			//	ht->hn[table_pos].val = val;
			//	ht->hn[table_pos].hash_value = inserted_hash_value;
			//	ht->element_num++;
			//	return table_pos;
			//}

		//ע�⣡������������֮����Ҫtable_pos + size - table_hash ������size����Ϊ�˱�֤���ػ�����ʼ���ʱ�����õ���ֵ��Ȼ����ȷ�ģ�������ܳ��ָ�ֵ
		//��Ԫ��
		//table_pos��Զ�Ǵ��ڵ���table_hash�ģ���Ϊ����Ԫ�ض�������λ���е�
		int table_dib = (table_pos + size - table_hash & 0x7FFFFFFFU)%size;

		//table_pos��һ������ inserted_hash_value������������Ǵ��ڵģ����ǵ���hash_table����ת��hash_table��ʼ��ʱ��
		//table_pos С��inserted_hash_value
		int inserted_dib = (table_pos  + size - (inserted_hash_value & 0x7FFFFFFFU) ) % size;
		
		/*
		hash table: 0	1	2	3	4	5	6	7	8	9
		hash value  8	1	1	1	2	4	-2	5	-1	8
		table_dib   -8	0	1	2	2	1	8	2	9	1

		-8���ԣ���ʵӦ���� ��-8+10��%10 = 2
		*/

		//swap���൱�ڰ���Ԫ��new_node���뵽��ǰtable_pos��λ�ã��൱�ڽ���new_node��table_pos�ϵ�old_node��Ȼ���old_node���Ų��
		if(table_dib < inserted_dib)
		{
			//deleted,���ܷ�������
			if (table_hash & 0x80000000)
			{
				ht->hn[table_pos].val = val;
				ht->hn[table_pos].hash_value = inserted_hash_value;
				ht->element_num++;
				return table_pos;
			}
			//���汻����λ�õ�ԭʼֵ
			hash_node tmp_hash_node = ht->hn[table_pos];
		
			//���뵽����
			ht->hn[table_pos].hash_value = inserted_hash_value;
			ht->hn[table_pos].val		 = val;

			//�ѱ��е�ֵ�����Ҫ�����ֵ
			//��������µ�hashֵ
			inserted_hash_value = tmp_hash_node.hash_value;
			val = tmp_hash_node.val;
		}
		table_pos++;
	}
}



int __rhht_find(hash_table *ht,int val,int *endpos=NULL)
{
	int size = ht->size;
	int hash_value = hash_function(val,size);
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
		}//��dibͻȻ���С����ʼ��Ӧ�е�dib��ʱ��˵���Ѿ�������ֵ�Ĳ����ˣ�������-2Ҳ�������⴦��
		else if (dib < find_len)
		{
			if (endpos)
				*endpos = table_pos;
			return -1;
		}
		else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].val == val)
		{
			*endpos = table_pos;
			return table_pos;
		}
		table_pos++;
		find_len++;
	}
}
//ͬһ��ֵֻ����һ��,�Ҳ�����֮ǰ��ֵ
int rhht_unique_insert(hash_table *ht,int val)
{
	int endpos = 0;
	int index = __rhht_find(ht,val,&endpos);
	//�����ڲŲ��룬���ݷ��ص�endpos���Լӿ������ٶ�
	if (index==-1)
	{
		index = rhht_insert(ht,val,endpos);
		ht->element_num++;
	}
	return index;

}

//ͬһ��ֵֻ����һ��,�Ҹ���֮ǰ���ڵ�ֵ
int rhht_unique_overwrite_insert(hash_table *ht,int val)
{
	int endpos = 0;
	int index = __rhht_find(ht,val,&endpos);
	//�����ڲŲ��룬���ݷ��ص�endpos���Լӿ������ٶ�
	if (index==-1)
	{
		index = rhht_insert(ht,val,endpos);
		ht->element_num++;
	}
	//���ڣ�ֱ�Ӹ���
	else
	{
		ht->hn[index].val = val;
	}
	return index;

}

int rhht_remove(hash_table *ht,int val)
{
	int index = __rhht_find(ht,val);
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