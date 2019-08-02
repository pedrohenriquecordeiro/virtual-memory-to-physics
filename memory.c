#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>

// Diretiva definida pelo kernel baseada na arquitetura. Reflete o tamanho da página.
#define PAGE_SHIFT 12
// Tamanho em bytes das informações de página no arquivo pagemap.
#define PAGEINFO_LENGTH 8

uint64_t addr_virtual_para_fisico(void *addr);

int var = 512;

int main() {

	printf("Virtual memory: %p\n",&var);
	printf("Physical memory: %" PRIu64 "\n", addr_virtual_para_fisico(&var));
	
}

uint64_t addr_virtual_para_fisico(void *addr) {

	FILE *pagemap = fopen("/proc/self/pagemap", "rb");

	unsigned long offset = (unsigned long)addr / getpagesize() * PAGEINFO_LENGTH;
	if(fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
		fprintf(stderr, "Failed to seek pagemap to proper location\n");
		exit(1);
	}

	unsigned long page_frame_number = 0;
	fread(&page_frame_number, 1, PAGEINFO_LENGTH-1, pagemap);
	page_frame_number &= 0x7FFFFFFFFFFFFF;

	fclose(pagemap);

	/* 
	printf("Virtual memory: %p\n", &var);
	printf("System page size: %d\n", getpagesize());
	printf("Page frame: %u\n", page_frame_number); 
	*/

	unsigned int distance_from_page_boundary = (unsigned long)&var % getpagesize();

	uint64_t physical = (page_frame_number << PAGE_SHIFT) + distance_from_page_boundary;

	/* 
	printf("Frame start: %u\n", (page_frame_number << PAGE_SHIFT));
	printf("Offset: %u\n", distance_from_page_boundary);
    printf("Physical memory: %" PRIu64 "\n", physical);

	printf("Value: %d\n", var); 
	*/

	return physical;

}
