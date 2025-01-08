# client-server-linux-tcp
```
cd "/../client-server-linux-tcp-master/server/"
mkdir build
cd build
cmake ..
make
./server -d /../client-server-linux-tcp-master/test_dir -t 42
```

Then in new terminal:
```

cd "/../client-server-linux-tcp-master/client/"
mkdir build
cd build
cmake ..
make
./client
```

Then create or delete file or dir in /../client-server-linux-tcp-master/test_dir
