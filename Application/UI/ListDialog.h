#pragma once

#include "Application/API.h"
#include "SortableListView.h"
#include <wx/wx.h>

namespace Nocturnal
{
  //////////////////////////////////////////////////////////////////////////////
  // Dialog that displays a list of messages.
  // 
  class APPLICATION_API ListDialog : public wxDialog 
  {
  protected:
    wxStaticText* m_StaticText;
    SortableListView* m_MsgList;
    wxButton* m_OK;
    std::string m_Description;

  public:
    ListDialog( wxWindow* parent, const std::string& title, const std::string& desc, const std::vector< std::string >& msgs );
  };
}