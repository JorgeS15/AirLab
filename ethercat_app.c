/*
 * AirLab EtherCAT Application - Multiple Slaves (Digital I/O + Analog Input)
 * Version: 2.0.2
 * Date: 2025-12-05
 * 
 * Merged from:
 * - Digital I/O slave code (EP2316-003 at position 1)
 * - Analog input slave code (Engel analog input at position 0)
 * 
 * Features:
 * - Controls digital outputs via /tmp/ethercat_outputs.txt
 * - Reads digital inputs to /tmp/ethercat_digital.txt  
 * - Reads 4 analog channels to /tmp/ethercat_data.txt
 * - Single domain for both slaves
 * - Explicit OP state transition for slave 1
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <stdint.h>
#include "ecrt.h"

// EtherCAT master and domain
static ec_master_t *master = NULL;
static ec_domain_t *domain = NULL;
static uint8_t *domain_pd = NULL;

// Slave configurations
static ec_slave_config_t *sc_digout = NULL;    // Digital I/O slave (position 1)
static ec_slave_config_t *sc_ana_in = NULL;    // Analog input slave (position 0)

// Process data offsets for digital I/O slave (EP2316-003)
static unsigned int off_dig_out = 0;      // Outputs byte (8 bits)
static unsigned int off_dig_ctrl = 0;     // Control byte (safe state, reset)
static unsigned int off_dig_in = 0;       // Inputs byte

// Process data offsets for analog input slave (Engel)
static unsigned int off_ana_in_ch1 = 0;
static unsigned int off_ana_in_ch2 = 0;
static unsigned int off_ana_in_ch3 = 0;
static unsigned int off_ana_in_ch4 = 0;

// Slave 1: EP2316-003 digital I/O - CORRECTED VENDOR ID
#define EP2316_VENDOR_ID    0x0000066b  // Correct from ethercat slaves -v
#define EP2316_PRODUCT_CODE 0x090c4052
#define SLAVE_DIG_POSITION  1

// Slave 0: Engel analog input
#define ENGEL_ANA_IN_ALIAS  0
#define ENGEL_ANA_IN_POS    0
#define ENGEL_VENDOR_ID     0x0000066b
#define ENGEL_PRODUCT_CODE  0x0ea0c252

// Digital I/O slave PDO configuration
static ec_pdo_entry_info_t slave_dig_pdo_entries[] = {
    // RxPDO 0x1600 - Outputs (8 bits + 8 bit gap = 2 bytes)
    {0x7000, 0x01, 1},  // Output 1
    {0x7000, 0x02, 1},  // Output 2
    {0x7000, 0x03, 1},  // Output 3
    {0x7000, 0x04, 1},  // Output 4
    {0x7000, 0x05, 1},  // Output 5
    {0x7000, 0x06, 1},  // Output 6
    {0x7000, 0x07, 1},  // Output 7
    {0x7000, 0x08, 1},  // Output 8
    {0x0000, 0x00, 8},  // Gap
    // RxPDO 0x1601 - Control (16 bits)
    {0xf700, 0x01, 1},  // Set safe state
    {0xf700, 0x02, 1},  // Reset outputs
    {0x0000, 0x00, 14}, // Gap
    // TxPDO 0x1A00 - Inputs (16 bits)
    {0x6000, 0x01, 1},  // Input 1
    {0x6000, 0x02, 1},  // Input 2
    {0x6000, 0x03, 1},  // Input 3
    {0x6000, 0x04, 1},  // Input 4
    {0x6000, 0x05, 1},  // Input 5
    {0x6000, 0x06, 1},  // Input 6
    {0x6000, 0x07, 1},  // Input 7
    {0x6000, 0x08, 1},  // Input 8
    {0x0000, 0x00, 5},  // Gap
    {0x1c32, 0x20, 1},  // Sync error
    {0x0000, 0x00, 2},  // Gap
    // TxPDO 0x1A01 - Diag Inputs (16 bits)
    {0x6001, 0x01, 1},  // Diag Input 1
    {0x6001, 0x02, 1},  // Diag Input 2
    {0x6001, 0x03, 1},  // Diag Input 3
    {0x6001, 0x04, 1},  // Diag Input 4
    {0x6001, 0x05, 1},  // Diag Input 5
    {0x6001, 0x06, 1},  // Diag Input 6
    {0x6001, 0x07, 1},  // Diag Input 7
    {0x6001, 0x08, 1},  // Diag Input 8
    {0x0000, 0x00, 8},  // Gap
    // TxPDO 0x1A02 - Device Status (16 bits)
    {0xf600, 0x01, 1},  // Safe state active
    {0xf600, 0x02, 1},  // Error channel 1
    {0xf600, 0x03, 1},  // Error channel 2
    {0x0000, 0x00, 10}, // Gap
    {0x1c32, 0x20, 1},  // Sync error
    {0x0000, 0x00, 1},  // Gap
    {0x1800, 0x09, 1},  // TxPDO state
};

static ec_pdo_info_t slave_dig_pdos[] = {
    {0x1600, 9, slave_dig_pdo_entries + 0},   // Outputs
    {0x1601, 3, slave_dig_pdo_entries + 9},   // Control
    {0x1A00, 11, slave_dig_pdo_entries + 12}, // Inputs
    {0x1A01, 9, slave_dig_pdo_entries + 23},  // Diag Inputs
    {0x1A02, 7, slave_dig_pdo_entries + 32},  // Device Status
};

static ec_sync_info_t slave_dig_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT,  0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 2, slave_dig_pdos + 0, EC_WD_ENABLE},  // SM2: Outputs + Control
    {3, EC_DIR_INPUT,  3, slave_dig_pdos + 2, EC_WD_DISABLE}, // SM3: Inputs
    {0xFF}
};

// Global variables
volatile sig_atomic_t running = 1;
static int cycle_counter = 0;

void signal_handler(int sig) {
    running = 0;
}

int main(int argc, char **argv) {
    ec_master_state_t ms;
    ec_domain_state_t ds;
    ec_slave_config_state_t scs_dig, scs_ana;
    
    printf("====================================================\n");
    printf("AirLab EtherCAT - Multiple Slaves Application v2.0.2\n");
    printf("====================================================\n");
    printf("Slave 0: EP3744-1041 (4 Analog Inputs)\n");
    printf("Slave 1: EP2316-0003 Digital I/O (8 in, 8 out)\n");
    printf("====================================================\n\n");
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Request EtherCAT master
    printf("[1/7] Requesting EtherCAT master...\n");
    master = ecrt_request_master(0);
    if (!master) {
        fprintf(stderr, "Failed to request master!\n");
        return -1;
    }
    printf("  Master acquired.\n");
    
    // Create single domain for both slaves
    printf("[2/7] Creating domain...\n");
    domain = ecrt_master_create_domain(master);
    if (!domain) {
        fprintf(stderr, "Failed to create domain!\n");
        ecrt_release_master(master);
        return -1;
    }
    printf("  Domain created.\n");
    
    // Configure Slave 0: Analog Input
    printf("[3/7] Configuring Slave 0 (Analog Input)...\n");
    printf("  Vendor ID: 0x%08X, Product Code: 0x%08X\n", 
           ENGEL_VENDOR_ID, ENGEL_PRODUCT_CODE);
    
    sc_ana_in = ecrt_master_slave_config(master, ENGEL_ANA_IN_ALIAS, 
                                         ENGEL_ANA_IN_POS, 
                                         ENGEL_VENDOR_ID, ENGEL_PRODUCT_CODE);
    if (!sc_ana_in) {
        fprintf(stderr, "Failed to configure analog input slave!\n");
        ecrt_release_master(master);
        return -1;
    }
    printf("  Slave 0 configured.\n");
    
    // Configure Slave 1: Digital I/O
    printf("[4/7] Configuring Slave 1 (Digital I/O)...\n");
    printf("  Vendor ID: 0x%08X, Product Code: 0x%08X\n", 
           EP2316_VENDOR_ID, EP2316_PRODUCT_CODE);
    
    sc_digout = ecrt_master_slave_config(master, 0, SLAVE_DIG_POSITION, 
                                         EP2316_VENDOR_ID, EP2316_PRODUCT_CODE);
    if (!sc_digout) {
        fprintf(stderr, "Failed to configure Slave 1!\n");
        ecrt_release_master(master);
        return -1;
    }
    
    // Configure PDOs for digital slave
    printf("  Configuring PDOs for digital slave...\n");
    if (ecrt_slave_config_pdos(sc_digout, EC_END, slave_dig_syncs)) {
        fprintf(stderr, "Failed to configure PDOs for digital slave!\n");
        ecrt_release_master(master);
        return -1;
    }
    printf("  Slave 1 PDOs configured.\n");
    
    // Register PDO entries for BOTH slaves in the same domain
    printf("[5/7] Registering PDO entries for both slaves...\n");
    ec_pdo_entry_reg_t domain_regs[] = {
        // Digital I/O slave (position 1)
        {0, SLAVE_DIG_POSITION, EP2316_VENDOR_ID, EP2316_PRODUCT_CODE, 
         0x7000, 0x01, &off_dig_out},   // First output bit (byte contains all 8)
        {0, SLAVE_DIG_POSITION, EP2316_VENDOR_ID, EP2316_PRODUCT_CODE,
         0xf700, 0x01, &off_dig_ctrl},  // Control byte
        {0, SLAVE_DIG_POSITION, EP2316_VENDOR_ID, EP2316_PRODUCT_CODE,
         0x6000, 0x01, &off_dig_in},    // First input bit
         
        // Analog input slave (position 0)
        {0, ENGEL_ANA_IN_POS, ENGEL_VENDOR_ID, ENGEL_PRODUCT_CODE,
         0x6020, 0x11, &off_ana_in_ch1},
        {0, ENGEL_ANA_IN_POS, ENGEL_VENDOR_ID, ENGEL_PRODUCT_CODE,
         0x6030, 0x11, &off_ana_in_ch2},
        {0, ENGEL_ANA_IN_POS, ENGEL_VENDOR_ID, ENGEL_PRODUCT_CODE,
         0x6040, 0x11, &off_ana_in_ch3},
        {0, ENGEL_ANA_IN_POS, ENGEL_VENDOR_ID, ENGEL_PRODUCT_CODE,
         0x6050, 0x11, &off_ana_in_ch4},
        {}
    };
    
    if (ecrt_domain_reg_pdo_entry_list(domain, domain_regs)) {
        fprintf(stderr, "Failed to register PDO entries!\n");
        ecrt_release_master(master);
        return -1;
    }
    
    printf("  PDO entries registered:\n");
    printf("    Digital Output: offset %u\n", off_dig_out);
    printf("    Digital Control: offset %u\n", off_dig_ctrl);
    printf("    Digital Input: offset %u\n", off_dig_in);
    printf("    Analog Ch1: offset %u\n", off_ana_in_ch1);
    printf("    Analog Ch2: offset %u\n", off_ana_in_ch2);
    printf("    Analog Ch3: offset %u\n", off_ana_in_ch3);
    printf("    Analog Ch4: offset %u\n", off_ana_in_ch4);
    
    // Activate master
    printf("[6/7] Activating master...\n");
    if (ecrt_master_activate(master)) {
        fprintf(stderr, "Failed to activate master!\n");
        ecrt_release_master(master);
        return -1;
    }
    printf("  Master activated.\n");
    
    // Get domain process data
    printf("[7/7] Getting domain process data...\n");
    domain_pd = ecrt_domain_data(domain);
    if (!domain_pd) {
        fprintf(stderr, "Failed to get domain process data!\n");
        ecrt_master_deactivate(master);
        ecrt_release_master(master);
        return -1;
    }
    printf("  Domain process data obtained.\n");
    
    // Initialize outputs and wait for OP state
    printf("\nInitializing slaves and waiting for OP state...\n");
    
    // Try to set slave 1 to OP state using ethercat command
    printf("  Attempting to set Slave 1 to OP state via ethercat command...\n");
    int ret = system("sudo ethercat states -p1 OP");
    if (ret != 0) {
        printf("  Warning: ethercat command returned %d. Continuing with cyclic communication...\n", ret);
    } else {
        printf("  ethercat command executed successfully.\n");
    }
    
    // Clear safe state and enable outputs for digital slave
    printf("  Writing control byte: safe_state=0, reset_outputs=0\n");
    
    // Wait for OP state with cyclic communication
    int timeout = 1000;
    int last_state_dig = -1;
    int last_state_ana = -1;
    int both_in_op = 0;
    
    while (timeout > 0 && running) {
        ecrt_master_receive(master);
        ecrt_domain_process(domain);
        
        ecrt_master_state(master, &ms);
        ecrt_slave_config_state(sc_digout, &scs_dig);
        ecrt_slave_config_state(sc_ana_in, &scs_ana);
        
        // Print state changes
        if (scs_dig.al_state != last_state_dig) {
            printf("Slave 1 State: ");
            switch (scs_dig.al_state) {
                case 1: printf("INIT\n"); break;
                case 2: printf("PREOP\n"); break;
                case 4: printf("SAFEOP\n"); break;
                case 8: printf("OP\n"); break;
                default: printf("0x%02X\n", scs_dig.al_state); break;
            }
            last_state_dig = scs_dig.al_state;
        }
        
        if (scs_ana.al_state != last_state_ana) {
            printf("Slave 0 State: ");
            switch (scs_ana.al_state) {
                case 1: printf("INIT\n"); break;
                case 2: printf("PREOP\n"); break;
                case 4: printf("SAFEOP\n"); break;
                case 8: printf("OP\n"); break;
                default: printf("0x%02X\n", scs_ana.al_state); break;
            }
            last_state_ana = scs_ana.al_state;
        }
        
        // Check if both slaves are in OP state
        if (scs_dig.al_state == 8 && scs_ana.al_state == 8) {
            both_in_op = 1;
            printf("\nBoth slaves reached OP state!\n");
            break;
        }
        
        // Initialize outputs and control
        EC_WRITE_U8(domain_pd + off_dig_ctrl, 0x00);  // Clear safe state
        EC_WRITE_U8(domain_pd + off_dig_out, 0x00);   // All outputs off
        
        ecrt_domain_queue(domain);
        ecrt_master_send(master);
        
        usleep(100000);  // 100ms wait during initialization
        timeout--;
    }
    
    if (!both_in_op) {
        fprintf(stderr, "\nTimeout waiting for OP state!\n");
        printf("Slave 1 final state: AL=0x%02X\n", scs_dig.al_state);
        printf("Slave 0 final state: AL=0x%02X\n", scs_ana.al_state);
        ecrt_master_deactivate(master);
        ecrt_release_master(master);
        return -1;
    }
    
    // Check domain state
    ecrt_domain_state(domain, &ds);
    printf("Domain Working Counter: %u (", ds.working_counter);
    switch (ds.wc_state) {
        case EC_WC_ZERO: printf("ZERO)\n"); break;
        case EC_WC_INCOMPLETE: printf("INCOMPLETE)\n"); break;
        case EC_WC_COMPLETE: printf("COMPLETE)\n"); break;
    }
    
    printf("\n====================================================\n");
    printf("Starting Main Control Loop\n");
    printf("====================================================\n");
    printf("Control files:\n");
    printf("  /tmp/ethercat_outputs.txt - Digital outputs (8 values, 0/1)\n");
    printf("  /tmp/ethercat_digital.txt - Digital inputs (read-only)\n");
    printf("  /tmp/ethercat_data.txt    - Analog inputs (4 channels)\n");
    printf("\nPress Ctrl+C to stop\n");
    printf("====================================================\n\n");
    
    // Initialize output control file
    FILE *init_file = fopen("/tmp/ethercat_outputs.txt", "w");
    if (init_file) {
        fprintf(init_file, "0,0,0,0,0,0,0,0\n");
        fclose(init_file);
        chmod("/tmp/ethercat_outputs.txt", 0666);
    }
    
    // Main control loop
    while (running) {
        ecrt_master_receive(master);
        ecrt_domain_process(domain);
        
        // Read digital states
        uint8_t dig_input_state = EC_READ_U8(domain_pd + off_dig_in);
        uint8_t dig_ctrl_state = EC_READ_U8(domain_pd + off_dig_ctrl);
        
        // Read analog values (32-bit signed integers)
        int32_t ana_ch1 = EC_READ_S32(domain_pd + off_ana_in_ch1);
        int32_t ana_ch2 = EC_READ_S32(domain_pd + off_ana_in_ch2);
        int32_t ana_ch3 = EC_READ_S32(domain_pd + off_ana_in_ch3);
        int32_t ana_ch4 = EC_READ_S32(domain_pd + off_ana_in_ch4);
        
        // Read output pattern from control file
        uint8_t output_pattern = 0;
        FILE *out_file = fopen("/tmp/ethercat_outputs.txt", "r");
        if (out_file) {
            char line[100];
            if (fgets(line, sizeof(line), out_file)) {
                int values[8];
                if (sscanf(line, "%d,%d,%d,%d,%d,%d,%d,%d", 
                          &values[0], &values[1], &values[2], &values[3],
                          &values[4], &values[5], &values[6], &values[7]) == 8) {
                    for (int i = 0; i < 8; i++) {
                        if (values[i]) output_pattern |= (1 << i);
                    }
                }
            }
            fclose(out_file);
        }
        
        // Print status every second (100 cycles at 10ms = 1 second)
        if (cycle_counter % 100 == 0) {
            printf("\rDigital Out: 0x%02X | Digital In: 0x%02X | Ctrl: 0x%02X | "
                   "Analog: %6d, %6d, %6d, %6d", 
                   output_pattern, dig_input_state, dig_ctrl_state,
                   ana_ch1, ana_ch2, ana_ch3, ana_ch4);
            fflush(stdout);
        }
        
        // Write digital output pattern
        EC_WRITE_U8(domain_pd + off_dig_out, output_pattern);
        
        // Keep safe state cleared (bit 0 = 0)
        EC_WRITE_U8(domain_pd + off_dig_ctrl, 0x00);
        
        // Write digital inputs to file (every 10 cycles = 100ms)
        if (cycle_counter % 10 == 0) {
            uint8_t dig_in[8];
            for (int i = 0; i < 8; i++) {
                dig_in[i] = (dig_input_state >> i) & 0x01;
            }
            FILE *dig_file = fopen("/tmp/ethercat_digital.txt", "w");
            if (dig_file) {
                fprintf(dig_file, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                        dig_in[0], dig_in[1], dig_in[2], dig_in[3],
                        dig_in[4], dig_in[5], dig_in[6], dig_in[7]);
                fclose(dig_file);
                chmod("/tmp/ethercat_digital.txt", 0666);
            }
        }
        
        // Write analog inputs to file (every cycle = 10ms)
        FILE *data_file = fopen("/tmp/ethercat_data.txt", "w");
        if (data_file) {
            fprintf(data_file, "%d,%d,%d,%d\n", ana_ch1, ana_ch2, ana_ch3, ana_ch4);
            fclose(data_file);
        }
        
        // Send process data
        ecrt_domain_queue(domain);
        ecrt_master_send(master);
        
        // Monitor state every second
        if (cycle_counter % 100 == 0 && cycle_counter > 0) {
            ecrt_slave_config_state(sc_digout, &scs_dig);
            ecrt_slave_config_state(sc_ana_in, &scs_ana);
            ecrt_domain_state(domain, &ds);
            
            if (scs_dig.al_state != 8 || scs_ana.al_state != 8) {
                printf("\nWARNING: Slave left OP state! Slave 1: 0x%02X, Slave 0: 0x%02X\n",
                       scs_dig.al_state, scs_ana.al_state);
            }
            if (ds.wc_state != EC_WC_COMPLETE) {
                printf("\nWARNING: WC not complete! WC=%u\n", ds.working_counter);
            }
        }
        
        cycle_counter++;
        usleep(10000);  // 10ms = 100Hz cycle time
    }
    
    printf("\n\n====================================================\n");
    printf("Shutting down...\n");
    
    // Turn off all digital outputs
    for (int i = 0; i < 10 && running; i++) {
        ecrt_master_receive(master);
        ecrt_domain_process(domain);
        EC_WRITE_U8(domain_pd + off_dig_out, 0x00);
        EC_WRITE_U8(domain_pd + off_dig_ctrl, 0x00);
        ecrt_domain_queue(domain);
        ecrt_master_send(master);
        usleep(10000);
    }
    
    printf("All outputs OFF.\n");
    
    ecrt_master_deactivate(master);
    ecrt_release_master(master);
    
    printf("Master deactivated and released.\n");
    printf("Application terminated.\n");
    printf("====================================================\n");
    
    return 0;
}