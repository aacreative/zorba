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
#pragma once
#ifndef ZORBA_SERIALIZER_H
#define ZORBA_SERIALIZER_H

#include <vector>

#include <zorba/sax2.h>
#include <zorba/options.h>

#include "zorbatypes/schema_types.h"

#include "common/shared_types.h"

#include "store/api/item.h"


namespace zorba
{

typedef Zorba_SerializerOptions_t* (*itemHandler)(void* aUserData);


namespace store
{
  class ChildrenIterator;
  class AttributesIterator;
}


namespace intern
{
  class Serializable;
}


/*******************************************************************************

********************************************************************************/
class serializer : public SimpleRCObject
{
protected:
  class emitter;

public:
  typedef enum
  {
    PARAMETER_VALUE_NO,
    PARAMETER_VALUE_YES,
    PARAMETER_VALUE_OMIT,

    PARAMETER_VALUE_XML,
    PARAMETER_VALUE_HTML,
    PARAMETER_VALUE_XHTML,
    PARAMETER_VALUE_TEXT,
    PARAMETER_VALUE_BINARY,
    PARAMETER_VALUE_JSON,
    PARAMETER_VALUE_JSON_XML_HYBRID,

    // Values for the XML/HTML version
    PARAMETER_VALUE_VERSION_1_0,          // used for XML 1.0
    PARAMETER_VALUE_VERSION_1_1,          // used for XML 1.1
    PARAMETER_VALUE_VERSION_4_0,          // used for HTML 4.0
    PARAMETER_VALUE_VERSION_4_01,         // used for HTML 4.01
    PARAMETER_VALUE_VERSION_OTHER         // given by the version string
    
  } PARAMETER_VALUE_TYPE;

protected:
  static const char	END_OF_LINE;

protected:
  XQueryDiagnostics* theXQueryDiagnostics;

  // Serialization parameters
  short int byte_order_mark;       // "yes" or "no", implemented
  zstring cdata_section_elements;  // list of expanded QNames, delimited by spaces
                                   // or semicolons, implemented
  zstring doctype_system;          // string, implemented
  zstring doctype_public;          // string, implemented
  zstring encoding;
  short int escape_uri_attributes; // TODO: yes/no requires unicode normalization
  short int include_content_type;  // yes/no, implemented
  zstring item_separator;
  bool item_separator_is_set;
  zstring media_type;              // string, implemented
  short int method;                // an expanded QName: "xml", "html", "xhtml",
                                   // "text" and "binary"
                                   // are handled
  zstring normalization_form;      // TODO:   requires unicode normalization
  short int omit_xml_declaration;  // "yes" or "no", implemented
  short int standalone;            // implemented
  short int undeclare_prefixes;    // "yes" or "no", implemented
  void* use_character_maps;        // TODO: list of pairs
  short int version;               // "1.0"
  zstring version_string;          // this the version as a string
  short int indent;                // "yes" or "no", implemented
  short int jsoniq_multiple_items;  // "no", "yes", implemented
  short int jsoniq_xdm_method;      // A legal value for "method", implemented
  bool version_has_default_value;  // Used during validation to set version to
                                   // "4.0" when output method is "html"
  rchandle<emitter>    e;
  std::ostream         *tr;

  // Used to hold the QNames of the cdata section elements after they have been tokenized
  std::vector<zstring> cdata_section_elements_tokens;

public:
  serializer(XQueryDiagnostics*);

  virtual ~serializer();

  /**
   * Serializes the object that implements the Serializale interface to the
   * output stream.
   *
   * @param object The serializable object that provides a sequence
   *        to be serialized.
   * @param stream The stream to serialize to.
   * @param aEmitAttributeValue If true, attributes are emitted.
   */
  void serialize(
      store::Iterator_t object, 
      std::ostream& stream, 
      bool aEmitAttributes = false);

  void serialize(
        store::Iterator_t object,
        std::ostream& stream,
        itemHandler aHandler,
        void* aHandlerData);

  /**
   * Serializes the object that implements the Serializale interface to the
   * output stream by issuing SAX event notification.
   *
   * @param object The serializable object that provides a sequence
   *        to be serialized.
   * @param stream The stream to serialize to.
   * @param handler The SAX handler.
   * @param aEmitAttributes If true, attributes are emitted.
   */
  void serialize(
        store::Iterator_t     object,
        std::ostream&         stream,
        SAX2_ContentHandler*  handler,
        bool aEmitAttributes = false);

  /**
   * Set the serializer's parameters. The list of handled parameters
   * is to be detailed.
   *
   * @param name The name of the parameter to be set.
   * @param value The value of the parameter.
   */
  void setParameter(const char* name, const char* value);

  /**
   * Get the serialization method.
   *
   * @return A value from the PARAMETER_VALUE_TYPE enum.
   */
  short getSerializationMethod() const;

  /**
   * Get the serialization method as a string
   *
   * @return the value of the serialization method as string
   */
  void getSerializationMethod(zstring&) const;

protected:
  void reset();

  void validate_parameters();

  bool setup(std::ostream& os, bool aEmitAttributes = false);

  void attach_transcoder(std::ostream& os);

  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class emitter                                        //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class emitter : public SimpleRCObject
  {
  protected:
    serializer                          * ser;
    std::ostream                        & tr;
    std::vector<store::NsBindings>        theBindings;

    enum ItemState
    {
      INVALID_ITEM,
      PREVIOUS_ITEM_WAS_TEXT,
      PREVIOUS_ITEM_WAS_NODE
    }                                     thePreviousItemKind;

    std::vector<store::ChildrenIterator*> theChildIters;
    ulong                                 theFirstFreeChildIter;
    store::AttributesIterator           * theAttrIter;

    bool                                  theIsFirstElementNode;
    bool                                  theEmitAttributes;

  public:
    virtual ~emitter();

    /**
     * Creates a new emitter object.
     *
     * @param the_serializer The parent serializer object.
     * @param the_stream Target output stream.
     * @param aEmitAttributes If true, attributes are emitted.
     */
    emitter(
        serializer* serializer, 
        std::ostream& stream,
        bool emitAttributes = false);

    /**
     * Outputs the start of the serialized document, which, depending on
     * the serialization method, can be the XML declaration, the HTML
     * declaration, etc.
     */
    virtual void emit_declaration();

    /**
     * Outputs the end of the serialized document.
     */
    virtual void emit_end();

    /**
     * Serializes the given item, depending on its type, and its children. This
     * will be called by serializer for each top-level item in the sequence.
     *
     * @param item the item to serialize
     */
    virtual void emit_item(store::Item* item);

    ItemState getPreviousItemKind() const { return thePreviousItemKind; }

    void setPreviousItemKind(ItemState v) { thePreviousItemKind = v; }

    // End of the "public" emitter API. All remaining methods are implementation
    // details and will not be called from outside.

  protected:
    /**
     * Outputs the doctype declaration. This function is not used by the
     * default emitter, it is intended to be defined by the XML, HTML and XHTML
     * serializers.
     */
    virtual void emit_doctype(const zstring& elementName);

    /**
     * Serializes the given streamable item.
     *
     * @param item the item to serialize
     */
    virtual void emit_streamable_item(store::Item* item);

    /**
     *  The root function that performs the serialization
     *  of a normalized sequence.
     */
    virtual void emit_node(const store::Item* item, int depth);

    /**
     *  Serializes a given text node. Also performs the processing of
     *  cdata-section-elements parameter, if set.
     */
    void emit_text_node(const store::Item* item, const zstring& text);

    /**
     *  Serializes the children of the given node, without the node itself.
     *
     *  @return  returns 1 if the functions has closed parent's tag with ">"
     */
    virtual int emit_node_children(
          const store::Item* item,
          int depth,
          bool perform_escaping = true);

    /**
     *  Serializes the namespace bindings, if any.
     *
     *  @return  returns true if the function has added a set of bindings to the list,
     *           which should be removed when the elememnt is out of the scope.
     */
    virtual bool emit_bindings(const store::Item* item, int depth);

    /**
     *  Serializes the given string, performing character expansion
     *  if necessary.
     *
     *  @return returns the number of bytes that have not been written. This
     *          info is used only for streamable items expansion.
     */
    virtual int emit_expanded_string(
            const char* str,
            zstring::size_type strlen,
            bool emit_attribute_value);

    /**
     * Outputs indentation whitespace, depending on depth.
     *
     * @param depth the level of indentation
     */
    void emit_indentation(int depth);

    bool haveBinding(std::pair<zstring, zstring>& nsBinding) const;

    bool havePrefix(const zstring& pre) const;

    store::ChildrenIterator* getChildIter();

    void releaseChildIter(store::ChildrenIterator* iter);

    store::AttributesIterator* getAttrIter();

    void releaseAttrIter(store::AttributesIterator* iter);
  };


  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class xml_emitter                                    //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class xml_emitter : public emitter
  {
  public:
    xml_emitter(
        serializer* the_serializer, 
        std::ostream& the_stream, 
        bool aEmitAttributes = false
    );

    virtual void emit_declaration();

  protected:
    virtual void emit_doctype(const zstring& elementName);
  };

  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class json_emitter                                   //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class json_emitter : public emitter
  {
  public:
    json_emitter(serializer* the_serializer, std::ostream& the_stream);

    virtual ~json_emitter();

    virtual void emit_declaration();

    virtual void emit_item(store::Item *item);

    virtual void emit_end();

  protected:

    /**
     * Outputs a JSON item. This method is called both for top-level JSON
     * items as well as any items within a JSON object or array, so it may
     * output simple typed values differently than standard XML serialization.
     */
    virtual void emit_json_item(store::Item* item, int depth);

    virtual void emit_json_object(store::Item* object, int depth);

    virtual void emit_json_array(store::Item* array, int depth);

    virtual void emit_jsoniq_xdm_node(store::Item *item, int depth);

    virtual void emit_json_string(zstring const &string);

    store::Item_t theJSONiqValueName;
    store::Item_t theTypeName;
    store::Item_t theValueName;
    store::Item_t theJSONiqXDMNodeName;

    bool          theMultipleItems;
  };


  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class hybrid_emitter (auto-detects JSON or XML)      //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class hybrid_emitter : public json_emitter
  {
  public:
    hybrid_emitter(
        serializer* the_serializer, 
        std::ostream& the_stream,
        bool aEmitAttributes = false);

    virtual ~hybrid_emitter();

    virtual void emit_declaration();

    virtual void emit_item(store::Item *item);

    virtual void emit_end();

  protected:
    virtual void emit_jsoniq_xdm_node(store::Item* item, int);

  private:
    enum JSONiqEmitterState
    {
      JESTATE_UNDETERMINED,
      JESTATE_JDM,
      JESTATE_XDM
    }                         theEmitterState;

    serializer::xml_emitter * theXMLEmitter;

    rchandle<emitter>         theNestedXMLEmitter;
    std::stringstream       * theNestedXMLStringStream;
  };


  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class xhtml_emitter                                  //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class xhtml_emitter : public xml_emitter
  {
  public:
    xhtml_emitter(serializer* the_serializer, std::ostream& the_stream);

  protected:
    virtual void emit_node(const store::Item* item, int depth);
  };


  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class html_emitter                                   //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class html_emitter : public emitter
  {
  public:
    html_emitter(serializer* the_serializer, std::ostream& the_stream);

    virtual void emit_declaration();
    virtual void emit_end();

  protected:
    virtual void emit_doctype(const zstring& elementName);
    virtual void emit_node(const store::Item* item, int depth);
  };


  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class text_emitter                                   //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class text_emitter : public emitter
  {
  public:
    text_emitter(serializer* the_serializer, std::ostream& the_stream);

    virtual void emit_declaration();

    virtual void emit_item(store::Item* item);

  protected:
    virtual void emit_node(const store::Item* item, int depth);

    virtual int emit_node_children(
        const store::Item* item,
        int depth,
        bool perform_escaping = true);

    virtual void emit_streamable_item(store::Item* item);
  };


  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class sax2_emitter                                   //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class sax2_emitter : public emitter
  {
  protected:
    SAX2_ContentHandler * theSAX2ContentHandler;
    SAX2_LexicalHandler * theSAX2LexicalHandler;
    store::NsBindings     theNameSpaces;
    std::stringstream&    theSStream;

  public:
    sax2_emitter(
          serializer* the_serializer,
          std::ostream& the_stream,
          std::stringstream& aSStream,
          SAX2_ContentHandler* aSAX2ContentHandler);

    void emit_declaration();
    void emit_item(store::Item* item );
    void emit_end();

  protected:
    void emit_startPrefixMapping(
          const store::Item* item,
          store::NsBindings& nsBindings );

    void emit_endPrefixMapping(store::NsBindings& nsBindings );

    void emit_node(const store::Item* item, int depth);

    void emit_node(const store::Item* item);

    int emit_expanded_string(
          const char* str,
          zstring::size_type strlen,
          bool aEmitAttributeValue = false );

    int emit_node_children(
          const store::Item* item,
          int depth,
          bool perform_escaping);

    void emit_node_children(const store::Item* item);

    bool emit_bindings(const store::Item* item, int depth);
  };


  ///////////////////////////////////////////////////////////
  //                                                       //
  //  class binary_emitter                                 //
  //                                                       //
  ///////////////////////////////////////////////////////////

  class binary_emitter : public emitter
  {
  public:
    binary_emitter(serializer* the_serializer, std::ostream& the_stream);

    void emit_item(store::Item* item);
  };
};


} // namespace zorba

#endif // #ifdef ZORBA_SERIALIZER_H

/*
 * Local variables:
 * mode: c++
 * End:
 */

/* vim:set et sw=2 ts=2: */
