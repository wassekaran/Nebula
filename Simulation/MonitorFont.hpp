#pragma once

namespace nebula {

namespace sim {

const std::array<std::pair<Word, Word>, 128> MONITOR_DEFAULT_FONT {{
        // NULL
        { B_( 10110111
              10011110 ),
          B_( 00111000
              10001110 ) },

        // SOH
        { B_( 01110010
              00101100 ),
          B_( 01110101
              11110100 ) },

        // STX
        { B_( 00011001
              10111011 ),
          B_( 01111111
              10001111 ) },

        // ETX
        { B_( 10000101
              11111001 ),
          B_( 10110001
              01011000 ) },

        // EOT
        { B_( 00100100
              00101110 ),
          B_( 00100100
              00000000 ) },

        // ENQ
        { B_( 00001000
              00101010 ),
          B_( 00001000
              00000000 ) },

        // ACK
        { B_( 00000000
              00001000 ),
          B_( 00000000
              00000000 ) },

        // BEL
        { B_( 00001000
              00001000 ),
          B_( 00001000
              00001000 ) },

        // BS
        { B_( 00000000
              11111111 ),
          B_( 00000000
              00000000 ) },

        // TAB
        { B_( 00000000
              11111000 ),
          B_( 00001000
              00001000 ) },

        // LF
        { B_( 00001000
              11111000 ),
          B_( 00000000
              00000000 ) },

        // VT
        { B_( 00001000
              00001111 ),
          B_( 00000000
              00000000 ) },

        // FF
        { B_( 00000000
              00001111 ),
          B_( 00001000
              00001000 ) },

        // CR
        { B_( 00000000
              11111111 ),
          B_( 00001000
              00001000 ) },

        // SO
        { B_( 00001000
              11111000 ),
          B_( 00001000
              00001000 ) },

        // SI
        { B_( 00001000
              11111111 ),
          B_( 00000000
              00000000 ) },

        // DLE
        { B_( 00001000
              00001111 ),
          B_( 00001000
              00001000 ) },

        // DC1
        { B_( 00001000
              11111111 ),
          B_( 00001000
              00001000 ) },

        // DC2
        { B_( 01100110
              00110011 ),
          B_( 10011001
              11001100 ) },

        // DC3
        { B_( 10011001
              00110011 ),
          B_( 01100110
              11001100 ) },

        // DC4
        { B_( 11111110
              11111000 ),
          B_( 11100000
              10000000 ) },

        // NAK
        { B_( 01111111
              00011111 ),
          B_( 00000111
              00000001 ) },

        // SYN
        { B_( 00000001
              00000111 ),
          B_( 00011111
              01111111 ) },

        // ETB
        { B_( 10000000
              11100000 ),
          B_( 11111000
              11111110 ) },

        // CAN
        { B_( 01010101
              00000000 ),
          B_( 10101010
              00000000 ) },

        // EM
        { B_( 01010101
              10101010 ),
          B_( 01010101
              10101010 ) },

        // SUB
        { B_( 11111111
              10101010 ),
          B_( 11111111
              01010101 ) },            

        // ESC
        { B_( 00001111
              00001111 ),
          B_( 00001111
              00001111 ) },

        // FS
        { B_( 11110000
              11110000 ),
          B_( 11110000
              11110000 ) },

        // GS
        { B_( 00000000
              00000000 ),
          B_( 11111111
              11111111 ) },

        // RS
        { B_( 11111111
              11111111 ),
          B_( 00000000
              00000000 ) },

        // US
        { 0xffff, 0xffff },

        // Space
        { 0, 0 },

        // !
        { B_( 00000000
              10111111 ),
          B_( 00000000
              00000000 ) },

        // "
        { B_( 00000011
              00000000 ),
          B_( 00000011
              00000000 ) },

        // #
        { B_( 00111110
              00010100 ),
          B_( 00111110
              00000000 ) },

        // $
        { B_( 01001100
              11010110 ),
          B_( 01100100
              00000000 ) },

        // %
        { B_( 11000010
              00111000 ),
          B_( 10000110
              00000000 ) },

        // &
        { B_( 01101100
              01010010 ),
          B_( 11101100
              10100000 ) },

        // '
        { B_( 00000000
              00000010 ),
          B_( 00000001
              00000000 ) },

        // (
        { B_( 00111100
              01000010 ),
          B_( 10000001
              00000000 ) },

        // )
        { B_( 10000001
              01000010 ),
          B_( 00111100
              00000000 ) },

        // *
        { B_( 00001010
              00000100 ),
          B_( 00001010
              00000000 ) },

        // +
        { B_( 00001000
              00011100 ),
          B_( 00001000
              00000000 ) },

        // ,
        { B_( 00000000
              10000000 ),
          B_( 01000000
              00000000 ) },

        // -
        { B_( 00001000
              00001000 ),
          B_( 00001000
              00000000 ) },

        // .
        { B_( 00000000
              10000000 ),
          B_( 00000000
              00000000 ) },

        // /
        { B_( 11000000
              00111000 ),
          B_( 00000110
              00000000 ) },

        // 0
        { B_( 01111100
              10010010 ),
          B_( 01111100
              00000000 ) },

        // 1
        { B_( 10000010
              11111110 ),
          B_( 10000000
              00000000 ) },

        // 2
        { B_( 11000100
              10100010 ),
          B_( 10011100
              00000000 ) },

        // 3
        { B_( 10000010
              10010010 ),
          B_( 01101100
              00000000 ) },

        // 4
        { B_( 00011110
              00010000 ),
          B_( 11111110
              00000000 ) },

        // 5
        { B_( 10011110
              10010010 ),
          B_( 01100010
              00000000 ) },

        // 6
        { B_( 01111100
              10010010 ),
          B_( 01100100
              00000000 ) },

        // 7
        { B_( 11000010
              00110010 ),
          B_( 00001110
              00000000 ) },

        // 8
        { B_( 01101100
              10010010 ),
          B_( 01101100
              00000000 ) },

        // 9
        { B_( 01001100
              10010010 ),
          B_( 01111100
              00000000 ) },

        // :
        { B_( 00000000
              01001000 ),
          B_( 00000000
              00000000 ) },

        // ;
        { B_( 00000000
              10000000 ),
          B_( 01001000
              00000000 ) },

        // <
        { B_( 00010000
              00101000 ),
          B_( 01000100
              00000000 ) },

        // =
        { B_( 00100100
              00100100 ),
          B_( 00100100
              00000000 ) },

        // >
        { B_( 01000100
              00101000 ),
          B_( 00010000
              00000000 ) },

        // ?
        { B_( 00000010
              10110001 ),
          B_( 00001110
              00000000 ) },

        // @
        { B_( 01111100
              10110010 ),
          B_( 10111100
              00000000 ) },

        // A
        { B_( 11111100
              00010010 ),
          B_( 11111100
              00000000 ) },

        // B
        { B_( 11111110
              10010010 ),
          B_( 01101100
              00000000 ) },

        // C
        { B_( 01111100
              10000010 ),
          B_( 01000100
              00000000 ) },

        // D
        { B_( 11111110
              10000010 ),
          B_( 01111100
              00000000 ) },

        // E
        { B_( 11111110
              10010010 ),
          B_( 10010010
              00000000 ) },

        // F
        { B_( 11111110
              00010010 ),
          B_( 00010010
              00000000 ) },

        // G
        { B_( 01111100
              10000010 ),
          B_( 11100100
              00000000 ) },

        // H
        { B_( 11111110
              00010000 ),
          B_( 11111110
              00000000 ) },

        // I
        { B_( 10000010
              11111110 ),
          B_( 10000010
              00000000 ) },

        // J
        { B_( 01000010
              10000010 ),
          B_( 11111110
              00000000 ) },

        // K
        { B_( 11111110
              00010000 ),
          B_( 11101110
              00000000 ) },

        // L
        { B_( 11111110
              10000000 ),
          B_( 10000000
              00000000 ) },

        // M
        { B_( 11111110
              00001100 ),
          B_( 11111110
              00000000 ) },

       // N
        { B_( 11111110
              00000010 ),
          B_( 11111100
              00000000 ) },

        // O
        { B_( 01111100
              10000010 ),
          B_( 01111100
              00000000 ) },

        // P
        { B_( 11111110
              00010010 ),
          B_( 00001100
              00000000 ) },

        // Q
        { B_( 01111100
              11000010 ),
          B_( 11111100
              00000000 ) },

        // R
        { B_( 11111110
              00010010 ),
          B_( 11101100
              00000000 ) },

        // S
        { B_( 10001100
              10010010 ),
          B_( 01100010
              00000000 ) },

        // T
        { B_( 00000010
              11111110 ),
          B_( 00000010
              00000000 ) },            

        // U
        { B_( 01111110
              10000000 ),
          B_( 01111110
              00000000 ) },

        // V
        { B_( 00111110
              11000000 ),
          B_( 00111110
              00000000 ) },

        // W
        { B_( 11111110
              01100000 ),
          B_( 11111110
              00000000 ) },

        // X
        { B_( 11101110
              00010000 ),
          B_( 11101110
              00000000 ) },

        // Y
        { B_( 00001110
              11110000 ),
          B_( 00001110
              00000000 ) },

        // Z
        { B_( 11100010
              10010010 ),
          B_( 10001110
              00000000 ) },

        // [
        { B_( 11111110
              10000010 ),
          B_( 00000000
              00000000 ) },

        // backslash
        { B_( 00000110
              00111000 ),
          B_( 11000000
              00000000 ) },

        // ]
        { B_( 00000000
              10000010 ),
          B_( 11111110
              00000000 ) },

        // ^
        { B_( 00000100
              00000010 ),
          B_( 00000100
              00000000 ) },

        // _
        { B_( 10000000
              10000000 ),
          B_( 10000000
              00000000 ) },

        // `
        { B_( 00000010
              00000100 ),
          B_( 00000000
              00000000 ) },

        // a
        { B_( 01001000
              10101000 ),
          B_( 11111000
              00000000 ) },

        // b
        { B_( 11111110
              10001000 ),
          B_( 01110000
              00000000 ) },

        // c
        { B_( 01110000
              10001000 ),
          B_( 01010000
              00000000 ) },

        // d
        { B_( 01110000
              10001000 ),
          B_( 11111110
              00000000 ) },

        // e
        { B_( 01110000
              10101000 ),
          B_( 10110000
              00000000 ) },

        // f
        { B_( 00010000
              11111100 ),
          B_( 00010010
              00000000 ) },

        // g
        { B_( 10010000
              10101000 ),
          B_( 01111000
              00000000 ) },

        // h
        { B_( 11111110
              00001000 ),
          B_( 11110000
              00000000 ) },

        // i
        { B_( 10001000
              11111010 ),
          B_( 10000000
              00000000 ) },

        // j
        { B_( 01000000
              10000000 ),
          B_( 01111010
              00000000 ) },

        // k
        { B_( 11111110
              00100000 ),
          B_( 11011000
              00000000 ) },

        // l
        { B_( 10000010
              11111110 ),
          B_( 10000000
              00000000 ) },

        // m
        { B_( 11111000
              00110000 ),
          B_( 11111000
              00000000 ) },

        // n
        { B_( 11111000
              00001000 ),
          B_( 11110000
              00000000 ) },

        // o
        { B_( 01110000
              10001000 ),
          B_( 01110000
              00000000 ) },

        // p
        { B_( 11111000
              00101000 ),
          B_( 00010000
              00000000 ) },

        // q
        { B_( 00010000
              00101000 ),
          B_( 11111000
              00000000 ) },

        // r
        { B_( 11111000
              00001000 ),
          B_( 00010000
              00000000 ) },

        // s
        { B_( 10010000
              10101000 ),
          B_( 01001000
              00000000 ) },

        // t
        { B_( 00001000
              11111100 ),
          B_( 10001000
              00000000 ) },

        // u
        { B_( 01111000
              10000000 ),
          B_( 11111000
              00000000 ) },

        // v
        { B_( 00111000
              11000000 ),
          B_( 00111000
              00000000 ) },

        // w
        { B_( 11111000
              01100000 ),
          B_( 11111000
              00000000 ) },

        // x
        { B_( 11011000
              00100000 ),
          B_( 11011000
              00000000 ) },

        // y
        { B_( 10011000
              10100000 ),
          B_( 01111000
              00000000 ) },

        // z
        { B_( 11001000
              10101000 ),
          B_( 10011000
              00000000 ) },

        // {
        { B_( 00010000
              01101100 ),
          B_( 10000010
              00000000 ) },

        // |
        { B_( 00000000
              11101110 ),
          B_( 00000000
              00000000 ) },

        // }
        { B_( 10000010
              01101100 ),
          B_( 00010000
              00000000 ) },

        // ~
        { B_( 00000100
              00000010 ),
          B_( 00000100
              00000010 ) },

        // DEL
        { B_( 00000010
              00000101 ),
          B_( 00000010
              00000000 ) },
} };

}

}
