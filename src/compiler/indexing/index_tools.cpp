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
#include <vector>
#include <utility>
#include "compiler/indexing/index_tools.h"
#include "system/globalenv.h"
#include "store/api/item_factory.h"

namespace zorba {

#define LOOKUP_FN( pfx, local, arity ) (sCtx->lookup_fn (pfx, local, arity))

static bool isConstant(expr *e)
{
  if (e->get_expr_kind() == var_expr_kind) {
    return false;
  }
  for(expr_iterator i = e->expr_begin(); !i.done(); ++i) {
    if (!isConstant((*i).getp())) {
      return false;
    }
  }
  return true;
}

static bool isHoistableCollection(expr *e, static_context *sCtx)
{
  if (e->get_expr_kind() != fo_expr_kind) {
    return false;
  }
  fo_expr *fo = static_cast<fo_expr *>(e);
  if (fo->get_func() != LOOKUP_FN("fn", "collection", 1)) {
    return false;
  }
  expr *arg = (*fo)[0].getp();
  return isConstant(arg);
}

static xqpStringStore *getCollectionName(expr *e)
{
  fo_expr *fo = static_cast<fo_expr *>(e);
  const_expr *arg = static_cast<const_expr *>((*fo)[0].getp());
  store::Item *val = arg->get_val();
  return val->getStringValueP();
}

typedef std::vector<std::pair<expr_t, expr_t> > hoisted_collections_t;

static rchandle<var_expr> createTempLetVar(const QueryLoc& loc, int counter)
{
  std::stringstream ss;
  ss << "$$opt_temp_" << (counter);
  std::string varname = ss.str();
  store::Item_t qname;
  GENV_ITEMFACTORY->createQName(qname, "", "", varname.c_str());
  rchandle<var_expr> var = new var_expr(loc, var_expr::let_var, qname);

  return var;
}

static expr_t hoistCollectionSources(expr_t e, static_context *sCtx, hoisted_collections_t& h, int& count)
{
  if (isHoistableCollection(e.getp(), sCtx)) {
    rchandle<var_expr> var(createTempLetVar(e->get_loc(), count));
    std::pair<expr_t, expr_t> p(var.getp(), e);
    h.push_back(p);
    return new wrapper_expr(e->get_loc(), var.getp());
  }
  expr_iterator i = e->expr_begin();
  while(!i.done()) {
    *i = hoistCollectionSources(*i, sCtx, h, count);
    ++i;
  }
  return e;
}

/*
 * This function tries to see if the given expression is a map
 * in the given variable. If this returns true, then it is
 * guaranteed to be a map. If it returns false, it may still
 * be a map, but this algorithm could not determine that.
 * It assumes that the variable occurs atmost once.
 */
static bool isMapInVar(expr *e, var_expr *var)
{
  if (e == var) {
    return true;
  }
  switch(e->get_expr_kind()) {
    case wrapper_expr_kind:
      return isMapInVar(static_cast<wrapper_expr *>(e)->get_expr().getp(), var);

    case flwor_expr_kind: {
      flwor_expr *flwor = static_cast<flwor_expr *>(e);
      for(int i = static_cast<int>(flwor->num_clauses()) - 1; i >= 0; --i) {
        flwor_clause *clause = (*flwor)[i];
        bool isMap = false;
        switch(clause->get_kind()) {
          case flwor_clause::for_clause:
            isMap = isMap || isMapInVar(static_cast<for_clause *>(clause)->get_expr(), var);

          case flwor_clause::let_clause:
          case flwor_clause::where_clause:
          case flwor_clause::order_clause:
            continue;

          default:
            return false;
        }
        return isMap;
      }
    }

    case relpath_expr_kind: {
      relpath_expr *re = static_cast<relpath_expr *>(e);
      return isMapInVar((*re)[0], var);
    }

    default:
      break;
  }
  return false;
}

void IndexTools::inferIndexCreators(ValueIndex *vi)
{
  expr *de = vi->getDomainExpression();
  expr_t ne = de->clone();
  hoisted_collections_t h;
  int count = 0;
  expr_t he = hoistCollectionSources(ne, vi->getStaticContext(), h, count);
  if (h.size() > 1) {
    return;
  }
  /*

    Commenting out for now. But these are the things that need to happen here:
      0. Create an expression that is of the following form:
        let $t := $externalVar
        for $de in $t
        let $k1 := key1($de)
        let $k2 := key1($de)
        .
        .
        .
        return createTuple($k1, $k2, ... $de)

      1. For each substituted variable that the DE is a map in, replace it
      with an external variable
      2. Create an IndexCreator that is capable of evaluating this expression
      with the external variable set to the given item in the appendIndexEntries() call.
      3. For each tuple that is returned by this plan, create an IndexEntry containing
      the contents of the tuple.



  expr_t e = new flwor_expr(de->get_loc(), true);
  flwor_expr *f = static_cast<flwor_expr *>(e.getp());
  for(hoisted_collections_t::iterator i = h.begin(); i != h.end(); ++i) {
    var_expr *var = static_cast<var_expr *>(i->first.getp());
    expr *ce = i->second.getp();
    xpqStringStore *cName = getCollectionName(ce);

    let_clause_t lc = new let_clause(i->first->get_loc(), var, i->second.getp());
    var->set_flwor_clause(lc.getp());

    f->add_clause(lc.getp());
  }
  f->set_return_expr(he.getp());
  */
}

}
