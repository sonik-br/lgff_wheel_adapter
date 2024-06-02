#pragma once

#include "config.h"

struct init_mode_commands {
  const uint8_t cmd_count; /* Number of commands to send */
  const uint8_t cmd[];
};

const uint8_t mode_cmd_interval = 20; // ms
const uint8_t mode_cmd_length = 7;

#ifdef DEACTIVATE_AUTO_CENTER
  #define CMD_DEACTIVATE_AUTO_CENTER 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#else
  #define CMD_DEACTIVATE_AUTO_CENTER 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
#define CMD_REVERT_MODE            0xf8, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00

static const struct init_mode_commands cmd_mode_none = {
  1,
  {CMD_DEACTIVATE_AUTO_CENTER}                  /* De-activate Auto-Center */
};

static const struct init_mode_commands cmd_mode_df = {
  3,
  {CMD_DEACTIVATE_AUTO_CENTER,                /* De-activate Auto-Center */
   CMD_REVERT_MODE,                           /* Revert mode upon USB reset */
   0xf8, 0x09, 0x00, 0x01, 0x00, 0x00, 0x00}  /* Switch mode to DF-EX with detach */
};

static const struct init_mode_commands cmd_mode_dfp = {
  3,
  {CMD_DEACTIVATE_AUTO_CENTER,                /* De-activate Auto-Center */
   CMD_REVERT_MODE,                           /* Revert mode upon USB reset */
   0xf8, 0x09, 0x01, 0x01, 0x00, 0x00, 0x00}  /* Switch mode to DFP with detach */
};

static const struct init_mode_commands cmd_mode_dfgt = {
  3,
  {CMD_DEACTIVATE_AUTO_CENTER,                /* De-activate Auto-Center */
   CMD_REVERT_MODE,                           /* Revert mode upon USB reset */
   0xf8, 0x09, 0x03, 0x01, 0x00, 0x00, 0x00}  /* Switch mode to DFGT with detach */
};

static const struct init_mode_commands cmd_mode_g25 = {
  3,
  {CMD_DEACTIVATE_AUTO_CENTER,                /* De-activate Auto-Center */
   CMD_REVERT_MODE,                           /* Revert mode upon USB reset */
   0xf8, 0x09, 0x02, 0x01, 0x00, 0x00, 0x00}  /* Switch mode to G25 with detach */
};

static const struct init_mode_commands cmd_mode_g27 = {
  3,
  {CMD_DEACTIVATE_AUTO_CENTER,                /* De-activate Auto-Center */
   CMD_REVERT_MODE,                           /* Revert mode upon USB reset */
   0xf8, 0x09, 0x04, 0x01, 0x00, 0x00, 0x00}  /* Switch mode to G27 with detach */
};

static const struct init_mode_commands cmd_mode_g29 = {
  3,
  {CMD_DEACTIVATE_AUTO_CENTER,                /* De-activate Auto-Center */
   CMD_REVERT_MODE,                           /* Revert mode upon USB reset */
   0xf8, 0x09, 0x05, 0x01, 0x01, 0x00, 0x00}  /* Switch mode to G29 with detach */
};

static const struct init_mode_commands cmd_mode_g923 = {
  2,
  {CMD_DEACTIVATE_AUTO_CENTER,                /* De-activate Auto-Center */
   0xf8, 0x09, 0x07, 0x01, 0x01, 0x00, 0x00}  /* Switch mode to G923 with detach */
};

static const struct init_mode_commands cmd_mode_g923_range = {
  2,
  {CMD_DEACTIVATE_AUTO_CENTER,                /* De-activate Auto-Center */
   0xf8, 0x81, 0x0e, 0x01, 0x00, 0x00, 0x00}  /* Set range to 270º */
};
