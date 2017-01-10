/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define THE_BAUD_RATE 9600

#define TEMP_HUM_DATA 3
#define TEMP_HUM_LIGHT_DATA 4
#define STATUS_DATA 5

typedef struct {
    uint8_t msg_id;
    uint8_t nodeId;
    uint16_t count;
} Header;

#define status_payload_size 11
typedef struct {
    Header header;
    uint8_t sw_version;
    uint16_t millivolts;
    uint16_t rf_error;
    uint16_t sensor_err;
} Status_Payload;

#define temp_hum_light_payload_size 10
typedef struct {
    Header header;
    float temperature;
    uint8_t humidity;
    uint8_t light;
} TEMP_HUM_LIGHT_Payload;


/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void); /* I/O and Peripheral Initialization */
