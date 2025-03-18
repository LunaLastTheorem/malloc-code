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
        if (ptr->size >= size + sizeof(node_t))
        {
            int remainder = ptr->size - (size + sizeof(node_t));

            node_t *new_block = (node_t *)((char *)ptr + sizeof(node_t) + size);
            new_block->size = remainder;
            new_block->next = ptr->next;

            ptr->size = remainder;

            if (prev)
            {
                prev->next = new_block;
            }
            else
            {
                head = new_block;
            }
            return ((char *)ptr) + sizeof(node_t);
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

        prev = ptr;
        ptr = ptr->next;
    }

    return NULL;
}

void dealloc(char *cPtr)
{
    if (cPtr == NULL)
        return;

    node_t *block = (node_t *)(cPtr - sizeof(node_t));

    node_t *ptr = head;
    node_t *prev = NULL;

    while (ptr != NULL && ptr < block)
    {
        prev = ptr;
        ptr = ptr->next;
    }

    block->next = ptr;

    if (prev)
    {
        prev->next = block;
    }
    else
    {
        head = block;
    }
}