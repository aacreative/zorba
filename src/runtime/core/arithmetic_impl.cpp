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

#include "diagnostics/xquery_diagnostics.h"
#include "diagnostics/assert.h"
#include "diagnostics/dict.h"
#include "diagnostics/util_macros.h"

#include "zorbatypes/datetime.h"
#include "zorbatypes/decimal.h"
#include "zorbatypes/duration.h"

#include "system/globalenv.h"

#include "context/dynamic_context.h"
#include "context/static_context.h"

#include "types/root_typemanager.h"
#include "types/typeops.h"
#include "types/casting.h"

#include "compiler/parser/query_loc.h"
#include "compiler/api/compilercb.h"

#include "runtime/core/arithmetic_impl.h"
#include "runtime/numerics/NumericsImpl.h"
#include "runtime/visitors/planiter_visitor.h"

#include "store/api/item_factory.h"

#include <zorba/internal/unique_ptr.h>

namespace zorba {

SERIALIZABLE_TEMPLATE_INSTANCE(GenericArithIterator,
                               GenericArithIterator<AddOperation>,
                               1)

SERIALIZABLE_TEMPLATE_INSTANCE(GenericArithIterator,
                               GenericArithIterator<SubtractOperation>,
                               2)

SERIALIZABLE_TEMPLATE_INSTANCE(GenericArithIterator,
                               GenericArithIterator<MultiplyOperation>,
                               3)

SERIALIZABLE_TEMPLATE_INSTANCE(GenericArithIterator,
                               GenericArithIterator<DivideOperation>,
                               4)

SERIALIZABLE_TEMPLATE_INSTANCE(GenericArithIterator,
                               GenericArithIterator<IntegerDivideOperation>,
                               5)

SERIALIZABLE_TEMPLATE_INSTANCE(GenericArithIterator,
                               GenericArithIterator<ModOperation>,
                               6)

void ArithOperationsCommons::createError(
    const TypeManager* tm,
    const char* aOp, 
    const QueryLoc* aLoc, 
    store::SchemaTypeCode aType0,
    store::SchemaTypeCode aType1)
{
  xqtref_t t0 = tm->create_builtin_atomic_type(aType0,SequenceType::QUANT_ONE);
  xqtref_t t1 = tm->create_builtin_atomic_type(aType1,SequenceType::QUANT_ONE);

  RAISE_ERROR(err::XPTY0004, aLoc,
  ERROR_PARAMS(ZED(OperationNotPossibleWithTypes_234), aOp, *t0, *t1 ));
}



/*******************************************************************************
  GenericArithIterator
********************************************************************************/

template< class Operations>
GenericArithIterator<Operations>::GenericArithIterator(
    static_context* sctx, 
    const QueryLoc& loc,
    PlanIter_t& iter0,
    PlanIter_t& iter1)
    :
    BinaryBaseIterator<GenericArithIterator<Operations>, PlanIteratorState >(sctx, loc, iter0, iter1)
{ 
}


template < class Operation >
bool GenericArithIterator<Operation>::nextImpl(
    store::Item_t& result,
    PlanState& planState) const
{
  store::Item_t n0;
  store::Item_t n1;
  bool status;
  
  PlanIteratorState* state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  if (this->consumeNext(n0, this->theChild0.getp(), planState))
  {
    if (this->consumeNext(n1, this->theChild1.getp(), planState))
    {
      status = compute(result,
                       planState.theLocalDynCtx,
                       this->theSctx->get_typemanager(),
                       this->loc,
                       n0,
                       n1);
      /*
      if (this->consumeNext(n0, this->theChild0.getp(), planState) ||
          this->consumeNext(n1, this->theChild1.getp(), planState))
      {
        throw XQUERY_EXCEPTION(
          err::XPTY0004,
          ERROR_PARAMS( ZED( NoSeqAsArithOp ) ),
          ERROR_LOC( this->loc )
        );
      }
      */
      STACK_PUSH ( status, state );
    }
  }

  STACK_END (state);
}


template <class Operation>
void GenericArithIterator<Operation>::accept(PlanIterVisitor& v) const 
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  this->theChild0->accept(v);
  this->theChild1->accept(v);

  v.endVisit(*this);
}


template < class Operation >
bool GenericArithIterator<Operation>::compute(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc& aLoc, 
    store::Item_t& n0,
    store::Item_t& n1)
{
  RootTypeManager& rtm = GENV_TYPESYSTEM;

  assert(n0->isAtomic());
  assert(n1->isAtomic());

  store::SchemaTypeCode type0 = n0->getTypeCode();
  store::SchemaTypeCode type1 = n1->getTypeCode();
  
  if (TypeOps::is_numeric(type0) &&
      (TypeOps::is_subtype(type1, store::XS_YM_DURATION) ||
       TypeOps::is_subtype(type1, store::XS_DT_DURATION)))
  {
    GenericCast::castToAtomic(n0, n0, &*rtm.DOUBLE_TYPE_ONE, tm, NULL, aLoc);

    if (TypeOps::is_subtype(type1, store::XS_YM_DURATION))
    {
      return Operation::template
             compute<store::XS_DOUBLE, store::XS_YM_DURATION>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else
    {
      return Operation::template
             compute<store::XS_DOUBLE,store::XS_DT_DURATION>
             (result, dctx, tm, &aLoc, n0, n1);
    }
  }
  else if (TypeOps::is_subtype(type0, store::XS_DT_DURATION) &&
           TypeOps::is_subtype(type1, store::XS_TIME))
  {
    return Operation::template
           compute<store::XS_DURATION,store::XS_TIME>
           (result, dctx, tm, &aLoc, n0, n1);
  }
  else if (TypeOps::is_subtype(type0, store::XS_YM_DURATION))
  {
    if (TypeOps::is_numeric(type1))
    {
      GenericCast::castToAtomic(n1, n1, &*rtm.DOUBLE_TYPE_ONE, tm, NULL, aLoc);
      return Operation::template
             compute<store::XS_YM_DURATION,store::XS_DOUBLE>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_DATETIME))
    {
      return Operation::template
             compute<store::XS_DURATION,store::XS_DATETIME>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_DATE))
    {
      return Operation::template
             compute<store::XS_DURATION,store::XS_DATE>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (type0 == type1)
    {
      return Operation::template
      computeSingleType<store::XS_YM_DURATION>
      (result, dctx, tm, &aLoc, n0, n1);
    }
  }
  else if (TypeOps::is_subtype(type0, store::XS_DT_DURATION))
  {
    if (TypeOps::is_numeric(type1))
    {
      GenericCast::castToAtomic(n1, n1, &*rtm.DOUBLE_TYPE_ONE, tm, NULL, aLoc);

      return Operation::template
             compute<store::XS_DT_DURATION,store::XS_DOUBLE>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_DATETIME))
    {
      return Operation::template 
             compute<store::XS_DURATION,store::XS_DATETIME>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_DATE))
    {
      return Operation::template
             compute<store::XS_DURATION,store::XS_DATE>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (type0 == type1)
    {
      return Operation::template
             computeSingleType<store::XS_DT_DURATION>
             (result, dctx, tm, &aLoc, n0, n1);
    }
  }
  else if (TypeOps::is_subtype(type0, store::XS_DATETIME))
  {
    if (TypeOps::is_subtype(type1, store::XS_DATETIME))
    {
      return Operation::template
             compute<store::XS_DATETIME,store::XS_DATETIME>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_YM_DURATION))
    {
      return Operation::template
             compute<store::XS_DATETIME,store::XS_DURATION>
            (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_DT_DURATION))
    {
      return Operation::template
             compute<store::XS_DATETIME,store::XS_DURATION>
            (result, dctx, tm, &aLoc, n0, n1);
    }
  }
  else if (TypeOps::is_subtype(type0, store::XS_DATE))
  {
    if (TypeOps::is_subtype(type1, store::XS_DATE))
    {
      return Operation::template
             compute<store::XS_DATE,store::XS_DATE>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_YM_DURATION))
    {
      return Operation::template
             compute<store::XS_DATE,store::XS_DURATION>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_DT_DURATION))
    {
      return Operation::template
             compute<store::XS_DATE,store::XS_DURATION>
             (result, dctx, tm, &aLoc, n0, n1);
    }
  }
  else if (TypeOps::is_subtype(type0, store::XS_TIME))
  {
    if (TypeOps::is_subtype(type1, store::XS_TIME))
    {
      return Operation::template
             compute<store::XS_TIME,store::XS_TIME>
             (result, dctx, tm, &aLoc, n0, n1);
    }
    else if (TypeOps::is_subtype(type1, store::XS_DT_DURATION))
    {
      return Operation::template 
             compute<store::XS_TIME,store::XS_DURATION>
             (result, dctx, tm, &aLoc, n0, n1);
    }
  }
  else if ((TypeOps::is_numeric(type0) || type0 == store::XS_UNTYPED_ATOMIC) &&
           (TypeOps::is_numeric(type1) || type1 == store::XS_UNTYPED_ATOMIC))
  {
    return NumArithIterator<Operation>::
           computeAtomic(result, dctx, tm, aLoc, n0, type0, n1, type1);
  }

  {  
    xqtref_t type0 = tm->create_value_type(n0);
    xqtref_t type1 = tm->create_value_type(n1);

    RAISE_ERROR(err::XPTY0004, aLoc,
    ERROR_PARAMS(ZED(ArithOpNotDefinedBetween_23), *type0, *type1));
  }

  return false; // suppresses wanring
}


/*******************************************************************************
  AddOperation 
********************************************************************************/

template<>
bool AddOperation::compute<store::XS_YM_DURATION,store::XS_YM_DURATION>
 ( store::Item_t& result,
   dynamic_context* dctx,
   const TypeManager* tm,
   const QueryLoc* loc,
   const store::Item* i0,
   const store::Item* i1 )
{
  std::unique_ptr<Duration> d(i0->getYearMonthDurationValue() + i1->getYearMonthDurationValue());
  return GENV_ITEMFACTORY->createYearMonthDuration(result, d.get());
}


template<>
bool AddOperation::compute<store::XS_DT_DURATION,store::XS_DT_DURATION>
( store::Item_t& result,
  dynamic_context* dctx,
  const TypeManager* tm,
  const QueryLoc* loc,
  const store::Item* i0,
  const store::Item* i1 )
{
  std::unique_ptr<Duration> d(i0->getDayTimeDurationValue() + i1->getDayTimeDurationValue());
  return GENV_ITEMFACTORY->createDayTimeDuration(result, d.get());
}


template<>
bool AddOperation::compute<store::XS_DATETIME,store::XS_DURATION>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<xs_dateTime> d(i0->getDateTimeValue().addDuration(i1->getDurationValue()));
  return GENV_ITEMFACTORY->createDateTime(result, d.get());
}


template<>
bool AddOperation::compute<store::XS_DURATION,store::XS_DATETIME>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<xs_dateTime> d(i1->getDateTimeValue().addDuration(i0->getDurationValue()));
  return GENV_ITEMFACTORY->createDateTime(result, d.get());
}


template<>
bool AddOperation::compute<store::XS_DATE,store::XS_DURATION>
( store::Item_t& result,
  dynamic_context* dctx,
  const TypeManager* tm,
  const QueryLoc* loc,
  const store::Item* i0,
  const store::Item* i1)
{
  std::unique_ptr<xs_date> d(i0->getDateValue().addDuration(i1->getDurationValue()));
  return GENV_ITEMFACTORY->createDate(result, d.get());
}


template<>
bool AddOperation::compute<store::XS_DURATION,store::XS_DATE>
( store::Item_t& result,
  dynamic_context* dctx,
  const TypeManager* tm,
  const QueryLoc* loc,
  const store::Item* i0,
  const store::Item* i1 )
{
  std::unique_ptr<xs_date> d(i1->getDateValue().addDuration(i0->getDurationValue()));
  return GENV_ITEMFACTORY->createDate (result, d.get());
}


template<>
bool AddOperation::compute<store::XS_TIME,store::XS_DURATION>
( store::Item_t& result,
  dynamic_context* dctx,
  const TypeManager* tm,
  const QueryLoc* loc,
  const store::Item* i0,
  const store::Item* i1 )
{
  std::unique_ptr<xs_time> t(i0->getTimeValue().addDuration(i1->getDurationValue()));
  return GENV_ITEMFACTORY->createTime (result, t.get());
}


template<>
bool AddOperation::compute<store::XS_DURATION,store::XS_TIME>
( store::Item_t& result,
  dynamic_context* dctx,
  const TypeManager* tm,
  const QueryLoc* loc,
  const store::Item* i0,
  const store::Item* i1 )
{
  std::unique_ptr<xs_time> t(i1->getTimeValue().addDuration(i0->getDurationValue()));
  return GENV_ITEMFACTORY->createTime (result, t.get());
}


/*******************************************************************************
  SubtractOperation
********************************************************************************/

template<>
bool SubtractOperation::compute<store::XS_YM_DURATION,store::XS_YM_DURATION>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1 )
{
  std::unique_ptr<Duration> d(i0->getYearMonthDurationValue() -
                            i1->getYearMonthDurationValue());

  return GENV_ITEMFACTORY->createYearMonthDuration(result, d.get());
}


template<>
bool SubtractOperation::compute<store::XS_DT_DURATION,store::XS_DT_DURATION>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<Duration> d(i0->getDayTimeDurationValue() -
                            i1->getDayTimeDurationValue());

  return GENV_ITEMFACTORY->createDayTimeDuration(result, d.get());
}


template<>
bool SubtractOperation::compute<store::XS_DATETIME,store::XS_DURATION>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<xs_dateTime> d(i0->getDateTimeValue().subtractDuration(i1->getDurationValue()));
  return GENV_ITEMFACTORY->createDateTime(result, d.get());
}


template<>
bool SubtractOperation::compute<store::XS_DATE,store::XS_DURATION>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<xs_date> d(i0->getDateValue().subtractDuration(i1->getDurationValue()));
  return GENV_ITEMFACTORY->createDate(result, d.get());
}


template<>
bool SubtractOperation::compute<store::XS_TIME,store::XS_DURATION>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<xs_time> t(i0->getTimeValue().subtractDuration(i1->getDurationValue()));
  return GENV_ITEMFACTORY->createTime(result, t.get());
}


template<>
bool SubtractOperation::compute<store::XS_DATETIME,store::XS_DATETIME>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<Duration> d;
  try 
  {
    d.reset(i0->getDateTimeValue().subtractDateTime(&i1->getDateTimeValue(),
                                                    dctx->get_implicit_timezone()));
  }
  catch (InvalidTimezoneException const &e) 
  {
    throw XQUERY_EXCEPTION(err::FODT0003, ERROR_PARAMS(e.get_tz_seconds()));
  }
  return GENV_ITEMFACTORY->createDayTimeDuration(result, d.get());
}


template<>
bool SubtractOperation::compute<store::XS_DATE,store::XS_DATE>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<Duration> d;
  try 
  {
    d.reset(i0->getTimeValue().subtractDateTime(&i1->getTimeValue(),
                                                dctx->get_implicit_timezone()));
  }
  catch (InvalidTimezoneException const &e)
  {
    throw XQUERY_EXCEPTION(err::FODT0003, ERROR_PARAMS(e.get_tz_seconds()));
  }
  return GENV_ITEMFACTORY->createDayTimeDuration(result, d.get());
}


template<>
bool SubtractOperation::compute<store::XS_TIME,store::XS_TIME>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<Duration> d;
  try 
  {
    d.reset(i0->getTimeValue().subtractDateTime(&i1->getTimeValue(),
                                                dctx->get_implicit_timezone()));
  }
  catch (InvalidTimezoneException const &e)
  {
    throw XQUERY_EXCEPTION(err::FODT0003, ERROR_PARAMS(e.get_tz_seconds()));
  }
  return GENV_ITEMFACTORY->createDayTimeDuration(result, d.get());
}


/*******************************************************************************
  MultiplyOperation
********************************************************************************/

template<>
bool MultiplyOperation::compute<store::XS_YM_DURATION,store::XS_DOUBLE>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<Duration> d;
  
  if ( i1->getDoubleValue().isPosInf() || i1->getDoubleValue().isNegInf() )
    throw XQUERY_EXCEPTION( err::FODT0002, ERROR_LOC( loc ) );
  else if (i1->getDoubleValue().isNaN())
    throw XQUERY_EXCEPTION( err::FOCA0005, ERROR_LOC( loc ) );
  else try {
    d.reset(i0->getYearMonthDurationValue() * (i1->getDoubleValue()));
  } catch (XQueryException& e) {
    set_source(e, *loc);
    throw;
  }
  
  return GENV_ITEMFACTORY->createYearMonthDuration(result, d.get());
}


template<>
bool MultiplyOperation::compute<store::XS_DT_DURATION,store::XS_DOUBLE>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<Duration> d;
  
  if ( i1->getDoubleValue().isPosInf() || i1->getDoubleValue().isNegInf() )
    throw XQUERY_EXCEPTION( err::FODT0002, ERROR_LOC( loc ) );
  else if (i1->getDoubleValue().isNaN())
    throw XQUERY_EXCEPTION( err::FOCA0005, ERROR_LOC( loc ) );
  else try {
    d.reset(i0->getDayTimeDurationValue() * (i1->getDoubleValue()));
  } catch (XQueryException& e) {
    set_source(e, *loc);
    throw;
  }
  
  return GENV_ITEMFACTORY->createDayTimeDuration(result, d.get());
}


template<>
bool MultiplyOperation::compute<store::XS_DOUBLE,store::XS_YM_DURATION>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  return MultiplyOperation::compute<store::XS_YM_DURATION,store::XS_DOUBLE>(result, dctx, tm, loc, i1, i0);
}


template<>
bool MultiplyOperation::compute<store::XS_DOUBLE,store::XS_DT_DURATION>
( store::Item_t& result,
  dynamic_context* dctx,
  const TypeManager* tm,
  const QueryLoc* loc,
  const store::Item* i0,
  const store::Item* i1 )
{
  return MultiplyOperation::compute<store::XS_DT_DURATION,store::XS_DOUBLE>(result, dctx, tm, loc, i1, i0);
}


/*******************************************************************************
  DivideOperation
********************************************************************************/

template<>
bool DivideOperation::compute<store::XS_YM_DURATION,store::XS_DOUBLE>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1)
{
  std::unique_ptr<Duration> d;

  if( i1->getDoubleValue().isPosInf() || i1->getDoubleValue().isNegInf() )
  {
    d = std::unique_ptr<Duration>(new Duration(Duration::YEARMONTHDURATION_FACET));
  }
  else if ( i1->getDoubleValue().isZero() )
    throw XQUERY_EXCEPTION( err::FODT0002, ERROR_LOC( loc ) );
  else if ( i1->getDoubleValue().isNaN() )
    throw XQUERY_EXCEPTION( err::FOCA0005, ERROR_LOC( loc ) );
  else try {
    d = std::unique_ptr<Duration>(i0->getYearMonthDurationValue() / i1->getDoubleValue());
  } catch (XQueryException& e) {
    set_source(e, *loc);
    throw;
  }

  return GENV_ITEMFACTORY->createYearMonthDuration(result, d.get());
}


template<>
bool DivideOperation::compute<store::XS_DT_DURATION,store::XS_DOUBLE>
( store::Item_t& result,
  dynamic_context* dctx,
  const TypeManager* tm,
  const QueryLoc* loc,
  const store::Item* i0,
  const store::Item* i1 )
{
  std::unique_ptr<Duration> d;

  if( i1->getDoubleValue().isPosInf() || i1->getDoubleValue().isNegInf() )
  {
    d.reset(new Duration());
  }
  else if ( i1->getDoubleValue().isZero() )
    throw XQUERY_EXCEPTION( err::FODT0002, ERROR_LOC( loc ) );
  else if ( i1->getDoubleValue().isNaN() )
    throw XQUERY_EXCEPTION( err::FOCA0005, ERROR_LOC( loc ) );
  else try {
    d.reset(i0->getDayTimeDurationValue() / i1->getDoubleValue());
  } catch (XQueryException& e) {
    set_source(e, *loc);
    throw;
  }

  return GENV_ITEMFACTORY->createDayTimeDuration(result, d.get());
}


template<>
bool DivideOperation::compute<store::XS_YM_DURATION, store::XS_YM_DURATION>
( store::Item_t& result,
  dynamic_context* dctx,
  const TypeManager* tm,
  const QueryLoc* loc,
  const store::Item* i0,
  const store::Item* i1 )
{
  xs_yearMonthDuration otherYMDuration = i1->getYearMonthDurationValue();
  if (otherYMDuration.isZero())
    throw XQUERY_EXCEPTION( err::FOAR0001, ERROR_LOC( loc ) );  
  xs_decimal d = i0->getYearMonthDurationValue() / otherYMDuration;
  return GENV_ITEMFACTORY->createDecimal(result, d);
}


template<>
bool DivideOperation::compute<store::XS_DT_DURATION, store::XS_DT_DURATION>(
    store::Item_t& result,
    dynamic_context* dctx,
    const TypeManager* tm,
    const QueryLoc* loc,
    const store::Item* i0,
    const store::Item* i1 )
{
  xs_dayTimeDuration otherDTDuration = i1->getDayTimeDurationValue();
  if (otherDTDuration.isZero())  
      throw XQUERY_EXCEPTION( err::FOAR0001, ERROR_LOC(loc));
  xs_decimal d = i0->getDayTimeDurationValue() / otherDTDuration;
  return GENV_ITEMFACTORY->createDecimal(result, d);
}



/*******************************************************************************
  instantiate GenericArithIterator for all kinds of arithmetic operators
********************************************************************************/
template class GenericArithIterator<AddOperation>;
template class GenericArithIterator<SubtractOperation>;
template class GenericArithIterator<MultiplyOperation>;
template class GenericArithIterator<DivideOperation>;
template class GenericArithIterator<IntegerDivideOperation>;
template class GenericArithIterator<ModOperation>;

} // namespace zorba
/* vim:set et sw=2 ts=2: */
