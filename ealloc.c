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
    if (size % MINALLOC != 0 || size > PAGESIZE - sizeof(node_t))
    {
        return NULL;
    }

    if (page_list != NULL)
    {
        node_t *ptr = page_list->head;
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
                        page_list->head = allocated;
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
                        page_list->head = ptr->next;
                    }
                }
                return ((char *)ptr) + sizeof(node_t);
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    // No suitable block found, allocate a new page
    page_t *new_page = (page_t *)mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (new_page == MAP_FAILED)
    {
        return NULL;
    }

    new_page->start = (void *)new_page;
    new_page->next = page_list;
    new_page->head = (node_t *)((char *)new_page + sizeof(page_t));

    new_page->head->size = PAGESIZE - sizeof(page_t) - sizeof(node_t);
    new_page->head->next = NULL;
    page_list = new_page;

    node_t *ptr = new_page->head;

    if (ptr->size >= size)
    {
        int remainder = ptr->size - (size + sizeof(node_t));
        if (remainder >= sizeof(node_t))
        {
            node_t *allocated = (node_t *)((char *)ptr + sizeof(node_t) + size);
            allocated->size = remainder;
            allocated->next = NULL;

            new_page->head = allocated;
        }
        return ((char *)ptr) + sizeof(node_t);
    }

    return NULL;
}

void dealloc(char *cPtr)
{
    if (cPtr == NULL)
    {
        return;
    }
}

void cleanup()
{
    page_t *ptr = page_list->start;

    while (ptr != NULL)
    {
        page_t *next = ptr->next;
        free(ptr);
        ptr = next;
    }

    page_list = NULL;
}
