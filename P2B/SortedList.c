#include <string.h>
#include <sched.h>
#include <stdlib.h>
#include "SortedList.h"

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
	SortedListElement_t *node, *prev_node;

	node = list->next;

	if (opt_yield & INSERT_YIELD)
		sched_yield();
	
	while (node != list) {
		if (strcmp(node->key, element->key) > 0)
			break;
		node = node->next;
	}

	prev_node = node->prev;
	prev_node->next = element;
	
	node->prev = element;

	element->prev = prev_node;
	element->next = node;
}

int SortedList_delete(SortedListElement_t *element)
{
	if (element->next->prev != element || element->prev->next != element)
		return 1;

	if (opt_yield & DELETE_YIELD)
		sched_yield();	

	element->next->prev = element->prev;
	element->prev->next = element->next;

	return 0;
}       

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
	SortedListElement_t *node;

	node = list->next;

	if (opt_yield & LOOKUP_YIELD)
		sched_yield();
	
	while (node != list) {
		if (strcmp(node->key, key) == 0)
			return node;
		node = node->next;
	}

	return NULL;
}

int SortedList_length(SortedList_t *list)
{
	SortedListElement_t *node;
	int length = 0;

	node = list->next;

	if (opt_yield & LOOKUP_YIELD)
		sched_yield();
	
	while (node != list) {
		if (node->next->prev != node || node->prev->next != node)
			return -1;
		
		length++;
		node = node->next;
	}

	return length;
}
