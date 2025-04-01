#include "ealloc.h"

typedef struct node_t
{
    int size;
    struct node_t *next;
} node_t;

typedef struct page_t
{
    struct page_t *next;
    void *start;
    node_t *head;
} page_t;

page_t *page_list = NULL;

void init_alloc()
{
    page_list = NULL;
}

char *alloc(int size)
{
    if (size % MINALLOC || size > PAGESIZE - sizeof(node_t))
    {
        return NULL;
    }
}

void dealloc(char *cPtr)
{
    if (cPtr == NULL)
    {
        return;
    }

    node_t *ptr = (node_t *)(cPtr - sizeof(node_t));
    page_t *curr_page = page_list;
}

void cleanup()
{
    page_t *ptr = page_list;

    while (ptr != NULL)
    {
        page_t *next = ptr->next;
        munmap(ptr->start, 4096);
        free(ptr);
        ptr = next;
    }

    page_list = NULL;
}
