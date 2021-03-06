#pragma once

#include "Reflect/Object.h"
#include "Reflect/TranslatorDeduction.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Base class for any data that can be stored on the clipboard.
        // 
        class ReflectClipboardData HELIUM_ABSTRACT : public Reflect::Object
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ReflectClipboardData, Reflect::Object );
            static void PopulateStructure( Reflect::Structure& comp );

            ReflectClipboardData();
            virtual ~ReflectClipboardData();

            // Derived classes must implement this function to combine their data
            // with another object's (and return true if successful).
            virtual bool Merge( const ReflectClipboardData* source ) = 0;
        };
        typedef Helium::StrongPtr< ReflectClipboardData > ReflectClipboardDataPtr;
    }
}