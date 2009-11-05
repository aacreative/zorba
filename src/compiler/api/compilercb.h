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
#ifndef XQP_COMPILERCB_H
#define XQP_COMPILERCB_H

#include <vector>
#include <map>

#include <zorba/config.h>

#include "common/shared_types.h"

#include "context/static_context.h"

#include "zorbaserialization/serialization_engine.h"

namespace zorba {

class ZorbaDebuggerCommons;
class static_context;

// exported for unit testing only

/*******************************************************************************
  There is one CompilerCB per module participating in a compilation. The
  compilerCB for the root module is created by the constructor of the XQueryImpl
  obj and remains alive for the whole duration of the query (including runtime).
  The CompilerCBs of imported modules are created and stay alive only during the
  translation of their associated modules.

  theIsLoadProlog :

  theRootSctx     : The root static ctx for the associated module. For each
                    module, this is a child of either a user provided static
                    ctx or the zorba default root context.
  m_cur_sctx      : The numeric id of the last sctx that was added to the 
                    m_context_map. Every time an expr is created, the current
                    value of m_cur_sctx is stored in the expr obj, so that each
                    expr will be executed in the appropriate sctx.
  theSctxMap      : A reference to the query-level map that maps sctx numeric ids
                    to sctx objs. In non-DEBUGGER mode, the map stores entries 
                    only for the root sctx of each module. In DEBUGGER mode, it
                    stores entries for all sctxs created by each module. By 
                    registering an sctx in this map, we make sure that it will
                    stay alive for the whole duration on the query. The map
                    is modified by the translator methods end_visit(ModuleImport)
                    and push_scope().
  m_sctx_list:    : A list of static contexts which need to be kept alive during
                    the translation of a module. This is different from the
                    query-level sctx map. The contexts in the query-level map are
                    used during runtime. Those in this list go away after the
                    translation process. It's managed in push_scope and pop_scope.
                    If the debugger is used, this list remains empty.
  m_error_manager : Pointer to an ErrorManager obj. In fact, all CompilerCBs
                    share the same ErrorManager.
  m_config        :
********************************************************************************/
class ZORBA_DLL_PUBLIC CompilerCB : public zorba::serialization::SerializeBaseClass
{
public:
  typedef struct config : public zorba::serialization::SerializeBaseClass
  {
    typedef enum 
    {
      O0,
      O1
    } opt_level_t;
    
    typedef void (* expr_callback) (const expr *, const std::string& name);
    typedef void (* ast_callback) (const parsenode *, const std::string& name);

    bool force_gflwor;
    opt_level_t opt_level;
    bool lib_module;
    ast_callback parse_cb;
    expr_callback translate_cb;
    expr_callback normalize_cb;
    expr_callback optimize_cb;
    bool print_item_flow;  // TODO: move to RuntimeCB

    config();

   public:
    SERIALIZABLE_CLASS(config)
    config(::zorba::serialization::Archiver &ar) 
      :
      parse_cb(NULL),
      translate_cb(NULL),
      normalize_cb(NULL),
      optimize_cb(NULL)
    {
    }

    ~config() {}

    void serialize(::zorba::serialization::Archiver &ar)
    {
      ar & force_gflwor;
      SERIALIZE_ENUM(opt_level_t, opt_level);
	    ar & lib_module;
      ar & print_item_flow;
    }
  } config_t;

  void
  setIsUpdating(bool aIsUpdating) { theIsUpdating = aIsUpdating; }

  bool
  isUpdating() const { return theIsUpdating; }

public:  
  bool                                theIsLoadProlog;

  bool                                theIsUpdating;
  
  static_context_t                    theRootSctx;
  short                               m_cur_sctx;
  std::map<short, static_context_t> * theSctxMap;
  std::vector<static_context_t>       m_sctx_list;

  error::ErrorManager               * theErrorManager;
  config_t                            theConfig;
  ZorbaDebuggerCommons              * theDebuggerCommons;

public:
  SERIALIZABLE_CLASS(CompilerCB);
  CompilerCB(::zorba::serialization::Archiver& ar);
  void serialize(::zorba::serialization::Archiver& ar);

public:
  CompilerCB(std::map<short, static_context_t>&);

  CompilerCB(const CompilerCB&);

  virtual ~CompilerCB();

  bool isLoadPrologQuery() const { return theIsLoadProlog; }

  void setLoadPrologQuery() { theIsLoadProlog = true; }

  static_context* getStaticContext(short c);
};


}

#endif

/* vim:set ts=2 sw=2: */
/*
 * Local variables:
 * mode: c++
 * End:
 */
