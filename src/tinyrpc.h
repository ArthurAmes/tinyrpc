#ifndef _TINYRPC_H
#define _TINYRPC_H

#include <tinyrpc-macros.h>
#include <Arduino.h>

typedef uint8_t* RPCArgPtr;
typedef void(*RPCFStub)(RPCArgPtr);
typedef struct RPCStubMap {
    const RPCFStub stub;
    const uint8_t nargs;
} RPCStubMap;

void trpc_init(HardwareSerial* serial);

#ifdef TINYRPC_SERVER

    void trpc_listenServe();
    void trpc_ret_stub(uint8_t nbret, void* v);

    #define rt_1(ret, name, ...) ((ret(*)(__VA_ARGS__))name)MAP(fargs, (__VA_ARGS__)); /* If the return type is void, this stub is called. */
    #define rt_0(ret, name, ...) ret x = ((ret(*)(__VA_ARGS__))name)MAP(fargs, (__VA_ARGS__)); trpc_ret_stub(sizeof(ret), (void*)&x); /* If the return type is not void, this stub is called */

    #define RPC_LIST_START
    #define RPC_LIST_END

    #define RPC_FUNC(ret, name, ...)                                                                    \
        void __rpcstub__ ##name(RPCArgPtr fargs) { GLUE(rt_, IS_VOID(_##ret))(ret, name, __VA_ARGS__) } \
        static const RPCStubMap stubentry_##name                                                        \
        __attribute((used, section("trpc_stubs"))) = {                                                  \
            .stub = __rpcstub__ ##name,                                                                 \
            .nargs = 0 MAPT_XX(+ sizeof, (__VA_ARGS__)) };

#endif

#ifdef TINYRPC_CLIENT
void trpc_ptx(uint8_t id, uint8_t nbargs, ...);
void trpc_recvRet(uint8_t s, void* retval);

    #define pt_1(id, ret, nbargs, ...) trpc_ptx(id, nbargs, __VA_ARGS__)
    #define pt_0(id, ret, nbargs, ...) trpc_ptx(id, nbargs, __VA_ARGS__); ret x; trpc_recvRet(sizeof(ret), &x); return x

    #define RPC_LIST_START \
        const int __TRPC_COUNTER_BASE = __COUNTER__;

    #define RPC_LIST_END 

    #define RPC_FUNC(ret, name, ...)  \
        ret name(FN_DEF_EXPAND((__VA_ARGS__))) {       \
            GLUE(pt_, IS_VOID(_##ret))(__COUNTER__-__TRPC_COUNTER_BASE-1, ret, VARCOUNT(__VA_ARGS__), PTX_EXPAND((__VA_ARGS__))); \
        }
#endif

#endif