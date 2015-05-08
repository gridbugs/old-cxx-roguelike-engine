#include "actor/knowledge.hpp"
#include "util/arith.hpp"

#define REMEMBERED  BIT(0)
#define VISIBLE     BIT(1)
#define MAGIC       BIT(2)

bool knowledge_cell::is_known() {
    return knowledge_ != 0;
}

void knowledge_cell::see() {
    knowledge_ |= (VISIBLE | REMEMBERED);
}

void knowledge_cell::memorize() {
    knowledge_ |= REMEMBERED;
}

void knowledge_cell::magically_reveal() {
    knowledge_ |= (MAGIC | REMEMBERED);
}

void knowledge_cell::unsee() {
    knowledge_ &= ~VISIBLE;
}

void knowledge_cell::forget() {
    knowledge_ &= ~REMEMBERED;
}

void knowledge_cell::unmagic() {
    knowledge_ &= ~MAGIC;
}

bool knowledge_cell::is_unknown() {
    return knowledge_ == 0;
}

bool knowledge_cell::is_remembered() {
    return knowledge_ & REMEMBERED;
}

bool knowledge_cell::is_visible() {
    return knowledge_ & VISIBLE;
}

bool knowledge_cell::is_magic() {
    return knowledge_ & MAGIC;
}