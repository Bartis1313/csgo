#pragma once

// this is a tricky hack for classes in SDK I used in C structs
// since this is not so good for C++ I leave it commented

//#define SDK_START(name) \
//class name final { \
//public: \
//name() = delete; \
//name(name&&) = delete; \
//name(const name&) = delete; \
//name& operator=(const name&) = delete;
//
//#define SDK_END(name) \
//};
//
//SDK_START(ok);
//
//SDK_END(ok);

//class meme : public ok -> is error
//{
//
//};