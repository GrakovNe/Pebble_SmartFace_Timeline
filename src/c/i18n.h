// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <stdint.h>

typedef enum {
  LanguageEnglish = 0,
  LanguageRussian = 1,
  LanguageGerman = 2,
  LanguageCount,
} Language;

static inline Language language_clamp(int32_t raw) {
  if (raw < LanguageEnglish || raw >= LanguageCount) {
    return LanguageEnglish;
  }
  return (Language)raw;
}
