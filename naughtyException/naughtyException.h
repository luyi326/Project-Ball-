#ifndef NAUGHTY_EXCEPTION_H
#define NAUGHTY_EXCEPTION_H

enum naughty_exception {
	naughty_exception_PVisionInitFail,
	naughty_exception_PVisionReadFail,
	naughty_exception_PVisionWriteFail,
	naughty_exception_MuxWriteFail,
	naughty_exception_I2CError,
	naughty_exception_GPIOError,
	naughty_exception_XBeeFail,
	naughty_exception_BLEFail
};

#endif
