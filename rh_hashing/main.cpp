#include "rhhashing.h"

int main()
{
	for (int i=0;i<255;i++)
	{
		int v = hash_function(i,255);
		printf("%d ",v);
	}

	return 0;
}