#include "messages.h"

grpcfs::FileId MakeFileId(std::int32_t id)
{
    grpcfs::FileId fid;
    fid.set_id(id);
    return fid;
}

grpcfs::FileContent MakeFileContent(std::int32_t id, std::string name, const void* data, size_t data_len)
{
    grpcfs::FileContent fc;
    fc.set_id(id);
    fc.set_name(std::move(name));
    fc.set_content(data, data_len);
    return fc;
}