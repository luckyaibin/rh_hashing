#include "hash_table_macro.h"
#include<Windows.h>
inline int hash_function_for_int(int v,int hash_size)

{
	v = v + (v << 5);
	v = v % hash_size;
	v |= v==0;
	return v;
}
typedef int (*hashfunction_type_intfloat)(int,int) ; hashfunction_type_intfloat hashfunction_intfloat= hash_function;
inline int hash_function_wrapper_intfloat(int v,int hash_size) { int hash_value = hashfunction_intfloat(v,hash_size);
hash_value = hash_value % hash_size;
hash_value |= hash_value==0;
return hash_value;
} struct hash_nodeintfloat { int key;
float value;
int hash_value;
};
struct hash_tableintfloat { int capacity;
int treshold;
int element_num;
float load_factor_percent;
hash_nodeintfloat *hn;
};
int __rhht_insert_helperintfloat(hash_tableintfloat *ht,int key,float value,int start_table_pos=-1);
hash_tableintfloat* create_hash_table(unsigned int initial_size = 256,float load_factor_percent=0.9f ) { hash_tableintfloat *ht= (hash_tableintfloat*)malloc(sizeof(hash_tableintfloat));
ht->capacity = initial_size;
ht->load_factor_percent = load_factor_percent;
ht->treshold = int(ht->capacity * ht->load_factor_percent);
if (ht->capacity - ht->treshold < 2) { ht->treshold = ht->capacity - 2;
} ht->hn = (hash_nodeintfloat *)malloc(sizeof(hash_nodeintfloat) * initial_size);
ht->element_num = 0;
memset(ht->hn,0,sizeof(hash_nodeintfloat)*initial_size);
return ht;
} int __get_hashintfloat(hash_tableintfloat* ht,int index) { int hash_value = ht->hn[index].hash_value & 0x7FFFFFFF;
return hash_value;
} int __get_dibintfloat(hash_tableintfloat* ht,int index) { int dib = (index + ht->capacity + ht->hn[index].hash_value &0x7FFFFFFF ) % ht->capacity;
return dib;
} int __rhht_check_increase_tointfloat(hash_tableintfloat *ht,float increase_factor = 1.50f) { if (ht->element_num < ht->treshold) { return 0;
} int old_capacity = ht->capacity;
hash_nodeintfloat* old_hn = ht->hn;
int new_capacity = int(ht->capacity * increase_factor);
hash_nodeintfloat *hn = (hash_nodeintfloat*)malloc(sizeof(hash_nodeintfloat) * new_capacity);
memset(hn,0,sizeof(hash_nodeintfloat) * new_capacity);
ht->hn = hn;
ht->element_num = 0;
ht->capacity = new_capacity;
ht->treshold = int(ht->capacity * ht->load_factor_percent);
printf("increased from %d to %d \n",old_capacity,new_capacity);
if (ht->capacity - ht->treshold < 2) { ht->treshold = ht->capacity - 2;
} for (int i=0;
i<old_capacity;
i++) { if (old_hn[i].hash_value > 0) { __rhht_insert_helperintfloat(ht,old_hn[i].key,old_hn[i].value);
} } free(old_hn);
return ht->capacity;
} void dump_hash_tableintfloat(hash_tableintfloat * ht){ static int g_dump_serial=0;
printf(" g_dump_serial is %d\n",g_dump_serial++);
int out_put_count = -1;
for (int i=0;
	i<ht->capacity;
	i++) { if (ht->hn[i].hash_value > 0) { printf("[index%d:hash_value%d,dib%d~key%d,value%d]    \n",i,ht->hn[i].hash_value,(ht->capacity + i - ht->hn[i].hash_value) % ht->capacity,ht->hn[i].key,ht->hn[i].value);
out_put_count++;
} if (out_put_count%4 == 3) { ;
} } printf("\n\n");
} int __rhht_insert_helperintfloat(hash_tableintfloat *ht,int key,float value,int start_table_pos_instinct){ int is_increased = __rhht_check_increase_tointfloat(ht);
int size = ht->capacity;
int inserted_hash_value = hash_function_wrapper_intfloat(key,size);
int table_pos = inserted_hash_value;
if ((!is_increased) && (-1!=start_table_pos_instinct)) { table_pos = start_table_pos_instinct;
} while (true) { table_pos = table_pos % size;
int table_hash = ht->hn[table_pos].hash_value;
if (table_hash == 0) { ht->hn[table_pos].key = key;
ht->hn[table_pos].value = value;
ht->hn[table_pos].hash_value = inserted_hash_value;
ht->element_num++;
return table_pos;
} int table_dib = (table_pos + size - table_hash & 0x7FFFFFFFU)%size;
int inserted_dib = (table_pos + size - (inserted_hash_value & 0x7FFFFFFFU) )%size;
if(table_dib < inserted_dib) { if (table_hash & 0x80000000) { ht->hn[table_pos].key = key;
ht->hn[table_pos].value = value;
ht->hn[table_pos].hash_value= inserted_hash_value;
ht->element_num++;
return table_pos;
} hash_nodeintfloattmp_hash_node = ht->hn[table_pos];
ht->hn[table_pos].hash_value = inserted_hash_value;
ht->hn[table_pos].key = key;
ht->hn[table_pos].value = value;
inserted_hash_value = tmp_hash_node.hash_value;
key = tmp_hash_node.key;
value = tmp_hash_node.value;
} table_pos++;
}} int __rhht_find_helper_intfloat(hash_tableintfloat*ht,int key,int *insertpos_output=0){ int size = ht->capacity;
int hash_value =hash_function_wrapper_intfloat(key,size);
int table_pos = hash_value;
int find_len = 0;
while(true) { int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;
if (ht->hn[table_pos].hash_value == 0) { return -1;
} if (dib < find_len) {return -1;
} if (ht->hn[table_pos].hash_value == hash_value) { if (insertpos_output && (*insertpos_output == -1) ) *insertpos_output = table_pos;
if (ht->hn[table_pos].key == key) { return table_pos;
} } table_pos++;
table_pos %= size;
find_len++;
}} int rhht_multi_insert_intfloat(hash_tableintfloat *ht,int key,float value){ return __rhht_insert_helperintfloat(ht,key,value);
} int rhht_unique_insert_intfloat(hash_tableintfloat *ht,int key,float value){ int endpos = -1;
int index = __rhht_find_helper_intfloat(ht,key,&endpos);
if (index==-1) { index = __rhht_insert_helperintfloat(ht,key,value,endpos);
} return index;
} int rhht_unique_overwrite_insert_intfloat(hash_tableintfloat *ht,int key,float value){ int endpos = -1;
int index = __rhht_find_helper_intfloat(ht,key,&endpos);
if (index==-1) { index = __rhht_insert_helperintfloat(ht,key,value,endpos);
} else { ht->hn[index].key = key;
ht->hn[index].value = value;
} return index;
} int __rhht_remove_one_helper_intfloat(hash_tableintfloat *ht,int key){ int shift_start,shift_end;
int index = __rhht_find_helper_intfloat(ht,key);
if (index == -1) return -1;
ht->hn[index].hash_value |= 0x80000000;
ht->element_num--;
return index;
} int __rhht_find_helper_bs_move_range_intfloat(hash_tableintfloat *ht,int key,int *findpos_output=0,int *shift_endpos_output=0){ int find_result = -1;
int size = ht->capacity;
int hash_value = hash_function_wrapper_intfloat(key,size);
int table_pos = hash_value;
int find_len = 0;
while(true) { int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;
if (ht->hn[table_pos].hash_value == 0) { if (findpos_output) *findpos_output = table_pos;
find_result = -1;
break;
} else if (dib < find_len) { if (findpos_output) *findpos_output = table_pos;
find_result = -1;
break;
} else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key) { if (findpos_output) *findpos_output = table_pos;
find_result = table_pos;
break;
} table_pos++;
table_pos %= size;
find_len++;
} if(find_result != -1) { int find_result2 = find_result;
if (shift_endpos_output) *shift_endpos_output = find_result2;
while (true) { if (ht->hn[find_result2].hash_value == 0) break;
int dib = (find_result2 + size - ht->hn[find_result2].hash_value & 0x7FFFFFFF) % size;
if ( (find_result2!=find_result) && (dib == 0) ) break;
if (shift_endpos_output) *shift_endpos_output = find_result2;
find_result2++;
find_result2 %=size;
} } return find_result;
} int __rhht_remove_one_helper_bs_intfloat(hash_tableintfloat *ht,int key){ int shift_start,shift_end;
int index = __rhht_find_helper_bs_move_range_intfloat(ht,key,&shift_start,&shift_end);
if (index == -1) return -1;
int size = ht->capacity;
int shift_index = shift_start;
int shift_count = 0;
while( shift_index != shift_end) { shift_count ++;
ht->hn[shift_index] = ht->hn[ (shift_index+1) % size];
shift_index = (shift_index+1)%size;
} if (shift_count>0) { } ht->hn[shift_end].hash_value = 0x00000000;
ht->element_num--;
returnindex;
} int __rhht_backshift_remove_on_fly_intfloat(hash_tableintfloat *ht,int key,int* remove_count=0,int remove_all=0){ int find_result = -1;
int size = ht->capacity;
int hash_value = hash_function_wrapper_intfloat(key,size);
int table_pos = hash_value;
int find_len = 0;
if (remove_count) { *remove_count = 0;
} while(true) { int dib = (table_pos + size - hash_value & 0x7FFFFFFFU)%size;
if (ht->hn[table_pos].hash_value == 0) { find_result = -1;
break;
} else if (dib < find_len) { find_result = -1;
break;
} else if (ht->hn[table_pos].hash_value == hash_value && ht->hn[table_pos].key == key) { find_result = table_pos;
break;
} table_pos++;
table_pos %= size;
find_len++;
} if(find_result != -1) { ht->hn[find_result].hash_value = 0;
if (remove_count) *remove_count += 1;
int shift_index = (find_result+1)%size;
int need_shift_left_count = 1;
while (true) { if (ht->hn[shift_index].hash_value == 0) break;
int dib = (shift_index + size - ht->hn[shift_index].hash_value & 0x7FFFFFFF) % size;
if ( (shift_index!=find_result) && (dib ==0) ) break;
if (remove_all && ht->hn[shift_index].key == key) { ht->hn[shift_index].hash_value = 0;
need_shift_left_count++;
if (remove_count) *remove_count +=1;
} else { need_shift_left_count = ((dib)<=(need_shift_left_count)?dib:need_shift_left_count);
if (need_shift_left_count>0) { int index = (shift_index + size - need_shift_left_count)%size;
ht->hn[index] = ht->hn[shift_index];
ht->hn[shift_index].hash_value = 0;
} } shift_index++;
shift_index %= size;
} } return find_result;
} int __rhht_remove_one_or_all_helper_bs_intfloat(hash_tableintfloat *ht,int key,int is_remove_all){ int remove_count=0;
int index = __rhht_backshift_remove_on_fly_intfloat(ht,key,&remove_count,is_remove_all);
if (index == -1) return-1;
ht->element_num -= remove_count;
return remove_count;
} int rhht_remove_one_intfloat(hash_tableintfloat *ht,int key){ int removed_count = __rhht_remove_one_helper_intfloat(ht,key);
if (-1==removed_count) return 0;
return 1;
} int rhht_remove_one_bs_intfloat(hash_tableintfloat *ht,int key){ int removed_count = __rhht_remove_one_or_all_helper_bs_intfloat(ht,key,0);
return removed_count;
} int rhht_remove_one_bs_slow_intfloat(hash_tableintfloat *ht,int key){ int removed_count = __rhht_remove_one_helper_bs_intfloat(ht,key);
if (-1==removed_count) return 0;
return 1;
} int rhht_remove_all_intfloat(hash_tableintfloat *ht,int key){ int removed_num = 0;
int removed_index = __rhht_remove_one_helper_intfloat(ht,key);
while (-1 != removed_index) { ++removed_num;
removed_index = __rhht_remove_one_helper_intfloat(ht,key);
} return removed_num;
} int rhht_remove_all_bs_intfloat(hash_tableintfloat *ht,int key){ int removed_num = __rhht_remove_one_or_all_helper_bs_intfloat(ht,key,1);
return removed_num;
} int rhht_remove_all_bs_slowintfloat(hash_tableintfloat *ht,int key){ int removed_num = 0;
int removed_index = __rhht_remove_one_helper_bs_intfloat(ht,key);
if(-1 != removed_index) { ++removed_num;
removed_index = __rhht_remove_one_helper_bs_intfloat(ht,key);
} return removed_num;
}

struct test_node_type
{
	int a;
};
inline int hash_function_test(test_node_type v,int hash_size)

{
	int hv = v.a;
	return hv;
}

//HT_declare_new_hashtable_type(hash_function_test,test_node_type,float);

int main()
{
	printf("%s",TO_STR(HT_declare_new_hashtable_type(hash_function,int,float)));
}
#if 0

//HT_declare_new_hashtable_type(hash_function,int,float);

int main()
{
	printf("%s",TO_STR(HT_declare_new_hashtable_type(hash_function,int,float)));
	return 0;
}
#endif
#if 0
#include "hash_table_macro.h"
#include<Windows.h>
#include<map>
#include <hash_map>
using namespace  std;
typedef multimap<int,int> IntMultimapType;
typedef hash_map<int,int> IntHashMapType;

int g_serial_id =0;

//测试map的插入删除操作的时间效率
int test1()
{
	int arry[25000] = {0};//{1,57,20,81,59,48,36,90,83,75,18,86,72,52,31,2,10,37,15,17,99,45,12,1,1,38,54,58,61,61,17,67,46,36,6,61,79,81,52,31,88,73,96,93,54,15,47,24,87,21,78,85,100,100,62,40,27,30,85,3,38,10,68,6,1,92,28,28,59,70,84,73,49,21,75,47,46,95,75,11,60,39,74,61,58,37,16,23,43,81,52,99,76,35,17,66,50,7,70,51};
	for ( int i=0;i<25000-1;i++)
	{
		arry[i] = rand();
	}
	int hash_table_size = 10;
	hash_table * ht = create_hash_table();
	IntHashMapType imt;

	printf("%d 's hash value is:%d \n",1,hash_function(1,6));
	printf("%d 's hash value is:%d \n",57,hash_function(57,6));
	printf("%d 's hash value is:%d \n",20,hash_function(20,6));
	printf("%d 's hash value is:%d \n",81,hash_function(81,6));
	printf("%d 's hash value is:%d \n",59,hash_function(59,6));

start:
	unsigned long curr_t = GetTickCount();
	for(int times  = 0;times<10;times++)
		for (int i=0;i<25000;i++)
		{
			int key = arry[i];
			int value = key * 10;
			rhht_unique_overwrite_insert(ht,key,value);
			if (key % 2 == 0)
			{
				rhht_remove_one_bs(ht,key);
			}
		}
	printf(" ht size:	%d\n ",ht->element_num);
	printf("my hash table elipsed time is %d \n",GetTickCount() - curr_t);
	//dump_hash_table(ht);

	curr_t = GetTickCount();
	for(int times  = 0;times<10;times++)
		for (int i=0;i<25000;i++)
		{
			int key = arry[i];
			int value = key * 10;
			imt.insert(make_pair(key,value));
			if (key % 2 == 0)
			{
				imt.erase(key);
			}
		}

	printf(" imt size:	%d\n ",imt.size());
	printf("map table	elipsed time is %d \n",GetTickCount() - curr_t);

	goto start;

	return 0;
}

//测试map插入删除的正确性
int test2()
{
	hash_table * ht_before_insert=NULL;
	hash_table * ht_before_remove=NULL;
	hash_table * ht_after_remove=NULL;
	hash_table * ht = create_hash_table(100);
	IntHashMapType imt;
	int key = 0;
	printf("//start code...\n");
	start:
	for (int i=0;i<250;i++)
	{
		key =  GetTickCount() % 2000;
		int value = key * 10;
		rhht_unique_overwrite_insert(ht,key,value);
		//imt.insert(make_pair(key,value));
		//duplicate(ht,&ht_before_insert);
		//rhht_unique_insert(ht,key,value);
		//duplicate(ht,&ht_before_remove);
		//printf("rhht_unique_insert(ht,%d,%d) \n",key,value);
		//imt.insert(make_pair(key,value));
		imt[key] = value;
		//::Sleep(1);
		int cnt = GetTickCount();
		if (cnt % 3 == 0)
		{
			//printf("rhht_remove_one2(ht,%d) \n ",key+1);

			rhht_remove_one_bs(ht,key+1);
			//duplicate(ht,&ht_after_remove);

			imt.erase(key+1);
		}
	}

	printf("my rh size is : %d,map size is :%d\n",ht->element_num,imt.size());

	for (int i=0;i<ht->capacity;i++)
	{
		if (ht->hn[i].hash_value > 0)
		{
			int key = ht->hn[i].key;
			int value = ht->hn[i].value;
			IntHashMapType::iterator it = imt.find(key);
			if (it == imt.end())
			{
				//dump_hash_table(ht_before_insert);
				//dump_hash_table(ht_before_remove);
				//dump_hash_table(ht_after_remove);

				dump_hash_table(ht);
				printf("error occurred.%d,%d,hash_value:%d,index:%d,总大小:%d\n",key,value,ht->hn[i].hash_value,i,ht->capacity);
			}
			else
			{
				if (it->second!=value)
				{
					printf("value not equal...%d,%d\n",key,value);
				}
			}
		}
	}

	goto start;
	return 0;
}

int test3()
{
	hash_table * ht = create_hash_table(1024);
	IntMultimapType imt;

start:
	for (int i=0;i<250;i++)
	{
		int key =  GetTickCount() % 1000;
		int value = key * 10;
		//rhht_unique_overwrite_insert(ht,key,value);
		//imt.insert(make_pair(key,value));

		rhht_multi_insert(ht,key,value);
		imt.insert(make_pair(key,value));
		::Sleep(1);
		if (GetTickCount() % 10 == -1)
		{
			//rhht_remove_helper(ht,key+1);
			rhht_remove_all(ht,key+1);
			imt.erase(key+1);
		}
	}

	printf("my rh size is : %d,map size is :%d\n",ht->element_num,imt.size());

	for (int i=0;i<ht->capacity;i++)
	{
		if (ht->hn[i].hash_value > 0)
		{
			int key = ht->hn[i].key;
			int value = ht->hn[i].value;
			IntMultimapType::iterator it = imt.find(key);
			if (it == imt.end())
			{
				printf("error occurred.%d,%d\n",key,value);
			}
			else
			{
				if (it->second!=value)
				{
					printf("value not equal...%d,%d\n",key,value);
				}
			}
		}
	}

	goto start;
	return 0;
}

//测试backshift deleteion的正确性
int test4()
{
	int g_serial = 1000;

	int arr[5] = {0};
	for (int i=0;i<5;i++)
	{
		arr[i] = rand() % 10;
	}

	hash_table * ht = create_hash_table(7);
	IntHashMapType imt;
	int loop_count = 0;
start:
	//插入列表里的某个随机值
	int index = rand() % 5;
	int insert_v = arr[index];

	printf("111111 inserted key%d ,loop_count:%d \n",insert_v,++loop_count);
	if (loop_count == 17)
		printf("");
	rhht_unique_insert(ht,insert_v,++g_serial);
	imt.insert(make_pair(insert_v,g_serial));
	dump_hash_table(ht);
	//删除列表里的某个随机值
	index = rand() % 5;
	int remove_v = arr[index];
	if (loop_count == 17)
	{
		//dump_hash_table(ht);
		printf("");
	}
	if (loop_count == 77)
	{
		dump_hash_table(ht);
	}
	printf("222222 removed key%d ,loop_count:%d \n",remove_v,loop_count);
	rhht_remove_one_bs(ht,remove_v);
	imt.erase(remove_v);
	dump_hash_table(ht);
	if (loop_count == 17)
	{
		//dump_hash_table(ht);
		printf("");
	}
	if (loop_count == 77)
	{
		dump_hash_table(ht);
	}
	//测试结果是否正确
	for (int i=0;i<ht->capacity;i++)
	{
		if (ht->hn[i].hash_value > 0)
		{
			int key = ht->hn[i].key;
			int value = ht->hn[i].value;
			IntHashMapType::iterator it = imt.find(key);
			if (it == imt.end())
			{
				dump_hash_table(ht);
				printf("error occurred.%d,%d,hash_value:%d,index:%d,total size:%d\n",key,value,ht->hn[i].hash_value,i,ht->capacity);
			}
			else
			{
				if (it->second!=value)
				{
					printf("value not equal...%d,%d\n",key,value);
				}
			}
		}
	}
	goto start;
	//dump_hash_table(ht);
	return 0;
}

//测试map的插入删除操作的时间效率
int test5()
{
	int arry[25000] = {0};//{1,57,20,81,59,48,36,90,83,75,18,86,72,52,31,2,10,37,15,17,99,45,12,1,1,38,54,58,61,61,17,67,46,36,6,61,79,81,52,31,88,73,96,93,54,15,47,24,87,21,78,85,100,100,62,40,27,30,85,3,38,10,68,6,1,92,28,28,59,70,84,73,49,21,75,47,46,95,75,11,60,39,74,61,58,37,16,23,43,81,52,99,76,35,17,66,50,7,70,51};
	for ( int i=0;i<25000-1;i++)
	{
		arry[i] = rand();
	}
	int hash_table_size = 10;
	hash_table * ht = create_hash_table();
	hash_table * ht_slow = create_hash_table();

start:
	unsigned long curr_t = GetTickCount();
	for(int times  = 0;times<10;times++)
		for (int i=0;i<25000;i++)
		{
			int key = arry[i];
			int value = key * 10;
			rhht_unique_insert(ht,key,value);
			if (key % 2 == 0)
			{
				rhht_remove_all_bs(ht,key);
			}
		}
		//printf(" ht size:	%d\n ",ht->element_num);
		printf("my hash table elipsed time is %d \n",GetTickCount() - curr_t);
		//dump_hash_table(ht);

		curr_t = GetTickCount();
		for(int times  = 0;times<10;times++)
			for (int i=0;i<25000;i++)
			{
				int key = arry[i];
				int value = key * 10;
				rhht_unique_insert(ht_slow,key,value);
				if (key % 2 == 0)
				{
					rhht_remove_all_bs_slow(ht_slow,key);
				}
			}

			//printf("ht_slow size:	%d\n ",ht_slow->element_num);
			printf("ht_slow	elipsed time is %d \n",GetTickCount() - curr_t);

			goto start;

			return 0;
}

void test6(hash_table*);

void test_dynamic_array();

int main()
{
	test5();
	//test_dynamic_array();
	//hash_table * ht = create_hash_table(100);
	//test6(ht);
	IntHashMapType imt;
	imt.insert(make_pair(10,20));
	imt.insert(make_pair(100,200));
	imt.insert(make_pair(100,2000));
	imt.insert(make_pair(3,300));

	//imt.erase(100);
	for(IntHashMapType::iterator it = imt.begin();it!=imt.end();it++)
	{
		printf("[%d,%d] \n",it->first,it->second);
	}

	return 0;
}

/*
void test_dynamic_array()
{
	dynamic_int_array arr;
	array_init(&arr);

	for (int i=0;i<100;i++)
	{
		array_set(&arr,i,i*10);
		//dump_array(&arr);
	}
	dump_array(&arr);
	for (int i=0;i<100;i++)
	{
		array_remove(&arr,5,NULL);
		dump_array(&arr);
	}
	dump_array(&arr);

	array_deinit(&arr);
}
*/

#endif