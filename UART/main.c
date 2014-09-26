#include <main.h>
#include <memory_test.h>
#include <print.h>
#include <port.h>

int main(void)
{
    uint8 * generic_p;
    uint16 index;

    // "I'm alive" LED trigger to signal code entry
    GREENLED = 0;
    for(index = 0; index < 50000; index++);
    GREENLED = 1;
    generic_p = code_memory_init();
    print_memory_block(64, generic_p);
    generic_p = xdata_memory_init();
    print_memory_block(64, generic_p);
    return 0;
}
