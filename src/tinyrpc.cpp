/*
    Author: Arthur Ames

    The main source file for the tinyRPC library.
    Written using (mostly) normal C (want to use for other non cpp projects, but lambdas are convienent in this case.)
*/

#include <tinyrpc.h>


static HardwareSerial* trpc_serial;

void trpc_init(HardwareSerial* serial) {
    trpc_serial = serial;
    while(!trpc_serial) {}
}

#ifdef TINYRPC_SERVER

void trpc_listenServe() {
    uint8_t code;
    if(trpc_serial->available()) {
        trpc_serial->readBytes(&code, 1);
        if(code == 0) return;

        uint8_t nb = g_trpc_STUBS[code].nargs;
        uint8_t args[nb] __attribute__ ((aligned (4)));
        trpc_serial->readBytes(args, nb);
        g_trpc_STUBS[code].stub((RPCArgPtr)args);
    }
}

void trpc_ret_stub(uint8_t s, uint32_t* v) {
    for(int i = 0; i < s; i++) {
        trpc_serial->write(*(((uint8_t*)v)+i));
    }
}

#endif

#ifdef TINYRPC_CLIENT

void ptx(uint8_t id, uint8_t nbargs, va_list args) {
    trpc_serial->write(id);
    for(int i = 0; i < nbargs; i++) {
        uint32_t s = va_arg(args, uint32_t);
        uint32_t a = va_arg(args, uint32_t);
        for(int j = 0; j < s; j++) {
            trpc_serial->write(*(((uint8_t*)&a)+j));
        }
    }
}

uint32_t trpc_packTransmit(uint8_t id, uint8_t retnb, uint8_t nbargs, ...) {
    va_list args;
    va_start(args, nbargs);

    ptx(id, nbargs, args);

    va_end(args);

    uint32_t ret = 0;
    trpc_serial->readBytes((uint8_t*)&ret, retnb);
    return ret;
}

void trpc_packTransmitVoid(uint8_t id, uint8_t retnb, uint8_t nbargs, ...) {
    va_list args;
    va_start(args, nbargs);

    ptx(id, nbargs, args);

    va_end(args);
}

#endif