#ifndef SDCARD_H
#define	SDCARD_H

enum E_SD_CARD
{
    SD_R1,
    SD_R3,
    SD_R7,
    SD_SDSC,
    SD_SDHC,
    SD_WRITE_NO_ERROR,
    SD_WRITE_ERROR_WRONG_R1,
    SD_WRITE_ERROR_WRONG_DATA_RESPONSE,
    SD_WRITE_ERROR_WRONG_CRC,
    SD_WRITE_ERROR_WRONG_TOKEN,
    SD_READ_NO_ERROR,
    SD_READ_ERROR_WRONG_R1,
    SD_READ_ERROR_WRONG_TOKEN
};

#endif	/* SDCARD_H */

