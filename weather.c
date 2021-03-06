#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/in.h>

struct wxdata {
    uint8_t ack;
    uint8_t magic[3];
    int8_t barTrend;
    uint8_t packetType;
    uint16_t nextRecord;
    uint16_t barometer;
    int16_t insideTemp;
    uint8_t insideHumidity;
    int16_t outsideTemp;
    uint8_t windSpeed;
    uint8_t windSpeedAvg;
    uint16_t windDirection;
    uint8_t extraTemperatures[7];
    uint8_t soilTemperatures[4];
    uint8_t leafTemperatures[4];
    uint8_t outsideHumidity;
    uint8_t extraHumidities[7];
    uint16_t rainRate;
    uint8_t uvIndex;
    uint16_t solarRadiation;
    uint16_t stormRain;
    uint16_t stormStart;
    uint16_t dayRain;
    uint16_t monthRain;
    uint16_t yearRain;
    uint16_t dayET;
    uint16_t monthET;
    uint16_t yearET;
    uint8_t soilMoistures[4];
    uint8_t leafWetnesses[4];
    uint8_t insideAlarms;
    uint8_t rainAlarms;
    uint8_t outsideAlarms[2];
    uint8_t extraTempHumAlarms[8];
    uint8_t soilLeafAlarms[4];
    uint8_t transmitterBatteryStatus;
    uint16_t consoleVoltage;
    uint8_t forecastIcons;
    uint8_t forecastRuleNumber;
    uint16_t timeSunrise;
    uint16_t timeSunset;
    uint8_t lineEndings[2];
    uint16_t crc;
} __attribute__ ((packed));

typedef enum {
    revA, revB
} packetversion_t;

unsigned short crc_table [] = {
    0x0,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
    0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
    0x1231,  0x210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
    0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
    0x2462,  0x3443,  0x420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
    0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
    0x3653,  0x2672,  0x1611,  0x630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
    0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
    0x48c4,  0x58e5,  0x6886,  0x78a7,  0x840,  0x1861,  0x2802,  0x3823,
    0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
    0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0xa50,  0x3a33,  0x2a12,
    0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
    0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0xc60,  0x1c41,
    0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
    0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0xe70,
    0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
    0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
    0x1080,  0xa1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
    0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
    0x2b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
    0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
    0x34e2,  0x24c3,  0x14a0,  0x481,  0x7466,  0x6447,  0x5424,  0x4405,
    0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
    0x26d3,  0x36f2,  0x691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
    0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
    0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x8e1,  0x3882,  0x28a3,
    0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
    0x4a75,  0x5a54,  0x6a37,  0x7a16,  0xaf1,  0x1ad0,  0x2ab3,  0x3a92,
    0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
    0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0xcc1,
    0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
    0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0xed1,  0x1ef0
};

int main(int argc, const char** argv) {
    
	if(argc < 2)return EXIT_FAILURE;
	
    const char loopMessage[] = "LOOP 1\n";
    
	const int port  = 22222;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[150];
    const int boolean_true = 1;
    const char tos = IPTOS_LOWDELAY;
    
	if(fd < 0) {
		printf("Error opening socket: %s\n", strerror(errno));
		return EXIT_FAILURE;	
	}
	
	server = gethostbyname(argv[1]);
	if(server == NULL) {
		printf("Error finding host %s: %s\n", argv[1], strerror(errno));
		return EXIT_FAILURE;
	}
	
	memset(&serv_addr,0x00,sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(port);
    
    setsockopt(fd,IPPROTO_IP,IPTOS_LOWDELAY,(char *)&boolean_true,sizeof(int));
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char *)&boolean_true,sizeof(int));
	setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(char *)&boolean_true,sizeof(int));
    
    memset(&buffer,0x00,sizeof(buffer));
    
    if(connect(fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in)) < 0) {
        printf("Error connecting to %s: %s\n", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }
    
    if(send(fd,&loopMessage,sizeof(loopMessage), 0) < 0) {
        printf("Error sending request to console: %s\n", strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }
    
    size_t n = read(fd,&buffer,sizeof(buffer));

    struct wxdata* data;
    packetversion_t version;
    
    if(n != sizeof(struct wxdata)) {
        printf("Data received from console is of invalid size. (Expected %lu, got %lu)\n", sizeof(struct wxdata), n);
        close(fd);
        return EXIT_FAILURE;
    }
    
    data = (struct wxdata *)&buffer;
    
    //printf("%x\n%x\n", (uint32_t)&buffer,(uint32_t)data);
    
    if(data->ack != 0x06 || data->magic[0] != 'L' || data->magic[1] != 'O' || data->magic[2] != 'O') {
        printf("The data received contained an invalid magic header.\n");
        close(fd);
        return EXIT_FAILURE;
    }
    
    if(data->barTrend == 'P')version = revA;
    else version = revB;
/*    
    uint16_t crc = 0;
	uint16_t *buffer_16 = buffer;
    
    for(size_t a = 1; a < sizeof(struct wxdata); a++) {
        crc = crc_table[(crc >> 8) ^ buffer_16[a]] ^ (crc << 8);
    }
    
    if(crc != 0x0000) {
        printf("CRC mismatch found, got 0x%x\n", data->crc);
        close(fd);
        return EXIT_FAILURE;
    }
    
  */  
    printf("%.1f\n", data->outsideTemp/10.0f);
    close(fd);
    return 0;
    
}
