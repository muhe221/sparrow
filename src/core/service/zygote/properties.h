#ifndef SPARROW_PROPERTIES_H
#define SPARROW_PROPERTIES_H

int property_set(const char *key, const char *value);

int property_get(const char *key, char *value, const char *default_value);

#endif
