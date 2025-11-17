#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <stdint.h>

#include "ecrt.h"

// EtherCAT
static ec_master_t *master = NULL;
static ec_domain_t *domain1 = NULL;
static uint8_t *domain1_pd = NULL;

static ec_slave_config_t *sc_ana_in = NULL;
static ec_slave_config_t *sc_dig_io = NULL;

// Analog Input Card (Slave 0)
#define ENGEL_ANA_IN  0, 0  // Alias, Position
#define ENGEL_ANA_VendorID    0x0000066b
#define ENGEL_ANA_ProductCode 0x0ea0c252

// Digital I/O Card EP2316-003 (Slave 1)
#define ENGEL_DIG_IO  0, 1  // Alias, Position
#define ENGEL_DIG_VendorID    0x0000066b
#define ENGEL_DIG_ProductCode 0x090c4052

// Offsets for Analog Input PDO entries
static unsigned int off_ana_in_ch1;
static unsigned int off_ana_in_ch2;
static unsigned int off_ana_in_ch3;
static unsigned int off_ana_in_ch4;

// Offsets for Digital I/O PDO entries
static unsigned int off_dig_in_byte;   // 8 digital inputs (1 byte starting at bit 0 of 0x6000:01)
static unsigned int off_dig_out_byte;  // 8 digital outputs (1 byte starting at bit 0 of 0x7000:01)

static unsigned int counter = 0;

// PDO entries
const static ec_pdo_entry_reg_t domain1_regs[] = {
    // Analog Input Card (Slave 0)
    {ENGEL_ANA_IN, ENGEL_ANA_VendorID, ENGEL_ANA_ProductCode, 0x6020, 0x11, &off_ana_in_ch1},
    {ENGEL_ANA_IN, ENGEL_ANA_VendorID, ENGEL_ANA_ProductCode, 0x6030, 0x11, &off_ana_in_ch2},
    {ENGEL_ANA_IN, ENGEL_ANA_VendorID, ENGEL_ANA_ProductCode, 0x6040, 0x11, &off_ana_in_ch3},
    {ENGEL_ANA_IN, ENGEL_ANA_VendorID, ENGEL_ANA_ProductCode, 0x6050, 0x11, &off_ana_in_ch4},
    // Digital I/O Card EP2316-003 (Slave 1) - Byte access to first bit captures all 8 bits
    {ENGEL_DIG_IO, ENGEL_DIG_VendorID, ENGEL_DIG_ProductCode, 0x6000, 0x01, &off_dig_in_byte},  // All 8 inputs
    {ENGEL_DIG_IO, ENGEL_DIG_VendorID, ENGEL_DIG_ProductCode, 0x7000, 0x01, &off_dig_out_byte}, // All 8 outputs
    {}
};

static int run = 1;

void signal_handler(int sig) {
    run = 0;
}

void cyclic_task()
{
    // Receive process data
    ecrt_master_receive(master);
    ecrt_domain_process(domain1);

    // Read analog values (32-bit signed integers)
    int32_t ch1 = EC_READ_S32(domain1_pd + off_ana_in_ch1);
    int32_t ch2 = EC_READ_S32(domain1_pd + off_ana_in_ch2);
    int32_t ch3 = EC_READ_S32(domain1_pd + off_ana_in_ch3);
    int32_t ch4 = EC_READ_S32(domain1_pd + off_ana_in_ch4);

    // Read digital inputs (1 byte with 8 bits)
    uint8_t dig_inputs = EC_READ_U8(domain1_pd + off_dig_in_byte);

    // Extract individual bits
    uint8_t dig_in[8];
    for (int i = 0; i < 8; i++) {
        dig_in[i] = (dig_inputs >> i) & 0x01;
    }

    // Read digital outputs control from file
    uint8_t dig_outputs = 0;
    FILE *out_file = fopen("/tmp/ethercat_outputs.txt", "r");
    if (out_file) {
        char line[100];
        if (fgets(line, sizeof(line), out_file)) {
            int values[8];
            if (sscanf(line, "%d,%d,%d,%d,%d,%d,%d,%d", 
                      &values[0], &values[1], &values[2], &values[3],
                      &values[4], &values[5], &values[6], &values[7]) == 8) {
                // Build output byte from individual bits
                for (int i = 0; i < 8; i++) {
                    if (values[i]) {
                        dig_outputs |= (1 << i);
                    }
                }
            }
        }
        fclose(out_file);
    }

    // Write digital outputs
    EC_WRITE_U8(domain1_pd + off_dig_out_byte, dig_outputs);

    // Print to console every 10 cycles
    if (counter++ % 10 == 0) {
        printf("\rCH1: %10d  CH2: %10d  CH3: %10d  CH4: %10d  |  DIG_IN: %d%d%d%d%d%d%d%d  OUT: %d%d%d%d%d%d%d%d", 
               ch1, ch2, ch3, ch4, 
               dig_in[0], dig_in[1], dig_in[2], dig_in[3],
               dig_in[4], dig_in[5], dig_in[6], dig_in[7],
               (dig_outputs & 0x01) ? 1 : 0,
               (dig_outputs & 0x02) ? 1 : 0,
               (dig_outputs & 0x04) ? 1 : 0,
               (dig_outputs & 0x08) ? 1 : 0,
               (dig_outputs & 0x10) ? 1 : 0,
               (dig_outputs & 0x20) ? 1 : 0,
               (dig_outputs & 0x40) ? 1 : 0,
               (dig_outputs & 0x80) ? 1 : 0);
        fflush(stdout);
    }

    // Write analog values to file for web dashboard
    FILE *data_file = fopen("/tmp/ethercat_data.txt", "w");
    if (data_file) {
        fprintf(data_file, "%d,%d,%d,%d\n", ch1, ch2, ch3, ch4);
        fclose(data_file);
    }

    // Write digital inputs to separate file
    FILE *dig_file = fopen("/tmp/ethercat_digital.txt", "w");
    if (dig_file) {
        fprintf(dig_file, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                dig_in[0], dig_in[1], dig_in[2], dig_in[3],
                dig_in[4], dig_in[5], dig_in[6], dig_in[7]);
        fclose(dig_file);
    }

    // Send process data
    ecrt_domain_queue(domain1);
    ecrt_master_send(master);
}

int main(int argc, char **argv)
{
    signal(SIGINT, signal_handler);
    
    printf("Starting EtherCAT master application...\n");

    // Request master
    master = ecrt_request_master(0);
    if (!master) {
        fprintf(stderr, "Failed to request master.\n");
        return -1;
    }

    // Create domain
    domain1 = ecrt_master_create_domain(master);
    if (!domain1) {
        fprintf(stderr, "Failed to create domain.\n");
        goto out_release_master;
    }

    // Configure slaves
    printf("Configuring analog input slave (0)...\n");
    if (!(sc_ana_in = ecrt_master_slave_config(master, ENGEL_ANA_IN, ENGEL_ANA_VendorID, ENGEL_ANA_ProductCode))) {
        fprintf(stderr, "Failed to get analog input slave configuration.\n");
        goto out_release_master;
    }

    printf("Configuring digital I/O slave (1)...\n");
    if (!(sc_dig_io = ecrt_master_slave_config(master, ENGEL_DIG_IO, ENGEL_DIG_VendorID, ENGEL_DIG_ProductCode))) {
        fprintf(stderr, "Failed to get digital I/O slave configuration.\n");
        goto out_release_master;
    }

    // Register PDO entries
    printf("Registering PDO entries...\n");
    if (ecrt_domain_reg_pdo_entry_list(domain1, domain1_regs)) {
        fprintf(stderr, "PDO entry registration failed!\n");
        goto out_release_master;
    }

    // Activate master
    printf("Activating master...\n");
    if (ecrt_master_activate(master)) {
        fprintf(stderr, "Failed to activate master.\n");
        goto out_release_master;
    }

    if (!(domain1_pd = ecrt_domain_data(domain1))) {
        fprintf(stderr, "Failed to get domain data pointer.\n");
        goto out_deactivate;
    }

    printf("\n");
    printf("====================================================\n");
    printf("Master activated! Reading values...\n");
    printf("  - Analog inputs: 4 channels (vacuum sensors)\n");
    printf("  - Digital inputs: 8 channels\n");
    printf("  - Digital outputs: 8 channels (ready for control)\n");
    printf("Press Ctrl+C to stop.\n");
    printf("====================================================\n\n");

    // Initialize outputs control file (all outputs OFF)
    FILE *out_init = fopen("/tmp/ethercat_outputs.txt", "w");
    if (out_init) {
        fprintf(out_init, "0,0,0,0,0,0,0,0\n");
        fclose(out_init);
        printf("Digital outputs initialized to OFF\n");
    }

    // Cyclic loop
    while (run) {
        cyclic_task();
        usleep(100000); // 100ms cycle time (10 Hz)
    }

    printf("\n\nShutting down...\n");

out_deactivate:
    ecrt_master_deactivate(master);
out_release_master:
    ecrt_release_master(master);

    return 0;
}
