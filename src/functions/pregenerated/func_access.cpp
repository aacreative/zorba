/*
 * Copyright 2006-2008 The FLWOR Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
// ******************************************
// *                                        *
// * THIS IS A GENERATED FILE. DO NOT EDIT! *
// * SEE .xml FILE WITH SAME NAME           *
// *                                        *
// ******************************************


#include "stdafx.h"
#include "runtime/access/access.h"
#include "functions/func_access.h"


namespace zorba{



PlanIter_t fn_unparsed_text::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new FnUnparsedTextIterator(sctx, loc, argv);
}

PlanIter_t fn_unparsed_text_available::codegen(
  CompilerCB*,
  static_context* sctx,
  const QueryLoc& loc,
  std::vector<PlanIter_t>& argv,
  expr& ann) const
{
  return new FnUnparsedTextAvailableIterator(sctx, loc, argv);
}

void populate_context_access(static_context* sctx)
{
  {
    

    DECL_WITH_KIND(sctx, fn_unparsed_text,
        (createQName("http://www.w3.org/2005/xpath-functions","","unparsed-text"), 
        GENV_TYPESYSTEM.STRING_TYPE_QUESTION, 
        GENV_TYPESYSTEM.STRING_TYPE_QUESTION),
        FunctionConsts::FN_UNPARSED_TEXT_1);

  }


  {
    

    DECL_WITH_KIND(sctx, fn_unparsed_text,
        (createQName("http://www.w3.org/2005/xpath-functions","","unparsed-text"), 
        GENV_TYPESYSTEM.STRING_TYPE_QUESTION, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_QUESTION),
        FunctionConsts::FN_UNPARSED_TEXT_2);

  }


  {
    

    DECL_WITH_KIND(sctx, fn_unparsed_text_available,
        (createQName("http://www.w3.org/2005/xpath-functions","","unparsed-text-available"), 
        GENV_TYPESYSTEM.STRING_TYPE_QUESTION, 
        GENV_TYPESYSTEM.STRING_TYPE_QUESTION),
        FunctionConsts::FN_UNPARSED_TEXT_AVAILABLE_1);

  }


  {
    

    DECL_WITH_KIND(sctx, fn_unparsed_text_available,
        (createQName("http://www.w3.org/2005/xpath-functions","","unparsed-text-available"), 
        GENV_TYPESYSTEM.STRING_TYPE_QUESTION, 
        GENV_TYPESYSTEM.STRING_TYPE_ONE, 
        GENV_TYPESYSTEM.STRING_TYPE_QUESTION),
        FunctionConsts::FN_UNPARSED_TEXT_AVAILABLE_2);

  }

}


}



