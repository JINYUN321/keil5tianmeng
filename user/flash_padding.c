/* Keep the final loader write at the end of the 128 KB main Flash. */
__attribute__((section(".flash_padding"), used))
const unsigned char flash_download_padding[4] = {0xFF, 0xFF, 0xFF, 0xFF};
