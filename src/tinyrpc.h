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

        #define rt_10(ret, name, ...) ((ret(*)(__VA_ARGS__))name)MAP(fargs, (__VA_ARGS__)); /* If the return type is void, this stub is called. */
        #define rt_00(ret, name, ...) ret x = ((ret(*)(__VA_ARGS__))name)MAP(fargs, (__VA_ARGS__)); TinyRPC::ret_stub(sizeof(ret), (void*)&x); /* If the return type is not void, this stub is called */
        #define rt_11(ret, name, ...) ((ret(*)(__VA_ARGS__))name)(); /* If the return type is void, this stub is called. */
        #define rt_01(ret, name, ...) ret x = ((ret(*)(__VA_ARGS__))name)(); TinyRPC::ret_stub(sizeof(ret), (void*)&x); /* If the return type is not void, this stub is called */

        #define RPC_LIST_START
        #define RPC_LIST_END

        #define RPC_FUNC(ret, name, ...)                                                                                \
            __rpcstub__ ##name(TinyRPC::RPCArgPtr fargs) { GLUE(GLUE(rt_, IS_VOID(_##ret)), IS_VOID(GLUE(_,FIRST(__VA_ARGS__))))(ret, name, __VA_ARGS__) } \
            static const TinyRPC::RPCStubMap stubentry_##name                                                           \
            __attribute((used, section("trpc_stubs"))) = {                                                              \
                .stub = __rpcstub__ ##name,                                                                             \
                .nargs = 0 MAPT_XX(+ sizeof, (__VA_ARGS__)) };

    #endif

    #ifdef TINYRPC_CLIENT
        extern bool call_mtx;
        extern void* last_p;
        extern uint8_t last_s; // we need to save the last size globally in case the promise goes out of scope before we can resolve it.

        void ptx(uint8_t id, uint8_t nbargs, ...);
        void recvRet(uint8_t s, void* retval);
        void resolve_last();


        /* 
            Promise kind of has weird semantics because since the atmega can only execute 1 thing at a time,
            we can only have at most 1 unresolved promise at a time. But, for our purposes, it's Good Enough (TM)
        */

        template <typename T>
        class Promise {
        private:
            T value;

        public:
            Promise<T>() {
                last_p = this;
                last_s = sizeof(T);
            }

            ~Promise<T>() {
                if(last_p == this)
                    last_p = nullptr; // we've gone out of scope, so we need to indicate that this is no longer a valid promise to write to.
            }

            T& await() {
                if(last_p == this && last_s == 0) // If this was the last promise, and size is 0, that means we've already been resolved.
                    return (T&)(this->value);

                TinyRPC::recvRet(sizeof(T), &value);
                return (T&)(this->value);
            };
        };

        #define pt_v_nv(id, ret, nbargs, ...) \
            TinyRPC::ptx(id, nbargs, __VA_ARGS__);

        #define pt_nv_nv(id, ret, nbargs, ...) \
            TinyRPC::ptx(id, nbargs, __VA_ARGS__); \
            TinyRPC::Promise<ret> x;               \
            return x;                              \

        #define pt_v_v(id, ret, nbargs, ...) \
            TinyRPC::ptx(id, nbargs);

        #define pt_nv_v(id, ret, nbargs, ...) \
            TinyRPC::ptx(id, nbargs); \
            TinyRPC::Promise<ret> x;               \
            return x;                              \

        #define RPC_FUNC_PROXY10(ret, name, ...) \
        void name(FN_DEF_EXPAND((__VA_ARGS__))) {       \
            pt_v_nv(__COUNTER__-__TRPC_COUNTER_BASE-1, ret, VARCOUNT(__VA_ARGS__), PTX_EXPAND((__VA_ARGS__)))    \
        }

        #define RPC_FUNC_PROXY00(ret, name, ...) \
        TinyRPC::Promise<ret> name(FN_DEF_EXPAND((__VA_ARGS__))) {       \
            pt_nv_nv(__COUNTER__-__TRPC_COUNTER_BASE-1, ret, VARCOUNT(__VA_ARGS__), PTX_EXPAND((__VA_ARGS__))) \
        }

        #define RPC_FUNC_PROXY11(ret, name, ...) \
        void name() {       \
            pt_v_v(__COUNTER__-__TRPC_COUNTER_BASE-1, ret, 0)    \
        }

        #define RPC_FUNC_PROXY01(ret, name, ...) \
        TinyRPC::Promise<ret> name() {       \
            pt_nv_v(__COUNTER__-__TRPC_COUNTER_BASE-1, ret, 0) \
        }


        #define RPC_LIST_START                              \
            const int __TRPC_COUNTER_BASE = __COUNTER__;    \
            namespace RPC { 

        #define RPC_LIST_END                                \
            };

        #define RPC_FUNC(ret, name, ...) \
            GLUE(GLUE(RPC_FUNC_PROXY, IS_VOID(_ ##ret)), IS_VOID(GLUE(_, FIRST(__VA_ARGS__))))(ret, name, __VA_ARGS__)
    #endif

}

#endif