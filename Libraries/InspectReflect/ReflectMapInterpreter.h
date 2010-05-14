#pragma once

#include "ReflectFieldInterpreter.h"

#include "Inspect/Canvas.h"
#include "Inspect/Container.h"

namespace Inspect
{
  class Button;
  class Container;

  class INSPECTREFLECT_API ReflectMapInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectMapInterpreter( Container* container );
    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) NOC_OVERRIDE;

  protected:
    virtual void TranslateInputKeyValueTUID( Reflect::TranslateInputEventArgs& args );
    virtual void TranslateOutputKeyValueTUID( Reflect::TranslateOutputEventArgs& args );

    void OnAdd( Button* button );
    void OnRemove( Button* button );
    void OnEdit( Button* button );
  };

  typedef Nocturnal::SmartPtr<ReflectMapInterpreter> ReflectMapInterpreterPtr;
  typedef std::vector< ReflectMapInterpreterPtr > V_ReflectMapInterpreterSmartPtr;
}