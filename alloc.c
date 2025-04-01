#include "alloc.h"

typedef struct node_t
{
    int size;
    struct node_t *next;
} node_t;

node_t *head;

int init_alloc()
{
    head = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (head == MAP_FAILED)
    {
        return 1;
    }

    head->size = PAGESIZE - sizeof(node_t);
    head->next = NULL;
    return 0;
}

int cleanup()
{
    return munmap(head, PAGESIZE);
}

char *alloc(int size)
{
    if (size % MINALLOC != 0)
    {
        return NULL;
    }

    node_t *ptr = head;
    node_t *prev = NULL;

    while (ptr != NULL)
    {
        if (ptr->size >= size)
        {
            int remainder = ptr->size - (size + sizeof(node_t));
            if (remainder >= sizeof(node_t))
            {
                node_t *allocated = (node_t *)((char *)ptr + sizeof(node_t) + size);
                allocated->size = remainder;
                allocated->next = ptr->next;

                ptr->size = size;
                ptr->next = NULL;

                if (prev)
                {
                    prev->next = allocated;
                }
                else
                {
                    head = allocated;
                }
            }
            else
            {
                if (prev)
                {
                    prev->next = ptr->next;
                }
                else
                {
                    head = ptr->next;
                }
            }
            return ((char *)ptr) + sizeof(node_t);
        }

        prev = ptr;
        ptr = ptr->next;
    }

    return NULL;
}

void dealloc(char *cPtr)
{
    node_t *curr_block = (node_t *)(cPtr - sizeof(node_t));
    node_t *ptr = head;
    node_t *prev = NULL;

    while (ptr != NULL && ptr < curr_block)
    {
        prev = ptr;
        ptr = ptr->next;
    }

    curr_block->next = ptr;

    if (ptr != NULL && (char *)curr_block + sizeof(node_t) + curr_block->size == (char *)ptr)
    {
        curr_block->size += sizeof(node_t) + ptr->size;
        curr_block->next = ptr->next;
    }

    else
    {
        curr_block->next = ptr;
    }

    if (prev != NULL && (char *)prev + sizeof(node_t) + prev->size == (char *)curr_block)
    {
        prev->size += sizeof(node_t) + curr_block->size;
        prev->next = curr_block->next;
    }

    else if (prev != NULL)
    {
        prev->next = curr_block;
    }
    
    else{
        head = curr_block;
    }
}