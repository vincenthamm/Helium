#pragma once

#include <string>

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Automation/Event.h"

#include "Texture/Texture.h"

namespace IG
{
  class IGSerializer;
}

namespace Symbol
{
  class Enum;
}

namespace TextureProcess
{
  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // These globals contain the defaults which can get overridden from the command line
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  extern bool                   g_PowerOfTwo;
  extern float                  g_DefaultScaleX;
  extern float                  g_DefaultScaleY;
  extern IG::OutputColorFormat  g_DefaultOutputFormat;
  extern IG::PostMipImageFilter g_DefaultPostMipFilter;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Texture
  //  Class to hold all the info about a single effects texture
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////

  class Definition : public Nocturnal::RefCountBase<Definition>
  {
  public:
    Definition()
    : m_texture (NULL)
    , m_mips (NULL)
    , m_mip_levels (0)
    , m_force_single_mip_level( false )
    , m_user_data(NULL)
    , m_is_normal_map( false )
    , m_is_detail_normal_map( false )
    , m_is_detail_map_only( false )
    {
      m_force_power_of_2 = g_PowerOfTwo;    
      m_relscale_x = g_DefaultScaleX;
      m_relscale_y = g_DefaultScaleY;
      m_output_format = g_DefaultOutputFormat;
      m_post_filter = g_DefaultPostMipFilter;

      m_runtime.m_wrap_u = IG::UV_WRAP;
      m_runtime.m_wrap_v = IG::UV_WRAP;
      m_runtime.m_wrap_w = IG::UV_WRAP;
      m_runtime.m_filter = IG::FILTER_LINEAR_LINEAR_MIP;
      m_runtime.m_direct_uvs = false;
      m_runtime.m_expand_range = false;
    }

    ~Definition()
    {
      delete m_texture;
      delete m_mips;
    }

    // input data
    std::string                     m_enum;
    std::string                     m_texture_file;
    IG::OutputColorFormat           m_output_format;
    IG::PostMipImageFilter          m_post_filter;
    float                           m_relscale_x;
    float                           m_relscale_y;
    bool                            m_force_power_of_2;
    bool                            m_force_single_mip_level;
    bool                            m_is_normal_map;
    bool                            m_is_detail_normal_map;
    bool                            m_is_detail_map_only;

    // output data
    IG::Texture*                 m_texture;
    IG::MipSet*                  m_mips;
    IG::MipSet::RuntimeSettings  m_runtime;
    u32                          m_mip_levels;

    // user data
    void*                        m_user_data;
  };

  typedef Nocturnal::SmartPtr<Definition> DefinitionPtr;
  typedef std::vector<DefinitionPtr> V_Definition;

  typedef Nocturnal::Signature<void, Definition*> DefinitionSignature;

  struct PostLoadArgs
  {

  };
  typedef Nocturnal::Signature<void, const PostLoadArgs&> PostLoadSignature;

  //
  // The Bank class builds Defs into a packed bank of textures
  //

  class Bank
  {
  private:

    DefinitionSignature::Event m_ProcessDefinition;
    PostLoadSignature::Event m_PostLoad;

  public:
    // The list of textures to work with
    V_Definition m_textures;

    // Events
    void AddDefinitionProcessor( const DefinitionSignature::Delegate& listener )
    {
      m_ProcessDefinition.Add( listener );
    }

    void AddPostLoadListener( const PostLoadSignature::Delegate& listener )
    {
      m_PostLoad.Add( listener );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  LoadImages
    //
    //  Loads the source textures and fills in the remainder of the process class
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool LoadImages();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  AdjustImages
    //
    //  Adjust the source image for being non power of 2 or if it requires prescaling
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool AdjustImages();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Compress
    //
    //  Compress and generate the output data along with all the associated mip maps
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool CompressImages();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Pack
    //
    //  Performs default processing of textures in global texture array
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool Pack();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  WriteDebugFile
    //
    //  Writes the debug text file with the filenames, compression settings, etc.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteDebugFile( const std::string& debug_file );
  };
}