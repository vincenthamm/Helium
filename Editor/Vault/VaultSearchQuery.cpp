#include "EditorPch.h"
#include "VaultSearchQuery.h"

#include "Editor/App.h"
#include "Foundation/Regex.h"
#include "Foundation/MD5.h"
#include "Foundation/Tokenize.h"
#include "Foundation/Log.h"

REFLECT_DEFINE_OBJECT( Helium::Editor::VaultSearchQuery );

using namespace Helium;
using namespace Helium::Editor;

//
// Searching
//

/////////////////////////////////////////////////////////////////////////////
// Phrase:
//  - literal-word
//  - double quoted group of literal-words
//  
// ColumnAlias:
//  - alias name of a searchable table column (must be 2 or more characters)
//
// ColumnQuery:
//  - ColumnAlias : Phrase
//  
// Query:
//  - Phrase
//  - ColumnQuery
//namespace TokenTypes
//{
//  enum TokenType
//  {
//    File,
//    Directory,
//    Phrase,
//    ColumnName,
//    ComponentName,
//  };
//}

#define MATCH_WORD             TXT( "[a-z0-9_\\-\\.\\\\/:\\*]+" )
#define MATCH_PHRASE           TXT( "[a-z0-9_\\-\\.\\\\/:\\s\\*]+" )
#define MATCH_COLUMN_NAME      TXT( "[a-z][a-z0-9_\\-]{1,}" )

const char* s_ParseWord             = TXT( "(" ) MATCH_WORD TXT( ")" );
const char* s_ParsePhrase           = TXT( "[\"](" ) MATCH_PHRASE TXT( ")[\"]" );
const char* s_ParseColumnName       = TXT( "(" ) MATCH_COLUMN_NAME TXT( ")\\s*[:=]\\s*" );
const char* s_TokenizeQueryString   = TXT( "(" ) MATCH_COLUMN_NAME TXT( "\\s*[:=]\\s*|[\"]" ) MATCH_PHRASE TXT( "[\"]|" ) MATCH_WORD TXT( ")" );

///////////////////////////////////////////////////////////////////////////////
void VaultSearchQuery::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &VaultSearchQuery::m_QueryString, TXT( "m_QueryString" ) );
}

///////////////////////////////////////////////////////////////////////////////
VaultSearchQuery::VaultSearchQuery()
: m_SQLQueryString( TXT("") )
{

}

VaultSearchQuery::~VaultSearchQuery()
{
}

///////////////////////////////////////////////////////////////////////////////
void VaultSearchQuery::PostDeserialize( const Reflect::Field* field )
{
    Base::PostDeserialize( field );

    if ( field == NULL )
    {
        std::string errors;
        if ( !ParseQueryString( m_QueryString, errors, this ) )
        {
            Log::Warning( TXT( "Errors occurred while parsing the query string: %s\n  %s\n" ), m_QueryString.c_str(), errors.c_str() );
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::SetQueryString( const std::string& queryString, std::string& errors )
{
    // Set the QueryString
    m_QueryString = queryString;
    m_SQLQueryString.clear();

    if ( !ParseQueryString( m_QueryString, errors, this ) )
    {
        Log::Warning( TXT( "Errors occurred while parsing the query string: %s\n  %s\n" ), m_QueryString.c_str(), errors.c_str() );
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////
const std::string& VaultSearchQuery::GetSQLQueryString() const
{
    if ( m_SQLQueryString.empty() )
    {
        m_SQLQueryString = m_QueryString;
        m_SQLQueryString = TXT( '*' ) + m_SQLQueryString + TXT( '*' );

        const tregex sqlQueryString( TXT( "\\*+" ) );
        m_SQLQueryString = std::tr1::regex_replace( m_SQLQueryString, sqlQueryString, std::string( TXT( "%" ) ) ); 
    }
    return m_SQLQueryString;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator<( const VaultSearchQuery& rhs ) const
{
    return CaseInsensitiveCompareString( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) < 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator==( const VaultSearchQuery& rhs ) const
{
    return CaseInsensitiveCompareString( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) == 0;
}

/////////////////////////////////////////////////////////////////////////////////
bool VaultSearchQuery::operator!=( const VaultSearchQuery& rhs ) const
{
    return CaseInsensitiveCompareString( GetQueryString().c_str(), rhs.GetQueryString().c_str() ) != 0;
}

///////////////////////////////////////////////////////////////////////////////
// The query->m_Search is filled out when provided.
//
bool TokenizeQuery( const std::string& queryString, std::vector< std::string >& tokens )
{
    const tregex parseTokens( s_TokenizeQueryString, std::tr1::regex::icase );

    // parse once to tokenize then match again
    tsregex_iterator parseItr( queryString.begin(), queryString.end(), parseTokens );
    tsregex_iterator parseEnd;

    std::string curToken;
    for ( ; parseItr != parseEnd; ++parseItr )
    {
        const std::tr1::match_results<std::string::const_iterator>& tokenizeResults = *parseItr;
        curToken = tokenizeResults[1].matched ? Helium::MatchResultAsString( tokenizeResults, 1 ) : TXT( "" );
        if ( !curToken.empty() )
        {
            tokens.push_back( curToken );
        }
    }

    return !tokens.empty();
}

bool ParsePhrase( const std::string& token, tsmatch& matchResults, std::string& phrase, std::string& errors )
{
    const tregex parsePhrase( s_ParsePhrase, std::tr1::regex::icase );
    const tregex parseWord( s_ParseWord, std::tr1::regex::icase );

    // Phrase or Word
    if ( std::tr1::regex_search( token, matchResults, parsePhrase ) 
        || std::tr1::regex_search( token, matchResults, parseWord ) )
    {
        if ( matchResults[1].matched )
        {
            phrase = Helium::MatchResultAsString( matchResults, 1 );
            return true;
        }
    }

    errors = TXT( "Vault could not parse search query phrase: " ) + token;
    return false;
}

bool VaultSearchQuery::ParseQueryString( const std::string& queryString, std::string& errors, VaultSearchQuery* query )
{
    tsmatch matchResult;
    const tregex parseColumnQuery( s_ParseColumnName, std::tr1::regex::icase );

    // parse once to tokenize then match again
    std::vector< std::string > tokens;
    if ( TokenizeQuery( queryString, tokens ) )
    {
        std::string curToken;
        std::string currentValue;

        tsmatch matchResults;
        std::vector< std::string >::const_iterator tokenItr = tokens.begin(), tokenEnd = tokens.end();
        for ( ; tokenItr != tokenEnd; ++tokenItr )
        {
            curToken = *tokenItr;

            //-------------------------------------------
            // Token Query
            if ( std::tr1::regex_search( curToken, matchResults, parseColumnQuery ) && matchResults[1].matched )
            {
                std::string columnAlias =  Helium::MatchResultAsString( matchResults, 1 );

                ++tokenItr;
                if ( tokenItr == tokenEnd )
                {
                    errors = TXT( "More information needed for search query \"" ) + columnAlias + TXT( ":\", missing argument." );
                    return false;
                }
                curToken = *tokenItr;
            }

            //-------------------------------------------
            // Phrase or Word
            if ( ParsePhrase( curToken, matchResults, currentValue, errors )  )
            {
                HELIUM_ASSERT( !currentValue.empty() );

                continue;
            }
            else
            {
                return false;
            }

        }

        return true;
    }

    return false;
}
