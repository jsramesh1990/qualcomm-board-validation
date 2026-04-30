#include "platform.h"
#include "logger.h"
#include "boot_sequence.h"
#include "config_manager.h"
#include "virtual_dashboard.h"
#include "test_framework.h"
#include <getopt.h>
#include <signal.h>

static volatile bool g_running = true;
static system_state_t g_system_state = {0};

void signal_handler(int sig) {
    (void)sig;
    LOG_INFO("Shutdown signal received");
    g_running = false;
}

void print_usage(const char* program) {
    printf("Usage: %s [OPTIONS]\n", program);
    printf("Options:\n");
    printf("  -m, --mode <simulation|hardware>  Run mode (default: simulation)\n");
    printf("  -t, --test                         Run test suite\n");
    printf("  -d, --dashboard                    Enable virtual dashboard\n");
    printf("  -c, --config <file>                Configuration file path\n");
    printf("  -l, --log-level <debug|info|warn|error>  Log level\n");
    printf("  -h, --help                         Show this help\n");
    printf("\nExamples:\n");
    printf("  %s --mode=simulation --dashboard\n", program);
    printf("  %s --test\n", program);
    printf("  %s --mode=hardware --config=config/board_config.h\n", program);
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool run_tests = false;
    bool enable_dashboard = false;
    const char* config_file = "config/sim_params.json";
    const char* mode = "simulation";
    log_level_t log_level = LOG_LEVEL_INFO;
    
    static struct option long_options[] = {
        {"mode", required_argument, 0, 'm'},
        {"test", no_argument, 0, 't'},
        {"dashboard", no_argument, 0, 'd'},
        {"config", required_argument, 0, 'c'},
        {"log-level", required_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "m:tdc:l:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'm':
                mode = optarg;
                break;
            case 't':
                run_tests = true;
                break;
            case 'd':
                enable_dashboard = true;
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'l':
                if (strcmp(optarg, "debug") == 0) log_level = LOG_LEVEL_DEBUG;
                else if (strcmp(optarg, "info") == 0) log_level = LOG_LEVEL_INFO;
                else if (strcmp(optarg, "warn") == 0) log_level = LOG_LEVEL_WARN;
                else if (strcmp(optarg, "error") == 0) log_level = LOG_LEVEL_ERROR;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Initialize logger
    logger_init(log_level, true, "simulation.log");
    LOG_INFO("=== Embedded Simulation Framework v1.0.0 ===");
    LOG_INFO("Branch: feature/EMB-123-embedded-simulation-framework");
    LOG_INFO("Platform: %s", PLATFORM_STRING);
    LOG_INFO("Mode: %s", mode);
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Run tests if requested
    if (run_tests) {
        LOG_INFO("Running test suite...");
        return run_all_tests();
    }
    
    // Load configuration
    if (config_load(config_file) != ERR_OK) {
        LOG_WARN("Using default configuration");
    }
    
    // Set simulation mode
    g_system_state.simulation_mode = (strcmp(mode, "simulation") == 0);
    
    // Initialize dashboard if enabled
    if (enable_dashboard && g_system_state.simulation_mode) {
        dashboard_init();
        LOG_INFO("Virtual dashboard enabled");
    }
    
    // Execute boot sequence
    error_code_t err = boot_sequence();
    if (err != ERR_OK) {
        LOG_FATAL("Boot failed: %s", error_to_string(err));
        return 1;
    }
    
    // Main loop
    LOG_INFO("System ready - entering main loop");
    while (g_running) {
        delay_ms(100);
        
        // Update dashboard
        if (enable_dashboard && g_system_state.simulation_mode) {
            // Collect system health data
            system_health_t health = {0};
            health.uptime_sec = g_system_state.uptime_ms / 1000;
            health.stage = g_system_state.stage;
            health.error = g_system_state.last_error;
            
            dashboard_update(&health);
        }
    }
    
    LOG_INFO("System shutdown complete");
    return 0;
}

system_state_t g_system_state = {0};
