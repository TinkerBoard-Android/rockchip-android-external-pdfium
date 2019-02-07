// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_CHARPOSLIST_H_
#define CORE_FPDFAPI_RENDER_CPDF_CHARPOSLIST_H_

#include <vector>

#include "core/fxcrt/fx_system.h"

class CPDF_Font;
class TextCharPos;

class CPDF_CharPosList {
 public:
  CPDF_CharPosList(const std::vector<uint32_t>& charCodes,
                   const std::vector<float>& charPos,
                   CPDF_Font* pFont,
                   float font_size);
  ~CPDF_CharPosList();

  uint32_t empty() const { return m_nChars == 0; }
  uint32_t GetCount() const;
  const TextCharPos& GetAt(size_t index) const;

 private:
  // TODO(thestig): Convert to unique_ptr or vector.
  TextCharPos* m_pCharPos = nullptr;
  uint32_t m_nChars = 0;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_CHARPOSLIST_H_
