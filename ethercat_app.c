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

#define ENGEL_ANA_IN  0, 0  // Alias, Position
#define ENGEL_VendorID    0x0000066b
#define ENGEL_ProductCode 0x0ea0c252

// Offsets for PDO entries
static unsigned int off_ana_in_ch1;
static unsigned int off_ana_in_ch2;
static unsigned int off_ana_in_ch3;
static unsigned int off_ana_in_ch4;
static unsigned int counter = 0;

// PDO entries
const static ec_pdo_entry_reg_t domain1_regs[] = {
    {ENGEL_ANA_IN, ENGEL_VendorID, ENGEL_ProductCode, 0x6020, 0x11, &off_ana_in_ch1},
    {ENGEL_ANA_IN, ENGEL_VendorID, ENGEL_ProductCode, 0x6030, 0x11, &off_ana_in_ch2},
    {ENGEL_ANA_IN, ENGEL_VendorID, ENGEL_ProductCode, 0x6040, 0x11, &off_ana_in_ch3},
    {ENGEL_ANA_IN, ENGEL_VendorID, ENGEL_ProductCode, 0x6050, 0x11, &off_ana_in_ch4},
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

    // Print to console every 10 cycles
    if (counter++ % 10 == 0) {
        printf("\rCH1: %10d  CH2: %10d  CH3: %10d  CH4: %10d", 
               ch1, ch2, ch3, ch4);
        fflush(stdout);
    }

    // Write to file for web dashboard
    FILE *data_file = fopen("/tmp/ethercat_data.txt", "w");
    if (data_file) {
        fprintf(data_file, "%d,%d,%d,%d\n", ch1, ch2, ch3, ch4);
        fclose(data_file);
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

    // Configure slave
    printf("Configuring slave 0...\n");
    if (!(sc_ana_in = ecrt_master_slave_config(master, ENGEL_ANA_IN, ENGEL_VendorID, ENGEL_ProductCode))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
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
    printf("Master activated! Reading analog values...\n");
    printf("Press Ctrl+C to stop.\n");
    printf("====================================================\n\n");

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
