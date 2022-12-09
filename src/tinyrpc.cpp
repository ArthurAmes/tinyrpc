/*
    Author: Arthur Ames

    The main source file for the tinyRPC library.
    Written using normal C, because I want to use for other non-cpp projects.
*/

#include <tinyrpc.h>

static HardwareSerial* trpc_serial;

void trpc_init(HardwareSerial* serial) {
    trpc_serial = serial;
    trpc_serial->setTimeout(9999999);
    while(!trpc_serial) {
        Serial.println("TRPC Serial failed to init!");
    }
}

#ifdef TINYRPC_SERVER

void trpc_listenServe() {
    uint8_t code;
    if(trpc_serial->available()) {
        trpc_serial->readBytes(&code, 1);
        if(code == 0) return;
        Serial.println(String("Read code: ") + String(code));

        extern uint8_t __start_trpc_stubs;
        uint8_t* stub = (&__start_trpc_stubs)+code*sizeof(RPCStubMap);

        uint8_t nb = pgm_read_byte(stub+2);

        uint8_t args[nb] __attribute__ ((aligned (4)));
        trpc_serial->readBytes(args, nb);
        
        RPCFStub fn = (RPCFStub)pgm_read_word(stub);
        fn((RPCArgPtr)args);
    }
}

void trpc_ret_stub(uint8_t s, void* v) {
    for(int i = 0; i < s; i++) {
        trpc_serial->write(*(((uint8_t*)v)+i));
    }
}

#endif

#ifdef TINYRPC_CLIENT

void flush_serial_buffer() {
    int x = trpc_serial->available();
    for(int i = 0; i < x; i++) {
        trpc_serial->read();
    }
}

void trpc_ptx(uint8_t id, uint8_t nbargs, ...) {
    flush_serial_buffer();
    va_list args;
    va_start(args,nbargs);
    trpc_serial->write(id);
    for(int i = 0; i < nbargs; i++) {
        uint32_t s = va_arg(args, uint32_t);
        uint32_t a = va_arg(args, uint32_t);
        for(int j = 0; j < s; j++) {
            trpc_serial->write(*(((uint8_t*)&a)+j));
        }
    }
    va_end(args);
}

void trpc_recvRet(uint8_t s, void* retval) {
    trpc_serial->readBytes((char*)retval, s);
}

#endif