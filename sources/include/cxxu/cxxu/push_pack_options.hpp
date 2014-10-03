// No header guard

#ifdef WIN32
#define CXXU_PACKED
#pragma pack(push, 1)
#endif

#ifdef UNIX
#define CXXU_PACKED __attribute__((packed))
#define CXXU_ALIGNED(ALIGNMENT) __attribute__((aligned(ALIGNMENT)))
#define CXXU_ALIGNED_AND_PACKED(ALIGNMENT) __attribute__((aligned(ALIGNMENT), packed))
#endif
