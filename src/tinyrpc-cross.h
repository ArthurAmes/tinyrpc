/* 
    Author: Arthur Ames

    This header defines the shared data types across both sides of the connection.
    The same file should be used for both sides of a shared connection.
*/

#ifndef _TINYRPC_CROSS
#define _TINYRPC_CROSS

#define RPC_CHAR       uint8_t 
#define RPC_SHORT      uint16_t
#define RPC_INT        uint16_t
#define RPC_LONG       uint32_t
#define RPC_FLOAT      float
#define RPC_VOID       void
#define RPC_BOOL       uint8_t

// #define RPC_PTR        uint32_t

#endif