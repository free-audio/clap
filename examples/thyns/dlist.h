#ifndef DLIST_H
# define DLIST_H

# define thyns_dlist_push_back(Head, Item)      \
  do {                                          \
    assert((Item)->prev == NULL);               \
    assert((Item)->next == NULL);               \
    if (!(Head)) {                              \
      (Head) = (Item);                          \
      (Item)->next = (Item);                    \
      (Item)->prev = (Item);                    \
    } else {                                    \
      (Item)->next = (Head);                    \
      (Item)->prev = (Head)->prev;              \
      (Item)->prev->next = (Item);              \
      (Item)->next->prev = (Item);              \
    }                                           \
  } while (0)

# define thyns_dlist_remove(Head, Item)         \
  do {                                          \
    assert((Head));                             \
    assert((Item)->prev);                       \
    assert((Item)->next);                       \
    if ((Item)->next == (Item))                 \
      (Head) = NULL;                            \
    else {                                      \
      (Head) = (Item)->next;                    \
      (Item)->next->prev = (Item)->prev;        \
      (Item)->prev->next = (Item)->next;        \
    }                                           \
    (Item)->next = NULL;                        \
    (Item)->prev = NULL;                        \
  } while (0)

#endif /* !DLIST_H */
