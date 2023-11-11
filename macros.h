
#ifndef MACROS_H
#define MACROS_H


#define set_bit(REG , BIT)   REG |=  (1 << (BIT))

#define clear_bit(REG , BIT) REG &= ~(1 << (BIT))

#define toggle_bit(REG, BIT) REG ^=  (1 << (BIT))

#define write_field(register_name, start_position, field_size, field_value) (register_name) = (((register_name) & (~(((1<<(field_size))-1)<< (start_position) ))) | ((((field_value)) <<(start_position))))

#define read_field(register_name, start_position, field_size) (((register_name) & ((((1<<(field_size))-1))<< (start_position) )) >> (start_position)) 



#endif