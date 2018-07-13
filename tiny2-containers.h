#pragma once

#include <stdint.h>

#include <tiny2-object.h>

/* 
 * Utils
 */

uint64_t tc_djb2(const char* str);

/*
 * Decls
 */

$class_decl(TCString)
$class_decl(TCListNode)
$class_decl(TCList)
$class_decl(TCVector)
$class_decl(TCQueue)
$class_decl(TCMapPair)
$class_decl(TCMap)

/*
 * TCString
 */

typedef TCString* (*TCStringConstructor)(TCString* self, const char* str);
typedef void (*TCStringInitVTable)(TCStringVTable* v);
typedef char* (*TCStringStr)(TCString* self);
typedef size_t (*TCStringSize)(TCString* self);
typedef TCString* (*TCStringCopy)(TCString* self);
typedef void (*TCStringAppend)(TCString* self, TCString* other);
typedef void (*TCStringAppendC)(TCString* self, const char* other);
typedef void (*TCStringPrepend)(TCString* self, TCString* other);
typedef void (*TCStringPrependC)(TCString* self, const char* other);

$class(TCString, TObject, _parent)
  $class_property(char*, str)
$class_end(TCString)

$mtable(TCString)
  $mtable_method(TCStringStr, str)
  $mtable_method(TCStringSize, size)
  $mtable_method(TCStringCopy, copy)
  $mtable_method(TCStringAppend, append)
  $mtable_method(TCStringAppendC, appendc)
  $mtable_method(TCStringPrepend, prepend)
  $mtable_method(TCStringPrependC, prependc)
$mtable_end(TCString)

$vtable(TCString, TObject)
$vtable_end(TCString)

#ifndef TINY2C_NO_SHORTCUT
  #define $STR(c) $new(TCString, c)
  #define $str(c) $new(TCString, c)

  #define $CSTR(s) $(TCString, (s), str)
  #define $cstr(s) $(TCString, (s), str)
#endif

/*
 * TCListNode
 */

typedef TCListNode* (*TCListNodeConstructor)(TCListNode* self, TObject* obj);
typedef void (*TCListNodeInitVTable)(TCListNodeVTable* v);
typedef TObject* (*TCListNodeObj)(TCListNode* self);
typedef TCListNode* (*TCListNodeNext)(TCListNode* self);
typedef TCListNode* (*TCListNodePrev)(TCListNode* self);

$class(TCListNode, TObject, _parent)
  $class_property(TObject*, obj)
  $class_property(TCListNode*, prev)
  $class_property(TCListNode*, next)
  $class_property(TCList*, list)
$class_end(TCListNode)

$mtable(TCListNode)
  $mtable_method(TCListNodeObj, obj)
  $mtable_method(TCListNodeNext, next)
  $mtable_method(TCListNodePrev, prev)
$mtable_end(TCListNode)

$vtable(TCListNode, TObject)
$vtable_end(TCListNode)

#define TC_LIST_LINK_NODES(l, r) if (l != NULL) { l->next = r; }; if (r != NULL) { r->prev = l; }
#define TC_LIST_LINK_3_NODES(l, c, r) TC_LIST_LINK_NODES(l, c); TC_LIST_LINK_NODES(c, r)
#define TC_LIST_UNLINK_NODE(n) if (n != NULL) { if (n->prev != NULL) { n->prev->next = NULL; }; n->prev = NULL; if (n->next != NULL) { n->next->prev = NULL; }; n->next = NULL; }

#ifndef TINY2C_NO_SHORTCUT
  #define $LIST_LINK_NODES TC_LIST_LINK_NODES
  #define $list_link_nodes TC_LIST_LINK_NODES

  #define $LIST_LINK_3_NODES TC_LIST_LINK_3_NODES
  #define $list_link_3_nodes TC_LIST_LINK_3_NODES

  #define $LIST_UNLINK_NODE TC_LIST_UNLINK_NODE
  #define $list_unlink_node TC_LIST_UNLINK_NODE
#endif

/*
 * TCList
 */

typedef TCList* (*TCListConstructor)(TCList* self);
typedef void (*TCListInitVTable)(TCListVTable* v);
typedef void (*TCListAppend)(TCList* self, TObject* obj);
typedef void (*TCListPrepend)(TCList* self, TObject* obj);
typedef TCListNode* (*TCListRemove)(TCList* self, TCListNode* n);
typedef bool (*TCListIterator)(TCList* list, TCListNode* node, TObject* userdata);
typedef void (*TCListForeach)(TCList* self, TCListIterator iter, TObject* userdata);

$class(TCList, TObject, _parent)
  $class_property(TCListNode*, head)
  $class_property(TCListNode*, tail)
$class_end(TCList)

$mtable(TCList)
  $mtable_method(TCListAppend, append)
  $mtable_method(TCListPrepend, prepend)
  $mtable_method(TCListRemove, remove)
  $mtable_method(TCListForeach, foreach)
$mtable_end(TCList)

$vtable(TCList, TObject)
$vtable_end(TCList)

/*
 * TCVector
 */

typedef TCVector* (*TCVectorConstructor)(TCVector* self, size_t prealloc, size_t step);
typedef void (*TCVectorInitVTable)(TCVectorVTable* v);
typedef void (*TCVectorPush)(TCVector* self, TObject* obj);
typedef TObject* (*TCVectorPop)(TCVector* self);
typedef void (*TCVectorInsert)(TCVector* self, TObject* obj, size_t idx);
typedef TObject* (*TCVectorGet)(TCVector* self, size_t idx);
typedef void (*TCVectorRemove)(TCVector* self, size_t idx);
typedef void (*TCVectorClear)(TCVector* self);

$class(TCVector, TObject, _parent)
  $class_property(size_t, alloc)
  $class_property(size_t, step)
  $class_property(size_t, len)
  $class_property(TObject**, arr)
$class_end(TCVector)

$mtable(TCVector)
  $mtable_method(TCVectorPush, push_back)
  $mtable_method(TCVectorPush, push_front)
  $mtable_method(TCVectorPop, pop_back)
  $mtable_method(TCVectorPop, pop_front)
  $mtable_method(TCVectorInsert, insert)
  $mtable_method(TCVectorGet, get)
  $mtable_method(TCVectorRemove, remove)
  $mtable_method(TCVectorClear, clear)
$mtable_end(TCVector)

$vtable(TCVector, TObject)
$vtable_end(TCVector)

/*
 * TCQueue
 */

typedef TCQueue* (*TCQueueConstructor)(TCQueue* self, size_t alloc);
typedef void (*TCQueueInitVTable)(TCQueueVTable* v);
typedef bool (*TCQueuePush)(TCQueue* self, TObject* obj);
typedef TObject* (*TCQueuePop)(TCQueue* self);
typedef TObject* (*TCQueuePeek)(TCQueue* self);

$class(TCQueue, TObject, _parent)
  $class_property(TObject**, arr)
  $class_property(size_t, alloc)
  $class_property(size_t, size)
  $class_property(size_t, head)
  $class_property(size_t, tail)
$class_end(TCQueue)

$mtable(TCQueue)
  $mtable_method(TCQueuePush, push)
  $mtable_method(TCQueuePop, pop)
  $mtable_method(TCQueuePeek, peek)
$mtable_end(TCQueue)

$vtable(TCQueue, TObject)
$vtable_end(TCQueue)

/*
 * TCMapPair
 */

typedef TCMapPair* (*TCMapPairConstructor)(TCMapPair* self, const char* key, TObject* value);
typedef void (*TCMapPairInitVTable)(TCMapPairVTable* v);
typedef void (*TCMapPairRename)(TCMapPair* self, const char* key);
typedef void (*TCMapPairSet)(TCMapPair* self, TObject* value);
typedef TObject* (*TCMapPairGet)(TCMapPair* self);

$class(TCMapPair, TObject, _parent)
  $class_property(char*, key)
  $class_property(uint64_t, hash)
  $class_property(TObject*, value)
$class_end(TCMapPair)

$mtable(TCMapPair)
  $mtable_method(TCMapPairRename, rename)
  $mtable_method(TCMapPairSet, set)
  $mtable_method(TCMapPairGet, get)
$mtable_end(TCMapPair)

$vtable(TCMapPair, TObject)
$vtable_end(TCMapPair)

/*
 * TCMap
 */

typedef TCMap* (*TCMapConstructor)(TCMap* self);
typedef void (*TCMapInitVTable)(TCMapVTable* v);
typedef TObject* (*TCMapGet)(TCMap* self, const char* key);
typedef TObject* (*TCMapGetByHash)(TCMap* self, uint64_t hash);
typedef void (*TCMapSet)(TCMap* self, const char* key, TObject* value);
typedef void (*TCMapRename)(TCMap* self, const char* old_key, const char* new_key);
typedef void (*TCMapRemove)(TCMap* self, const char* key);
typedef void (*TCMapRemoveByHash)(TCMap* self, uint64_t hash);

$class(TCMap, TObject, _parent)
  $class_property(TCList*, pairs)
$class_end(TCMap)

$mtable(TCMap)
  $mtable_method(TCMapGet, get)
  $mtable_method(TCMapGetByHash, get_by_hash)
  $mtable_method(TCMapSet, set)
  $mtable_method(TCMapRename, rename)
  $mtable_method(TCMapRemove, remove)
  $mtable_method(TCMapRemoveByHash, remove_by_hash)
$mtable_end(TCMap)

$vtable(TCMap, TObject)
$vtable_end(TCMap)