#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phonebook_opt.h"

#ifdef HT
hashTable_t hashTable;

unsigned int hash1(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal += *key++;
    }
    return hashVal % ht->bucketSize;
}

#define hashFunc(k, h) hash1(k, h)

void initHashTable(unsigned int bucket_size, unsigned int slot_size)
{
    int i, j;

    memset(&hashTable, 0,sizeof(hashTable));

    hashTable.bucketSize = bucket_size;
    hashTable.slotSize = slot_size;

    for (i = 0; i < NUM_OF_THREADS; i++) {
        hashTable.bucket[i] = malloc(sizeof(hashEntry_t) * hashTable.bucketSize);
        for (j = 0; j < bucket_size; j++) {
            (hashTable.bucket[i] + j)->slot = (entry *)malloc(sizeof(entry) * hashTable.slotSize);
            (hashTable.bucket[i] + j)->slot_count = 0;
        }
    }
}

void freeHashTable()
{
    int i, j;

    for (i = 0; i < NUM_OF_THREADS; i++) {
        for (j = 0; j < hashTable.bucketSize; j++) {
            free((hashTable.bucket[i] + j)->slot);
            (hashTable.bucket[i] + j)->slot = NULL;
        }
        free(hashTable.bucket[i]);
        hashTable.bucket[i] = NULL;
    }
}
#endif

/* FILL YOUR OWN IMPLEMENTATION HERE! */
#ifdef HT
entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;
    int i,j;

    key = hashFunc(lastName, &hashTable);

    for (i = 0; i <= total_thread_used; i++) {
        hash = hashTable.bucket[i] + key;
        j = 0;
        while (j <= hash->slot_count) {
            if (strcasecmp(lastName, (hash->slot + j)->lastName) == 0) {
                return (hash->slot + j);
            }
            j++;
        }
    }
    return NULL;
}

entry *append(char lastName[], entry *e, int thd)
{
    unsigned int key = 0;
    hashEntry_t *hash;

    key = hashFunc(lastName, &hashTable);
    hash = hashTable.bucket[thd] + key ;

    strcpy((hash->slot + (hash->slot_count))->lastName, lastName);
    e = (hash->slot + (hash->slot_count));
    hash->slot_count++;
    assert(hash->slot_count <= NUM_OF_SLOT);
    return e;
}
#else
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
    static unsigned int phonebook_count = 0;

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
#endif
