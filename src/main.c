#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limine.h>
#include <system.h>
#include <memory.h>
#include <vga.h>
#include <psf.h>
#include <console.h>
#include <gdt.h>

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    initiateVGA();
    initiateConsole();
    initiateGDT();

    print("Hello World!\n");
    print("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin semper interdum ipsum, ac tincidunt libero ullamcorper in. In euismod consectetur tortor vel volutpat. Vestibulum malesuada tellus vitae gravida viverra. ");

    // We're done, just hang...
    hcf();
}
