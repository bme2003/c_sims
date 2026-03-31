static void write_string(const char *message, unsigned short color)
{
    volatile unsigned char *video_memory = (volatile unsigned char *)0xB8000;
    int index = 0;

    while (message[index] != '\0')
    {
        video_memory[index * 2] = (unsigned char)message[index];
        video_memory[index * 2 + 1] = (unsigned char)color;
        index++;
    }
}

void kernel_main(void)
{
    write_string("Hello from your kernel", 0x0F);

    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}
