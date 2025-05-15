// Optimized modular addition (XOR)
modAddOptimized:
        ...null checks
        // Initialize word offset
        mov     x4, 0
// Word processing loop
        ...byte processing loops
.L2:
        // Calculate next word offset
        add     w6, w4, 4
        // Check if enough bytes remain for a word
        cmp     w6, w3
        // If yes, process a word
        bls     .L3
        // Adjust pointers for byte processing
        add     x0, x0, w4, uxtw
        add     x1, x1, w4, uxtw
        add     x2, x2, w4, uxtw
        // Initialize byte offset
        mov     x5, 0
        ...byte processing loop
// Process one word
.L3:
        ldr     w5, [x1, x4]    // Load word from a
        ldr     w7, [x2, x4]    // Load word from b
        eor     w5, w5, w7      // XOR words
        str     w5, [x0, x4]    // Store result word
        uxtw    x4, w6          // Move to next word offset
        b       .L2             // Loop back
