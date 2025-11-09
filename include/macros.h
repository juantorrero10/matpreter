#ifndef MACROS_H_
#define MACROS_H_

#define _out_
#define MAX_VARIABLE 4

//token catergory
#define TC_START_IDX 0
#define TC_OPERATIONS_IDX 32
#define TC_LITERALS_IDX 64

#define mp_isoperation(token) (token.type < TC_LITERALS_IDX && token.type >= TC_OPERATIONS_IDX)


#endif // MACROS_H_
