#include "ealloc.h"

#define NUM_PAGES 4

typedef struct node_t
{
    int size;
    struct node_t *next;
} node_t;

node_t *pages[NUM_PAGES];
void *page_starts[NUM_PAGES];

void init_alloc()
{
    for (int i = 0; i < 4; i++)
    {
        pages[i] = NULL;
        page_starts[i] = NULL;
    }
}

char *alloc(int size)
{
    if (size % MINALLOC != 0 || size > PAGESIZE - sizeof(node_t))
    {
        return NULL;
    }

    for (int i = 0; i < NUM_PAGES; i++)
    {
        if (pages[i] == NULL)
        {
            void *ptr = (node_t *)mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
            if (ptr == MAP_FAILED)
            {
                return NULL;
            }

            page_starts[i] = ptr;
            pages[i] = (node_t *)ptr;
            pages[i]->size = PAGESIZE - sizeof(node_t);
            pages[i]->next = NULL;
        }

        node_t *ptr = pages[i];
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

                    if (prev)
                    {
                        prev->next = allocated;
                    }
                    else
                    {
                        pages[i] = allocated;
                    }

                    ptr->size = size;
                }
                else
                {
                    if (prev)
                    {
                        prev->next = ptr->next;
                    }
                    else
                    {
                        pages[i] = ptr->next;
                    }
                }
                return ((char *)ptr) + sizeof(node_t);
            }
            prev = ptr;
            ptr = ptr->next;
        }
    }
    return NULL;
}

void dealloc(char *cPtr)
{
    if (cPtr == NULL)
    {
        return;
    }

    for (int i = 0; i < NUM_PAGES; i++)
    {
        if (page_starts[i] == NULL)
        {
            continue;
        }
        char *page_start = page_starts[i];
        char *page_end = page_start + PAGESIZE;

        if (cPtr >= page_start && cPtr < page_end)
        {
            node_t *curr_block = (node_t *)(cPtr - sizeof(node_t));
            node_t *ptr = pages[i];
            node_t *prev = NULL;

            while (ptr != NULL && ptr < curr_block)
            {
                prev = ptr;
                ptr = ptr->next;
            }
            if (ptr != NULL && (char *)curr_block + sizeof(node_t) + curr_block->size == (char *)ptr)
            {
                curr_block->size += sizeof(node_t) + ptr->size;
                curr_block->next = ptr->next;
            }
            else
            {
                curr_block->next = ptr;
            }

            if (prev != NULL && (char *)prev + sizeof(node_t) + prev->size == (char *)curr_block) {
                prev->size += sizeof(node_t) + curr_block->size;
                prev->next = curr_block->next;
            } else {
                if (prev != NULL) {
                    prev->next = curr_block;
                } else {
                    pages[i] = curr_block;
                }
            }

            return;
        }
    }
}

void cleanup()
{
    printf("%s", "not necessary for this according to directions\n");
}
