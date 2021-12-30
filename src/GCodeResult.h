/*
 * GCodeResult.h
 *
 *  Created on: 1 Oct 2017
 *      Author: David
 */

#ifndef SRC_GCODERESULT_H_
#define SRC_GCODERESULT_H_

#include <cctype>

// Enumeration to specify the result of attempting to process a GCode command
enum class GCodeResult : uint8_t
{
	notFinished,					// we haven't finished processing this command
	ok,								// we have finished processing this code in the current state, and if the GCodeState is 'normal' then we have finished it completely
	warning,
	warningNotSupported,
	error,
	errorNotSupported,
	notSupportedInCurrentMode,
	badOrMissingParameter,
	remoteInternalError
};

// Convert a true/false error/no-error indication to a GCodeResult
inline GCodeResult GetGCodeResultFromError(bool err)
{
	return (err) ? GCodeResult::error : GCodeResult::ok;
}

// Convert a true/false success/failure indication to a GCodeResult
inline GCodeResult GetGCodeResultFromSuccess(bool ok) noexcept
{
	return (ok) ? GCodeResult::ok : GCodeResult::error;
}

// Convert a true/false finished/not-finished indication to a GCodeResult
inline GCodeResult GetGCodeResultFromFinished(bool finished)
{
	return (finished) ? GCodeResult::ok : GCodeResult::notFinished;
}

#endif /* SRC_GCODERESULT_H_ */