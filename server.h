#pragma once

class QsyncServer {
    struct DataStreamContext {
        QsyncServer* Server;
        MsQuicStream* Stream;
        std::atomic_uint64_t RefCount;
        QUIC_BUFFER Buffers[2];
        uint32_t BufferCount;
        SerializedFileInfo FileInfo;
        uintmax_t NewFileSize;
        uint64_t BytesWritten;
        std::chrono::file_time<std::chrono::seconds> FileTime;
        std::filesystem::path DestinationPath;
        std::filesystem::path TempDestinationPath;
        std::fstream FileWriteStream;
        uint64_t SnapshotDestSize;
        std::filesystem::file_time_type SnapshotDestModTime;
        bool FinalReceive;
        bool FileExists;

        DataStreamContext() = default;
        ~DataStreamContext() = default;

        void FileIoWorker();
    };

    uint32_t Pkcs12Length;
    std::filesystem::path BasePath;
    Threadpool Pool;
    Threadpool IoPool;
    QUIC_CERTIFICATE_PKCS12 Pkcs12Config;
    MsQuicCredentialConfig Creds;
    std::string CertPw;
    std::unique_ptr<uint8_t[]> Pkcs12;
    std::unique_ptr<MsQuicRegistration> Reg;
    std::unique_ptr<MsQuicListener> Listener;
    std::unique_ptr<MsQuicConfiguration> Config;
    std::unique_ptr<MsQuicConnection> Connection;
    std::unique_ptr<MsQuicStream> ControlStream;
    uint32_t SkipMessageBytes;
    union {
        uint32_t RemainderSize;
        uint8_t RemainderSizeBytes[4];
        QUIC_BUFFER RemainderMessage;
    };
    uint32_t RemainderFilled;
    enum PartialData : uint8_t {
        NoPartialData = 0,
        PartialSize = 1,
        PartialMessage = 2,
    } PartialData;

public:
    QsyncServer() : Pool(1), IoPool(8) {};
    QsyncServer(const QsyncServer&) = delete;
    QsyncServer(QsyncServer&&) = default;
    ~QsyncServer() = default;

    bool
    Start(
        uint16_t ListenPort,
        const std::string& Root,
        const std::string& Password);

private:
    static
    QUIC_STATUS
    QsyncListenerCallback(
        _In_ HQUIC /*Listener*/,
        _In_opt_ void* Context,
        _Inout_ QUIC_LISTENER_EVENT* Event);

    static
    QUIC_STATUS
    QsyncServerConnectionCallback(
        _In_ MsQuicConnection* /*Connection*/,
        _In_opt_ void* Context,
        _Inout_ QUIC_CONNECTION_EVENT* Event);

    static
    QUIC_STATUS
    QSyncServerControlStreamCallback(
        _In_ MsQuicStream* /*Stream*/,
        _In_opt_ void* Context,
        _Inout_ QUIC_STREAM_EVENT* Event);

    static
    QUIC_STATUS
    QSyncServerDataStreamCallback(
        _In_ MsQuicStream* /*Stream*/,
        _In_opt_ void* Context,
        _Inout_ QUIC_STREAM_EVENT* Event);

    void
    QSyncServerWorkerCallback(
        _In_ const ReceivedFileInfo& Info);

    void
    AddFileToList(
        QsyncServer* Server,
        uint8_t* Buffer,
        uint32_t Length);
};
