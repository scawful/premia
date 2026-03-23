#ifndef PREMIA_VERSION_H
#define PREMIA_VERSION_H

/**
 * @file premia_version.h
 * @brief Premia version helpers.
 *
 * When building with CMake, include the generated premia_config.h (from the
 * build directory) for the definitive values:
 *   - PREMIA_VERSION_MAJOR
 *   - PREMIA_VERSION_MINOR
 *   - PREMIA_VERSION_PATCH
 *   - PREMIA_VERSION_STRING  (e.g., "2.0.0")
 *   - PREMIA_VERSION_NUMBER  (e.g., 20000)
 *
 * Single source of truth: project(premia VERSION X.Y.Z) in CMakeLists.txt
 */

/* Include the generated config if available (set by CMake). */
#if defined(__has_include)
#  if __has_include("premia_config.h")
#    include "premia_config.h"
#  endif
#elif defined(PREMIA_HAS_CONFIG)
#  include "premia_config.h"
#endif

/* Fallback if premia_config.h was not included — will be overridden by build */
#ifndef PREMIA_VERSION_STRING
#define PREMIA_VERSION_STRING "2.0.0"
#define PREMIA_VERSION_NUMBER 20000
#define PREMIA_VERSION_MAJOR 2
#define PREMIA_VERSION_MINOR 0
#define PREMIA_VERSION_PATCH 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the current Premia version string (e.g., "2.0.0").
 */
static inline const char* premia_get_version_string(void) {
  return PREMIA_VERSION_STRING;
}

/**
 * @brief Get the current Premia version number (major*10000 + minor*100 + patch).
 */
static inline int premia_get_version_number(void) {
  return PREMIA_VERSION_NUMBER;
}

#ifdef __cplusplus
}
#endif

#endif /* PREMIA_VERSION_H */
