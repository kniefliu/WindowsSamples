#ifndef QUI_CORE_PREREQUISITES_H_
#define QUI_CORE_PREREQUISITES_H_

#include "qui/core/qui_export.h"

// include std required
#include <cstdint>

// version
#include "qui/core/qui_version.h"

// Pre-declare classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files


/* Include all the standard header *after* all the configuration settings have been made. */
#include "qui/core/qui_std_headers.h"

#endif
