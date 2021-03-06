#include <unistd.h>                             //Needed for I2C port
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>                              //Needed for I2C port
#include <sys/ioctl.h>                  //Needed for I2C port
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

/*
 * SMBus access function definitions.
 */
static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command,
                                     int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(file,I2C_SMBUS,&args);
}


static inline __s32 i2c_smbus_write_quick(int file, __u8 value)
{
    return i2c_smbus_access(file,value,0,I2C_SMBUS_QUICK,NULL);
}

static inline __s32 i2c_smbus_read_byte(int file)
{
    union i2c_smbus_data data;
    if (i2c_smbus_access(file,I2C_SMBUS_READ,0,I2C_SMBUS_BYTE,&data))
        return -1;
    else
        return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte(int file, __u8 value)
{
    return i2c_smbus_access(file,I2C_SMBUS_WRITE,value,
                            I2C_SMBUS_BYTE,NULL);
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
    union i2c_smbus_data data;
    if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
                         I2C_SMBUS_BYTE_DATA,&data))
        return -1;
    else
        return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command,
                                              __u8 value)
{
    union i2c_smbus_data data;
    data.byte = value;
    return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
                            I2C_SMBUS_BYTE_DATA, &data);
}

static inline __s32 i2c_smbus_read_word_data(int file, __u8 command)
{
    union i2c_smbus_data data;
    if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
                         I2C_SMBUS_WORD_DATA,&data))
        return -1;
    else
        return 0x0FFFF & data.word;
}

static inline __s32 i2c_smbus_write_word_data(int file, __u8 command,
                                              __u16 value)
{
    union i2c_smbus_data data;
    data.word = value;
    return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
                            I2C_SMBUS_WORD_DATA, &data);
}

static inline __s32 i2c_smbus_process_call(int file, __u8 command, __u16 value)
{
    union i2c_smbus_data data;
    data.word = value;
    if (i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
                         I2C_SMBUS_PROC_CALL,&data))
        return -1;
    else
        return 0x0FFFF & data.word;
}


/* Returns the number of read bytes */
static inline __s32 i2c_smbus_read_block_data(int file, __u8 command,
                                              __u8 *values)
{
    union i2c_smbus_data data;
    int i;
    if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
                         I2C_SMBUS_BLOCK_DATA,&data))
        return -1;
    else {
        for (i = 1; i <= data.block[0]; i++)
            values[i-1] = data.block[i];
        return data.block[0];
    }
}

static inline __s32 i2c_smbus_write_block_data(int file, __u8 command,
                                               __u8 length, __u8 *values)
{
    union i2c_smbus_data data;
    int i;
    if (length > 32)
        length = 32;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i-1];
    data.block[0] = length;
    return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
                            I2C_SMBUS_BLOCK_DATA, &data);
}

static inline __s32 i2c_smbus_write_i2c_block_data(int file, __u8 command,
                                                   __u8 length, __u8 *values)
{
    union i2c_smbus_data data;
    int i;
    if (length > 32)
        length = 32;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i-1];
    data.block[0] = length;
    return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
                            I2C_SMBUS_I2C_BLOCK_DATA, &data);
}

/*
 * RPI I2C functions
 */
int rpi_i2c_open(const char* filename, uint8_t addr) {
    int file_i2c;
    if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return -1;
	}
    if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return -1;
	}
    return file_i2c;
}
int rpi_i2c_read_byte(int file, uint8_t *data) {
    union i2c_smbus_data s_data;
    int res = i2c_smbus_access(file,I2C_SMBUS_READ,0,I2C_SMBUS_BYTE,&s_data);
    if (data != NULL) {
        *data = 0x0FF & s_data.byte;
    }
    return res;
}
int rpi_i2c_write_byte(int file, uint8_t data) {
    return i2c_smbus_write_byte(file, data);
}
