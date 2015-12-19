#ifndef SPARROW_SYSTEM_PROPERTY_H
#define SPARROW_SYSTEM_PROPERTY_H

#include <string.h>

#include "data.h"

#define PROP_NAME_MAX 32
#define PROP_VALUE_MAX  92

// Infact, PAGE_SIZE is set to 128*1024 in Android system.
// We don't need a so big memory.
#define PAGE_SIZE 4096

#define PROP_MSG_SETPROP 1

//#define PROP_FILENAME "./dev/_properties_"
#define PROP_FILENAME "_properties_"

#define PROP_SERVICE_NAME "property_service"

/*
 * Properties are stored in a hybrid trie/binary tree structure.
 * Each property's name is delimited at '.' characters, and the tokens are put
 * into a trie structure.  Siblings at each level of the trie are stored in a
 * binary tree.  For instance, "ro.secure"="1" could be stored as follows:
 *
 * +-----+   children    +----+   children    +--------+
 * |     |-------------->| ro |-------------->| secure |
 * +-----+               +----+               +--------+
 *                       /    \                /   |
 *                 left /      \ right   left /    |  prop   +===========+
 *                     v        v            v     +-------->| ro.secure |
 *                  +-----+   +-----+     +-----+            +-----------+
 *                  | net |   | sys |     | com |            |     1     |
 *                  +-----+   +-----+     +-----+            +===========+
 */

// Represents a node in the trie.
struct prop_bt {
    uint8_t namelen;
    uint8_t reserved[3];

    // The property trie is updated only by the init process (single threaded) which provides
    // property service. And it can be read by multiple threads at the same time.
    // As the property trie is not protected by locks, we use atomic_uint_least32_t types for the
    // left, right, children "pointers" in the trie node. To make sure readers who see the
    // change of "pointers" can also notice the change of prop_bt structure contents pointed by
    // the "pointers", we always use release-consume ordering pair when accessing these "pointers".

    // prop "points" to prop_info structure if there is a propery associated with the trie node.
    // Its situation is similar to the left, right, children "pointers". So we use
    // atomic_uint_least32_t and release-consume ordering to protect it as well.

    // We should also avoid rereading these fields redundantly, since not
    // all processor implementations ensure that multiple loads from the
    // same field are carried out in the right order.
    atomic_uint_least32_t prop;

    atomic_uint_least32_t left;
    atomic_uint_least32_t right;

    atomic_uint_least32_t children;

    char name[0];

    prop_bt(const char *name, const uint8_t name_length) {
        this->namelen = name_length;
        memcpy(this->name, name, name_length);
        this->name[name_length] = '\0';
    }
};

struct prop_area {
    uint32_t bytes_used;
    atomic_uint_least32_t serial;
    uint32_t magic;
    uint32_t version;
    uint32_t reserved[28];
    char data[0];

    prop_area(const uint32_t magic, const uint32_t version) :
        magic(magic), version(version) {
        //atomic_init(&serial, 0);
        memset(reserved, 0, sizeof(reserved));
        // Allocate enough space for the root node.
        bytes_used = sizeof(prop_bt);
    }
};

struct prop_info {
    atomic_uint_least32_t serial;
    char value[PROP_VALUE_MAX];
    char name[0];

    prop_info(const char *name, const uint8_t namelen, const char *value,
              const uint8_t valuelen) {
        memcpy(this->name, name, namelen);
        this->name[namelen] = '\0';
        //atomic_init(&this->serial, valuelen << 24);
        memcpy(this->value, value, valuelen);
        this->value[valuelen] = '\0';
    }
};


struct prop_msg
{
    unsigned cmd;
    char name[PROP_NAME_MAX];
    char value[PROP_VALUE_MAX];
};

typedef struct prop_msg prop_msg;

int __system_property_area_init();

int __map_prop_area();

int __system_property_set(const char *key, const char* value);

#endif


