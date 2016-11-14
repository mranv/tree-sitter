#ifndef RUNTIME_LANGUAGE_H_
#define RUNTIME_LANGUAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tree_sitter/parser.h"
#include "runtime/tree.h"

typedef struct {
  const TSParseAction *actions;
  uint32_t action_count;
  bool is_reusable;
  bool depends_on_lookahead;
} TableEntry;

void ts_language_table_entry(const TSLanguage *, TSStateId, TSSymbol, TableEntry *);

TSSymbolMetadata ts_language_symbol_metadata(const TSLanguage *, TSSymbol);

static inline const TSParseAction *ts_language_actions(const TSLanguage *self,
                                                       TSStateId state,
                                                       TSSymbol symbol,
                                                       uint32_t *count) {
  TableEntry entry;
  ts_language_table_entry(self, state, symbol, &entry);
  *count = entry.action_count;
  return entry.actions;
}

static inline TSStateId ts_language_next_state(const TSLanguage *self,
                                               TSStateId state,
                                               TSSymbol symbol) {
  if (symbol == ts_builtin_sym_error) {
    return 0;
  } else if (symbol < self->token_count) {
    uint32_t count;
    const TSParseAction *actions = ts_language_actions(self, state, symbol, &count);
    if (count > 0) {
      TSParseAction action = actions[count - 1];
      if (action.type == TSParseActionTypeShift || action.type == TSParseActionTypeRecover) {
        return action.params.to_state;
      }
    }
    return 0;
  } else {
    return self->parse_table[state * self->symbol_count + symbol];
  }
}

#ifdef __cplusplus
}
#endif

#endif  // RUNTIME_LANGUAGE_H_
