#include "rhhashing.h"
#include "rh_hahs_table.hpp"
#include<Windows.h>
#include<map>
using namespace  std;
multimap<int,int> mm;
typedef map<int,int> IntMapType;

int g_serial_id =0;
int main()
{	////			0	1	2	3	4	5	6	7	8	9
	int arr[] = {8,	1,	1,	1,	2,	4,	-4,	5,	-8,	8};
	//	dib		 2	0	1	2	2	1	8	2	9	1
	/*hash_table<int,int> ht;

	for (int i=0;i<10;i++)
	{
		ht.buffer[i].hash = arr[i];
		ht.buffer[i].key =  100+i;
	}
	ht.dump();
	ht.insert(5,200);
	ht.dump();*/
	/*
	[8,100] [1,101] [1,102] [1,103] [2,104] [4,105] [-2,106]   [5,107]	[5,5]   [8,109]
	*/
	/*for (int i=0;i<10;i++)
	{
	int v = hash_function(i,10);
	printf("[%d]=%d,\n ",i,v);
	}*/
	int hash_table_size = 10;
	hash_table * ht = create_hash_table();
	IntMapType imt;


	unsigned long curr_t = GetTickCount();
	for(int times  = 0;times<1000;times++)
		for (int i=0;i<250;i++)
		{
			int key = rand() % 100;
			int value = rand() % 200 + 100;
			if (ht->element_num==100)
			{
				printf("stop..");
			}
			rhht_unique_overwrite_insert(ht,key,value);
			if (GetTickCount() % 2 == 0)
			{
				rhht_remove(ht,key+1);
			}		
		}

	printf("my hash table elipsed time is %d",GetTickCount() - curr_t);


	curr_t = GetTickCount();
	for(int times  = 0;times<1000;times++)
		for (int i=0;i<250;i++)
		{
			int key = rand() % 100;
			int value = rand() % 200 + 100;
			imt.insert(make_pair(key,value));
			if (GetTickCount() % 2 == 0)
			{
				imt.erase(key+1);
			}		
		}

	printf("	%d",imt.size());
	printf("elipsed time is %d",GetTickCount() - curr_t);
 

	return 0;
}