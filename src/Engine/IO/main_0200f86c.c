#include "Engine/IO/Str.h"

#define STRLIST_MAX_LEN 0x100

void func_0200f86c(StrListOwner* owner) {
    StrListNode* node;

    for (node = owner->listB; node != NULL; node = node->next) {
        if (node->str != NULL) {
            Str_Validate(node->str, STRLIST_MAX_LEN);
        }
    }
}

void func_0200f8a4(StrListOwner* owner) {
    StrListNode* node;

    for (node = owner->listA; node != NULL; node = node->next) {
    }
}

void func_0200f8c0(StrListOwner* owner) {
    StrListNode* node;

    for (node = owner->listC; node != NULL; node = node->next) {
        if (node->str != NULL) {
            Str_Validate(node->str, STRLIST_MAX_LEN);
        }
    }
}

void func_0200f8f8(StrListOwner* owner) {
    func_0200f86c(owner);
    func_0200f8a4(owner);
    func_0200f8c0(owner);
}

s32 func_0200f918(StrListNode* node) {
    s32 count = 0;

    for (; node != NULL; node = node->next) {
        count++;
    }
    return count;
}

s32 func_0200f93c(StrListOwner* owner) {
    return func_0200f918(owner->listB);
}
