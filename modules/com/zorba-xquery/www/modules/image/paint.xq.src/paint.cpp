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

#include "paint.h"
#include <list> 
#include <sstream>
#include <string>
#include <zorba/empty_sequence.h>
#include <zorba/singleton_item_sequence.h>
#include <zorba/zorba.h>
#include "paint_module.h"
#include "draw_in_c.h"


namespace zorba {  namespace imagemodule { namespace paintmodule {

using namespace zorba::imagemodule;  

//*****************************************************************************

DrawStrokedPolyLineFunction::DrawStrokedPolyLineFunction(const ImageModule* aModule) : ImageFunction(aModule)
{
}

ItemSequence_t
DrawStrokedPolyLineFunction::evaluate(
  const StatelessExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  Magick::Image lImage;
  ImageFunction::getOneImageArg(aArgs, 0, lImage);
  Magick::Blob lBlob;
  lImage.write(&lBlob); 
 
  double lStrokeLength = ImageFunction::getOneDoubleArg(aArgs, 3);
  double lGapLength = ImageFunction::getOneDoubleArg(aArgs, 4);
  double lPattern[] = {lStrokeLength, lGapLength};
  double lPatternLength;
  if (lGapLength == 0) {
    lPatternLength = 0;
  } else {
    lPatternLength = 2;
  }
 
  Item lXItem;
  Item lYItem;
  aArgs[1]->next(lXItem);
  aArgs[2]->next(lYItem);
  std::vector<double> lXValues;
  std::vector<double> lYValues;
  lXValues.push_back(lXItem.getDoubleValue());
  lYValues.push_back(lYItem.getDoubleValue());
  while (aArgs[1]->next(lXItem) && aArgs[2]->next(lYItem)) {
    lXValues.push_back(lXItem.getDoubleValue());
    lYValues.push_back(lYItem.getDoubleValue());
  }

  Item lArgsItem;
  std::string lStrokeColor;
  if (aArgs[5]->next(lArgsItem)) {
    lStrokeColor = lArgsItem.getStringValue().c_str();
  } else {
    lStrokeColor = "black";
  }

  double lStrokeWidth = ImageFunction::getStrokeWidthArg(aArgs, 6);
  bool lAntiAlias = ImageFunction::getAntiAliasingArg(aArgs, 7);
  // use C function to effectively draw the polygon
  long lBlobLength = (long) lBlob.length();
  void * lBlobPointer = DrawPolyLine(lBlob.data(), &lBlobLength, &lXValues[0], &lYValues[0], lXValues.size(), lStrokeColor, lStrokeWidth, lAntiAlias, lPattern, lPatternLength);
  Magick::Blob lBlobWithPolyLine(lBlobPointer, lBlobLength);
  // now read the blob back into an image to pass it back as encoded string 
  String lEncodedContent = ImageFunction::getEncodedStringFromBlob(lBlobWithPolyLine);
  Item lItem = theModule->getItemFactory()->createBase64Binary(lEncodedContent.c_str(), lEncodedContent.bytes());
  ImageFunction::checkIfItemIsNull(lItem);  
  return ItemSequence_t(new SingletonItemSequence(lItem));  
}

//*****************************************************************************

DrawRectangleFunction::DrawRectangleFunction(const ImageModule* aModule) : ImageFunction(aModule)
{
}


ItemSequence_t
DrawRectangleFunction::evaluate(
  const StatelessExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{

  Magick::Image lImage;
  ImageFunction::getOneImageArg(aArgs, 0, lImage);
  double lUpperLeftX = ImageFunction::getOneDoubleArg(aArgs, 1);
  double lUpperLeftY = ImageFunction::getOneDoubleArg(aArgs, 2);
  double lLowerRightX = ImageFunction::getOneDoubleArg(aArgs, 3);
  double lLowerRightY = ImageFunction::getOneDoubleArg(aArgs, 4);    


  // get stroke length and push it into poly-line
  Magick::ColorRGB lStrokeColor;             
  ImageFunction::getOneColorArg(aArgs, 5, lStrokeColor);
  lImage.strokeColor(lStrokeColor);
  Item lFillColorItem;                           
  if (aArgs[6]->next(lFillColorItem)) {         
    Magick::ColorRGB lFillColor;                 
    ImageFunction::getColorFromString(lFillColorItem.getStringValue(), lFillColor);
    lImage.fillColor(lFillColor); 
  } else {
    Magick::ColorRGB lFillColor ("white");
    lFillColor.alpha(1.0);
    lImage.fillColor(lFillColor);
 } 
  double lStrokeWidth = ImageFunction::getStrokeWidthArg(aArgs, 7);
  lImage.strokeWidth(lStrokeWidth); 
  bool lAntiAlias = ImageFunction::getAntiAliasingArg(aArgs, 8);
  lImage.strokeAntiAlias(lAntiAlias); 
  lImage.draw(Magick::DrawableRectangle(lUpperLeftX, lUpperLeftY, lLowerRightX, lLowerRightY));
  String lEncodedContent = ImageFunction::getEncodedStringFromImage(lImage);
  Item lItem = theModule->getItemFactory()->createBase64Binary(lEncodedContent.c_str(), lEncodedContent.bytes());
  ImageFunction::checkIfItemIsNull(lItem);
  return ItemSequence_t(new SingletonItemSequence(lItem));
}

//*****************************************************************************

DrawRoundedRectangleFunction::DrawRoundedRectangleFunction(const ImageModule* aModule) : ImageFunction(aModule)
{
}

ItemSequence_t
DrawRoundedRectangleFunction::evaluate(
  const StatelessExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{

  Magick::Image lImage;
  ImageFunction::getOneImageArg(aArgs, 0, lImage);
  double lUpperLeftX = ImageFunction::getOneDoubleArg(aArgs, 1);
  double lUpperLeftY = ImageFunction::getOneDoubleArg(aArgs, 2);
  double lLowerRightX = ImageFunction::getOneDoubleArg(aArgs, 3);
  double lLowerRightY = ImageFunction::getOneDoubleArg(aArgs, 4);
  double lCornderWidth = ImageFunction::getOneDoubleArg(aArgs, 5);
  double lCornerHeight = ImageFunction::getOneDoubleArg(aArgs, 6);

  // get stroke length and push it into poly-line
  Magick::ColorRGB lStrokeColor;             
  ImageFunction::getOneColorArg(aArgs, 7, lStrokeColor);
  lImage.strokeColor(lStrokeColor); 
  Item lFillColorItem;                           
  if (!aArgs[8]->next(lFillColorItem)) {         
    Magick::ColorRGB lFillColor ("white");
    lFillColor.alpha(1.0);
    lImage.fillColor(lFillColor);
  } else {                                       
    Magick::ColorRGB lFillColor;                 
    ImageFunction::getColorFromString(lFillColorItem.getStringValue(), lFillColor);
    lImage.fillColor(lFillColor); 
  } 


  double lStrokeWidth = ImageFunction::getStrokeWidthArg(aArgs, 9);
  lImage.strokeWidth(lStrokeWidth); 
  bool lAntiAlias = ImageFunction::getAntiAliasingArg(aArgs, 10);
  lImage.strokeAntiAlias(lAntiAlias); 
  lImage.draw(Magick::DrawableRoundRectangle(lUpperLeftX, lUpperLeftY, lLowerRightX, lLowerRightY, lCornderWidth, lCornerHeight));
  
  String lEncodedContent = ImageFunction::getEncodedStringFromImage(lImage);
  Item lItem = theModule->getItemFactory()->createBase64Binary(lEncodedContent.c_str(), lEncodedContent.bytes());
  ImageFunction::checkIfItemIsNull(lItem);
  return ItemSequence_t(new SingletonItemSequence(lItem));
}

//*****************************************************************************

DrawArcFunction::DrawArcFunction(const ImageModule* aModule) : ImageFunction(aModule)
{
}

ItemSequence_t
DrawArcFunction::evaluate(
  const StatelessExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{

  Magick::Image lImage;
  ImageFunction::getOneImageArg(aArgs, 0, lImage);
  double lXCoordinate = ImageFunction::getOneDoubleArg(aArgs, 1);
  double lYCoordinate = ImageFunction::getOneDoubleArg(aArgs, 2);
  double lPerimeterX = ImageFunction::getOneDoubleArg(aArgs, 3);
  double lPerimeterY = ImageFunction::getOneDoubleArg(aArgs, 4);
  double lStartDegrees = ImageFunction::getOneDoubleArg(aArgs, 5);
  double lEndDegrees = ImageFunction::getOneDoubleArg(aArgs, 6);
  // get stroke length and push it into poly-line
  Magick::ColorRGB lStrokeColor;             
  ImageFunction::getOneColorArg(aArgs, 7, lStrokeColor);
  lImage.strokeColor(lStrokeColor);
  Item lFillColorItem;                           
  if (!aArgs[8]->next(lFillColorItem)) {         
    Magick::ColorRGB lFillColor ("white");
    lFillColor.alpha(1.0);
    lImage.fillColor(lFillColor);
  } else {                                       
    Magick::ColorRGB lFillColor;                 
    ImageFunction::getColorFromString(lFillColorItem.getStringValue(), lFillColor);
    lImage.fillColor(lFillColor);   
  } 

  double lStrokeWidth = ImageFunction::getStrokeWidthArg(aArgs, 9);
  lImage.strokeWidth(lStrokeWidth); 
  bool lAntiAlias = ImageFunction::getAntiAliasingArg(aArgs, 10);
  lImage.strokeAntiAlias(lAntiAlias);
  lImage.draw(Magick::DrawableEllipse(lXCoordinate, lYCoordinate, lPerimeterX, lPerimeterY, lStartDegrees, lEndDegrees));
  
  String lEncodedContent = ImageFunction::getEncodedStringFromImage(lImage);
  Item lItem = theModule->getItemFactory()->createBase64Binary(lEncodedContent.c_str(), lEncodedContent.bytes());
  ImageFunction::checkIfItemIsNull(lItem);
  return ItemSequence_t(new SingletonItemSequence(lItem));
}

//*****************************************************************************

DrawPolygonFunction::DrawPolygonFunction(const ImageModule* aModule) : ImageFunction(aModule)
{
}

ItemSequence_t
DrawPolygonFunction::evaluate(
  const StatelessExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{
  
  Magick::Image lImage;
  ImageFunction::getOneImageArg(aArgs, 0, lImage);
  Magick::Blob lBlob;
  lImage.write(&lBlob);
  Item lXItem;
  Item lYItem;
  aArgs[1]->next(lXItem);
  aArgs[2]->next(lYItem);
  std::vector<double> lXValues;
  std::vector<double> lYValues;
  lXValues.push_back(lXItem.getDoubleValue());
  lYValues.push_back(lYItem.getDoubleValue());
  while (aArgs[1]->next(lXItem) && aArgs[2]->next(lYItem)) {
    lXValues.push_back(lXItem.getDoubleValue());
    lYValues.push_back(lYItem.getDoubleValue());
  }  
  Item lArgsItem;
  std::string lStrokeColor;
  if (aArgs[3]->next(lArgsItem)) {
    lStrokeColor = lArgsItem.getStringValue().c_str();
  } else {
    lStrokeColor = "black";
  }

  std::string lFillColor;
  if (aArgs[4]->next(lArgsItem)) {
    lFillColor = lArgsItem.getStringValue().c_str();
  } else {
    lFillColor = "opaque";
  }
  double lStrokeWidth = ImageFunction::getStrokeWidthArg(aArgs, 5);
  
  bool lAntiAlias = ImageFunction::getAntiAliasingArg(aArgs, 6);
    
  // use C function to effectively draw the polygon
  long lBlobLength = (long) lBlob.length();      
  void * lBlobPointer = DrawPolygon(lBlob.data(), &lBlobLength, &lXValues[0], &lYValues[0], lXValues.size(), lStrokeColor, lFillColor, lStrokeWidth, lAntiAlias);
  Magick::Blob lBlobWithPolygon(lBlobPointer, lBlobLength);
  // now read the blob back into an image to pass it back as encoded string 
  String lEncodedContent = ImageFunction::getEncodedStringFromBlob(lBlobWithPolygon);
  Item lItem = theModule->getItemFactory()->createBase64Binary(lEncodedContent.c_str(), lEncodedContent.bytes());
  ImageFunction::checkIfItemIsNull(lItem);
  return ItemSequence_t(new SingletonItemSequence(lItem));

}


//*****************************************************************************

DrawTextFunction::DrawTextFunction(const ImageModule* aModule) : ImageFunction(aModule)
{
}

ItemSequence_t
DrawTextFunction::evaluate(
  const StatelessExternalFunction::Arguments_t& aArgs,
  const StaticContext*                          aSctxCtx,
  const DynamicContext*                         aDynCtx) const
{

  Magick::Image lImage;
  ImageFunction::getOneImageArg(aArgs, 0, lImage);
  String lText = ImageFunction::getOneStringArg(aArgs, 1);
  double lXCoordinate = ImageFunction::getOneDoubleArg(aArgs, 2);
  double lYCoordinate = ImageFunction::getOneDoubleArg(aArgs, 3); 
  String lFontFamily = ImageFunction::getOneStringArg(aArgs, 4);
  

  double lFontSize = 12;
  int lFontWeight = 400;
  // get values that are possibly just an empty sequence
  
  Item lItem;
  if (aArgs[5]->next(lItem)) {
    lFontSize = lItem.getDoubleValue();
  }
  
  int lRed = 0;
  int lGreen = 0;
  int lBlue = 0;

  if (aArgs[6]->next(lItem)) {
    String lTmpString = lItem.getStringValue();
    sscanf(lTmpString.substring(1,2).c_str(), "%x", &lRed);
    sscanf(lTmpString.substring(3,2).c_str(), "%x", &lGreen);
    sscanf(lTmpString.substring(5,2).c_str(), "%x", &lBlue);
  }

  // push all values into a drawable
  lImage.strokeAntiAlias(false);
  lImage.strokeColor(Magick::ColorRGB((double)lRed/255.0, (double)lGreen/255.0, (double)lBlue/255.0));
  lImage.fillColor(Magick::ColorRGB((double)lRed/255.0, (double)lGreen/255.0, (double)lBlue/255.0));
  lImage.font(lFontFamily.c_str());
  lImage.fontPointsize(lFontWeight);

  lImage.draw(Magick::DrawableText(lXCoordinate, lYCoordinate, lText.c_str()));
  String lEncodedContent = ImageFunction::getEncodedStringFromImage(lImage);
  lItem = theModule->getItemFactory()->createBase64Binary(lEncodedContent.c_str(), lEncodedContent.bytes());
  ImageFunction::checkIfItemIsNull(lItem);
  return ItemSequence_t(new SingletonItemSequence(lItem));
}


} /* namespace paintmodule */  } /* namespace imagemodule */ }  /* namespace zorba */


#ifdef WIN32
#  define DLL_EXPORT __declspec(dllexport)
#else
#  define DLL_EXPORT __attribute__ ((visibility("default")))
#endif

extern "C" DLL_EXPORT zorba::ExternalModule* createModule() {
  return new zorba::imagemodule::paintmodule::PaintModule();
}

