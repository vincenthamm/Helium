#include "Precompile.h"
#include "CollectionManager.h"

#include "Common/Boost/Regex.h"
#include "Common/Checksum/MD5.h"
#include "Common/Flags.h"
#include "FileSystem/FileSystem.h"
#include "Common/String/Utilities.h"
#include "File/Manager.h"
#include "Reflect/Element.h"
#include "Reflect/Version.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
/// class CollectionManager
///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( CollectionManager )
void CollectionManager::EnumerateClass( Reflect::Compositor<CollectionManager>& comp )
{
  Reflect::Field* fieldCollectionFilePaths = comp.AddField( &CollectionManager::m_CollectionFilePaths, "m_CollectionFilePaths" );
}

/////////////////////////////////////////////////////////////////////////////
CollectionManager::CollectionManager()
{
  AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnAssetCollectionsChanged ) );
}

/////////////////////////////////////////////////////////////////////////////
CollectionManager::~CollectionManager()
{
  RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnAssetCollectionsChanged ) );
  
  ClearCollectionMap();
  m_CollectionFilePaths.clear();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::PreSerialize()
{
  __super::PreSerialize();

  SaveAllCollections();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::PostDeserialize()
{
  __super::PostDeserialize();

  S_string copyCollectionFilePaths = m_CollectionFilePaths;

  ClearCollectionMap();
  for ( S_string::const_iterator itr = copyCollectionFilePaths.begin(), end = copyCollectionFilePaths.end(); itr != end; ++itr )
  {
    AssetCollectionPtr collection = AssetCollection::LoadFromFile( *itr );

    if ( collection )
    {
      AddCollectionToMap( collection );
    }
    else
    {
      // TODO debug warning
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the manager already contains the specified collection.
bool CollectionManager::ContainsCollection( AssetCollection* collection ) const
{
  if ( !collection )
  {
    return false;
  }
  
  return ( FindCollection( collection->GetID() ) == collection );
}

///////////////////////////////////////////////////////////////////////////////
// Checks that collection name is unique
bool CollectionManager::IsCollectionNameAvailable( AssetCollection* collection, const std::string& name, std::string& errors )
{
  AssetCollection* foundCollection = FindCollection( name );
  if ( collection && foundCollection && (*collection) != (*foundCollection) )
  {
    errors = "Collection names need to be unique, you already have a collection called: " + name;
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::GetUniqueName( std::string& name, const char* baseName )
{
  // try the base name first
  name = baseName;
  
  int numTries = 0;
  while ( FindCollection( name.c_str() ) )
  {
    ++numTries;
    std::ostringstream str ( name );
    str << baseName << " " << numTries;
    name = str.str();
  }
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::AddCollection( AssetCollection* collection )
{
  if ( !collection )
    return false;

  Nocturnal::Insert<M_AssetCollections>::Result inserted = AddCollectionToMap( collection );

  return inserted.second;
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::RenameCollection( AssetCollection* collection, const std::string& name, std::string& errors )
{
  if ( !collection )
    return false;

  if ( !AssetCollection::IsValidCollectionName( name, errors ) )
  {
    return false;
  }

  if ( !IsCollectionNameAvailable( collection, name, errors ) )
  {
    return false;
  }

  const tuid oldID = collection->GetID();
  std::string oldName = collection->GetName();
  std::string oldFilePath = collection->GetFilePath();
  AssetCollection* foundCollection = FindCollection( oldID );
  if ( !foundCollection )
  {
    errors = "";
    return false;
  }

  // only do the work if we have to
  if ( oldName != name )
  {
    foundCollection->SetName( name );

    std::string filePath;
    AssetCollection::CreateFilePath( name, filePath );
    foundCollection->SetFilePath( filePath );

    // erase the old reference from the map
    if ( !filePath.empty() && FileSystem::Exists( oldFilePath ) )
    {
      FileSystem::Win32Name( oldFilePath );
      ::DeleteFile( oldFilePath.c_str() );
    }

    RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );    
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::DeleteCollection( AssetCollection* collection )
{
  if ( !collection )
    return;

  std::string filePath = collection->GetFilePath();

  CloseCollection( collection );

  if ( !filePath.empty() && FileSystem::Exists( filePath ) )
  {
    FileSystem::Win32Name( filePath );
    ::DeleteFile( filePath.c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
AssetCollection* CollectionManager::OpenCollection( const std::string& path, bool copyLocal )
{
  AssetCollectionPtr loadCollection = AssetCollection::LoadFromFile( path );
  if ( loadCollection )
  {
    std::string name;
    GetUniqueName( name, ( loadCollection->GetName().empty() ? "Imported Collection" : loadCollection->GetName().c_str() ) );
    loadCollection->SetName( name );

    u32 flags = loadCollection->GetFlags();
    if ( copyLocal )
    {
      std::string filePath;
      AssetCollection::CreateFilePath( name, filePath );
      loadCollection->SetFilePath( filePath );

      flags |= AssetCollectionFlags::CanRename | AssetCollectionFlags::CanHandleDragAndDrop;
    }
    loadCollection->SetFlags( flags );

    // Append the newly imported collection
    Nocturnal::Insert<M_AssetCollections>::Result inserted = AddCollectionToMap( loadCollection );

    return inserted.first->second;
  }
  
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::CloseCollection( AssetCollection* collection )
{
  if ( !collection )
    return;

  RemoveCollectionFromMap( collection );
}

///////////////////////////////////////////////////////////////////////////////
AssetCollection* CollectionManager::ImportCollection( const std::string& path )
{
  return OpenCollection( path, true );
}

///////////////////////////////////////////////////////////////////////////////
AssetCollection* CollectionManager::FindCollection( const tuid id ) const
{
  M_AssetCollections::const_iterator findCollection = m_AssetCollections.find( id );
  if ( findCollection != m_AssetCollections.end() )
  {
    return findCollection->second;
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
AssetCollection* CollectionManager::FindCollection( const std::string& name ) const
{
  for ( M_AssetCollections::const_iterator itr = m_AssetCollections.begin(), end = m_AssetCollections.end(); itr != end; ++itr )
  {    
    AssetCollection* collection = itr->second;
    if ( collection->GetName() == name )
    {
      return collection;
    }
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::ImportIntoStaticCollection( AssetCollection* collection, const std::string& path )
{
  if ( !collection )
    return false;

  AssetCollection* foundCollection = FindCollection( collection->GetID() );
  if ( foundCollection )
  {
    AssetCollectionPtr loadCollection = AssetCollection::LoadFromFile( path );
    if ( loadCollection )
    {
      if ( foundCollection->AddAssetIDs( loadCollection->GetAssetIDs() ) )
      {
        Dirty( foundCollection );
        
        RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );

        return true;
      }
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::ExportCollection( AssetCollection* collection, const std::string& path )
{
  if ( !collection )
    return false;

  return AssetCollection::SaveToFile( collection, path );
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::SaveCollection( AssetCollection* collection )
{
  if ( !collection )
    return false;

  bool result = false;

  if ( !collection->ReadOnly()
    && !collection->IsTemporary()
    && !collection->GetFilePath().empty() )
  {
    // Only save dirty files
    S_tuid::iterator findCollection = m_DirtyCollectionSigs.find( collection->GetID() );
    if ( findCollection != m_DirtyCollectionSigs.end() )
    {
      result = AssetCollection::SaveToFile( collection, collection->GetFilePath() );
      m_DirtyCollectionSigs.erase( *findCollection );
    }

    m_CollectionFilePaths.insert( collection->GetFilePath() );
  }
  else
  {
    // TODO debug warning
    result = false;
  }

  return result;
}


///////////////////////////////////////////////////////////////////////////////
void CollectionManager::SaveAllCollections()
{
  m_CollectionFilePaths.clear();
  for ( M_AssetCollections::const_iterator itr = m_AssetCollections.begin(), end = m_AssetCollections.end(); itr != end; ++itr )
  {    
    AssetCollection* collection = itr->second;
    SaveCollection( collection );
  }

  m_DirtyCollectionSigs.clear();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::Dirty( const AssetCollection* collection )
{
  if ( !collection )
    return;

  m_DirtyCollectionSigs.insert( collection->GetID() );
}

///////////////////////////////////////////////////////////////////////////////
Nocturnal::Insert<M_AssetCollections>::Result CollectionManager::AddCollectionToMap( AssetCollection* collection )
{
  Nocturnal::Insert<M_AssetCollections>::Result inserted = m_AssetCollections.insert(
    M_AssetCollections::value_type( collection->GetID(), collection ) );

  NOC_ASSERT( inserted.second );

  if ( inserted.second )
  {
    collection->InitializeCollection();
    collection->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnCollectionModified ) );

    Dirty( collection );
    m_CollectionAdded.Raise( collection );
    RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );
  }

  return inserted;
}

///////////////////////////////////////////////////////////////////////////////
bool CollectionManager::RemoveCollectionFromMap( AssetCollection* collection )
{
  if ( !collection )
    return false;

  const tuid collectionID = collection->GetID();
  const std::string collectionName = collection->GetName();

  m_CollectionRemoving.Raise( collection );
  
  collection->CleanupCollection();
  collection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnCollectionModified ) );
  
  if ( m_AssetCollections.erase( collectionID ) > 0 )
  {
    m_DirtyCollectionSigs.erase( collectionID );
    RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::ClearCollectionMap()
{
  if ( !m_AssetCollections.empty() )
  {
    m_ClearAll.Raise( NULL );

    for ( M_AssetCollections::const_iterator itr = m_AssetCollections.begin(), end = m_AssetCollections.end(); itr != end; ++itr )
    {    
      AssetCollection* collection = itr->second;
      collection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionManager::OnCollectionModified ) );
    }

    m_AssetCollections.clear();
    m_DirtyCollectionSigs.clear();
    RaiseChanged( GetClass()->FindField( &CollectionManager::m_AssetCollections ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::OnAssetCollectionsChanged( const Reflect::ElementChangeArgs& args )
{
  SaveAllCollections();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionManager::OnCollectionModified( const Reflect::ElementChangeArgs& args )
{
  const AssetCollection* collection = Reflect::ConstObjectCast<AssetCollection>( args.m_Element );
  
  if ( collection )
  {
    Dirty( collection );
  }
}