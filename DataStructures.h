#pragma once
#include <array>

// L1 cahce 256 KB = 256*1024bytes = 2*128*16 
// L1 I 128 KB = 128*16 lines? if line size is 64 bytes
// L1 D 128 KB  ... ?
// L2 cahce 1.0 MB
// L3 cahce 8.0 MB
// Total memory 15.8 GB
// unusable memory \approx 6.0 GB


struct UserFromUEConnectionInfo
{
    // total size is 8+56 = 64 bytes
    unsigned __int64 user_id;
    std::array<char, 56> user_name;
};
