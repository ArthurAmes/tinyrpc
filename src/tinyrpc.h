#ifndef _TINYRPC_H
#define _TINYRPC_H

#include <tinyrpc-macros.h>
#include <Arduino.h>

namespace TinyRPC {
    typedef uint8_t* RPCArgPtr;
    typedef void(*RPCFStub)(RPCArgPtr);
    typedef struct RPCStubMap {
        const RPCFStub stub;
        const uint8_t nargs;
    } RPCStubMap;

    void init(HardwareSerial* serial);

    #ifdef TINYRPC_SERVER

        void listenServe();
        void ret_stub(uint8_t nbret, void* v);

        /* 
        Since we want to optimize compile sizes as much as possible, we won't use templates for the function stubs
        Templating the stubs would mean that the compiler would generate a new function for every type we send over 
        the network, while macros allow us to just use one. 
        
        Also, Arduino has no STL, so it makes templated types a pain to work with beyond the basics.
        */

        #define rt_1(ret, name, ...) ((ret(*)(__VA_ARGS__))name)MAP(fargs, (__VA_ARGS__)); /* If the return type is void, this stub is called. */
        #define rt_0(ret, name, ...) ret x = ((ret(*)(__VA_ARGS__))name)MAP(fargs, (__VA_ARGS__)); TinyRPC::ret_stub(sizeof(ret), (void*)&x); /* If the return type is not void, this stub is called */

        #define RPC_LIST_START
        #define RPC_LIST_END

        #define RPC_FUNC(ret, name, ...)                                                                                \
            void __rpcstub__ ##name(TinyRPC::RPCArgPtr fargs) { GLUE(rt_, IS_VOID(_##ret))(ret, name, __VA_ARGS__) }    \
            static const TinyRPC::RPCStubMap stubentry_##name                                                           \
            __attribute((used, section("trpc_stubs"))) = {                                                              \
                .stub = __rpcstub__ ##name,                                                                             \
                .nargs = 0 MAPT_XX(+ sizeof, (__VA_ARGS__)) };

    #endif

    #ifdef TINYRPC_CLIENT
        void ptx(uint8_t id, uint8_t nbargs, ...);
        void recvRet(uint8_t s, void* retval);

        template <typename T>
        class Promise {
        public:
            static T await() {
                T r;
                TinyRPC::recvRet(sizeof(T), &r);
                return r;
            };
        };

        #define pt_void(id, ret, nbargs, ...) \
            TinyRPC::ptx(id, nbargs, __VA_ARGS__);

        #define pt_nonvoid(id, ret, nbargs, ...) \
            TinyRPC::ptx(id, nbargs, __VA_ARGS__); \
            TinyRPC::Promise<ret> x;               \
            return x;                              \

        #define RPC_FUNC_PROXY1(ret, name, ...) \
        void name(FN_DEF_EXPAND((__VA_ARGS__))) {       \
            pt_void(__COUNTER__-__TRPC_COUNTER_BASE-1, ret, VARCOUNT(__VA_ARGS__), PTX_EXPAND((__VA_ARGS__)))    \
        }

        #define RPC_FUNC_PROXY0(ret, name, ...) \
        TinyRPC::Promise<ret> name(FN_DEF_EXPAND((__VA_ARGS__))) {       \
            pt_nonvoid(__COUNTER__-__TRPC_COUNTER_BASE-1, ret, VARCOUNT(__VA_ARGS__), PTX_EXPAND((__VA_ARGS__))) \
        }

        #define RPC_LIST_START                              \
            const int __TRPC_COUNTER_BASE = __COUNTER__;    \
            namespace RPC { 

        #define RPC_LIST_END                                \
            };

        #define RPC_FUNC(ret, name, ...) \
            GLUE(RPC_FUNC_PROXY, IS_VOID(_ ##ret))(ret, name, __VA_ARGS__)
    #endif

}

#endif