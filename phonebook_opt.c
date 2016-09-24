#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phonebook_opt.h"

#ifdef MLC
static unsigned int phonebook_count = 0;
#endif

/* FILL YOUR OWN IMPLEMENTATION HERE! */
entry *findName(char lastName[], entry *pHead)
{
    /* TODO: implement */
#ifdef MLC
    while (pHead != NULL) {
        if (strcasecmp(lastName, pHead->lastName) == 0)
            return pHead;
        pHead++;
    }
#else
    while (pHead != NULL) {
        if (strcasecmp(lastName, pHead->lastName) == 0)
            return pHead;
        pHead = pHead->pNext;
    }
#endif

    return NULL;
}

entry *append(char lastName[], entry *e)
{
#ifdef MLC
    phonebook_count++;
    assert(phonebook_count < MAX_PHONE_BOOK_DATA);
    strcpy(e->lastName, lastName);
    e++;
#else
    /* allocate memory for the new entry and put lastName */
    e->pNext = (entry *) malloc(sizeof(entry));
    e = e->pNext;
    strcpy(e->lastName, lastName);
    e->pNext = NULL;
#endif

    return e;
}
