#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#define MAX_LAST_NAME_SIZE 16

/* TODO: After modifying the original version, uncomment the following
 * line to set OPT properly */
#define OPT 1

#if defined(MLC) || defined(HT)
#define MAX_PHONE_BOOK_DATA 400000
#endif

typedef struct __PHONE_BOOK_DATA {
    char firstName[16];
    char email[16];
    char phone[10];
    char cell[10];
    char addr1[16];
    char addr2[16];
    char city[16];
    char state[2];
    char zip[5];
} phonebook_data;

typedef struct __PHONE_BOOK_ENTRY {
    char lastName[MAX_LAST_NAME_SIZE];
    phonebook_data *data;
#if !defined(MLC) && !defined(HT)
    struct __PHONE_BOOK_ENTRY *pNext;
#endif
} entry;

#ifdef HT
#define MAX_BUCKET_NUMS 7919
#define HANDLE_DATA_SIZE 50000
#define NUM_OF_THREADS (MAX_PHONE_BOOK_DATA/HANDLE_DATA_SIZE + 1)
#define NUM_OF_BUCKET (MAX_BUCKET_NUMS / NUM_OF_THREADS + 1)
#define NUM_OF_SLOT 2000

extern unsigned int total_thread_used;

typedef struct hashEntry_s {
    entry *slot;
    unsigned int slot_count;
} hashEntry_t;

typedef struct hashTable_s {
    unsigned int bucketSize;
    unsigned int slotSize;
    hashEntry_t *bucket[NUM_OF_THREADS];
} hashTable_t;

typedef struct thread_data_s {
    int start;
    int end;
    int thd;
} thread_data_t;

void initHashTable(unsigned int bucket_size, unsigned int slot_size);
void freeHashTable();
#endif

entry *findName(char lastName[], entry *pHead);

#ifdef HT
entry *append(char lastName[], entry *e, int thd);
#else
entry *append(char lastName[], entry *e);
#endif

#endif
