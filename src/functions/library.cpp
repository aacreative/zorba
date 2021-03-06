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
#include "stdafx.h"

#include <iostream>

#include "zorbamisc/ns_consts.h"

#include "functions/library.h"
#include "functions/function.h"
#include "functions/signature.h"

#include "functions/func_accessors.h"
#include "functions/func_accessors_impl.h"
#include "functions/func_any_uri.h"
#include "functions/func_apply.h"
#include "functions/func_arithmetic.h"
#include "functions/func_base64.h"
#include "functions/func_booleans.h"
#include "functions/func_booleans_impl.h"
#include "functions/func_collections.h"
#include "functions/func_context.h"
#include "functions/func_csv.h"
#include "functions/func_datetime.h"
#include "functions/func_dctx.h"
#include "functions/func_documents.h"
#include "functions/func_durations_dates_times.h"
#include "functions/func_durations_dates_times_impl.h"
#include "functions/func_enclosed.h"
#include "functions/func_errors_and_diagnostics.h"
#include "functions/func_eval.h"
#include "functions/func_fetch.h"
#include "functions/func_fnput.h"
#include "functions/func_hoist.h"
#include "functions/func_ic_ddl.h"
#include "functions/func_index_ddl.h"
#include "functions/func_index_func.h"
#include "functions/func_item.h"
#include "functions/func_json.h"
#include "functions/func_jsound.h"
#include "functions/func_maps.h"
#include "functions/func_maths.h"
#include "functions/func_nodes.h"
#include "functions/func_reference.h"
#include "functions/func_node_position.h"
#include "functions/func_node_sort_distinct.h"
#include "functions/func_nodes.h"
#include "functions/func_numerics.h"
#include "functions/func_numerics_impl.h"
#include "functions/func_other_diagnostics.h"
#include "functions/func_parse_fragment.h"
#include "functions/func_parse_fragment_impl.h"
#include "functions/func_parsing_and_serializing.h"
#include "functions/func_qnames.h"
#include "functions/func_random.h"
#include "functions/func_reflection.h"
#include "functions/func_schema.h"
#include "functions/func_sctx.h"
#include "functions/func_seq.h"
#include "functions/func_sequences.h"
#include "functions/func_sequences_impl.h"
#include "functions/func_strings.h"
#include "functions/func_strings_impl.h"
#include "functions/func_uris.h"
#include "functions/func_var_decl.h"
#include "functions/func_xqdoc.h"
#ifndef ZORBA_NO_FULL_TEXT
#include "functions/func_ft_module.h"
#include "runtime/full_text/ft_module_impl.h"
#endif /* ZORBA_NO_FULL_TEXT */

#include "functions/func_fn_hof_functions.h"
#include "functions/func_fn_hof_functions_impl.h"

#include "zorbaserialization/archiver.h"

#include "functions/func_jsoniq_functions.h"
#include "functions/func_jsoniq_functions_impl.h"


namespace zorba
{

// clear static initializer state

// dummy function to tell the windows linker to keep the library.obj
// even though it contains no public objects or functions
// this is called at initializeZorba
void library_init()
{
}


BuiltinFunctionLibrary::BuiltinFunctionLibrary()
{
  theFunctions = new function*[FunctionConsts::FN_MAX_FUNC];

  memset(&theFunctions[0], 0, FunctionConsts::FN_MAX_FUNC * sizeof(function*));
}


void BuiltinFunctionLibrary::populate(static_context* sctx)
{
  populate_context_accessors(sctx);
  populate_context_any_uri(sctx);
  populate_context_accessors_impl(sctx);
  populate_context_base64(sctx);
  populate_context_booleans(sctx);
  populate_context_booleans_impl(sctx);
  populate_context_collections(sctx);
  populate_context_context(sctx);
  populate_context_csv(sctx);
  populate_context_datetime(sctx);
  populate_context_dctx(sctx);
  populate_context_durations_dates_times(sctx);
  populate_context_durations_dates_times_impl(sctx);
  populate_context_errors_and_diagnostics(sctx);
  populate_context_fnput(sctx);
  populate_context_index_ddl(sctx);
  populate_context_index_func(sctx);
  populate_context_ic_ddl(sctx);
  populate_context_json(sctx);
  populate_context_jsound(sctx);
  populate_context_maths(sctx);
  populate_context_nodes(sctx);
  populate_context_item(sctx);
  populate_context_reference(sctx);
  populate_context_node_position(sctx);
  populate_context_numerics(sctx);
  populate_context_other_diagnostics(sctx);
  populate_context_parsing_and_serializing(sctx);
  populate_context_parse_fragment(sctx);
  populate_context_parse_fragment_impl(sctx);
  populate_context_qnames(sctx);
  populate_context_random(sctx);
  populate_context_schema(sctx);
  populate_context_sctx(sctx);
  populate_context_seq(sctx);
  populate_context_strings(sctx);
  populate_context_strings_impl(sctx);
  populate_context_uris(sctx);
  populate_context_sequences(sctx);
  populate_context_sequences_impl(sctx);
  populate_context_xqdoc(sctx);
  populate_context_fn_hof_functions(sctx);
  populate_context_hof_impl(sctx);
  populate_context_documents(sctx);
  populate_context_maps(sctx);

  populateContext_Arithmetics(sctx);
  populateContext_Numerics(sctx);
  populateContext_DocOrder(sctx);
  populateContext_Comparison(sctx);
  populateContext_Constructors(sctx);
  populateContext_VarDecl(sctx);
  populateContext_Hoisting(sctx);
  populate_context_eval(sctx);
  populate_context_reflection(sctx);
  populate_context_apply(sctx);

  populate_context_fetch(sctx);
#ifndef ZORBA_NO_FULL_TEXT
  populate_context_ft_module(sctx);
#endif /* ZORBA_NO_FULL_TEXT */

  populate_context_jsoniq_functions(sctx);
  populate_context_jsoniq_functions_impl(sctx);

#ifdef PRE_SERIALIZE_BUILTIN_FUNCTIONS
  ar.set_loading_hardcoded_objects(false);
#endif
}


BuiltinFunctionLibrary::~BuiltinFunctionLibrary()
{
  for (csize i = 0; i < FunctionConsts::FN_MAX_FUNC; ++i)
  {
    delete theFunctions[i];
  }

  delete [] theFunctions;
}



} /* namespace zorba */
/* vim:set et sw=2 ts=2: */
