#include "rhhashing.h"
#include "rh_hahs_table.hpp"
#include<map>
using namespace  std;
multimap<int,int> mm;


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
	for (int i=0;i<10;i++)
	{
		int v = hash_function(i,10);
		printf("[%d]=%d,\n ",i,v);
	}
	int hash_table_size = 10;
	hash_table * ht = create_hash_table(hash_table_size);

	dump_hash_table(ht);

	//			 0	1	2	3	4	5	6	7	8	9
	//int arr[] = {8,	1,	1,	1,	2,	4,	-4,	5,	-8,	8};
	//	dib		 2	0	1	2	2	1	8	2	9	1
	for (int i=0;i<10;i++)
	{
		ht->hn[i].hash_value = arr[i];
		ht->hn[i].val = 100+i;
	}
	dump_hash_table(ht);


	rhht_insert(ht,5);
	
	dump_hash_table(ht);

	//int index = __rhht_find(ht,7);
	//index = __rhht_find(ht,6);
	//index = __rhht_find(ht,5);
	//index = __rhht_find(ht,555);
	return 0;
}