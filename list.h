#include <stdlib.h>

typedef struct list_iterator_st list_iterator;
typedef struct list_st list;

struct list_iterator_st {
  list_iterator *next;
  list_iterator *prev;
  list *list;
  union {
    void *data;
    int integer;
  };
};

struct list_st {
  int list_length;
  list_iterator *elements;
  list_iterator *last;
};

static inline list_iterator *list_begin(list *l)
{
  return l ? l->elements : (list_iterator*)0;
}

static inline list_iterator *list_next(list_iterator *iter)
{
  return iter ? iter->next : (list_iterator*)0;
}

static inline void *list_get_data(list_iterator *iter)
{
  return iter ? iter->data : (void*)0;
}


static inline list_iterator *list_get_nth(list *l, int n)
{
  list_iterator *ret = list_begin(l);
  while (ret && n > 0) {
    ret = list_next(ret);
    n -= 1;
  }
  return ret;
}

static inline void *list_get_nth_data(list *l, int n)
{
  return list_get_data(list_get_nth(l, n));
}

static inline list_iterator *make_new_iter(list *l, void *data, list_iterator *prev, list_iterator *next) {
  list_iterator *new_iter = malloc(sizeof(*new_iter));
  new_iter->data = data;
  new_iter->list = l;
  new_iter->next = next;
  return new_iter;
}

static inline list_iterator *list_append(list* l, void *data)
{
  list_iterator *new_element = make_new_iter(l, data, l->last, (list_iterator*)0);
  if (l->last) {
    l->last->next = new_element;
    l->last = new_element;
  } else {
    l->last = l->elements = make_new_iter(l, data, (list_iterator*) 0, (list_iterator*)0);
  }
  return new_element;
}

static inline list_iterator *list_prepend(list *l, void *data)
{
  return (l->elements = make_new_iter(l, data, (list_iterator*) 0, l->elements));
}

static inline list_iterator *list_insert_after(list_iterator *iter, void* data)
{
  list_iterator *new_element = make_new_iter(iter->list, data, iter, iter->next);
  if (!iter->next) {
    iter->list->last = new_element;
  }
  iter->next = new_element;
  return new_element;
}

static inline list_iterator *list_insert_before(list_iterator *iter, void *data)
{
  list_iterator *new_element = make_new_iter(iter->list, data, iter->prev, iter);
  if (!iter->prev) {
    iter->list->elements = new_element;
  }
  iter->prev = new_element;
  return new_element;
}

static inline list *list_new(void)
{
  return (list*)calloc(1, sizeof(list));
}

static inline void list_free_ex(list *l, void (*free_cb)(void*))
{
  list_iterator *iter;
  while((iter = l->elements)) {
    l->elements = iter->next;
    if (free_cb) {
      free_cb(iter->data);
    }
    free(iter);
  }
  free(l);
}

static inline void list_free(list *l)
{
  list_free_ex(l, (void (*)(void*))0);
}
