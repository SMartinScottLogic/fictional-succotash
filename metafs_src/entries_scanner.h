#ifndef ENTRIES_SCANNER_H
#define ENTRIES_SCANNER_H

#include "entries.h"

void scan(Entries *const entries, const std::list<std::string> &paths, const std::list<pattern_component> &pattern_parts);

#endif//ENTRIES_SCANNER_H
