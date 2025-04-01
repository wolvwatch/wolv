/*
 * parseCommands.h
 *
 *  Created on: Mar 31, 2025
 *      Author: ryankaelle
 */

#ifndef PARSECOMMANDS_H
#define PARSECOMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parse and handle a Bluetooth command string
 *
 * This function checks the command prefix (e.g. "CMD:1", "CMD:2:", etc.),
 * extracts parameters if needed, and updates the GUI, RTC, or other
 * application state accordingly.
 *
 * @param cmd Pointer to the null-terminated command string
 */
void parseBluetoothCommand(const char *cmd);

#ifdef __cplusplus
}
#endif

#endif // PARSECOMMANDS_H
