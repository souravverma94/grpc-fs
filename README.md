# grpc-fs
A gRPC based file transfer system in C++

#Installation Guide:
- Install gRPC using this link: https://grpc.io/docs/quickstart/cpp/
- Create a new environment variable pointing to the gRPC library using following command:
  - $ export PKG_CONFIG_PATH=/home/user/local/lib/pkgconfig/ (replace /home/user/local with GRPC installation location)
- Clone the project abd run the follwong commands:
  - cd grpc-fs
  - make
- create a folder where you want to store the files which are sent to server(ex. data).
- change directory to that folder and run the server from that directory:
  - cd data
  - ../grpc_fs_server (Run the server)
  - create a folder to keep the files you want to send to the server (ex. build)
- Go inside the new folder and run the client and upload file to the server 
  - cd build
  - ../grpc_fs_client put <file_id> <file_name>
- To Download the files use the following command
  - ../grpc_fs_client get <file_id>
