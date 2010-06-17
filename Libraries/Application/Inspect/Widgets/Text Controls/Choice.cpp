#include "Choice.h"
#include "Application/Inspect/Widgets/Container.h"

#include "Application/UI/AutoCompleteComboBox.h"

using namespace Inspect;

class ComboBox : public Nocturnal::AutoCompleteComboBox
{
public:
  Choice* m_ComboBox;

  bool m_Override;

  ComboBox (wxWindow* parent, Choice* comboBox, int flags)
    : Nocturnal::AutoCompleteComboBox (parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, flags)
    , m_ComboBox (comboBox)
    , m_Override (false)
  {

  }

  void OnConfirm(wxCommandEvent& event)
  {
    if (!m_Override)
    {
      m_ComboBox->Write(); 
    }

    event.Skip();
  }

  void OnTextEnter(wxCommandEvent& event)
  {
    if (!m_Override)
    {
      m_ComboBox->Write(); 
    }

    // process the event
  }

  void OnSetFocus(wxFocusEvent& event)
  {
    if ( !(GetWindowStyle() & wxCB_READONLY) )
    {
      SetSelection(0, GetLastPosition());
    }

    event.Skip();
  }

  void OnKillFocus(wxFocusEvent& event)
  {
    if (!m_Override)
    {
      m_ComboBox->Write();
    }

    event.Skip();
  }

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(ComboBox, AutoCompleteComboBox)
EVT_COMBOBOX(wxID_ANY, ComboBox::OnConfirm)
EVT_TEXT_ENTER(wxID_ANY, ComboBox::OnTextEnter)
EVT_SET_FOCUS(ComboBox::OnSetFocus)
EVT_KILL_FOCUS(ComboBox::OnKillFocus)
END_EVENT_TABLE();

Choice::Choice()
: m_Sorted (false)
, m_DropDown (false)
, m_EnableAdds (false)
{
  m_ContextMenu = new ContextMenu (this);
  m_ContextMenu->AddItem("Set To Default", ContextMenuSignature::Delegate(this, &Choice::SetToDefault));
}

bool Choice::Process(const std::string& key, const std::string& value)
{
  if (__super::Process(key, value))
    return true;

  if (key == ITEMS_ATTR_REQUIRED)
  {
    if (value == ATTR_VALUE_TRUE)
    {
      m_Required = true;
      return true;
    }
    else if (value == ATTR_VALUE_FALSE)
    {
      m_Required = false;
      return true;
    }
  }
  else if (key == CHOICE_ATTR_ENUM)
  {
    m_Enum = value;
    return true;
  }
  else if (key == CHOICE_ATTR_SORTED)
  {
    if (value == ATTR_VALUE_TRUE)
    {
      m_Sorted = true;
      return true;
    }
    else if (value == ATTR_VALUE_FALSE)
    {
      m_Sorted = false;
      return true;
    }
  }
  else if (key == CHOICE_ATTR_DROPDOWN)
  {
    if (value == ATTR_VALUE_TRUE)
    {
      m_DropDown = true;
      return true;
    }
    else if (value == ATTR_VALUE_FALSE)
    {
      m_DropDown = false;
      return true;
    }
  }

  return false;
}

void Choice::SetOverride( bool isOverride )
{
  Control::Cast<ComboBox>(this)->m_Override = isOverride;
}

void Choice::SetDefaultAppearance(bool def)
{
  SetHighlight(def);
}

void Choice::SetToDefault(const ContextMenuEventArgs& event)
{
  event.m_Control->SetDefault();

  event.m_Control->Read();
}

void Choice::SetSorted(bool sorted)
{
  NOC_ASSERT(!IsRealized());
  m_Sorted = sorted;
}

void Choice::SetDropDown(bool dropDown)
{
  NOC_ASSERT(!IsRealized());
  m_DropDown = dropDown;
}

void Choice::SetEnableAdds(bool enabled)
{
  NOC_ASSERT(!IsRealized()); 
  m_EnableAdds = enabled; 
}

std::string Choice::GetValue()
{
  if ( IsRealized() )
  {
    ComboBox* combo = Control::Cast<ComboBox>(this);

    wxString value = combo->GetValue();

    V_Item::const_iterator itr = m_Items.begin();
    V_Item::const_iterator end = m_Items.end();
    for ( ; itr != end; ++itr )
    {
      if ( itr->m_Key == value )
      {
        return itr->m_Data;
      }
    }

    return value.c_str();
  }
  else
  {
    return m_Value;
  }
}

void Choice::SetValue(const std::string& data)
{
  m_Value = data;

  if ( IsRealized() )
  {
    ComboBox* combo = Control::Cast<ComboBox>(this);

    V_Item::const_iterator itr = m_Items.begin();
    V_Item::const_iterator end = m_Items.end();
    for ( ; itr != end; ++itr )
    {
      if ( itr->m_Data == data )
      {
        Control::Cast<ComboBox>(this)->SetValue(itr->m_Key.c_str());
        return;
      }
    }

    combo->SetValue(data);
  }
}

void Choice::Realize(Container* parent)
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if (m_Window != NULL)
    return;

  m_Window = new ComboBox(parent->GetWindow(), this, (m_DropDown ? wxCB_DROPDOWN | wxCB_READONLY : wxTE_PROCESS_ENTER) | (m_Sorted ? wxCB_SORT : 0));

  __super::Realize(parent);
}

void Choice::Populate()
{
  Clear();

  SetItems( m_Statics );

  m_Populate.Raise( ChoiceArgs (this) );

  m_Enumerate.Raise( ChoiceEnumerateArgs (this, m_Enum) );
}

void Choice::Read()
{
  if ( IsBound() && IsRealized() )
  {
    std::string str;
    ReadData(str);

    SetOverride( true );
    SetValue(str);
    SetOverride( false );

    __super::Read();
  }
}

bool Choice::Write()
{
  if ( IsBound() )
  {
    ComboBox* comboBox = Control::Cast< ComboBox >( this );

    std::string value = GetValue(); 
    bool shouldWrite = m_EnableAdds ? true : Contains(value); 
    
    bool result = false; 
    if(shouldWrite)
    {
      SetOverride( true );
      result = WriteData( value );
      SetOverride( false );
    }
    else
    {
      // refresh the old data
      Read(); 
    }

    return result;
  }

  return false;
}