#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


//#define ASSERT_OK
#define POISON 666

//#ifndef ASSERT_OK
#define ASSERT_OK
#ifndef ASSERT_OK
#define ASSERT_OK
#else
#define ASSERT_OK if (ListOK(list) != 0) { Dump(list, ListOK(list)); assert(!"Ok");}
#endif
#define GYST_COUNTER 2

typedef int elem_t;

int const max_size = 30;

struct element {
    elem_t value;
    elem_t next;
    elem_t prev;
};

struct List2 {
    int max_size;
    int size;
    int end;

    struct element* head;
    struct element* tail;
    struct element free;
    struct element* elem;
};


int ListOK(struct List2* list) {

    int ret = 0;

    if (list == NULL) {
        ret = -1;
    }

    if (list->max_size < 0) {
        ret = 1;
    }

    if (list->size > max_size) {
        ret = 2;
    }

    if (list->head == NULL || list->tail == NULL || &list->free == NULL || list->elem == NULL || list->elem == NULL) {
        ret = 3;
    }

    if (list->tail->next != list->head->value) {
        ret = 4;
    }
    return ret;
}

int Dump(struct List2* list, int error) {

    FILE* dump_file = fopen("dump.txt", "a");

    fprintf(dump_file, "Calling Dump for list\n");

    if (list == NULL) {

        fprintf(dump_file, "Calling Dump for nullptr list\n");

    } else {

        fprintf(dump_file, "List %p\n", list);
        fprintf(dump_file, "List max size: %d\n", list->max_size);
        fprintf(dump_file, "Current list size: %d\n", list->size);


        if (list->head != NULL) {

            fprintf(dump_file, "Head value: %d\n", list->head->value);
            fprintf(dump_file, "Head next:  %d\n", list->head->next);
            fprintf(dump_file, "Head prev:  %d\n", list->head->prev);

        }

        if (list->tail != NULL) {

            fprintf(dump_file, "Tail value: %d\n", list->tail->value);
            fprintf(dump_file, "Tail next:  %d\n", list->tail->next);
            fprintf(dump_file, "Tail prev:  %d\n", list->tail->prev);

        }
        if (&list->free != NULL) {

            fprintf(dump_file, "Free current value: %d\n", list->free.value);
            fprintf(dump_file, "Free next value: %d\n", list->free.next);
            fprintf(dump_file, "Free prev value: %d\n", list->free.prev);

        }
        fprintf(dump_file, "\n\n\n");
        fprintf(dump_file, "Dumping elements:\n");


        for (int i = 0; i < list->max_size; ++i) {

            fprintf(dump_file, "Element [%d] value = %d;\n", i, list->elem[i].value);
            fprintf(dump_file, "Element [%d] next = %d;\n", i, list->elem[i].next);
            fprintf(dump_file, "Element [%d] prev = %d;\n", i, list->elem[i].prev);
            fprintf(dump_file, "\n");

        }
    }


    fprintf(dump_file, "Error code: %d ", error);
    if (error == -1) {
        printf("MEM REALLOCATION FAIL");
    }

    return 0;
}


int init_list(struct List2* list) {

    assert(max_size > 0);

    list->end = 1;
    list->max_size = max_size;
    list->elem = (struct element*) calloc(max_size + 1, sizeof(struct element));

    for (int i = 0; i < max_size; ++i) {

        list->elem[i].prev = POISON;
        list->elem[i].next = POISON;
        list->elem[i].value = POISON;

    }

    list->size = 0;

    list->elem[0].value = 0;
    list->elem[0].next = 0;
    list->elem[0].prev = 0;

    list->head = (struct element*) calloc(1, sizeof(struct element));
    list->tail = (struct element*) calloc(1, sizeof(struct element));

    list->tail->value = 0;
    list->tail->next = 0;
    list->tail->prev = 0;

    list->head->value = 0;
    list->head->next = 0;
    list->head->prev = 0;

    list->free.prev = -1;
    list->free.value = list->end + 1;
    list->free.next = list->end + 2;

    list->end += 2;


    return 0;

}

int DeleteAfter(struct List2* list, int phys_index) {
    ASSERT_OK

    if (phys_index < 0 || phys_index > list->size || phys_index == list->tail->value) {
        assert(!"Ok");
    }

    int next_index = list->elem[phys_index].next;

    if (phys_index == list->head->value) {

        list->elem[list->elem[next_index].next].prev = list->elem[next_index].prev;

        list->elem[phys_index].next = list->elem[next_index].next;

        list->head->next = list->elem[next_index].next;

        list->elem[next_index].value = 0;
        list->elem[next_index].next = -1;
        list->elem[next_index].prev = -1;

    } else if (phys_index == list->tail->prev){


        list->elem[list->elem[next_index].next].prev = list->elem[next_index].prev;

        list->elem[phys_index].next = list->elem[next_index].next;
        list->tail->prev = list->elem[phys_index].prev;
        list->tail->value = phys_index;

        list->elem[next_index].value = 0;
        list->elem[next_index].next = -1;
        list->elem[next_index].prev = -1;
    } else {

        list->elem[list->elem[next_index].next].prev = list->elem[next_index].prev;

        list->elem[phys_index].next = list->elem[next_index].next;

        list->elem[next_index].value = 0;
        list->elem[next_index].next = -1;
        list->elem[next_index].prev = -1;
    }


    list->free.prev = list->free.value;
    list->free.value = next_index;
    list->free.next = list->end;


    return 0;


}


elem_t PushFront(struct List2* list, elem_t element) {
    ASSERT_OK

    int index = list->free.value;

    list->size++;

    if (list->size >= max_size) {
        struct List2* p = (struct List2*) realloc(list, list->size * GYST_COUNTER);
        if (p == NULL) {
            Dump(list, -1);
        }
        list-> max_size = list->size * GYST_COUNTER;

    }


    if (list->size == 1) {

        list->elem[index].value = element;
        list->elem[index].next = 0;
        list->elem[index].prev = 0;

        list->head->value = index;
        list->head->next = index;
        list->head->prev = index;

        list->tail->value = index;
        list->tail->next = index;
        list->tail->prev = index;

        if (list->free.prev == -1) {

            list->free.value = list->free.next;
            list->end++;
            list->free.next = list->end;

        } else {

            list->free.next = list->free.value;
            list->free.value = list->free.prev;
            list->free.prev = -1;
        }

        ASSERT_OK
        return element;
    }

    if (list->size == 2) {

        list->elem[index].value = element;
        list->elem[index].prev = list->head->value;
        list->elem[index].next = list->head->value;

        list->head->next = list->head->value;
        list->head->prev = list->head->value;
        list->head->value = index;

        list->tail->value = list->head->next;
        list->tail->next = list->head->value;
        list->tail->prev = list->head->value;

        list->elem[list->tail->value].next = list->tail->next;
        list->elem[list->tail->value].prev = list->tail->next;

        list->free.value = list->free.next;

        if (list->free.prev == -1) {

            list->free.value = list->free.next;
            list->end++;
            list->free.next = list->end;

        } else {

            list->free.next = list->free.value;
            list->free.value = list->free.prev;
            list->free.prev = -1;
        }

        ASSERT_OK
        return element;

    }

    list->elem[index].value = element;
    list->elem[index].next = list->head->value;
    list->elem[index].prev = list->tail->value;
    list->elem[list->head->value].prev = index;

    list->head->value = index;
    list->head->prev = list->tail->value;
    list->head->next = index; //////

    list->tail->next = list->head->value;

//    if (list->free.prev == -1) {
//        list->free.value = list->free.next;
//        list->end++;
//        list->free.next = list->end;
//    } else {
//        list->free.value = list->free.prev;
//        list->free.prev = -1;
//    }
    if (list->free.prev == -1) {

        list->free.value = list->free.next;
        list->end++;
        list->free.next = list->end;

    } else {

        list->free.next = list->free.value;
        list->free.value = list->free.prev;
        list->free.prev = -1;
    }

    ASSERT_OK
    return element;

}

elem_t PushBack(struct List2* list, elem_t element) {

    ASSERT_OK


    int index = list->free.value;

    list->size++;
    if (list->size >= max_size) {
        struct List2* p = (struct List2*) realloc(list, list->size * GYST_COUNTER);
        if (p == NULL) {
            Dump(list, -1);
        }
        list-> max_size = list->size * GYST_COUNTER;

    }//for 1

    if (list->size == 2) {

        list->elem[index].value = element;
        list->elem[index].prev = list->head->value;
        list->elem[index].next = list->head->value;

        list->elem[list->elem[index].prev].next = index;
        list->elem[list->elem[index].prev].prev = index;

        list->head->value = list->tail->value;
        list->head->next = index;
        list->head->prev = index;

        list->tail->next = list->head->value;
        list->tail->value = index;
        list->tail->prev = list->head->value;

        if (list->free.prev == -1) {

            list->free.value = list->free.next;
            list->end++;
            list->free.next = list->end;

        } else {

            list->free.next = list->free.value;
            list->free.value = list->free.prev;
            list->free.prev = -1;
        }

        ASSERT_OK
        return element;

    }


    list->elem[index].value = element;
    list->elem[index].prev = list->tail->value;
    list->elem[index].next = list->head->value;

    list->head->prev = index;
    list->elem[list->tail->value].next = index;

    list->tail->prev = list->tail->value;
    list->tail->value = index;


    if (list->free.prev == -1) {

        list->free.value = list->free.next;
        list->end++;
        list->free.next = list->end;

    } else {

        //list->free.next = list->free.value;
        list->free.value = list->free.prev;
        list->free.prev = -1;
    }

    ASSERT_OK
    return element;
}

int InsertAfter(struct List2* list, int phys_index, elem_t element) {

    int index = list->free.value;

    list->size++;
    if (list->size >= max_size) {
        struct List2* p = (struct List2*) realloc(list, list->size * GYST_COUNTER);
        if (p == NULL) {
            Dump(list, -1);
        }
        list-> max_size = list->size * GYST_COUNTER;

    }

    if (list->tail->value == phys_index) {
        PushBack(list, element);
    }

    list->elem[index].value = element;

    if (list->head->value == phys_index) {

        list->elem[index].prev = list->head->value;
        list->elem[index].next = list->head->next;
        list->elem[list->head->next].prev = index;
        list->head->next = index;

    } else {

        list->elem[index].prev = phys_index;
        list->elem[index].next = list->elem[phys_index].next;
        list->elem[list->elem[phys_index].next].prev = index;
        list->elem[phys_index].next = index;

    }

    if (list->free.prev == -1) {

        list->free.value = list->free.next;
        list->end++;
        list->free.next = list->end;

    } else {

        list->free.next = list->free.value;
        list->free.value = list->free.prev;
        list->free.prev = -1;
    }

    ASSERT_OK
    return element;
}




int PrintList(struct List2* list) {

    ASSERT_OK

    assert(list != NULL);

    int i = list->head->value;
    while (list->elem[i].next != list->tail->value)  {
        printf("%d ", list->elem[i].value);
        i = list->elem[i].next;
    }
    printf("%d ", list->elem[i].value);
    i = list->elem[i].next;
    printf("%d ", list->elem[i].value);

    printf("\n");

    ASSERT_OK

    return 0;
}



int main() {
    struct List2 mylist = {};

    init_list(&mylist);


    PushFront(&mylist, 5);
    PushFront(&mylist, -8);
    PushFront(&mylist, 6);
    PushFront(&mylist, 9);
    PushBack(&mylist, 17);
    PushBack(&mylist, 21);
    PushFront(&mylist, 9);

    DeleteAfter(&mylist, 3);
    DeleteAfter(&mylist, 6);
    DeleteAfter(&mylist, 4);

    PushBack(&mylist, 1000);
    PushBack(&mylist, 2000);

    InsertAfter(&mylist, 3, 500);

    PrintList(&mylist);

    Dump(&mylist, 0);



    return 0;
}