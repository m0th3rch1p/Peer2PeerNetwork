#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dictionary.h"

void insert_dictionary(void* key, int keyLen, void* val, int valLen, struct Dictionary* dictionary);
void remove_dictionary(void* key, struct Dictionary* dictionary);
struct DictionaryNode* search_dictionary(void* key, struct Dictionary* dictionary);

struct Dictionary* dictionary_constructor(int (*cmp)(void* data1, void* data2)) {
	struct Dictionary* dictionary = (struct Dictionary*)calloc(1, sizeof(struct Dictionary));
	if (!dictionary) {
		fprintf(stderr, "[-] error allocating error for dictionary\n");
		return NULL;
	}

	dictionary->dictionary = binary_tree_constructor(cmp);
	if (!dictionary->dictionary) {
		fprintf(stderr, "[-] error constructing dictionary tree\n");
		free(dictionary);
		return NULL;
	}

	dictionary->insert = insert_dictionary;
	dictionary->remove = remove_dictionary;
	dictionary->search = search_dictionary;

	return dictionary;
}

void dictionary_deconstructor(struct Dictionary* dictionary) {
	binary_tree_deconstructor(dictionary->dictionary);
	free(dictionary);
}

struct DictionaryNode dictionary_node_constructor(void* key, int keyLen, void* val, int valLen) {
	struct DictionaryNode dictionaryNode = {0};
	
	dictionaryNode.key = malloc(keyLen);
	if (!dictionaryNode.key) {
		fprintf(stderr, "[-] error allocating memory for dictionary node key\n");
		exit(-1);
	}

	memcpy(dictionaryNode.key, key, keyLen);
	dictionaryNode.keyLen = keyLen;


	dictionaryNode.val = malloc(valLen);
	if (!dictionaryNode.val) {
		free(dictionaryNode.key);
		fprintf(stderr, "[-] error allocating memory for dictionary node val\n");
		exit(-1);
	}

	memcpy(dictionaryNode.val, val, valLen);
	dictionaryNode.valLen = valLen;

	return dictionaryNode;
}

void insert_dictionary(void* key, int keyLen, void* val, int valLen, struct Dictionary* dictionary) {
	struct DictionaryNode dictNode = dictionary_node_constructor(key, keyLen, val, valLen);
	dictionary->dictionary->insert(&dictNode, sizeof(dictNode), dictionary->dictionary);
}

void remove_dictionary(void* key, struct Dictionary* dictionary) {
	dictionary->dictionary->remove(key, dictionary->dictionary);
}

struct DictionaryNode* search_dictionary(void* key, struct Dictionary* dictionary) {
	struct DictionaryNode dictionaryNode = {0};
	dictionaryNode.key = key;

	return (struct DictionaryNode*)dictionary->dictionary->search(&dictionaryNode, dictionary->dictionary);
}

int dict_str_cmp(struct DictionaryNode* data1, struct DictionaryNode* data2) {
	int cmp = strncmp((char*)data1->key, (char*)data2->key, strlen(data1->key));
	if (cmp > 0) return 1;
	else if (cmp < 0) return -1;
	return cmp;
}