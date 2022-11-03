#ifndef CHECKED_CONFIG_H
#define CHECKED_CONFIG_H

#include "statemachine_config.h"

/// Clients may select an alternative assert() implementation by specifying it
/// in the configuration file.
#ifndef SM_ASSERT
#include <assert.h>
#define SM_ASSERT(x) assert(x)
#endif

#endif /* CHECKED_CONFIG_H */
