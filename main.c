#include <stdio.h>
#include "ThinkGearStreamParser.h"
#include "ThinkGearStreamParser.c"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/**
 * 1) Function which acts on the value[] bytes of each ThinkGear DataRow as it is received.
 */
void
handleDataValueFunc( unsigned char extendedCodeLevel,
                     unsigned char code,
                     unsigned char valueLength,
                     const unsigned char *value,
                     void *customData ) {

    printf("%d \n",extendedCodeLevel);
    if( extendedCodeLevel == 0 ) {

        switch( code ) {

            /* [CODE]: ATTENTION eSense */
            case( 0x04 ):
                printf( "Attention Level: %d\n", value[0] & 0xFF );
                break;

                /* [CODE]: MEDITATION eSense */
            case( 0x05 ):
                printf( "Meditation Level: %d\n", value[0] & 0xFF );
                break;

//                /* Other [CODE]s */
            default:
                printf( "EXCODE level: %d CODE: 0x%02X vLength: %d\n",
                        extendedCodeLevel, code, valueLength );
                printf( "Data value(s):" );
                int i;
                for( i=0; i<valueLength; i++ ) printf( " %02X", value[i] & 0xFF );
                printf( "\n" );
        }
    }
}

/**
 * Program which reads ThinkGear Data Values from a COM port.
 */
int
main( int argc, char **argv ) {


    int fd; /* port file descriptor */
    char port[20] = "/dev/ttyUSB0"; /* port to connect to */
    speed_t baud = B57600; /* baud rate */

    fd = open(port, O_RDWR); /* connect to port */

/* set the other settings (in this case, 9600 8N1) */
    struct termios settings;
    tcgetattr(fd, &settings);

    cfsetospeed(&settings, baud); /* baud rate */
    settings.c_cflag &= ~PARENB; /* no parity */
    settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
    settings.c_cflag &= ~CSIZE;
    settings.c_cflag |= CS8 | CLOCAL; /* 8 bits */
    settings.c_lflag = ICANON; /* canonical mode */
    settings.c_oflag &= ~OPOST; /* raw output */

    tcsetattr(fd, TCSANOW, &settings); /* apply the settings */
    tcflush(fd, TCOFLUSH);

/* — code to use the port here — */



    /* 2) Initialize ThinkGear stream parser */
    ThinkGearStreamParser parser;
    THINKGEAR_initParser( &parser, PARSER_TYPE_PACKETS,
                          handleDataValueFunc, NULL );

    /* TODO: Initialize 'stream' here to read from a serial data
     * stream, or whatever stream source is appropriate for your
     * application.  See documentation for "Serial I/O" for your
     * platform for details.
     */
    FILE *stream = fopen( "/dev/ttyUSB0", "r" );

    /* 3) Stuff each byte from the stream into the parser.  Every time
     *    a Data Value is received, handleDataValueFunc() is called.
     */
    unsigned char streamByte;
    while( 1 ) {
        fread( &streamByte, 1, 1, stream );
        printf("0x%02X\n", streamByte);
        THINKGEAR_parseByte( &parser, streamByte );
    }
}