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
				rhht_remove_one2(ht,key);
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
		key =  GetTickCount() % 200;
		int value = key * 10;
		rhht_unique_overwrite_insert(ht,key,value);
		//imt.insert(make_pair(key,value));
		//duplicate(ht,&ht_before_insert);
		//rhht_unique_insert(ht,key,value);
		//duplicate(ht,&ht_before_remove);
		//printf("rhht_unique_insert(ht,%d,%d) \n",key,value);
		//imt.insert(make_pair(key,value));
		imt[key] = value;
		::Sleep(1);
		int cnt = GetTickCount();
		if (cnt % 3 == 0)
		{
			//printf("rhht_remove_one2(ht,%d) \n ",key+1);
				
			rhht_remove_one2(ht,key+1);
			//duplicate(ht,&ht_after_remove);
			
			imt.erase(key+1);
		}		
	
	}

	//printf("my rh size is : %d,map size is :%d\n",ht->element_num,imt.size());

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
	rhht_remove_one2(ht,remove_v);
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

int test5()
{
	return 0;
}
void test6(hash_table*);

void test_dynamic_array();
int main()
{
	test_dynamic_array();
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

void test6(hash_table *ht)
{
	//start code...
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_unique_insert(ht,197,1970);
	rhht_remove_one2(ht,198);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,13,130);
	rhht_remove_one2(ht,14);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_remove_one2(ht,29);
	rhht_unique_insert(ht,28,280);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,44,440);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,59,590);
	rhht_unique_insert(ht,75,750);
	rhht_unique_insert(ht,75,750);
	rhht_remove_one2(ht,76);
	rhht_unique_insert(ht,91,910);
	rhht_remove_one2(ht,92);
	rhht_unique_insert(ht,91,910);
	rhht_remove_one2(ht,92);
	rhht_unique_insert(ht,106,1060);
	rhht_remove_one2(ht,107);
	rhht_unique_insert(ht,106,1060);
	rhht_remove_one2(ht,107);
	rhht_unique_insert(ht,106,1060);
	rhht_remove_one2(ht,107);
	rhht_unique_insert(ht,106,1060);
	rhht_unique_insert(ht,122,1220);
	rhht_unique_insert(ht,122,1220);
	rhht_unique_insert(ht,122,1220);
	rhht_unique_insert(ht,122,1220);
	rhht_unique_insert(ht,122,1220);
	rhht_unique_insert(ht,122,1220);
	rhht_unique_insert(ht,122,1220);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,137,1370);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_unique_insert(ht,153,1530);
	rhht_remove_one2(ht,154);
	rhht_unique_insert(ht,169,1690);
	rhht_remove_one2(ht,170);
	rhht_unique_insert(ht,169,1690);
	rhht_remove_one2(ht,170);
	rhht_unique_insert(ht,169,1690);
	rhht_remove_one2(ht,170);
	rhht_unique_insert(ht,169,1690);
	rhht_remove_one2(ht,170);
	rhht_unique_insert(ht,169,1690);
	rhht_remove_one2(ht,170);
	rhht_unique_insert(ht,184,1840);
	rhht_remove_one2(ht,185);
	rhht_unique_insert(ht,184,1840);
	rhht_remove_one2(ht,185);
	rhht_unique_insert(ht,184,1840);
	rhht_remove_one2(ht,185);
	rhht_unique_insert(ht,184,1840);
	rhht_unique_insert(ht,0,0);
	rhht_unique_insert(ht,0,0);
	rhht_unique_insert(ht,0,0);
	rhht_unique_insert(ht,0,0);
	rhht_unique_insert(ht,0,0);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,15,150);
	rhht_unique_insert(ht,31,310);
	rhht_unique_insert(ht,31,310);
	rhht_unique_insert(ht,31,310);
	rhht_unique_insert(ht,31,310);
	rhht_unique_insert(ht,31,310);
	rhht_unique_insert(ht,31,310);
	rhht_unique_insert(ht,31,310);
	rhht_unique_insert(ht,31,310);
	rhht_remove_one2(ht,32);
	rhht_unique_insert(ht,47,470);
	rhht_remove_one2(ht,48);
	rhht_unique_insert(ht,47,470);
	rhht_remove_one2(ht,48);
	rhht_unique_insert(ht,47,470);
	rhht_remove_one2(ht,48);
	rhht_unique_insert(ht,47,470);
	rhht_remove_one2(ht,48);
	rhht_unique_insert(ht,62,620);
	rhht_remove_one2(ht,63);
	rhht_unique_insert(ht,62,620);
	rhht_remove_one2(ht,63);
	rhht_unique_insert(ht,62,620);
	rhht_remove_one2(ht,63);
	rhht_unique_insert(ht,62,620);
	rhht_remove_one2(ht,63);
	rhht_unique_insert(ht,62,620);
	rhht_remove_one2(ht,63);
	rhht_unique_insert(ht,78,780);
	rhht_unique_insert(ht,78,780);
	rhht_unique_insert(ht,78,780);
	rhht_unique_insert(ht,78,780);
	rhht_unique_insert(ht,78,780);
	rhht_unique_insert(ht,78,780);
	rhht_unique_insert(ht,93,930);
	rhht_unique_insert(ht,93,930);
	rhht_unique_insert(ht,93,930);
	rhht_unique_insert(ht,93,930);
	rhht_unique_insert(ht,93,930);
	rhht_unique_insert(ht,93,930);
	rhht_unique_insert(ht,109,1090);
	rhht_unique_insert(ht,109,1090);
	rhht_unique_insert(ht,109,1090);
	rhht_unique_insert(ht,109,1090);
	rhht_unique_insert(ht,109,1090);
	rhht_unique_insert(ht,109,1090);
	rhht_unique_insert(ht,109,1090);
	rhht_unique_insert(ht,109,1090);
	rhht_unique_insert(ht,109,1090);
	rhht_remove_one2(ht,110);
	rhht_unique_insert(ht,125,1250);
	rhht_remove_one2(ht,126);
	rhht_unique_insert(ht,125,1250);
	rhht_remove_one2(ht,126);
	rhht_unique_insert(ht,125,1250);
	rhht_remove_one2(ht,126);
	rhht_unique_insert(ht,125,1250);
	rhht_remove_one2(ht,126);
	rhht_unique_insert(ht,140,1400);
	rhht_remove_one2(ht,141);
	rhht_unique_insert(ht,140,1400);
	rhht_remove_one2(ht,141);
	rhht_unique_insert(ht,140,1400);
	rhht_remove_one2(ht,141);
	rhht_unique_insert(ht,140,1400);
	rhht_remove_one2(ht,141);
	rhht_unique_insert(ht,140,1400);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,156,1560);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,171,1710);
	rhht_unique_insert(ht,187,1870);
	rhht_unique_insert(ht,187,1870);
	rhht_unique_insert(ht,187,1870);
	rhht_unique_insert(ht,187,1870);
	rhht_unique_insert(ht,187,1870);
	rhht_unique_insert(ht,187,1870);
	rhht_unique_insert(ht,187,1870);
	rhht_unique_insert(ht,187,1870);
	rhht_remove_one2(ht,188);
	rhht_unique_insert(ht,3,30);
	rhht_remove_one2(ht,4);
	rhht_unique_insert(ht,3,30);
	rhht_remove_one2(ht,4);
	rhht_unique_insert(ht,3,30);
	rhht_remove_one2(ht,4);
	rhht_unique_insert(ht,3,30);
	rhht_remove_one2(ht,4);
	rhht_unique_insert(ht,3,30);
	rhht_remove_one2(ht,4);
	rhht_unique_insert(ht,18,180);
	rhht_remove_one2(ht,19);
	rhht_unique_insert(ht,18,180);
	rhht_remove_one2(ht,19);
	rhht_unique_insert(ht,34,340);
	rhht_unique_insert(ht,34,340);
	rhht_unique_insert(ht,34,340);
	rhht_unique_insert(ht,49,490);
	rhht_unique_insert(ht,49,490);
	rhht_unique_insert(ht,49,490);
	rhht_unique_insert(ht,65,650);
	rhht_unique_insert(ht,65,650);
	rhht_unique_insert(ht,65,650);
	rhht_unique_insert(ht,65,650);
	rhht_unique_insert(ht,65,650);
	rhht_remove_one2(ht,66);
	rhht_unique_insert(ht,81,810);
	rhht_remove_one2(ht,82);
	rhht_unique_insert(ht,81,810);
	rhht_remove_one2(ht,82);
	rhht_unique_insert(ht,96,960);
	rhht_remove_one2(ht,97);
	rhht_unique_insert(ht,96,960);
	rhht_unique_insert(ht,112,1120);
	rhht_unique_insert(ht,112,1120);
	rhht_unique_insert(ht,112,1120);
	rhht_unique_insert(ht,112,1120);
	rhht_unique_insert(ht,112,1120);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,127,1270);
	rhht_unique_insert(ht,143,1430);
	rhht_unique_insert(ht,143,1430);
	rhht_unique_insert(ht,143,1430);
	rhht_unique_insert(ht,143,1430);
	rhht_unique_insert(ht,143,1430);
	rhht_unique_insert(ht,143,1430);
	rhht_unique_insert(ht,143,1430);
	rhht_unique_insert(ht,143,1430);
	rhht_remove_one2(ht,144);
	rhht_unique_insert(ht,159,1590);
	rhht_remove_one2(ht,160);
	rhht_unique_insert(ht,159,1590);
	rhht_remove_one2(ht,160);
	rhht_unique_insert(ht,159,1590);
	rhht_remove_one2(ht,160);
	rhht_unique_insert(ht,159,1590);
	rhht_remove_one2(ht,160);
	rhht_unique_insert(ht,159,1590);
	rhht_remove_one2(ht,160);
	rhht_unique_insert(ht,174,1740);
	rhht_remove_one2(ht,175);
	rhht_unique_insert(ht,174,1740);
	rhht_remove_one2(ht,175);
	rhht_unique_insert(ht,174,1740);
	rhht_unique_insert(ht,190,1900);
	rhht_unique_insert(ht,190,1900);
	rhht_unique_insert(ht,190,1900);
	rhht_unique_insert(ht,190,1900);
	rhht_unique_insert(ht,190,1900);
	rhht_unique_insert(ht,5,50);
	rhht_unique_insert(ht,5,50);
	rhht_unique_insert(ht,5,50);
	rhht_unique_insert(ht,5,50);
	rhht_unique_insert(ht,5,50);
	rhht_unique_insert(ht,5,50);
	rhht_unique_insert(ht,21,210);
	rhht_unique_insert(ht,21,210);
	rhht_unique_insert(ht,21,210);
	rhht_unique_insert(ht,21,210);
	rhht_remove_one2(ht,22);
	rhht_unique_insert(ht,37,370);
	rhht_remove_one2(ht,38);
	rhht_unique_insert(ht,37,370);
	rhht_remove_one2(ht,38);
	rhht_unique_insert(ht,37,370);
	rhht_remove_one2(ht,38);
	rhht_unique_insert(ht,37,370);
	rhht_remove_one2(ht,38);
	rhht_unique_insert(ht,52,520);
	rhht_remove_one2(ht,53);
	rhht_unique_insert(ht,52,520);
	rhht_remove_one2(ht,53);
	rhht_unique_insert(ht,52,520);
	rhht_remove_one2(ht,53);
	rhht_unique_insert(ht,52,520);
	rhht_unique_insert(ht,68,680);
	rhht_unique_insert(ht,68,680);
	rhht_unique_insert(ht,68,680);
	rhht_unique_insert(ht,68,680);
	rhht_unique_insert(ht,68,680);
	rhht_unique_insert(ht,68,680);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,83,830);
	rhht_unique_insert(ht,99,990);
	rhht_unique_insert(ht,99,990);
	rhht_unique_insert(ht,99,990);
	rhht_unique_insert(ht,99,990);
	rhht_unique_insert(ht,99,990);
	rhht_unique_insert(ht,99,990);
	rhht_unique_insert(ht,99,990);
	rhht_remove_one2(ht,100);
	rhht_unique_insert(ht,115,1150);
	rhht_remove_one2(ht,116);
	rhht_unique_insert(ht,115,1150);
	rhht_remove_one2(ht,116);
	rhht_unique_insert(ht,115,1150);
	rhht_remove_one2(ht,116);
	rhht_unique_insert(ht,115,1150);
	rhht_remove_one2(ht,116);
	rhht_unique_insert(ht,115,1150);
	rhht_remove_one2(ht,116);
	rhht_unique_insert(ht,130,1300);
	rhht_remove_one2(ht,131);
	rhht_unique_insert(ht,130,1300);
	rhht_remove_one2(ht,131);
	rhht_unique_insert(ht,130,1300);
	rhht_unique_insert(ht,146,1460);
	rhht_unique_insert(ht,146,1460);
	rhht_unique_insert(ht,146,1460);
	rhht_unique_insert(ht,146,1460);
	rhht_unique_insert(ht,146,1460);
	rhht_unique_insert(ht,146,1460);
	rhht_unique_insert(ht,146,1460);
	rhht_unique_insert(ht,146,1460);
	rhht_unique_insert(ht,161,1610);
	rhht_unique_insert(ht,161,1610);
	rhht_unique_insert(ht,161,1610);
	rhht_unique_insert(ht,161,1610);
	rhht_unique_insert(ht,161,1610);
	rhht_unique_insert(ht,161,1610);
	rhht_unique_insert(ht,161,1610);
	rhht_unique_insert(ht,177,1770);
	rhht_unique_insert(ht,177,1770);
	rhht_unique_insert(ht,177,1770);
	rhht_unique_insert(ht,177,1770);
	rhht_unique_insert(ht,177,1770);
	rhht_unique_insert(ht,177,1770);
	rhht_unique_insert(ht,177,1770);
	rhht_remove_one2(ht,178);
	rhht_unique_insert(ht,193,1930);
	rhht_remove_one2(ht,194);
	rhht_unique_insert(ht,193,1930);
	rhht_remove_one2(ht,194);
	rhht_unique_insert(ht,193,1930);
	rhht_remove_one2(ht,194);
	rhht_unique_insert(ht,8,80);
	rhht_remove_one2(ht,9);
	rhht_unique_insert(ht,8,80);
	rhht_remove_one2(ht,9);
	rhht_unique_insert(ht,8,80);
	rhht_remove_one2(ht,9);
	rhht_unique_insert(ht,8,80);
	rhht_remove_one2(ht,9);
	rhht_unique_insert(ht,8,80);
	rhht_unique_insert(ht,24,240);
	rhht_unique_insert(ht,24,240);
	rhht_unique_insert(ht,24,240);
	rhht_unique_insert(ht,39,390);
	rhht_unique_insert(ht,55,550);
	rhht_unique_insert(ht,55,550);
	rhht_unique_insert(ht,55,550);
	rhht_unique_insert(ht,55,550);
	rhht_remove_one2(ht,56);
	rhht_unique_insert(ht,71,710);
	rhht_remove_one2(ht,72);
	rhht_unique_insert(ht,71,710);
	rhht_remove_one2(ht,72);
	rhht_unique_insert(ht,71,710);
	rhht_remove_one2(ht,72);
	rhht_unique_insert(ht,71,710);
	rhht_remove_one2(ht,72);
	rhht_unique_insert(ht,86,860);
	rhht_remove_one2(ht,87);
	rhht_unique_insert(ht,86,860);
	rhht_remove_one2(ht,87);
	rhht_unique_insert(ht,86,860);
	rhht_unique_insert(ht,102,1020);
	rhht_unique_insert(ht,102,1020);
	rhht_unique_insert(ht,102,1020);
	rhht_unique_insert(ht,102,1020);
	rhht_unique_insert(ht,102,1020);
	rhht_unique_insert(ht,102,1020);
	rhht_unique_insert(ht,133,1330);
	rhht_unique_insert(ht,133,1330);
	rhht_unique_insert(ht,133,1330);
	rhht_unique_insert(ht,133,1330);
	rhht_unique_insert(ht,133,1330);
	rhht_remove_one2(ht,134);
	rhht_unique_insert(ht,149,1490);
	rhht_remove_one2(ht,150);
	rhht_unique_insert(ht,149,1490);
	rhht_remove_one2(ht,150);
	rhht_unique_insert(ht,149,1490);
	rhht_remove_one2(ht,150);
	rhht_unique_insert(ht,164,1640);
	rhht_remove_one2(ht,165);
	rhht_unique_insert(ht,164,1640);
	rhht_remove_one2(ht,165);
	rhht_unique_insert(ht,164,1640);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,180,1800);
	rhht_unique_insert(ht,195,1950);
	rhht_unique_insert(ht,195,1950);
	rhht_unique_insert(ht,195,1950);
	rhht_unique_insert(ht,195,1950);
	rhht_unique_insert(ht,195,1950);
	rhht_unique_insert(ht,195,1950);
	rhht_unique_insert(ht,195,1950);
	rhht_unique_insert(ht,195,1950);
	rhht_unique_insert(ht,11,110);
	rhht_unique_insert(ht,11,110);
	rhht_unique_insert(ht,11,110);
	rhht_unique_insert(ht,11,110);
	rhht_unique_insert(ht,11,110);
	rhht_unique_insert(ht,11,110);
	rhht_unique_insert(ht,11,110);
	rhht_unique_insert(ht,11,110);
	rhht_remove_one2(ht,12);
	rhht_unique_insert(ht,27,270);
	rhht_remove_one2(ht,28);
	rhht_unique_insert(ht,27,270);
	rhht_remove_one2(ht,28);
	rhht_unique_insert(ht,27,270);
	rhht_remove_one2(ht,28);
	rhht_unique_insert(ht,27,270);
	rhht_remove_one2(ht,28);
	rhht_unique_insert(ht,42,420);
	rhht_remove_one2(ht,43);
	rhht_unique_insert(ht,42,420);
	rhht_remove_one2(ht,43);
	rhht_unique_insert(ht,42,420);
	rhht_remove_one2(ht,43);
	rhht_unique_insert(ht,42,420);
	rhht_unique_insert(ht,58,580);
	rhht_unique_insert(ht,58,580);
	rhht_unique_insert(ht,58,580);
	rhht_unique_insert(ht,58,580);
	rhht_unique_insert(ht,58,580);
	rhht_unique_insert(ht,73,730);
	rhht_unique_insert(ht,73,730);
	rhht_unique_insert(ht,73,730);
	rhht_unique_insert(ht,73,730);
	rhht_unique_insert(ht,89,890);
	rhht_unique_insert(ht,89,890);
	rhht_unique_insert(ht,89,890);
	rhht_unique_insert(ht,89,890);
	rhht_unique_insert(ht,89,890);
	rhht_unique_insert(ht,89,890);
	rhht_remove_one2(ht,90);
	rhht_unique_insert(ht,105,1050);
	dump_hash_table(ht);
	rhht_remove_one2(ht,106);
	rhht_unique_insert(ht,105,1050);
	rhht_remove_one2(ht,106);
	rhht_unique_insert(ht,105,1050);
	rhht_remove_one2(ht,106);
	rhht_unique_insert(ht,105,1050);
	rhht_remove_one2(ht,106);
	rhht_unique_insert(ht,120,1200);
	rhht_remove_one2(ht,121);
	rhht_unique_insert(ht,120,1200);
	rhht_remove_one2(ht,121);
	rhht_unique_insert(ht,120,1200);
	rhht_remove_one2(ht,121);
	rhht_unique_insert(ht,120,1200);
	rhht_remove_one2(ht,121);
	rhht_unique_insert(ht,120,1200);
	rhht_unique_insert(ht,136,1360);
	rhht_unique_insert(ht,136,1360);
	rhht_unique_insert(ht,136,1360);
	rhht_unique_insert(ht,136,1360);
	rhht_unique_insert(ht,136,1360);
	rhht_unique_insert(ht,136,1360);
	rhht_unique_insert(ht,136,1360);
	rhht_unique_insert(ht,136,1360);
	rhht_unique_insert(ht,151,1510);
	rhht_unique_insert(ht,151,1510);
	rhht_unique_insert(ht,151,1510);
	rhht_unique_insert(ht,151,1510);
	rhht_unique_insert(ht,151,1510);
	rhht_unique_insert(ht,151,1510);
	rhht_unique_insert(ht,167,1670);
	rhht_unique_insert(ht,167,1670);
	rhht_unique_insert(ht,167,1670);
	rhht_remove_one2(ht,168);
	rhht_unique_insert(ht,183,1830);
	rhht_remove_one2(ht,184);
	rhht_unique_insert(ht,183,1830);
	rhht_remove_one2(ht,184);
	rhht_unique_insert(ht,198,1980);
	rhht_remove_one2(ht,199);
	rhht_unique_insert(ht,198,1980);
	rhht_remove_one2(ht,199);
	rhht_unique_insert(ht,198,1980);
	rhht_remove_one2(ht,199);
	rhht_unique_insert(ht,14,140);
	rhht_unique_insert(ht,14,140);
	rhht_unique_insert(ht,14,140);
	rhht_unique_insert(ht,14,140);
	rhht_unique_insert(ht,14,140);
	rhht_unique_insert(ht,14,140);
	rhht_unique_insert(ht,14,140);
	rhht_unique_insert(ht,14,140);
	rhht_unique_insert(ht,29,290);
	rhht_unique_insert(ht,29,290);
	rhht_unique_insert(ht,29,290);
	rhht_unique_insert(ht,29,290);
	rhht_unique_insert(ht,29,290);
	rhht_unique_insert(ht,29,290);
	rhht_unique_insert(ht,29,290);
	rhht_unique_insert(ht,29,290);
	rhht_unique_insert(ht,45,450);
	rhht_unique_insert(ht,45,450);
	rhht_unique_insert(ht,45,450);
	rhht_unique_insert(ht,45,450);
	rhht_unique_insert(ht,45,450);
	rhht_unique_insert(ht,45,450);
	rhht_unique_insert(ht,45,450);
	rhht_unique_insert(ht,45,450);
	rhht_remove_one2(ht,46);
	rhht_unique_insert(ht,61,610);
	rhht_remove_one2(ht,62);
	rhht_unique_insert(ht,61,610);
	rhht_remove_one2(ht,62);
	rhht_unique_insert(ht,61,610);
	rhht_remove_one2(ht,62);
	rhht_unique_insert(ht,61,610);
	rhht_remove_one2(ht,62);
	rhht_unique_insert(ht,76,760);
	rhht_remove_one2(ht,77);
	rhht_unique_insert(ht,76,760);
	rhht_unique_insert(ht,92,920);
	rhht_unique_insert(ht,92,920);
	rhht_unique_insert(ht,92,920);
	rhht_unique_insert(ht,92,920);
	rhht_unique_insert(ht,92,920);
	rhht_unique_insert(ht,107,1070);
	rhht_unique_insert(ht,107,1070);
	rhht_unique_insert(ht,107,1070);
	rhht_unique_insert(ht,107,1070);
	rhht_unique_insert(ht,107,1070);
	rhht_unique_insert(ht,107,1070);
	rhht_unique_insert(ht,123,1230);
	rhht_unique_insert(ht,123,1230);
	rhht_unique_insert(ht,123,1230);
	rhht_unique_insert(ht,123,1230);
	rhht_unique_insert(ht,123,1230);
	rhht_unique_insert(ht,123,1230);
	rhht_remove_one2(ht,124);
	rhht_unique_insert(ht,139,1390);
	rhht_remove_one2(ht,140);
	rhht_unique_insert(ht,139,1390);
	rhht_remove_one2(ht,140);
	rhht_unique_insert(ht,139,1390);
	rhht_remove_one2(ht,140);
	rhht_unique_insert(ht,139,1390);
	rhht_remove_one2(ht,140);
	rhht_unique_insert(ht,139,1390);
	rhht_remove_one2(ht,140);
	rhht_unique_insert(ht,154,1540);
	rhht_remove_one2(ht,155);
}