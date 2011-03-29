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

#include <iostream>

#include "zorbaerrors/error_manager.h"
#include "errors/user_error.h"

#include "context/static_context.h"

#include "runtime/errors_and_diagnostics/errors_and_diagnostics.h"
#include "runtime/util/iterator_impl.h"

#include "store/api/item.h"
#include "store/api/item_factory.h"

#include "system/globalenv.h"
#include "zorbatypes/zstring.h"

namespace zorba 
{

/*******************************************************************************
  3.1.1 fn:error
********************************************************************************/
bool
ErrorIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  static const char *err_ns = "http://www.w3.org/2005/xqt-errors";
  store::Item_t err_qname;
  GENV_ITEMFACTORY->createQName(err_qname, err_ns, "err", "FOER0000");
  store::Item_t lTmpQName;
  store::Item_t lTmpErrorObject;
  store::Item_t lTmpDescr;
  zstring description;
  std::vector<store::Item_t> lErrorObject; 

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  if (theChildren.size() >= 1) 
  {
    if (consumeNext(lTmpQName, theChildren[0].getp(), planState))
      err_qname = lTmpQName;
  }

  if (theChildren.size() >= 2) 
  {
    consumeNext(lTmpDescr, theChildren[1].getp(), planState);
    description = lTmpDescr->getStringValue();
  }

  if (theChildren.size() == 3) 
  {
    while (consumeNext(lTmpErrorObject, theChildren[2].getp(), planState)) 
    {
      lErrorObject.push_back(lTmpErrorObject);
    }
  }

  {
    error::ZorbaUserError lError(err_qname, description, loc, 
                                 __FILE__, __LINE__, lErrorObject);
    throw lError;
  }

  STACK_END(state);
}

/*******************************************************************************
  3.2.1 fn:trace
********************************************************************************/
bool
TraceIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  TraceIteratorState *state;
  DEFAULT_STACK_INIT(TraceIteratorState, state, planState);

  if (!consumeNext(state->theTagItem, theChildren[1], planState)) {
    ZORBA_ERROR_LOC_DESC( FORG0006, loc,
        "An empty sequence is not allowed as as second argument to fn:trace");
  }

  state->theOS = theSctx->get_trace_stream();

  while (consumeNext(result, theChildren[0], planState)) 
  {
    (*state->theOS) << state->theTagItem->getStringValue() 
      << " [" << state->theIndex << "]: "
      << result->show()
      << std::endl;
    ++state->theIndex;

    STACK_PUSH(true, state);
  }


  STACK_END (state);
}

} /* namespace zorba */