#define _GNU_SOURCE

#include "tiny2-containers.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#define strdup _strdup
#endif

/*
 * Utils
 */

uint64_t tc_djb2(const char* str) {
  uint64_t hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}

/*
 * TCString
 */

static TCString* tc_string_constructor(TCString* self, const char* str);
static void tc_string_destructor(TCString* self);
static void tc_string_init_vtable(TCStringVTable* v);
static char* tc_string_str(TCString* self);
static size_t tc_string_size(TCString* self);
static TCString* tc_string_copy(TCString* self);
static void tc_string_append(TCString* self, TCString* other);
static void tc_string_appendc(TCString* self, const char* other);
static void tc_string_prepend(TCString* self, TCString* other);
static void tc_string_prependc(TCString* self, const char* other);

$mtable_define(TCString, tc_string_constructor, tc_string_destructor, tc_string_init_vtable)
  $mtable_define_method(TCStringStr, str, tc_string_str)
  $mtable_define_method(TCStringSize, size, tc_string_size)
  $mtable_define_method(TCStringCopy, copy, tc_string_copy)
  $mtable_define_method(TCStringAppend, append, tc_string_append)
  $mtable_define_method(TCStringAppendC, appendc, tc_string_appendc)
  $mtable_define_method(TCStringPrepend, prepend, tc_string_prepend)
  $mtable_define_method(TCStringPrependC, prependc, tc_string_prependc)
$mtable_define_end(TCString)

$vtable_define(TCString)
$vtable_define_end(TCString)

static TCString* tc_string_constructor(TCString* self, const char* str) {
  $init(TObject, self);
  $setup(TCString, self, tc_string_destructor);
  $reg(TCString, TObject);

  if (str) {
    self->str = strdup(str);
  } else {
    self->str = NULL;
  }

  return self;
}

static void tc_string_destructor(TCString* self) {
  assert($is(self, TCString));

  if (self->str != NULL) {
    free(self->str);
  }

  $destroy_parent(TObject, self);
}

static void tc_string_init_vtable(TCStringVTable* v) {
  $vtable_init(v, TCString, TObject);
}

static char* tc_string_str(TCString* self) {
  assert($is(self, TCString));

  return self->str;
}

static size_t tc_string_size(TCString* self) {
  assert($is(self, TCString));
  
  $ref(self);
  
  size_t s = strlen(self->str);
  
  $unref(self);

  return s;
}

static TCString* tc_string_copy(TCString* self) {
  assert($is(self, TCString));

  $ref(self);
  
  TCString* s = $new(TCString, self->str);
  
  $unref(self);
  
  return s;
}

static void tc_concat(char** out, const char* s1, const char* s2) {
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
  size_t sz = strlen(s1) + strlen(s2) + 1;
  char* r = (char*)malloc(sz);
  memset(r, 0, sz);
  strcat_s(r, sz, s1);
  strcat_s(r, sz, s2);
  *out = r;
#else
  asprintf(out, "%s%s", s1, s2);
#endif
}

static void tc_string_append(TCString* self, TCString* other) {
  assert($is(self, TCString));

  $ref(self);
  $ref(other);

  char* old = self->str;

  tc_concat(&(self->str), old, other->str);

  free(old);

  $unref(other);
  $unref(self);
}

static void tc_string_appendc(TCString* self, const char* other) {
  assert($is(self, TCString));

  $ref(self);

  char* old = self->str;

  tc_concat(&(self->str), old, other);

  free(old);

  $unref(self);
}

static void tc_string_prepend(TCString* self, TCString* other) {
  assert($is(self, TCString));

  $ref(self);
  $ref(other);

  char* old = self->str;

  tc_concat(&(self->str), other->str, old);

  free(old);

  $unref(other);
  $unref(self);
}

static void tc_string_prependc(TCString* self, const char* other) {
  assert($is(self, TCString));

  $ref(self);

  char* old = self->str;

  tc_concat(&(self->str), other, old);

  free(old);

  $unref(self);
}

/*
 * TCListNode
 */

static TCListNode* tc_list_node_constructor(TCListNode* self, TObject* obj);
static void tc_list_node_destructor(TCListNode* self);
static void tc_list_node_init_vtable(TCListNodeVTable* v);
static TObject* tc_list_node_obj(TCListNode* self);
static TCListNode* tc_list_node_next(TCListNode* self);
static TCListNode* tc_list_node_prev(TCListNode* self);

$mtable_define(TCListNode, tc_list_node_constructor, tc_list_node_destructor, tc_list_node_init_vtable)
  $mtable_define_method(TCListNodeObj, obj, tc_list_node_obj)
  $mtable_define_method(TCListNodeNext, next, tc_list_node_next)
  $mtable_define_method(TCListNodePrev, prev, tc_list_node_prev)
$mtable_define_end(TCListNode)

$vtable_define(TCListNode)
$vtable_define_end(TCListNode)

static TCListNode* tc_list_node_constructor(TCListNode* self, TObject* obj) {
  $init(TObject, self);
  $setup(TCListNode, self, tc_list_node_destructor);
  $reg(TCListNode, TObject);

  if (obj != NULL) $ref(obj);
  self->obj = obj;

  self->prev = NULL;
  self->next = NULL;

  return self;
}

static void tc_list_node_destructor(TCListNode* self) {
  assert($is(self, TCListNode));
  if (self->obj != NULL) $unref(self->obj);
  $destroy_parent(TObject, self);
}

static void tc_list_node_init_vtable(TCListNodeVTable* v) {
  $vtable_init(v, TCListNode, TObject);
}

static TObject* tc_list_node_obj(TCListNode* self) {
  assert($is(self, TCListNode));

  $ref(self);

  TObject* obj = self->obj;
  $ref(obj);

  $unref(self);

  return obj;
}

static TCListNode* tc_list_node_next(TCListNode* self) {
  assert($is(self, TCListNode));

  $ref(self);

  if (self->next) $ref(self->next);
  TCListNode* n = self->next;
  
  $unref(self);

  return n;
}

static TCListNode* tc_list_node_prev(TCListNode* self) {
  assert($is(self, TCListNode));

  $ref(self);
  
  if (self->prev) $ref(self->prev);
  TCListNode* n = self->prev;
  
  $unref(self);

  return n;
}

/*
 * TCList
 */

static TCList* tc_list_constructor(TCList* self);
static void tc_list_destructor(TCList* self);
static void tc_list_init_vtable(TCListVTable* v);
static void tc_list_append(TCList* self, TObject* obj);
static void tc_list_prepend(TCList* self, TObject* obj);
static void tc_list_remove(TCList* self, TCListNode* n);
static void tc_list_foreach(TCList* self, TCListIterator iter, void* userdata);

$mtable_define(TCList, tc_list_constructor, tc_list_destructor, tc_list_init_vtable)
  $mtable_define_method(TCListAppend, append, tc_list_append)
  $mtable_define_method(TCListPrepend, prepend, tc_list_prepend)
  $mtable_define_method(TCListRemove, remove, tc_list_remove)
  $mtable_define_method(TCListForeach, foreach, tc_list_foreach)
$mtable_define_end(TCList)

$vtable_define(TCList)
$vtable_define_end(TCList)

static TCList* tc_list_constructor(TCList* self) {
  $init(TObject, self);
  $setup(TCList, self, tc_list_destructor);
  $reg(TCList, TObject);

  self->head = NULL;
  self->tail = NULL;

  return self;
}

static void tc_list_destructor(TCList* self) {
  assert($is(self, TCList));
  TCListNode* next = NULL;
  TCListNode* n = self->head;
  while (n != NULL) {
    next = n->next;
    TC_LIST_UNLINK_NODE(n);
    $unref(n);
    n = next;
  }
  $destroy_parent(TObject, self);
}

static void tc_list_init_vtable(TCListVTable* v) {
  $vtable_init(v, TCList, TObject);
}

static void tc_list_append(TCList* self, TObject* obj) {
  assert($is(self, TCList));

  $ref(self);

  TCListNode* n = $new(TCListNode, obj);

  if (self->head == NULL) {
    self->head = n;
  }

  if (self->tail == NULL) {
    self->tail = n;
  } else {
    TC_LIST_LINK_NODES(self->tail, n);
    self->tail = n;
  }

  n->list = self;

  $unref(self);
}

static void tc_list_prepend(TCList* self, TObject* obj) {
  assert($is(self, TCList));
  
  $ref(self);

  TCListNode* n = $new(TCListNode, obj);

  if (self->head == NULL) {
    self->head = n;
  } else {
    TC_LIST_LINK_NODES(n, self->head);
    self->head = n;
  }

  if (self->tail == NULL) {
    self->tail = n;
  }

  n->list = self;

  $unref(self);
}

static void tc_list_remove(TCList* self, TCListNode* n) {
  assert($is(self, TCList));
  
  $ref(self);

  if (n->list != self) return;

  if (n == self->head && n == self->tail) {
    self->head = NULL;
    self->tail = NULL;
  }

  if (n == self->head)
    self->head = n->next;

  if (n == self->tail)
    self->tail = n->prev;

  TCListNode* l = TO_CALL(TCListNode, n, prev);
  TCListNode* r = TO_CALL(TCListNode, n, next);
  TC_LIST_UNLINK_NODE(n);
  TC_LIST_LINK_NODES(l, r);
  if (l != NULL) $unref(l); 
  if (r != NULL) $unref(r);

  $unref(n);

  $unref(self);
}

static void tc_list_foreach(TCList* self, TCListIterator iter, void* userdata) {
  assert($is(self, TCList));
  
  $ref(self);

  TCListNode* next = NULL;
  TCListNode* n = self->head;
  bool c = true;
  while (n != NULL && c == true) {
    $ref(n);
    next = n->next;
    c = iter(self, n, userdata);
    $unref(n);
    n = next;
  }

  $unref(self);
}

/*
 * TCVector
 */

static TCVector* tc_vector_constructor(TCVector* self, size_t prealloc, size_t step);
static void tc_vector_destructor(TCVector* self);
static void tc_vector_init_vtable(TCVectorVTable* v);
static void tc_vector_push_back(TCVector* self, TObject* obj);
static void tc_vector_push_front(TCVector* self, TObject* obj);
static TObject* tc_vector_pop_back(TCVector* self);
static TObject* tc_vector_pop_front(TCVector* self);
static void tc_vector_insert(TCVector* self, TObject* obj, size_t idx);
static TObject* tc_vector_get(TCVector* self, size_t idx);
static void tc_vector_remove(TCVector* self, size_t idx);
static void tc_vector_clear(TCVector* self);

$mtable_define(TCVector, tc_vector_constructor, tc_vector_destructor, tc_vector_init_vtable)
  $mtable_define_method(TCVectorPush, push_back, tc_vector_push_back)
  $mtable_define_method(TCVectorPush, push_front, tc_vector_push_front)
  $mtable_define_method(TCVectorPop, pop_back, tc_vector_pop_back)
  $mtable_define_method(TCVectorPop, pop_front, tc_vector_pop_front)
  $mtable_define_method(TCVectorInsert, insert, tc_vector_insert)
  $mtable_define_method(TCVectorGet, get, tc_vector_get)
  $mtable_define_method(TCVectorRemove, remove, tc_vector_remove)
  $mtable_define_method(TCVectorClear, clear, tc_vector_clear)
$mtable_define_end(TCVector)

$vtable_define(TCVector)
$vtable_define_end(TCVector)

static TCVector* tc_vector_constructor(TCVector* self, size_t prealloc, size_t step) {
  $init(TObject, self);
  $setup(TCVector, self, tc_vector_destructor);
  $reg(TCVector, TObject);

  self->alloc    = (prealloc == 0 ? 16 : prealloc);
  self->step     = (step     == 0 ? 16 : step);

  self->len = 0;
  self->arr = (TObject**) malloc(sizeof(TObject*) * self->alloc);

  return self;
}

static void tc_vector_destructor(TCVector* self) {
  assert($is(self, TCVector));
  for (int i = 0; i < self->len; ++i) {
    if (self->arr[i] != NULL)
      $unref(self->arr[i]);
  }
  free(self->arr);
  $destroy_parent(TObject, self);
}

static void tc_vector_init_vtable(TCVectorVTable* v) {
  $vtable_init(v, TCVector, TObject);
}

static void tc_vector_push_back(TCVector* self, TObject* obj) {
  assert($is(self, TCVector));
  
  $ref(self);
  $ref(obj);
  
  if (self->len == self->alloc) {
    self->arr = realloc(self->arr, sizeof(TObject*) * (self->alloc + self->step));
    self->alloc += self->step;
  }
  self->arr[self->len] = obj;
  ++self->len;

  $unref(self);
}

static void tc_vector_push_front(TCVector* self, TObject* obj) {
  assert($is(self, TCVector));
  
  $ref(self);
  $(TCVector, self, insert, obj, 0);
  $unref(self);
}

static TObject* tc_vector_pop_back(TCVector* self) {
  assert($is(self, TCVector));
  
  $ref(self);

  if (self->len == 0) {
    $unref(self);
    return NULL;
  }
  
  TObject* obj = self->arr[0];
  
  for (int i = 0; i < self->len; ++i) {
    if (i != self->len - 1)
      self->arr[i] = self->arr[i+1];
  }
  --self->len;

  if ((self->alloc - self->len) > self->step) {
    self->arr = realloc(self->arr, sizeof(TObject*) * (self->alloc - self->step));
    self->alloc -= self->step;
  }

  $unref(self);

  return obj;
}

static TObject* tc_vector_pop_front(TCVector* self) {
  assert($is(self, TCVector));
  
  $ref(self);

  if (self->len == 0) {
    $unref(self);
    return NULL;
  }

  TObject* obj = self->arr[self->len-1];
  --self->len;

  if ((self->alloc - self->len) > self->step) {
    self->arr = realloc(self->arr, sizeof(TObject*) * (self->alloc - self->step));
    self->alloc -= self->step;
  }

  $unref(self);

  return obj;
}

static void tc_vector_insert(TCVector* self, TObject* obj, size_t idx) {
  assert($is(self, TCVector));
  
  $ref(self);

  if (idx > self->len) {
    $(TCVector, self, push_back, obj);
    $unref(self);
    return;
  }
  $ref(obj);

  if (self->len == self->alloc) {
    self->arr = realloc(self->arr, sizeof(TObject*) * (self->alloc + self->step));
    self->alloc += self->step;
  }
  for (int i = self->len; i > idx; --i) {
    self->arr[i] = self->arr[i-1];
  }
  self->arr[idx] = obj;
  ++self->len;

  $unref(self);
}

static TObject* tc_vector_get(TCVector* self, size_t idx) {
  assert($is(self, TCVector));
  
  $ref(self);

  TObject* obj = NULL;

  if (idx < self->len) {
    $ref(self->arr[idx]);
    obj = self->arr[idx];
  }

  $unref(self);

  return obj;
}

static void tc_vector_remove(TCVector* self, size_t idx) {
  assert($is(self, TCVector));

  $ref(self);

  if (idx >= self->len) {
    $unref(self);
    return;
  }
  $unref(self->arr[idx]);
  for (int i = idx; i < self->len; ++i) {
    if (i != self->len - 1)
      self->arr[i] = self->arr[i+1];
  }
  --self->len;

  $unref(self);
}

static void tc_vector_clear(TCVector* self) {
  assert($is(self, TCVector));

  $ref(self);

  for (int i = 0; i < self->len; ++i) {
    if (self->arr[i] != NULL)
      $unref(self->arr[i]);
  }

  $unref(self);
}

/*
 * TCQueue
 */

static TCQueue* tc_queue_constructor(TCQueue* self, size_t alloc);
static void tc_queue_destructor(TCQueue* self);
static void tc_queue_init_vtable(TCQueueVTable* v);
static bool tc_queue_push(TCQueue* self, TObject* obj);
static TObject* tc_queue_pop(TCQueue* self);
static TObject* tc_queue_peek(TCQueue* self);

$mtable_define(TCQueue, tc_queue_constructor, tc_queue_destructor, tc_queue_init_vtable)
  $mtable_define_method(TCQueuePush, push, tc_queue_push)
  $mtable_define_method(TCQueuePop, pop, tc_queue_pop)
  $mtable_define_method(TCQueuePeek, peek, tc_queue_peek)
$mtable_define_end(TCQueue)

$vtable_define(TCQueue)
$vtable_define_end(TCQueue)

static TCQueue* tc_queue_constructor(TCQueue* self, size_t alloc) {
  $init(TObject, self);
  $setup(TCQueue, self, tc_queue_destructor);
  $reg(TCQueue, TObject);

  if (alloc == 0) alloc = 64;

  self->alloc = alloc;
  self->arr = malloc(sizeof(TObject*) * alloc);
  self->size = 0;
  self->head = 0;
  self->tail = 0;

  return self;
}

static void tc_queue_destructor(TCQueue* self) {
  assert($is(self, TCQueue));

  for (TObject* o = $(TCQueue, self, pop); o != NULL; o = $(TCQueue, self, pop)) {
    $unref(o);
  }

  free(self->arr);

  $destroy_parent(TObject, self);
}

static void tc_queue_init_vtable(TCQueueVTable* v) {
  $vtable_init(v, TCQueue, TObject);
}

static bool tc_queue_push(TCQueue* self, TObject* obj) {
  assert($is(self, TCQueue));

  $ref(self);

  if (self->size >= self->alloc) {
    return false;
  }

  ++(self->size);
  $ref(obj);
  self->arr[self->tail] = obj;
  ++(self->tail);
  if (self->tail >= self->alloc) {
    self->tail = 0;
  }

  $unref(self);
  return true;
}

static TObject* tc_queue_pop(TCQueue* self) {
  assert($is(self, TCQueue));
  
  $ref(self);

  if (self->size <= 0) {
    return NULL;
  }

  --(self->size);
  TObject* obj = self->arr[self->head];
  ++(self->head);
  if (self->head >= self->alloc) {
    self->head = 0;
  }

  $unref(self);
  return obj;
}

static TObject* tc_queue_peek(TCQueue* self) {
  assert($is(self, TCQueue));

  $ref(self);

  if (self->head == self->tail || self->size <= 0) {
    return NULL;
  }

  TObject* obj = self->arr[self->head];
  $ref(obj);

  $unref(self);
  return obj;
}

/*
 * TCMapPair
 */

static TCMapPair* tc_map_pair_constructor(TCMapPair* self, const char* key, TObject* value);
static void tc_map_pair_destructor(TCMapPair* self);
static void tc_map_pair_init_vtable(TCMapPairVTable* v);
static void tc_map_pair_rename(TCMapPair* self, const char* key);
static void tc_map_pair_set(TCMapPair* self, TObject* value);
static TObject* tc_map_pair_get(TCMapPair* self);

$mtable_define(TCMapPair, tc_map_pair_constructor, tc_map_pair_destructor, tc_map_pair_init_vtable)
  $mtable_define_method(TCMapPairRename, rename, tc_map_pair_rename)
  $mtable_define_method(TCMapPairSet, set, tc_map_pair_set)
  $mtable_define_method(TCMapPairGet, get, tc_map_pair_get)
$mtable_define_end(TCMapPair)

$vtable_define(TCMapPair)
$vtable_define_end(TCMapPair)

static TCMapPair* tc_map_pair_constructor(TCMapPair* self, const char* key, TObject* value) {
  $init(TObject, self);
  $setup(TCMapPair, self, tc_map_pair_destructor);
  $reg(TCMapPair, TObject);

  $ref(value);

  self->key = strdup(key);
  self->hash = tc_djb2(self->key);
  self->value = value;

  return self;
}

static void tc_map_pair_destructor(TCMapPair* self) {
  assert($is(self, TCMapPair));

  $destroy_parent(TObject, self);

  free(self->key);
  $unref(self->value);
}

static void tc_map_pair_init_vtable(TCMapPairVTable* v) {
  $vtable_init(v, TCQueue, TObject);
}

static void tc_map_pair_rename(TCMapPair* self, const char* key) {
  assert($is(self, TCMapPair));

  $ref(self);

  free(self->key);
  self->key = strdup(key);
  self->hash = tc_djb2(self->key);

  $unref(self);
}

static void tc_map_pair_set(TCMapPair* self, TObject* value) {
  assert($is(self, TCMapPair));

  $ref(self);

  $unref(self->value);
  $ref(value);
  self->value = value;

  $unref(self);
}

static TObject* tc_map_pair_get(TCMapPair* self) {
  assert($is(self, TCMapPair));

  $ref(self);

  $ref(self->value);
  TObject* obj = self->value;

  $unref(self);
  return obj;
}

/*
 * TCMap
 */

static TCMap* tc_map_constructor(TCMap* self);
static void tc_map_destructor(TCMap* self);
static void tc_map_init_vtable(TCMapVTable* v);
static TObject* tc_map_get(TCMap* self, const char* key);
static TObject* tc_map_get_by_hash(TCMap* self, uint64_t hash);
static void tc_map_set(TCMap* self, const char* key, TObject* value);
static void tc_map_rename(TCMap* self, const char* old_key, const char* new_key);
static void tc_map_remove(TCMap* self, const char* key);
static void tc_map_remove_by_hash(TCMap* self, uint64_t hash);

$mtable_define(TCMap, tc_map_constructor, tc_map_destructor, tc_map_init_vtable)
  $mtable_define_method(TCMapGet, get, tc_map_get)
  $mtable_define_method(TCMapGetByHash, get_by_hash, tc_map_get_by_hash)
  $mtable_define_method(TCMapSet, set, tc_map_set)
  $mtable_define_method(TCMapRename, rename, tc_map_rename)
  $mtable_define_method(TCMapRemove, remove, tc_map_remove)
  $mtable_define_method(TCMapRemoveByHash, remove_by_hash, tc_map_remove_by_hash)
$mtable_define_end(TCMap)

$vtable_define(TCMap)
$vtable_define_end(TCMap)

static TCMap* tc_map_constructor(TCMap* self) {
  $init(TObject, self);
  $setup(TCMap, self, tc_map_destructor);
  $reg(TCMap, TObject);

  self->pairs = $new(TCList);

  return self;
}

static void tc_map_destructor(TCMap* self) {
  assert($is(self, TCMap));
  $unref(self->pairs);

  $destroy_parent(TObject, self);
}

static void tc_map_init_vtable(TCMapVTable* v) {
  $vtable_init(v, TCQueue, TObject);
}

static TObject* tc_map_get(TCMap* self, const char* key) {
  assert($is(self, TCMap));

  $ref(self);

  uint64_t hash = tc_djb2(key);
  TObject* obj = $(TCMap, self, get_by_hash, hash);

  $unref(self);
  return obj;
}

typedef struct TCMapIter {
  uint64_t hash;
  TCMapPair* pair;
} TCMapIter;

static bool tc_map_get_iter(TCList* pairs, TCListNode* pairn, TCMapIter* iter) {
  TCMapPair* pair = (TCMapPair*) pairn->obj;
  if (pair->hash == iter->hash) {
    iter->pair = pair;
    return false;
  } else {
    return true;
  }
}

static TObject* tc_map_get_by_hash(TCMap* self, uint64_t hash) {
  assert($is(self, TCMap));
  
  $ref(self);

  TCMapIter* iter = (TCMapIter*) malloc(sizeof(TCMapIter));
  iter->hash = hash;
  iter->pair = NULL;
  $(TCList, self->pairs, foreach, (TCListIterator) tc_map_get_iter, (void*) iter);

  TCMapPair* pair = iter->pair;

  free(iter);

  $unref(self);
  if (pair != NULL) {
    $ref(pair->value);
    return pair->value;
  } else {
    return NULL;
  }
}

static void tc_map_set(TCMap* self, const char* key, TObject* value) {
  assert($is(self, TCMap));

  $ref(self);

  TCMapPair* p = $new(TCMapPair, key, value);
  $(TCList, self->pairs, append, (TObject*) p);
  $unref(p);

  $unref(self);
}

static void tc_map_rename(TCMap* self, const char* old_key, const char* new_key) {
  assert($is(self, TCMap));

  $ref(self);

  TCMapIter* iter = (TCMapIter*) malloc(sizeof(TCMapIter));
  iter->hash = tc_djb2(old_key);
  iter->pair = NULL;
  $(TCList, self->pairs, foreach, (TCListIterator) tc_map_get_iter, (void*) iter);

  TCMapPair* pair = iter->pair;
  $ref(pair);

  free(iter);

  if (pair != NULL) {
    $(TCMapPair, pair, rename, new_key);
  }

  $unref(pair);

  $unref(self);
}

static bool tc_map_remove_iter(TCList* pairs, TCListNode* pairn, TCMapIter* iter) {
  TCMapPair* pair = (TCMapPair*) pairn->obj;
  if (pair->hash == iter->hash) {
    $(TCList, pairs, remove, pairn);
    return false;
  } else {
    return true;
  }
}

static void tc_map_remove(TCMap* self, const char* key) {
  assert($is(self, TCMap));

  $ref(self);

  $(TCMap, self, remove_by_hash, tc_djb2(key));

  $unref(self);
}

static void tc_map_remove_by_hash(TCMap* self, uint64_t hash) {
  assert($is(self, TCMap));

  $ref(self);

  TCMapIter* iter = (TCMapIter*) malloc(sizeof(TCMapIter));
  iter->hash = hash;
  iter->pair = NULL;
  $(TCList, self->pairs, foreach, (TCListIterator) tc_map_remove_iter, (void*) iter);
  free(iter);

  $unref(self);
}
