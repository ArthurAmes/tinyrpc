#ifndef _TINYRPC_H
#define _TINYRPC_H

#include <tinyrpc-macros.h>
#include <tinyrpc-cross.h>
#include <Arduino.h>

typedef uint8_t* RPCArgPtr;
typedef void(*RPCFStub)(RPCArgPtr);
typedef struct RPCStubMap {
    RPCFStub stub;
    uint8_t nargs;
} RPCStubMap;

void trpc_init(HardwareSerial* serial);

#ifdef TINYRPC_SERVER
void trpc_listenServe();
void trpc_ret_stub(uint8_t nbret, uint32_t* v);

extern const RPCStubMap g_trpc_STUBS[];

    #define RPC_LIST_START \
        const RPCStubMap g_trpc_STUBS[] = { \
            {NULL, 0}, // Idx 0 reserved for RPC resolve.

    #define RPC_LIST_END \
        };

    #define RPC_FUNC(ret, name, ...)                                                                     \
    {.stub = [](RPCArgPtr fargs) {                                                                       \
        ret x __attribute__ ((aligned (4))) = ((ret(*)(__VA_ARGS__))name)MAPT(fargs, (__VA_ARGS__));     \
        trpc_ret_stub(sizeof(ret), (uint32_t*)&x);                                                       \
    }, .nargs = 0 MAPT_XX(+ sizeof, (__VA_ARGS__))},
#endif

#ifdef TINYRPC_CLIENT
uint32_t trpc_packTransmit(uint8_t id, uint8_t retnb, uint8_t nbargs, ...);
void trpc_packTransmitVoid(uint8_t id, uint8_t retnb, uint8_t nbargs, ...);

    #define pt_RPC_VOID(...) trpc_packTransmitVoid(__VA_ARGS__)
    #define pt_RPC_CHAR(...) return (RPC_CHAR)trpc_packTransmit(__VA_ARGS__)
    #define pt_RPC_SHORT(...) return (RPC_SHORT)trpc_packTransmit(__VA_ARGS__)
    #define pt_RPC_INT(...) return (RPC_INT)trpc_packTransmit(__VA_ARGS__)
    #define pt_RPC_LONG(...) return (RPC_LONG)trpc_packTransmit(__VA_ARGS__)
    #define pt_RPC_FLOAT(...) uint32_t r = trpc_packTransmit(__VA_ARGS__); return *((float*)&r) // reinterpret cast
    #define pt_RPC_BOOL(...) return (RPC_BOOL)trpc_packTransmit(__VA_ARGS__)
    // #define pt_RPC_PTR(...) return (RPC_PTR)trpc_packTransmit(__VA_ARGS__)

    #define RPC_LIST_START \
        const int __TRPC_COUNTER_BASE = __COUNTER__;

    #define RPC_LIST_END 

    #define RPC_FUNC(ret, name, ...)  \
        ret name(MAPN((__VA_ARGS__))) {       \
            pt_ ##ret(__COUNTER__-__TRPC_COUNTER_BASE, sizeof(ret), VARCOUNT(__VA_ARGS__), MAPN_X((__VA_ARGS__))); \
        }
#endif

#endif