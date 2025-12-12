#include <stdio.h>
#include "shell.h"

int main(void){
    shell_t sh; shell_init(&sh);

    printf("\n=== Test 1: Basic commands ===\n");
    shell_rx_bytes(&sh, "get\r\n");
    for(int i=0;i<5;i++) shell_tick(&sh);

    shell_rx_bytes(&sh, "set 0.42\r\n");
    for(int i=0;i<5;i++) shell_tick(&sh);

    shell_rx_bytes(&sh, "stat\r\n");
    for(int i=0;i<5;i++) shell_tick(&sh);

    shell_rx_bytes(&sh, "echo hello world\r\n");
    for(int i=0;i<5;i++) shell_tick(&sh);

    printf("\n=== Test 2: Partial command + burst (should break line) ===\n");
    // Send incomplete command
    shell_rx_bytes(&sh, "set 1.5");  // No \r\n yet
    for(int i=0;i<2;i++) shell_tick(&sh);
    
    // Then burst that will overflow
    for(int i=0;i<200;i++) shell_rx_bytes(&sh, "noop\r\n");
    for(int i=0;i<300;i++) shell_tick(&sh);

    printf("After burst: dropped=%zu, broken=%u\n", sh.rx.dropped, sh.broken_lines);

    // Try to complete the original command
    shell_rx_bytes(&sh, "\r\n");
    for(int i=0;i<5;i++) shell_tick(&sh);

    printf("\nFinal: dropped=%zu, broken=%u\n", sh.rx.dropped, sh.broken_lines);

    return 0;
}
