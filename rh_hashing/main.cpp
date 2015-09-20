#include "rhhashing.h"
#include "rh_hahs_table.hpp"
#include<map>
using namespace  std;
multimap<int,int> mm;
typedef multimap<int,int> IntMapType;

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
	int should_delete = 0;
	for (int i=0;i<250;i++)
	{
		int key = rand() % 100;
		if (key==12 || key==84)
		{
			printf("stop...");
		}
		
		int value = rand() % 200 + 100;
		//imt[v]=v;
		
		//imt[key]=value;
			 
		imt.insert(std::make_pair(key,value));
		//rhht_unique_insert(ht,key,value);
		rhht_insert(ht,key,value);
 
		//rhht_remove(ht,key);
		//rhht_unique_insert(ht,key,value);
		//rhht_unique_overwrite_insert(ht,key,value);
	}
	 
	
	//dump_hash_table(ht);

	for (int i=0;i<ht->size;i++)
	{
		if (ht->hn[i].hash_value > 0)
		{
			mm.insert(std::make_pair(ht->hn[i].key,ht->hn[i].value));
		}
		
	}
	int i=0;
	for (IntMapType::iterator it = mm.begin();it!=mm.end();it++)
	{
		if(i++ %5==0)
			printf("\n");
		printf("[%d]=%d,",it->first,it->second);
	}

	printf("\n\n");
	

	i=0;
	for (IntMapType::iterator it = imt.begin();it!=imt.end();it++)
	{
		if(i++ %5==0)
			printf("\n");
		printf("[%d]=%d,",it->first,it->second);
	}

	//int index = __rhht_find(ht,7);
	//index = __rhht_find(ht,6);
	//index = __rhht_find(ht,5);
	//index = __rhht_find(ht,555);
	return 0;
}