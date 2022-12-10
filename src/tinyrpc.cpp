/*
    Author: Arthur Ames

    The main source file for the tinyRPC library.
    Written using normal C, because I want to use for other non-cpp projects.
*/

#include <tinyrpc.h>

static HardwareSerial* trpc_serial;
extern uint8_t __start_trpc_stubs;
extern uint8_t __stop_trpc_stubs;

void TinyRPC::init(HardwareSerial* serial) {
    trpc_serial = serial;
    trpc_serial->setTimeout(5);
    while(!trpc_serial) {
        Serial.println("TRPC Serial failed to init!");
    }
}

#ifdef TINYRPC_SERVER

void TinyRPC::listenServe() {
    uint8_t code;
    if(trpc_serial->available()) {
        trpc_serial->readBytes(&code, 1);
        if(code == 0 || code > (&__stop_trpc_stubs - &__start_trpc_stubs) / sizeof(TinyRPC::RPCStubMap)) return;

        uint8_t* stub = (&__start_trpc_stubs)+code*sizeof(RPCStubMap);

        uint8_t nb = pgm_read_byte(stub+2);

        uint8_t args[nb] __attribute__ ((aligned (4)));
        trpc_serial->readBytes(args, nb);
        
        // Hacky Checksum, but fast and should provide protection against random bytes messing with stuff.
        uint8_t sum = code;
        uint8_t read = trpc_serial->readBytes(&sum, 1);
        for(int i = 0; i < nb; i++) {
            sum ^= args[i];
        }

        if(sum != code || !read) return;
        Serial.println("Calling " + String(code));
        
        RPCFStub fn = (RPCFStub)pgm_read_word(stub);
        fn((RPCArgPtr)args);
    }
}

void TinyRPC::ret_stub(uint8_t s, void* v) {
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

void TinyRPC::ptx(uint8_t id, uint8_t nbargs, ...) {
    flush_serial_buffer();
    va_list args;
    va_start(args,nbargs);
    trpc_serial->write(id);
    uint8_t sum = id;
    for(int i = 0; i < nbargs; i++) {
        uint32_t s = va_arg(args, uint32_t);
        uint32_t a = va_arg(args, uint32_t);
        for(int j = 0; j < s; j++) {
            trpc_serial->write(*(((uint8_t*)&a)+j));
            sum ^= *(((uint8_t*)&a)+j);
        }
    }
    trpc_serial->write(sum);
    va_end(args);
}

void TinyRPC::recvRet(uint8_t s, void* retval) {
    while(trpc_serial->available() < s) {}
    trpc_serial->readBytes((char*)retval, s);
}

#endif