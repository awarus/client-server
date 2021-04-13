# client-server
Simple multithreaded TCP\IP Client-Server with mapped files.

Type 'make' for build

./clean.sh - for cleaning /var/tmp/server_data dir(where stored messages from client)

Generally we have N/3 worker threads on the server side and N client threads. N defined in constraints.h
