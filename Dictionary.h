#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "BinaryTree.h"

struct DictionaryNode {
	void* key;
	int keyLen;

	void* val;
	int valLen;
};

struct Dictionary {
	struct BinaryTree* dictionary;

	void (*insert)(void* key, int keyLen, void* value, int valueLen, struct Dictionary* dict);
	void (*remove)(void* key, struct Dictionary* dict);
	struct DictionaryNode* (*search)(void* key, struct Dictionary* dict);
};

struct Dictionary* dictionary_constructor(int (*cmp)(void* data1, void* data2));
void dictionary_deconstructor(struct Dictionary* dictionary);

int dict_str_cmp(struct DictionaryNode* data1, struct DictionaryNode* data2);
#endif