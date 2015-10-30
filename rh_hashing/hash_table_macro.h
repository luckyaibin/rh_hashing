#ifndef __ROBIN_HASHING_H__
#define __ROBIN_HASHING_H__
#include <memory.h>
#include <stdio.h>
#include <malloc.h>

#if 1

#ifndef min
#define min(a,b) ((a)<=(b)?a:b)
#endif

#define HT_declare_new_hashtable_type(hashfunction_name,key_type,value_type)											\
	typedef int (*hashfunction_type_##key_type##value_type)(key_type,int) ;												\
	hashfunction_type_##key_type##value_type  hashfunction_##key_type##value_type= hashfunction_name;					\
	\
	inline int hash_function_wrapper_##key_type##value_type(key_type v,int hash_size)									\
{																		\
	int hash_value = hashfunction_##key_type##value_type(v,hash_size);	\
	hash_value = hash_value % hash_size;								\
	hash_value |= hash_value==0;										\
	return hash_value;													\
}\
	\
struct hash_node##key_type##value_type	\
{										\
	key_type key;						\
	value_type value;					\
	int hash_value;						\
};										\
	\
struct hash_table##key_type##value_type	\
{																						\
	int capacity;																		\
	int treshold;																		\
	int element_num;																	\
	float load_factor_percent;															\
	hash_node##key_type##value_type *hn;												\
};																						\
	\
	\
	int __rhht_insert_helper##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,value_type value,int start_table_pos=-1);						\
	\
	hash_table##key_type##value_type* create_hash_table(unsigned int initial_size = 256,float load_factor_percent=0.9f )			\
{																																\
	hash_table##key_type##value_type *ht= (hash_table##key_type##value_type*)malloc(sizeof(hash_table##key_type##value_type));	\
	ht->capacity = initial_size;																								\
	ht->load_factor_percent = load_factor_percent;																				\
	ht->treshold = int(ht->capacity * ht->load_factor_percent);																	\
	if (ht->capacity - ht->treshold < 2)																						\
	{																															\
	ht->treshold = ht->capacity - 2;																						\
	}																															\
	ht->hn = (hash_node##key_type##value_type *)malloc(sizeof(hash_node##key_type##value_type) * initial_size);					\
	ht->element_num = 0;																										\
	memset(ht->hn,0,sizeof(hash_node##key_type##value_type)*initial_size);														\
	return ht;																													\
}																																\
	\
	int __get_hash##key_type##value_type(hash_table##key_type##value_type* ht,int index)																	\
{																																\
	int hash_value = ht->hn[index].hash_value & 0x7FFFFFFF;																		\
	return hash_value;																											\
}																																\
	int __get_dib##key_type##value_type(hash_table##key_type##value_type* ht,int index)																	\
{																																\
	int dib = (index + ht->capacity + ht->hn[index].hash_value & 0x7FFFFFFF  ) % ht->capacity;									\
	return dib;																													\
}																																\
	\
	int __rhht_check_increase_to##key_type##value_type(hash_table##key_type##value_type *ht,float increase_factor = 1.50f)								\
{																																\
	if (ht->element_num < ht->treshold)																							\
	{																															\
	return 0;																												\
	}																															\
	int old_capacity = ht->capacity;																							\
	hash_node##key_type##value_type* old_hn   = ht->hn;																			\
	int new_capacity = int(ht->capacity * increase_factor);																		\
	hash_node##key_type##value_type *hn = (hash_node##key_type##value_type*)malloc(sizeof(hash_node##key_type##value_type) * new_capacity);\
	memset(hn,0,sizeof(hash_node##key_type##value_type) * new_capacity);														\
	ht->hn = hn;																												\
	ht->element_num = 0;																										\
	ht->capacity = new_capacity;																								\
	ht->treshold = int(ht->capacity * ht->load_factor_percent);																	\
	printf("increased from %d to %d \n",old_capacity,new_capacity);																\
	if (ht->capacity - ht->treshold < 2)																						\
	{																															\
	ht->treshold = ht->capacity - 2;																						\
	}																															\
	for (int i=0;i<old_capacity;i++)																							\
	{																															\
	if (old_hn[i].hash_value > 0)																							\
		{																														\
		__rhht_insert_helper##key_type##value_type(ht,old_hn[i].key,old_hn[i].value);																\
		}																														\
	}																															\
	free(old_hn);																												\
	return ht->capacity;																										\
}\
	\
	void dump_hash_table##key_type##value_type(hash_table##key_type##value_type * ht)\
{\
	static int g_dump_serial=0;\
	printf(" g_dump_serial is %d\n",g_dump_serial++);\
	int out_put_count = -1;\
	for (int i=0;i<ht->capacity;i++)\
	{\
	if (ht->hn[i].hash_value > 0)\
		{\
		printf("[index%d:hash_value%d,dib%d~key%d,value%d]    \n",i,ht->hn[i].hash_value,(ht->capacity + i - ht->hn[i].hash_value) % ht->capacity,ht->hn[i].key,ht->hn[i].value);\
		out_put_count++;\
		}\
		if (out_put_count%4 == 3)\
		{\
		;\
		}\
	}\
	printf("\n\n");\
}\
	\
	\
	int __rhht_insert_helper##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,value_type value,int start_table_pos_instinct)\
{\
	int is_increased = __rhht_check_increase_to##key_type##value_type(ht);\
	int size = ht->capacity;\
	int inserted_hash_value = hash_function_wrapper_##key_type##value_type(key,size);\
	int table_pos = inserted_hash_value;\
	if ((!is_increased) && (-1!=start_table_pos_instinct))\
	{\
	table_pos = start_table_pos_instinct;\
	}\
	while (true)\
	{\
	table_pos = table_pos % size;\
	int table_hash = ht->hn[table_pos].hash_value;\
	if (table_hash == 0)\
		{\
		ht->hn[table_pos].key = key;\
		ht->hn[table_pos].value = value;\
		ht->hn[table_pos].hash_value = inserted_hash_value;\
		ht->element_num++;\
		return table_pos;\
		}\
		int table_dib = (table_pos + size - table_hash & 0x7FFFFFFFU)%size;\
		int inserted_dib = (table_pos  + size - (inserted_hash_value & 0x7FFFFFFFU) )%size;\
		if(table_dib < inserted_dib)\
		{\
		if (table_hash & 0x80000000)\
			{\
			ht->hn[table_pos].key = key;\
			ht->hn[table_pos].value = value;\
			ht->hn[table_pos].hash_value = inserted_hash_value;\
			ht->element_num++;\
			return table_pos;\
			}\
			hash_node##key_type##value_type tmp_hash_node = ht->hn[table_pos];\
			\
			ht->hn[table_pos].hash_value = inserted_hash_value;\
			ht->hn[table_pos].key         = key;\
			ht->hn[table_pos].value         = value;\
			\
			\
			\
			inserted_hash_value = tmp_hash_node.hash_value;\
			key = tmp_hash_node.key;\
			value = tmp_hash_node.value;\
		}\
		table_pos++;\
	}\
}\
	\
	\
	\
	\
	int __rhht_find_helper_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,int *insertpos_output=NULL)\
{\
	int size = ht->capacity;\
	int hash_value = hash_function_wrapper_##key_type##value_type(key,size);\
	int table_pos = hash_value;\
	int find_len = 0;\
	while(true)\
	{\
	int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;\
	\
	if (ht->hn[table_pos].hash_value == 0)\
		{   \
		return -1;\
		}\
		if (dib < find_len)\
		{\
		return -1;\
		}\
		if (ht->hn[table_pos].hash_value == hash_value)\
		{\
		\
		if (insertpos_output && (*insertpos_output == -1) )\
		*insertpos_output = table_pos;\
		if (ht->hn[table_pos].key == key)\
			{\
			return table_pos;\
			}\
		}\
		\
		table_pos++;\
		table_pos %= size;\
		find_len++;\
	}\
}\
	\
	\
	int rhht_multi_insert_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,value_type value)\
{\
	return __rhht_insert_helper##key_type##value_type(ht,key,value);\
}\
	\
	\
	int rhht_unique_insert_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,value_type value)\
{\
	int endpos = -1;\
	int index = __rhht_find_helper_##key_type##value_type(ht,key,&endpos);\
	\
	if (index==-1)\
	{\
	index = __rhht_insert_helper##key_type##value_type(ht,key,value,endpos);\
	}\
	return index;\
}\
	\
	\
	int rhht_unique_overwrite_insert_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,value_type value)\
{\
	int endpos = -1;\
	int index = __rhht_find_helper_##key_type##value_type(ht,key,&endpos);\
	\
	if (index==-1)\
	{\
	index = __rhht_insert_helper##key_type##value_type(ht,key,value,endpos);\
	}\
	\
	else\
	{\
	ht->hn[index].key = key;\
	ht->hn[index].value = value;\
	}\
	return index;\
}\
	\
	\
	\
	int __rhht_remove_one_helper_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key)\
{\
	int shift_start,shift_end;\
	int index = __rhht_find_helper_##key_type##value_type(ht,key);\
	if (index == -1)\
	return -1;\
	\
	ht->hn[index].hash_value |= 0x80000000;\
	ht->element_num--;\
	\
	return index;\
}\
	\
	\
	\
	\
	\
	\
	int __rhht_find_helper_bs_move_range_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,int *findpos_output=NULL,int *shift_endpos_output=NULL)\
{\
	int find_result = -1;\
	int size = ht->capacity;\
	int hash_value = hash_function_wrapper_##key_type##value_type(key,size);\
	int table_pos = hash_value;\
	int find_len = 0;\
	\
	while(true)\
	{\
	int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;\
	\
	if (ht->hn[table_pos].hash_value == 0)\
		{   \
		if (findpos_output)\
		*findpos_output = table_pos;\
		find_result = -1;\
		break;\
		}\
		else if (dib < find_len)\
		{\
		if (findpos_output)\
		*findpos_output = table_pos;\
		find_result = -1;\
		break;\
		}\
		else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key)\
		{\
		if (findpos_output)\
		*findpos_output = table_pos;\
		find_result =  table_pos;\
		break;\
		}\
		table_pos++;\
		table_pos %= size;\
		find_len++;\
	}\
	\
	\
	\
	if(find_result != -1)\
	{\
	\
	int find_result2 = find_result;\
	if (shift_endpos_output)\
	*shift_endpos_output = find_result2;\
	while (true)\
		{\
		\
		if (ht->hn[find_result2].hash_value == 0)\
		break;\
		\
		int dib = (find_result2 + size - ht->hn[find_result2].hash_value & 0x7FFFFFFF) % size;\
		if ( (find_result2!=find_result) &&  (dib == 0) )\
		break;\
		if (shift_endpos_output)\
		*shift_endpos_output = find_result2;\
		find_result2++;\
		find_result2 %= size;\
		}\
	}\
	return find_result;\
}\
	\
	int __rhht_remove_one_helper_bs_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key)\
{\
	int shift_start,shift_end;\
	int index = __rhht_find_helper_bs_move_range_##key_type##value_type(ht,key,&shift_start,&shift_end);\
	if (index == -1)\
	return -1;\
	\
	\
	int size = ht->capacity;\
	int shift_index = shift_start;\
	int shift_count = 0;\
	while( shift_index != shift_end)\
	{\
	shift_count ++;\
	ht->hn[shift_index] = ht->hn[ (shift_index+1) % size];\
	shift_index = (shift_index+1)%size;\
	}\
	if (shift_count>0)\
	{\
	\
	}\
	\
	ht->hn[shift_end].hash_value = 0x00000000;\
	ht->element_num--;\
	\
	return index;\
}\
	\
	\
	\
	\
	int __rhht_backshift_remove_on_fly_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,int* remove_count=NULL,int remove_all=0)\
{\
	int find_result = -1;\
	int size = ht->capacity;\
	int hash_value = hash_function_wrapper_##key_type##value_type(key,size);\
	int table_pos = hash_value;\
	int find_len = 0;\
	if (remove_count)\
	{\
	*remove_count = 0;\
	}\
	while(true)\
	{\
	int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;\
	\
	if (ht->hn[table_pos].hash_value == 0)\
		{   \
		find_result = -1;\
		break;\
		}\
		else if (dib < find_len)\
		{\
		find_result = -1;\
		break;\
		}\
		else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key)\
		{\
		find_result =  table_pos;\
		break;\
		}\
		table_pos++;\
		table_pos %= size;\
		find_len++;\
	}\
	\
	\
	if(find_result != -1)\
	{\
	\
	ht->hn[find_result].hash_value = 0;\
	if (remove_count)\
	*remove_count += 1;\
	\
	int shift_index = (find_result+1)%size;\
	int need_shift_left_count = 1;\
	while (true)\
		{\
		\
		if (ht->hn[shift_index].hash_value == 0)\
		break;\
		\
		int dib = (shift_index + size - ht->hn[shift_index].hash_value & 0x7FFFFFFF) % size;\
		if ( (shift_index!=find_result) &&  (dib == 0) )\
		break;\
		\
		\
		if (remove_all && ht->hn[shift_index].key == key)\
			{\
			ht->hn[shift_index].hash_value = 0;\
			need_shift_left_count++;\
			if (remove_count)\
			*remove_count += 1;\
			}\
			else\
			{\
			\
			need_shift_left_count = min( dib,need_shift_left_count);\
			if (need_shift_left_count>0)\
				{\
				int index = (shift_index + size - need_shift_left_count)%size;\
				ht->hn[index] = ht->hn[shift_index];\
				ht->hn[shift_index].hash_value = 0;\
				}\
			}\
			shift_index++;\
			shift_index %= size;\
		}\
	}\
	return find_result;\
}\
	\
	int __rhht_remove_one_or_all_helper_bs_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key,int is_remove_all)\
{\
	int remove_count=0;\
	int index = __rhht_backshift_remove_on_fly_##key_type##value_type(ht,key,&remove_count,is_remove_all);\
	if (index == -1)\
	return -1;\
	ht->element_num -= remove_count;\
	return remove_count;\
}\
	\
	\
	\
	\
	int rhht_remove_one_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key)\
{\
	int removed_count = __rhht_remove_one_helper_##key_type##value_type(ht,key);\
	if (-1==removed_count)\
	return 0;\
	return 1;\
}\
	\
	\
	int rhht_remove_one_bs_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key)\
{\
	int removed_count =  __rhht_remove_one_or_all_helper_bs_##key_type##value_type(ht,key,0);\
	return removed_count;\
}\
	\
	\
	int rhht_remove_one_bs_slow_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key)\
{\
	int removed_count = __rhht_remove_one_helper_bs_##key_type##value_type(ht,key);\
	if (-1==removed_count)\
	return 0;\
	return 1;\
}\
	\
	\
	int rhht_remove_all_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key)\
{\
	int removed_num = 0;\
	int removed_index = __rhht_remove_one_helper_##key_type##value_type(ht,key);\
	while (-1 != removed_index)\
	{\
	++removed_num;\
	removed_index = __rhht_remove_one_helper_##key_type##value_type(ht,key);\
	}\
	return removed_num;\
}\
	\
	\
	int rhht_remove_all_bs_##key_type##value_type(hash_table##key_type##value_type *ht,key_type key)\
{\
	int removed_num = __rhht_remove_one_or_all_helper_bs_##key_type##value_type(ht,key,1);\
	return removed_num;\
}\
	\
	\
	int rhht_remove_all_bs_slow##key_type##value_type(hash_table##key_type##value_type *ht,key_type key)\
{\
	int removed_num = 0;     \
	int removed_index = __rhht_remove_one_helper_bs_##key_type##value_type(ht,key);\
	if(-1 != removed_index)\
	{\
	++removed_num;\
	removed_index = __rhht_remove_one_helper_bs_##key_type##value_type(ht,key);\
	}\
	return removed_num;\
}\

#define TO_STR(macro) TO_STR2(macro)
#define TO_STR2(macro) #macro
#endif

#endif
