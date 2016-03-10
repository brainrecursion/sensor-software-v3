/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define THE_BAUD_RATE 9600

/* TODO Application specific user parameters used in user.c may go here */

#define long_payload_size 13
typedef struct {
    uint16_t millivolts;
    uint16_t rf_max_rt;
    uint16_t rf_timeout;
    uint16_t dht_csum;
    uint16_t dht_timeout;
    uint16_t count;
    uint8_t nodeId;
} Long_Payload;
extern Long_Payload long_payload;

#define short_payload_size 9
typedef struct {
    float temp;
    uint16_t count;
    uint8_t humidity;
    uint8_t light;
    uint8_t nodeId;
} Short_Payload;
extern Short_Payload short_payload;


/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void); /* I/O and Peripheral Initialization */
