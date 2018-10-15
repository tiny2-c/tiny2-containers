#define _GNU_SOURCE

#include "tiny2-containers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#define strdup _strdup
#endif

void test_strings() {
  char* s = strdup("foo bar baz");
  TCString* str = $new(TCString, s);
  TCString* str2 = $new(TCString, "asdf ");

  $(TCString, str, appendc, " quux spam");
  $(TCString, str, append, str);
  const char* s2 = $(TCString, str, str);
  $(TCString, str, prependc, s2);
  $(TCString, str, prepend, str2);

  $unref(str2);
  $unref(str);
  free(s);
}

bool test_lists_iter(TCList* list, TCListNode* n, TCString* str) {
  $(TCList, list, remove, n);
  return true;
}

void test_lists() {
  TCString* foo = $new(TCString, "Hello, World!");

  TCList* l0 = $new(TCList);
  for (int i = 0; i < 1024; ++i) {
    TObject* o = $new(TObject);
    $(TCList, l0, prepend, o);
    $unref(o);
  }
  $unref(l0);


  TCList* l1 = $new(TCList);
  for (int i = 0; i < 1024; ++i) {
    TObject* o = $new(TObject);
    $(TCList, l1, append, o);
    $unref(o);
  }
  $unref(l1);


  TCList* l2 = $new(TCList);
  for (int i = 0; i < 1024; ++i) {
    char* s = NULL;
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
    size_t sz = _scprintf("%d", i) + 1;
    s = (char*)malloc(sz);
    sprintf_s(s, sz, "%d", i);
#else
    asprintf(&s, "%d", i);
#endif
    TCString* str = $new(TCString, s);
    $(TCList, l2, append, (TObject*) str);
    $unref(str);
    free(s);
  }
  $(TCList, l2, foreach, (TCListIterator) test_lists_iter, (TObject*) foo);
  $unref(l2);

  $unref(foo);
}

TCVector* get_vector() {
  TCVector* vector = $new(TCVector, 0, 0);

  for (int i = 0; i < 128; ++i) {
    char* s = NULL;
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
    size_t sz = _scprintf("%d", i) + 1;
    s = (char*)malloc(sz);
    sprintf_s(s, sz, "%d", i);
#else
    asprintf(&s, "%d", i);
#endif
    TCString* str = $new(TCString, s);
    $(TCVector, vector, push_back, (TObject*) str);
    $unref(str);
    free(s);
  }

  return vector;
}

void test_vectors() {
  TObject* o = NULL;

  TCVector* v1 = get_vector();
  for (int i = 0; i < v1->len; ++i) {
    TObject* o = v1->arr[i];
    $ref(o);
    $unref(o);
  }
  $unref(v1);


  o = NULL;
  TCVector* v2 = get_vector();
  while ((o = $(TCVector, v2, pop_back), o != NULL)) {
    $unref(o);
  }
  $unref(v2);


  o = NULL;
  TCVector* v3 = get_vector();
  while ((o = $(TCVector, v3, pop_front), o != NULL)) {
    $unref(o);
  }
  $unref(v3);


  TCVector* v4 = get_vector();
  $(TCVector, v4, remove, 0);
  $(TCVector, v4, remove, 1);
  $(TCVector, v4, remove, 2);
  for (int i = 0; i < v4->len; ++i) {
    TObject* o = $(TCVector, v4, get, i);
    if (o == NULL) continue;
    $unref(o);
  }
  $unref(v4);
}

void test_queues() {
  TCQueue* q = $new(TCQueue, 128);

  for (int i = 0; i < 128; ++i) {
    TObject* o = $new(TObject);
    $(TCQueue, q, push, o);
    $unref(o);
  }

  for (int i = 0; i < 64; ++i) {
    $unref($(TCQueue, q, pop));
  }

  $unref(q);
}

void test_maps() {
  TCMap* map = $new(TCMap);

  TCString* s1 = $str("Bar");
  $(TCMap, map, set, "foo", (TObject*) s1);
  $unref(s1);

  TCString* s2 = (TCString*) $(TCMap, map, get, "foo");
  $unref(s2);

  TCString* s3 = $str("Baz");
  $(TCMap, map, set, "foo", (TObject*) s3);
  $unref(s3);

  TCString* s4 = (TCString*) $(TCMap, map, get, "foo");
  $unref(s4);

  TCString* s5 = $str("spam");
  $(TCMap, map, set, "quux", (TObject*) s5);
  $unref(s5);

  TCString* s6 = (TCString*) $(TCMap, map, get, "quux");
  $unref(s6);

  $(TCMap, map, rename, "quux", "asdf");

  TCString* s7 = (TCString*) $(TCMap, map, get, "asdf");
  $unref(s7);

  $(TCMap, map, remove, "asdf");

  $unref(map);
}

int main() {
  test_strings();
  test_lists();
  test_vectors();
  test_queues();
  test_maps();

  return 0;
}