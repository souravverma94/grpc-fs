#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <map>
#include <cstdint>
#include <stdexcept>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include "sequential_file_writer.h"
#include "file_reader_into_stream.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;

using grpcfs::FileId;
using grpcfs::FileContent;
using grpcfs::FileSystem;

class FileSystemImpl final : public FileSystem::Service {
private:
    typedef google::protobuf::int32 FileIdKey;

public:
    FileSystemImpl() = default;

    Status UploadFile(
      ServerContext* context, ServerReader<FileContent>* reader,
      FileId* summary) override
    {
        FileContent contentPart;
        SequentialFileWriter writer;
        while (reader->Read(&contentPart)) {
            try {
               
                writer.OpenIfNecessary(contentPart.name());
                auto* const data = contentPart.mutable_content();
                writer.Write(*data);
                summary->set_id(contentPart.id());
                m_FileIdToName[contentPart.id()] = contentPart.name();
            }
            catch (const std::system_error& ex) {
                const auto status_code = writer.NoSpaceLeft() ? StatusCode::RESOURCE_EXHAUSTED : StatusCode::ABORTED;
                return Status(status_code, ex.what());
            }
        }

        return Status::OK;
    }

    Status DownloadFile(
        ServerContext* context,
        const FileId* request,
        ServerWriter<FileContent>* writer) override
    {
        const auto id = request->id();
        const auto it = m_FileIdToName.find(id);
        if (m_FileIdToName.end() == it) {
            return Status(grpc::StatusCode::NOT_FOUND, "No file with the id " + std::to_string(id));
        }
        const std::string filename = it->second;

        try {
            FileReaderIntoStream< ServerWriter<FileContent> > reader(filename, id, *writer);

            const size_t chunk_size = 1UL << 20;    // Hardcoded to 1MB
            reader.Read(chunk_size);
        }
        catch (const std::exception& ex) {
            std::ostringstream sts;
            sts << "Error sending the file " << filename << ": " << ex.what();
            std::cerr << sts.str() << std::endl;
            return Status(StatusCode::ABORTED, sts.str());
        }

        return Status::OK;
    }

private:
    std::map<FileIdKey, std::string> m_FileIdToName;
};


void RunServer(std::string server_address) {
//   std::string server_address("0.0.0.0:50051");
  FileSystemImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << ". Press Ctrl-C to end." << std::endl;
  server->Wait();
}

int main(int argc, char** argv)
{
    std::string server_address("0.0.0.0:50051");
    if(argc==2)
        server_address = std::string(argv[1]);

    RunServer(server_address);
    return 0;
}
