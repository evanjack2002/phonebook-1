#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#ifdef HT
#include <pthread.h>
#endif

#include IMPL

#ifdef OPT
#define OUT_FILE "opt.txt"
#else
#define OUT_FILE "orig.txt"
#endif

#define DICT_FILE "./dictionary/words.txt"

#ifdef HT
char buf[MAX_PHONE_BOOK_DATA][MAX_LAST_NAME_SIZE];
unsigned int total_thread_used = 0;

void *processArray(void *args)
{
    thread_data_t *data = (thread_data_t *)args;
    int start = data->start;
    int end   = data->end;
    int thd = data->thd;
    int i = 0;
    entry *e = NULL;

    for (i = start; i < end; i++) {
        e = append((char *) &(buf[i]), e, thd);
    }

    pthread_exit(NULL);
}
#endif

#ifdef RUN_TEST
void runTest(entry *pHead)
{
    int i = 0;
    char test[9][MAX_LAST_NAME_SIZE] = {
        "uninvolved",
        "bucket",
        "whiteshank",
        "odontomous",
        "pungoteague",
        "reweighted",
        "xiphisternal",
        "aaah",
        "yakattalo"
    };

    for (i = 0; i < 9; i++) {
        assert(0 == strcmp(findName(test[i], pHead)->lastName, test[i]));
    }
}
#endif

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec - t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i = 0;
#ifdef HT
    pthread_t threads[NUM_OF_THREADS];
    thread_data_t thread_data[NUM_OF_THREADS];
    unsigned int buf_line = 0;
    unsigned int  line_start = 0;
    void *tret;
    int j = 0;
#else
    char line[MAX_LAST_NAME_SIZE];
#endif
    struct timespec start, end;
    double cpu_time1, cpu_time2;


#ifdef HT
    memset(threads, 0, (sizeof(pthread_t) * NUM_OF_THREADS));
    memset(thread_data, 0, (sizeof(thread_data_t) * NUM_OF_THREADS));
    initHashTable(NUM_OF_BUCKET, NUM_OF_SLOT);
#endif

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

    /* build the entry */
    entry *pHead, *e;
#ifdef MLC
    pHead = (entry *) malloc(sizeof(entry) * MAX_PHONE_BOOK_DATA);
    memset(pHead, 0, sizeof(sizeof(entry) * MAX_PHONE_BOOK_DATA));
#else
    pHead = (entry *) malloc(sizeof(entry));
#endif
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
#if !defined(MLC) && !defined(HT)
    e->pNext = NULL;
#endif

#if defined(__GNUC__)
#ifdef MLC
    __builtin___clear_cache((char *) pHead, (char *) pHead + (sizeof(entry) * MAX_PHONE_BOOK_DATA));
#else
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
#endif
    clock_gettime(CLOCK_REALTIME, &start);
#ifdef HT
    while (fgets((char *)(&(buf[buf_line])), MAX_LAST_NAME_SIZE, fp)) {
        while (buf[buf_line][i] != '\0')
            i++;
        buf[buf_line][i - 1] = '\0';
        i = 0;
        buf_line++;

        if ((buf_line % HANDLE_DATA_SIZE) == 0) {
            thread_data[total_thread_used].start = line_start * HANDLE_DATA_SIZE;
            thread_data[total_thread_used].end   = buf_line - 1;
            thread_data[total_thread_used].thd = total_thread_used;
            pthread_create(&threads[total_thread_used], NULL, processArray, (void *)&thread_data[total_thread_used]);
            line_start++;
            total_thread_used++;
        }
        assert (total_thread_used < NUM_OF_THREADS);
    }

    if ((buf_line % HANDLE_DATA_SIZE) != 0) {
        thread_data[total_thread_used].start = line_start * HANDLE_DATA_SIZE;
        thread_data[total_thread_used].end   = buf_line - 1;
        thread_data[total_thread_used].thd   = total_thread_used;
        pthread_create(&threads[total_thread_used], NULL, processArray, (void *)&thread_data[total_thread_used]);
    }

    for (j = 0; j <= total_thread_used; j++) {
        pthread_join(threads[j], &tret);
    }
#else
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        e = append(line, e);
    }
#endif
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    /* close file as soon as possible */
    fclose(fp);

    e = pHead;

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
    e = pHead;

    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));

#if defined(__GNUC__)
#ifdef MLC
    __builtin___clear_cache((char *) pHead, (char *) pHead + (sizeof(entry) * MAX_PHONE_BOOK_DATA));
#else
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    findName(input, e);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

#ifdef RUN_TEST
    runTest(pHead);
#endif

    FILE *output = fopen(OUT_FILE, "a");
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

#if !defined(MLC) && !defined(HT)
    if (pHead->pNext) free(pHead->pNext);
#endif
    free(pHead);

#ifdef HT
    freeHashTable();
#endif

    return 0;
}
