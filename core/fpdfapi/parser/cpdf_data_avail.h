// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_DATA_AVAIL_H_
#define CORE_FPDFAPI_PARSER_CPDF_DATA_AVAIL_H_

#include <memory>
#include <set>
#include <vector>

#include "core/fpdfapi/parser/cpdf_parser.h"
#include "core/fpdfapi/parser/cpdf_syntax_parser.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Dictionary;
class CPDF_HintTables;
class CPDF_IndirectObjectHolder;
class CPDF_LinearizedHeader;
class CPDF_Parser;
class CPDF_ReadValidator;

enum PDF_DATAAVAIL_STATUS {
  PDF_DATAAVAIL_HEADER = 0,
  PDF_DATAAVAIL_FIRSTPAGE,
  PDF_DATAAVAIL_HINTTABLE,
  PDF_DATAAVAIL_END,
  PDF_DATAAVAIL_CROSSREF,
  PDF_DATAAVAIL_CROSSREF_ITEM,
  PDF_DATAAVAIL_TRAILER,
  PDF_DATAAVAIL_LOADALLCROSSREF,
  PDF_DATAAVAIL_ROOT,
  PDF_DATAAVAIL_INFO,
  PDF_DATAAVAIL_ACROFORM,
  PDF_DATAAVAIL_ACROFORM_SUBOBJECT,
  PDF_DATAAVAIL_PAGETREE,
  PDF_DATAAVAIL_PAGE,
  PDF_DATAAVAIL_PAGE_LATERLOAD,
  PDF_DATAAVAIL_RESOURCES,
  PDF_DATAAVAIL_DONE,
  PDF_DATAAVAIL_ERROR,
  PDF_DATAAVAIL_LOADALLFILE,
};

enum PDF_PAGENODE_TYPE {
  PDF_PAGENODE_UNKNOWN = 0,
  PDF_PAGENODE_PAGE,
  PDF_PAGENODE_PAGES,
  PDF_PAGENODE_ARRAY,
};

class CPDF_DataAvail final {
 public:
  // Must match PDF_DATA_* definitions in public/fpdf_dataavail.h, but cannot
  // #include that header. fpdfsdk/fpdf_dataavail.cpp has static_asserts
  // to make sure the two sets of values match.
  enum DocAvailStatus {
    DataError = -1,        // PDF_DATA_ERROR
    DataNotAvailable = 0,  // PDF_DATA_NOTAVAIL
    DataAvailable = 1,     // PDF_DATA_AVAIL
  };

  // Must match PDF_*LINEAR* definitions in public/fpdf_dataavail.h, but cannot
  // #include that header. fpdfsdk/fpdf_dataavail.cpp has static_asserts
  // to make sure the two sets of values match.
  enum DocLinearizationStatus {
    LinearizationUnknown = -1,  // PDF_LINEARIZATION_UNKNOWN
    NotLinearized = 0,          // PDF_NOT_LINEARIZED
    Linearized = 1,             // PDF_LINEARIZED
  };

  // Must match PDF_FORM_* definitions in public/fpdf_dataavail.h, but cannot
  // #include that header. fpdfsdk/fpdf_dataavail.cpp has static_asserts
  // to make sure the two sets of values match.
  enum DocFormStatus {
    FormError = -1,        // PDF_FORM_ERROR
    FormNotAvailable = 0,  // PDF_FORM_NOTAVAIL
    FormAvailable = 1,     // PDF_FORM_AVAIL
    FormNotExist = 2,      // PDF_FORM_NOTEXIST
  };

  class FileAvail {
   public:
    virtual ~FileAvail();
    virtual bool IsDataAvail(FX_FILESIZE offset, uint32_t size) = 0;
  };

  class DownloadHints {
   public:
    virtual ~DownloadHints();
    virtual void AddSegment(FX_FILESIZE offset, uint32_t size) = 0;
  };

  CPDF_DataAvail(FileAvail* pFileAvail,
                 const RetainPtr<IFX_SeekableReadStream>& pFileRead,
                 bool bSupportHintTable);
  ~CPDF_DataAvail();

  bool IsDataAvail(FX_FILESIZE offset, uint32_t size);
  DocAvailStatus IsDocAvail(DownloadHints* pHints);
  void SetDocument(CPDF_Document* pDoc);
  DocAvailStatus IsPageAvail(uint32_t dwPage, DownloadHints* pHints);
  DocFormStatus IsFormAvail(DownloadHints* pHints);
  DocLinearizationStatus IsLinearizedPDF();
  bool IsLinearized();
  RetainPtr<IFX_SeekableReadStream> GetFileRead() const;
  int GetPageCount() const;
  CPDF_Dictionary* GetPage(int index);
  RetainPtr<CPDF_ReadValidator> GetValidator() const;

 protected:
  class PageNode {
   public:
    PageNode();
    ~PageNode();

    PDF_PAGENODE_TYPE m_type;
    uint32_t m_dwPageNo;
    std::vector<std::unique_ptr<PageNode>> m_ChildNodes;
  };

  static const int kMaxPageRecursionDepth = 1024;

  bool AreObjectsAvailable(std::vector<CPDF_Object*>& obj_array,
                           bool bParsePage,
                           std::vector<CPDF_Object*>& ret_array);
  bool CheckDocStatus();
  bool CheckHeader();
  bool CheckFirstPage();
  bool CheckHintTables();
  bool CheckEnd();
  bool CheckCrossRef();
  bool CheckCrossRefItem();
  bool CheckTrailer();
  bool CheckRoot();
  bool CheckInfo();
  bool CheckPages();
  bool CheckPage();
  bool CheckResources();
  bool CheckAcroForm();
  bool CheckAcroFormSubObject();
  bool CheckPageStatus();

  bool IsLinearizedFile(uint8_t* pData, uint32_t dwLen);
  void SetStartOffset(FX_FILESIZE dwOffset);
  bool GetNextToken(ByteString* token);
  bool GetNextChar(uint8_t& ch);
  std::unique_ptr<CPDF_Object> ParseIndirectObjectAt(
      FX_FILESIZE pos,
      uint32_t objnum,
      CPDF_IndirectObjectHolder* pObjList = nullptr);
  std::unique_ptr<CPDF_Object> GetObject(uint32_t objnum,
                                         bool* pExistInFile);
  bool GetPageKids(CPDF_Parser* pParser, CPDF_Object* pPages);
  bool PreparePageItem();
  bool LoadPages();
  bool LoadAllXref();
  bool LoadAllFile();
  DocAvailStatus CheckLinearizedData();
  bool CheckPageAnnots(uint32_t dwPage);

  DocAvailStatus CheckLinearizedFirstPage(uint32_t dwPage);
  bool CheckPage(uint32_t dwPage);
  bool LoadDocPages();
  bool LoadDocPage(uint32_t dwPage);
  bool CheckPageNode(const PageNode& pageNode,
                     int32_t iPage,
                     int32_t& iCount,
                     int level);
  bool CheckUnknownPageNode(uint32_t dwPageNo, PageNode* pPageNode);
  bool CheckArrayPageNode(uint32_t dwPageNo, PageNode* pPageNode);
  bool CheckPageCount();
  bool IsFirstCheck(uint32_t dwPage);
  void ResetFirstCheck(uint32_t dwPage);
  bool ValidatePage(uint32_t dwPage);
  bool ValidateForm();

  FileAvail* const m_pFileAvail;
  RetainPtr<CPDF_ReadValidator> m_pFileRead;
  CPDF_Parser m_parser;
  CPDF_SyntaxParser m_syntaxParser;
  std::unique_ptr<CPDF_Object> m_pRoot;
  uint32_t m_dwRootObjNum;
  uint32_t m_dwInfoObjNum;
  std::unique_ptr<CPDF_LinearizedHeader> m_pLinearized;
  UnownedPtr<CPDF_Object> m_pTrailer;
  bool m_bDocAvail;
  FX_FILESIZE m_dwHeaderOffset;
  FX_FILESIZE m_dwLastXRefOffset;
  FX_FILESIZE m_dwXRefOffset;
  FX_FILESIZE m_dwTrailerOffset;
  FX_FILESIZE m_dwCurrentOffset;
  PDF_DATAAVAIL_STATUS m_docStatus;
  FX_FILESIZE m_dwFileLen;
  CPDF_Document* m_pDocument;
  std::set<uint32_t> m_ObjectSet;
  std::vector<CPDF_Object*> m_objs_array;
  FX_FILESIZE m_Pos;
  FX_FILESIZE m_bufferOffset;
  uint32_t m_bufferSize;
  ByteString m_WordBuf;
  uint8_t m_bufferData[512];
  std::vector<uint32_t> m_XRefStreamList;
  std::vector<uint32_t> m_PageObjList;
  uint32_t m_PagesObjNum;
  bool m_bLinearedDataOK;
  bool m_bMainXRefLoadTried;
  bool m_bMainXRefLoadedOK;
  bool m_bPagesTreeLoad;
  bool m_bPagesLoad;
  CPDF_Parser* m_pCurrentParser;
  FX_FILESIZE m_dwCurrentXRefSteam;
  bool m_bAnnotsLoad;
  bool m_bHaveAcroForm;
  uint32_t m_dwAcroFormObjNum;
  bool m_bAcroFormLoad;
  std::vector<std::unique_ptr<CPDF_Object>> m_Acroforms;
  CPDF_Dictionary* m_pPageDict;
  CPDF_Object* m_pPageResource;
  bool m_bNeedDownLoadResource;
  bool m_bPageLoadedOK;
  bool m_bLinearizedFormParamLoad;
  std::vector<std::unique_ptr<CPDF_Object>> m_PagesArray;
  uint32_t m_dwEncryptObjNum;
  FX_FILESIZE m_dwPrevXRefOffset;
  bool m_bTotalLoadPageTree;
  bool m_bCurPageDictLoadOK;
  PageNode m_PageNode;
  std::set<uint32_t> m_pageMapCheckState;
  std::set<uint32_t> m_pagesLoadState;
  std::set<uint32_t> m_SeenPrevPositions;
  std::unique_ptr<CPDF_HintTables> m_pHintTables;
  bool m_bSupportHintTable;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_DATA_AVAIL_H_
