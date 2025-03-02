#include "power.h"
#include "screen.h"

/* Port definitions */
#define PORT_KEYBOARD_CONTROLLER 0x64
#define PORT_KEYBOARD_DATA      0x60
#define PORT_SYSTEM_CONTROL_A   0x92

/* Virtual Machine ports */
#define QEMU_OLD_PORT         0xB004
#define QEMU_NEW_PORT         0x604
#define VIRTUALBOX_PORT       0x4004
#define CLOUD_HYPERV_PORT     0x600

/* VM shutdown commands */
#define QEMU_SHUTDOWN_CMD     0x2000
#define VIRTUALBOX_CMD        0x3400
#define CLOUD_HYPERV_CMD      0x34

/* Port I/O functions */
static inline void outb(u16 port, u8 value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void outw(u16 port, u16 value) {
    __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline u8 inb(u16 port) {
    u8 value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* Wait for keyboard controller buffer to be empty */
static void wait_keyboard_buffer(void) {
    int i;
    for (i = 0; i < 100000 && (inb(PORT_KEYBOARD_CONTROLLER) & 2); i++) {
        __asm__ volatile("pause");
    }
}

/* Try VM-specific shutdown methods with delay */
static void try_vm_shutdown(void) {
    int i;

    /* Try VirtualBox shutdown - with proper delay and retries */
    for (i = 0; i < 3; i++) {
        outw(VIRTUALBOX_PORT, VIRTUALBOX_CMD);
        
        /* Add small delay between attempts */
        for (int j = 0; j < 10000; j++) {
            __asm__ volatile("pause");
        }
    }

    /* Try QEMU (newer versions) */
    outw(QEMU_NEW_PORT, QEMU_SHUTDOWN_CMD);
    
    /* Try QEMU (older versions) */
    outw(QEMU_OLD_PORT, QEMU_SHUTDOWN_CMD);
    
    /* Try Cloud Hypervisor */
    outw(CLOUD_HYPERV_PORT, CLOUD_HYPERV_CMD);
}

/* Safe system shutdown */
void shutdown_system(void) {
    print_string("Initiating safe shutdown...\n");
    
    /* Disable interrupts gracefully */
    __asm__ volatile("cli" : : : "memory");

    /* Attempt VM-specific shutdown methods first */
    try_vm_shutdown();

    /* If VM shutdown fails, try keyboard controller reset as fallback */
    wait_keyboard_buffer();
    outb(PORT_KEYBOARD_CONTROLLER, 0xFE);

    /* If all else fails, perform a safe halt */
    print_string("System is halted. It is now safe to power off.\n");
    
    /* Final safe halt loop */
    while (1) {
        __asm__ volatile(
            "cli\n"
            "hlt"
            : : : "memory"
        );
    }
}

/* System reboot implementation */
void reboot_system(void) {
    print_string("Rebooting system...\n");

    /* Disable interrupts */
    __asm__ volatile("cli" : : : "memory");

    /* Try keyboard controller reset */
    wait_keyboard_buffer();
    outb(PORT_KEYBOARD_CONTROLLER, 0xFE);

    /* If keyboard controller reset fails, try system control port */
    outb(PORT_SYSTEM_CONTROL_A, inb(PORT_SYSTEM_CONTROL_A) | 1);

    /* If all else fails, halt safely */
    while (1) {
        __asm__ volatile(
            "cli\n"
            "hlt"
            : : : "memory"
        );
    }
}
