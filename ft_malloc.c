#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>


typedef char ALIGN[16];

typedef union header {
	struct {
		size_t size;
		unsigned is_free;
		union header *next;
	} s;
	ALIGN stub;
} header_t;


pthread_mutex_t global_malloc_lock;

header_t *head, *tail;

header_t *get_free_block(size_t size);

void *ft_malloc(size_t size)
{
	size_t total_size;
	void *block;
	header_t *header;

	if (!size)
		return (NULL);
	pthread_mutex_lock(&global_malloc_lock);
	header = get_free_block(size);
	if(header)
	{
		header->s.is_free = 0;
		pthread_mutex_unlock(&global_malloc_lock);
		return ((void *) (header + 1));
	}
	total_size = sizeof(header_t) + size;
	block = sbrk(total_size);
	if(block == (void *) - 1)
		return (NULL);
	header = block;
	header->s.size = size;
	header->s.is_free = 0;
	header->s.next = NULL;
	if(!head)
		head = header;
	if(tail)
		tail->s.next = header;
	tail = header;
	pthread_mutex_unlock(&global_malloc_lock);
	return ((void *) (header + 1));
	
}

void ft_free(void *block)
{
	header_t *header, *tmp;
	void *curr_brk;
	
	if(!block)
		return ;
	header = (header_t *)block - 1;
	curr_brk = sbrk(0);

	// check if it's at the end of the heap
	if((char *) block + header->s.size == curr_brk)
	{
		if(head == tail)
			head = tail = NULL;
		else
		{
			tmp = head;
			while(tmp)
			{
				if(tmp->s.next == tail)
				{
					tmp->s.next = NULL;
					tail = tmp;
				}
				tmp = tmp->s.next;
			}
		}
		sbrk(0 - sizeof(header_t) - header->s.size);
		pthread_mutex_unlock(&global_malloc_lock);
		return ;
	}
	header->s.is_free = 1;
	pthread_mutex_unlock(&global_malloc_lock);
	return ;

}

header_t *get_free_block(size_t size)
{
	header_t *curr = head;
	while(curr)
	{
		if(curr->s.is_free && curr->s.size >= size)
			return curr;
		curr = curr->s.next;
	}
	return (NULL);
}


// Simple test
int main(void)
{
	char *str = ft_malloc(6);

	if(!str)
	{
		printf("Chi 7aja mahyach\n");
		return 0;
	}
	
	strcpy(str, "Walid");
	printf("%s\n", str);
}
