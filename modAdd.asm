// Standard modular addition (XOR)
modAdd:
        ...null checks
.L6:
        ret
// Byte processing loop
.L8:
        ldrb    w5, [x1, x4]    // Load byte from a
        ldrb    w6, [x2, x4]    // Load byte from b
        eor     w5, w5, w6      // XOR bytes
        strb    w5, [x0, x4]    // Store result byte
        add     x4, x4, 1       // Increment byte offset
// Loop condition for byte processing
.L7:
        cmp     w3, w4          // Compare size with offset
        bhi     .L8             // If more bytes, loop
        b       .L6             // Else, return
// Initialize for byte processing
.L10:
        mov     x4, 0           // Set initial offset to 0
        b       .L7             // Start loop