/*
 * Copyright (c) 2020 Henrik Brix Andersen <henrik@brixandersen.dk>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief DAC shell commands.
 */

#include <shell/shell.h>
#include <drivers/dac.h>
#include <stdlib.h>

struct args_index {
	u8_t device;
	u8_t channel;
	u8_t reference;
	u8_t value;
	u8_t buffered;
	u8_t resolution;
};

static const struct args_index args_indx = {
	.device = 1,
	.channel = 2,
	.reference = 3,
	.value = 3,
	.buffered = 4,
	.resolution = 4,
};

static int cmd_setup(const struct shell *shell, size_t argc, char **argv)
{
	struct dac_channel_cfg cfg;
	struct device *dac;
	int err;

	dac = device_get_binding(argv[args_indx.device]);
	if (!dac) {
		shell_error(shell, "DAC device not found");
		return -EINVAL;
	}

	cfg.channel_id = strtoul(argv[args_indx.channel], NULL, 0);
	/* TODO: Allow text input for reference */
	cfg.reference = strtoul(argv[args_indx.reference], NULL, 0);

	if (argc == (args_indx.buffered + 1)) {
		cfg.buffered = strtoul(argv[args_indx.buffered], NULL, 2);
	} else {
		cfg.buffered = false;
	}

	err = dac_channel_setup(dac, &cfg);
	if (err) {
		shell_error(shell, "Failed to setup DAC channel (err %d)", err);
		return err;
	}

	return 0;
}

static int cmd_write_value(const struct shell *shell, size_t argc, char **argv)
{
	struct device *dac;
	u8_t resolution;
	u8_t channel;
	u32_t value;
	int err;

	dac = device_get_binding(argv[args_indx.device]);
	if (!dac) {
		shell_error(shell, "DAC device not found");
		return -EINVAL;
	}

	channel = strtoul(argv[args_indx.channel], NULL, 0);
	value = strtoul(argv[args_indx.value], NULL, 0);
	resolution = strtoul(argv[args_indx.resolution], NULL, 0);

	err = dac_write_value(dac, channel, value, resolution);
	if (err) {
		shell_error(shell, "Failed to write DAC value (err %d)", err);
		return err;
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(dac_cmds,
	SHELL_CMD_ARG(setup, NULL, "<device> <channel> <reference> [buffered]",
		      cmd_setup, 4, 1),
	SHELL_CMD_ARG(write_value, NULL, "<device> <channel> <value> <resolution>",
		      cmd_write_value, 5, 0),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(dac, &dac_cmds, "DAC shell commands", NULL);