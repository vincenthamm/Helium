#pragma once

#include "Platform/Pipe.h"

#include "IPC.h"
#include "Connection.h"

// Debug printing
//#define IPC_PIPE_DEBUG_PIPES
//#define IPC_PIPE_DEBUG_PIPES_CHUNKS

namespace IPC
{
    //
    // The size of the pipe in bytes, make this number bigger for faster transfers but longer
    // timeouts when small amounts of data are sent. Making this number too high (like 64K) makes
    // the comminication very unreliable.
    //

    class FOUNDATION_API PipeConnection : public Connection
    {
    private:
        char              m_PipeName[256];                // name of the pipe passed in by the user
        char              m_ServerName[256];              // name of the server passed in by the user

        char              m_ReadName[256];                // name of the pipe
        Platform::Pipe    m_ReadPipe;                     // handle of the pipe

        char              m_WriteName[256];               // name of the pipe
        Platform::Pipe    m_WritePipe;                    // handle of the pipe

    public:
        PipeConnection();
        virtual ~PipeConnection();

    public:
        bool Initialize(bool server, const char* name, const char* pipe_name, const char* server_name = 0);

    protected:
        void ServerThread();
        void ClientThread();

        virtual bool ReadMessage(Message** msg);
        virtual bool WriteMessage(Message* msg);
        virtual bool Read(void* buffer, u32 bytes);
        virtual bool Write(void* buffer, u32 bytes);
    };
}