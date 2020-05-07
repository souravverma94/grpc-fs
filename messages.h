#pragma once

#include <cstdint>
#include <string>

#include "grpc_fs.grpc.pb.h"

grpcfs::FileId MakeFileId(std::int32_t id);
grpcfs::FileContent MakeFileContent(std::int32_t id, std::string name, const void* data, size_t data_len);
