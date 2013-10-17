/*
 * In this file we prototype all the system functions and declare global
 * vars and data structures.  We define types that are used for all variables.
 * These are declared for consistency because compilers often have a different
 * sizes for int and long (ie, to make porting to other compilers/architectures easier).
 */
#ifndef AX25_H
#define    AX25_H

void configDefault(); // Configure the TNC
void tncPreparePacket(char * message); // Prepare a packet to send
void tncSendPacket(void); // Send a packet via the 4 bit DAC
void sysInit(void); // Initializes all the uC peripherals
void calTones(unsigned bitValue); // generate a mark or space tone to allow calibration
uint16_t sysCRC16(uint8_t *buffer, uint16_t length); // Generate a 16 bit CRC

/*
 * Declare global vars and data structures
 */
/// Configuration Structure

typedef struct {
    /// Sets how many sync flags we'll send at the beginning of the packet
    uint8_t txDelay;
    /// The Beacon's Callsign
    uint8_t callSign[7];
    /// Destination Callsign
    uint8_t destCallSign[7];
    /// Relay Callsign1
    uint8_t relayCallSign1[7];
    /// Relay Callsign2
    uint8_t relayCallSign2[7];
    /// SSID of our callsign
    uint8_t callSignSSID;
    /// SSID to be used for landing prediction packets
    uint8_t callSignLandingZoneSSID;
    /// SSID to be used for the first relay
    uint8_t relayCallSignSSID1;
    /// SSID to be used with the second relay
    uint8_t relayCallSignSSID2;
    /// Keeps track of the total flight time
    uint16_t flightTime;
} CONFIG_STRUCT;

/// GPS data structure

typedef struct {
    /// Set when a packet has successfully been received from the GPS
    uint8_t updateFlag;
    /// GPS month
    uint8_t month;
    /// GPS day
    uint8_t day;
    /// GPS hour
    uint8_t hours;
    /// GPS minute
    uint8_t minutes;
    /// GPS second
    uint8_t seconds;
    /// GPS year
    uint16_t year;
    /// Latitude in milli-arc seconds (MAS)
    int32_t latitude;
    /// Longitude in MAS
    int32_t longitude;
    /// Altitude in meters
    int32_t altitudeMeters;
    /// Altitude in ft
    int32_t altitudeFeet;
    /// Ascent rate in ft/s
    uint16_t vSpeed;
    /// Groundspeed in mph
    uint16_t hSpeed;
    /// Heading in deg off true north
    uint16_t heading;
    /// Horizonal Diolution of position
    uint16_t hdop;
    /// Vertical Diolution of position
    uint16_t vdop;
    /// Number of SVs used in fix
    uint16_t trackedSats;
    /// Last logged altitude.  Used in ascent/descent rate calculation
    int32_t lastAltitude;
    /// Accumulator used in ascent/descent rate LPF
    int16_t vSpeedAccum;
} GPSPOSITION_STRUCT;

/// A complete NMEA string has been received on the UART
#define GPS_COMPLETE_STRING 0
/// Waiting for an NMEA string to complete
#define GPS_RECEIVING_STRING 1
/// in console mode
#define GPS_CONSOLE_MODE 2
/// The maximum number of characters we can send through the TNC
#define TNC_MAX_TX 128
/// The maximum number of characters we can receive through the UART
#define SER_MAX_RX 128
/// The TNC is currently not sending
#define TNC_RX_FLAG 0
/// We're currently preparing a packet to send
#define TNC_TX_PREPARE 3
/// We're sending sync flags to start the packet(0x7E)
#define TNC_TX_SYNC 4
/// We're sending the packet data
#define TNC_TX_DATA 5
/// We're closing the packet
#define TNC_TX_END 6
/// Used here and there :-)
#define FALSE 0

extern uint16_t secCount;
extern uint8_t gpsMode, serIndex;
extern uint8_t serBuffer[SER_MAX_RX];


/**
 * 1200 Hz tone for a mark.  Calculated by (Fosc/4/2)/(1200*16)
 */
#define     MARK     208
/**
 * 2200 Hz tone for a space.  Calculated by (Fosc/4/2)/(2200*16)
 */
#define        SPACE    113
/**
 * 1200 Baud.  In units of timer 2 (no pre, post scalar 1:2), so calculated by (Fosc/4/2)/(1200)
 */
#define        BAUD    3333

#endif /* AX25_H */