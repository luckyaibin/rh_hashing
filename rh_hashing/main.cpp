#include "rhhashing.h"
#include "rh_hahs_table.hpp"
#include<Windows.h>
#include<map>
#include <hash_map>
using namespace  std;
typedef multimap<int,int> IntMultimapType;
typedef hash_map<int,int> IntHashMapType;

//this is test branch
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
		for (int i=0;i<2500;i++)
		{
			int key = arry[i];
			int value = key * 10;			 
			rhht_unique_overwrite_insert(ht,key,value);
			if (key % 2 == 0)
			{
				rhht_remove_one(ht,key);
			}		
		}
	printf(" ht size:	%d\n ",ht->element_num);
	printf("my hash table elipsed time is %d \n",GetTickCount() - curr_t);
	//dump_hash_table(ht);

	curr_t = GetTickCount();
	for(int times  = 0;times<10;times++)
		for (int i=0;i<2500;i++)
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
	hash_table * ht = create_hash_table(1024);
	IntHashMapType imt;

	start:
	for (int i=0;i<250;i++)
	{
		int key =  GetTickCount() % 1000;
		int value = key * 10;
		//rhht_unique_overwrite_insert(ht,key,value);
		//imt.insert(make_pair(key,value));

		rhht_unique_insert(ht,key,value);
		imt.insert(make_pair(key,value));
		::Sleep(1);
		if (GetTickCount() % 5 == 0)
		{
			rhht_remove_one2(ht,key+1);
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
		if (GetTickCount() % 5 == 0)
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


	int arr[16] = {0};
	for (int i=0;i<16;i++)
	{
		arr[i] = rand() % 10;
	}

	hash_table * ht = create_hash_table(20);
	IntHashMapType imt;
	int loop_count = 0;
start:
	//插入列表里的某个随机值
	int index = rand() % 16;
	int insert_v = arr[index];

	printf("inserted value:%d ,loop_count:%d \n",insert_v,++loop_count);
	rhht_unique_insert(ht,insert_v,++g_serial);
	imt.insert(make_pair(insert_v,g_serial));

	//删除列表里的某个随机值
	index = rand() % 16;
	int remove_v = arr[index];
	if (loop_count == 13)
	{
		dump_hash_table(ht);
	}
	if (loop_count == 77)
	{
		dump_hash_table(ht);
	}
	printf("removed value:%d \n",remove_v);
	rhht_remove_one2(ht,remove_v);
	imt.erase(remove_v);
	if (loop_count == 13)
	{
		dump_hash_table(ht);
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

int main()
{
	test4();
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