
struct dynamic_int_array
{
	int size;
	int capacity;
	int buff_stack[256];
	int *buff_heap;
};

void dump_array(dynamic_int_array* arr)
{
	for (int i=0;i<arr->size;i++)
	{
		if (arr->buff_heap)
			printf("%d	",arr->buff_heap[i]);
		else
			printf("%d	",arr->buff_stack[i]);		
	}
	printf("\n\n\n");
}

int array_init(dynamic_int_array *arr)
{
	arr->size = 0;
	arr->capacity = 256;
	arr->buff_heap=NULL;
	return 0;
}

int array_deinit(dynamic_int_array *arr)
{
	arr->size = 0;
	arr->capacity = 256;
	if (arr->buff_heap)
		free(arr->buff_heap);
	arr->buff_heap=NULL;
	return 0;
}

int array_set(dynamic_int_array *arr,int index,int value)
{
	//��Ҫ���·�����ڴ�
	if (index>=arr->capacity)
	{
		int *old_buff_heap = arr->buff_heap;
		int old_capacity = arr->capacity;
		arr->capacity = (index+1) * 2;
		//memeory
		arr->buff_heap = (int*)malloc(sizeof(int) * arr->capacity);
		//clear memory
		memset(arr->buff_heap,0,sizeof(int) * arr->capacity);
		//֮ǰ�õ��Ƕ��ڴ棬�������ͷ�
		if (old_buff_heap)
		{
			memcpy(arr->buff_heap,old_buff_heap,sizeof(int) * old_capacity);
			free(old_buff_heap);
		}
		else//֮ǰ�õ���ջ�ڴ棬ֻ��Ҫ����
		{
			memcpy(arr->buff_heap,arr->buff_stack,sizeof(int) * old_capacity);
		}
	}
	if (index >= arr->size)
	{
		arr->size++;
	}
	if (arr->buff_heap)
		arr->buff_heap[index] = value;
	else
		arr->buff_stack[index] = value;
	return 0;
}
int array_get(dynamic_int_array *arr,int index,int *value)
{
	//out of range.
	if (index >= arr->size)
		return -1;
	if (arr->buff_heap)
		*value = arr->buff_heap[index];
	else
		*value = arr->buff_stack[index];
}
int array_remove(dynamic_int_array *arr,int index,int *value)
{
	//out of range.
	if (index >= arr->size)
		return -1;
	//remove and shift
	if (arr->buff_heap)
	{
		if(value)
			*value = arr->buff_heap[index];
		//ɾ���Ĳ������һ������Ҫ�ƶ�������ɾ���������һ��������Ҫ�ƶ�
		if (index < arr->size-1)
		{
			memcpy(&arr->buff_heap[index],&arr->buff_heap[index+1],sizeof(int)*(arr->size-1 - index));
		}
	}
	else
	{
		if(value)
			*value = arr->buff_stack[index];
		//ɾ���Ĳ������һ������Ҫ�ƶ�������ɾ���������һ��������Ҫ�ƶ�
		if (index < arr->size-1)
		{
			memcpy(&arr->buff_stack[index],&arr->buff_stack[index+1],sizeof(int)*(arr->size-1 - index));
		}
	}
	arr->size--;
	return 0;
}
