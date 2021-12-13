/*
 * RRF3Common.h
 *
 *  Created on: 17 Oct 2021
 *      Author: David
 *
 *  This file contains definitions common to all implementations of RRF3 on Duets, including those that don't support CAN
 */

#ifndef SRC_RRF3COMMON_H_
#define SRC_RRF3COMMON_H_

#include <cstdint>
#include <CoreTypes.h>				// for PwmFrequency
#include <General/NamedEnum.h>
#include <General/StringRef.h>
#include <General/SimpleMath.h>

// Constants etc. that are common across Duet main and expansion boards

// Generic constants
constexpr float ABS_ZERO = -273.15;						// Celsius
constexpr float NEARLY_ABS_ZERO = -273.0;				// Celsius

// Timeouts
constexpr uint32_t FanCheckInterval = 500;				// Milliseconds
constexpr uint32_t OpenLoadTimeout = 500;				// Milliseconds
constexpr uint32_t MinimumWarningInterval = 4000;		// Milliseconds, must be at least as long as FanCheckInterval
constexpr uint32_t DriverCoolingTimeout = 4000;			// Milliseconds

constexpr uint32_t MinimumOpenLoadFullStepsPerSec = 20;

// FanCheckInterval must be lower than MinimumWarningInterval to avoid giving driver over temperature warnings too soon when thermostatic control of electronics cooling fans is used
static_assert(FanCheckInterval < MinimumWarningInterval, "FanCheckInterval too large");

constexpr float DefaultThermistorR25 = 100000.0;
constexpr float DefaultThermistorBeta = 4725.0;
constexpr float DefaultThermistorC = 7.060e-8;

constexpr float DefaultMinFanPwm = 0.1;					// minimum fan PWM
constexpr uint32_t DefaultFanBlipTime = 100;			// fan blip time in milliseconds

// Heater values
constexpr uint32_t HeatSampleIntervalMillis = 250;		// interval between taking temperature samples
constexpr float HeatPwmAverageTime = 5.0;				// Seconds

constexpr uint8_t SensorsTaskTotalDelay = 250;			// Interval between runs of sensors task

constexpr float TEMPERATURE_CLOSE_ENOUGH = 1.5;			// Celsius
constexpr float TEMPERATURE_LOW_SO_DONT_CARE = 40.0;	// Celsius
constexpr float HOT_ENOUGH_TO_EXTRUDE = 160.0;			// Celsius
constexpr float HOT_ENOUGH_TO_RETRACT = 90.0;			// Celsius

constexpr unsigned int MaxBadTemperatureCount = 2000/HeatSampleIntervalMillis;	// Number of bad temperature samples permitted before a heater fault is reported (2 seconds)
constexpr float BadLowTemperature = -10.0;				// Celsius
constexpr float DefaultHotEndTemperatureLimit = 285.0;	// Celsius - E3D say to tighten the hot end at 285C
constexpr float DefaultBedTemperatureLimit = 125.0;		// Celsius
constexpr float DefaultHotEndFanTemperature = 45.0;		// Temperature at which a thermostatic hot end fan comes on
constexpr float ThermostatHysteresis = 1.0;				// How much hysteresis we use to prevent noise turning fans on/off too often
constexpr float BadErrorTemperature = 2000.0;			// Must exceed any reasonable temperature limit including DEFAULT_TEMPERATURE_LIMIT
constexpr uint32_t DefaultHeaterFaultTimeout = 10 * 60 * 1000;	// How long we wait (in milliseconds) for user intervention after a heater fault before shutting down

// Heating model default parameters. For the chamber heater, we use the same values as for the bed heater.
// These parameters are about right for an E3Dv6 hot end with 30W heater, cooling time constant is about 140 seconds with the fan off
constexpr float DefaultToolHeaterHeatingRate = 2.43;
constexpr float DefaultToolHeaterBasicCoolingRate = 0.56;
constexpr float DefaultToolHeaterCoolingRateExponent = 1.35;
constexpr float DefaultToolHeaterDeadTime = 5.5;

// These parameters are about right for a typical PCB bed heater that maxes out at 110C and has a cooling time constant of 700 seconds
constexpr float DefaultBedHeaterHeatingRate = 0.13;
constexpr float DefaultBedHeaterBasicCoolingRate = 0.15;
constexpr float DefaultBedHeaterCoolingRateExponent = 1.35;
constexpr float DefaultBedHeaterDeadTime = 10.0;

// Parameters used to detect heating errors
constexpr float DefaultMaxHeatingFaultTime = 5.0;		// How many seconds we allow a heating fault to persist
constexpr float AllowedTemperatureDerivativeNoise = 0.12;	// How much fluctuation in the averaged temperature derivative we allow
constexpr float MaxAmbientTemperature = 45.0;			// We expect heaters to cool to this temperature or lower when switched off
constexpr float NormalAmbientTemperature = 25.0;		// The ambient temperature we assume - allow for the printer heating its surroundings a little
constexpr float LowAmbientTemperature = 15.0;			// A lower ambient temperature that we assume when checking heater performance
constexpr float DefaultMaxTempExcursion = 15.0;			// How much error we tolerate when maintaining temperature before deciding that a heater fault has occurred
constexpr float MinimumConnectedTemperature = -5.0;		// Temperatures below this we treat as a disconnected thermistor

static_assert(DefaultMaxTempExcursion > TEMPERATURE_CLOSE_ENOUGH, "DefaultMaxTempExcursion is too low");

// PWM frequencies
constexpr PwmFrequency DefaultHeaterPwmFreq = 250;		// normal PWM frequency used for hot ends
constexpr PwmFrequency MaxHeaterPwmFrequency = 1000;	// maximum supported heater PWM frequency, to avoid overheating the mosfets
constexpr PwmFrequency DefaultFanPwmFreq = 250;			// increase to 25kHz using M106 command to meet Intel 4-wire PWM fan specification
constexpr PwmFrequency DefaultPinWritePwmFreq = 500;	// default PWM frequency for M42 pin writes and extrusion ancillary PWM
constexpr PwmFrequency ServoRefreshFrequency = 50;

// Firmware module numbers
enum class FirmwareModule : uint8_t
{
	main = 0,
	wifi = 1,
	reserved = 2,
	bootloader = 3
};

NamedEnum(FilamentSensorStatus, uint8_t,
	noMonitor,
	ok,
	noDataReceived,
	noFilament,
	tooLittleMovement,
	tooMuchMovement,
	sensorError
);

NamedEnum(LogLevel, uint8_t, off, warn, info, debug);

// Meaning of the driver status bits. The lowest 8 bits of these have the same bit positions as in the TMC2209 DRV_STATUS register. The TMC5160 DRV_STATUS is different.
union StandardDriverStatus
{
	uint32_t all;
	struct
	{
		uint32_t otpw : 1,								// over temperature warning
				 ot : 1,								// over temperature error
				 s2ga : 1,								// short to ground phase A
				 s2gb : 1,								// short to ground phase B
				 s2vsa : 1,								// short to VS phase A
				 s2vsb : 1,								// short to VS phase B
				 ola : 1,								// open load phase A
				 olb : 1,								// open load phase B
				 // The remaining bit assignments do not correspond to TMC2209 bit positions
				 standstill : 1,						// standstill indicator
				 stall : 1,								// stall, or closed loop error exceeded
				 notPresent : 1,						// smart driver not present
				 externalDriverError : 1,				// external driver signalled error
				 closedLoopPositionWarning : 1,			// close to stall, or closed loop warning
				 closedLoopPositionNotMaintained : 1,	// failed to achieve position
				 closedLoopNotTuned : 1,				// closed loop driver has not been tuned
				 closedLoopTuningError : 1,				// closed loop tuning failed
				 closedLoopIllegalMove : 1,				// move attempted in closed loop mode when driver not tuned
				 zero : 5,								// reserved for future use - don't use the MSB because it will make the value negative in the OM
				 sgresultMin : 10;						// minimum stallguard result seen
	};

	static constexpr unsigned int OtBitPos = 0;
	static constexpr unsigned int OtpwBitPos = 1;
	static constexpr unsigned int StandstillBitPos = 8;
	static constexpr unsigned int StallBitPos = 10;
	static constexpr unsigned int SgresultBitPos = 16;

	static constexpr uint32_t ErrorMask =   0b10010101000111110;		// bit positions that usually correspond to errors
	static constexpr uint32_t WarningMask = 0b01001000011000001;		// bit positions that correspond to warnings
	static constexpr uint32_t InfoMask =    0b00100010100000000;		// bit positions that correspond to information

	static_assert((ErrorMask & WarningMask) == 0);
	static_assert((ErrorMask & InfoMask) == 0);
	static_assert((InfoMask & WarningMask) == 0);

	void AppendText(const StringRef& str, unsigned int severity) const noexcept;

private:
	// Strings representing the meaning of each bit in DriverStatus
	static constexpr const char * _ecv_array BitMeanings[] =
	{
		"over temperature warning",
		"over temperature shutdown",
		"phase A short to ground",
		"phase B short to ground",
		"phase A short to Vin",
		"phase B short to Vin",
		"phase A may be disconnected",
		"phase B may be disconnected",
		"standstill",
		"stalled",
		"not present",
		"external driver error",
		"position tolerance exceeded",
		"failed to maintain position",
		"not tuned",
		"tuning failed",
		"move attempted when not tuned"
	};

	static_assert((1u << ARRAY_SIZE(BitMeanings)) - 1 == (ErrorMask | WarningMask | InfoMask));
};

static_assert(sizeof(StandardDriverStatus) == sizeof(uint32_t));

// Structure to represent the minimum, current and maximum values of a floating point quantity
struct MinCurMax
{
	float minimum;
	float current;
	float maximum;
};

// Enum to represent a heater state
enum class HeaterMode : uint8_t
{
	// The order of these is important because we test "mode > HeatingMode::suspended" to determine whether the heater is active
	// and "mode >= HeatingMode::off" to determine whether the heater is either active or suspended
	fault,
	offline,
	off,
	suspended,
	heating,
	cooling,
	stable,
	// All states from here onwards must be PID tuning states because function IsTuning assumes that
	tuning0,
	tuning1,
	tuning2,
	tuning3,
	firstTuningMode = tuning0,
	lastTuningMode = tuning3
};

// Enum to represent an event type. Earlier values in the list have higher priority.
// The type names are also the names of the associated macro files that are run in response to the event.
// Note: main board power failure is not currently handled by the event system but is included here as a placeholder in case in future it is.
// mcu_temperature_warning is not current used.
NamedEnum(EventType, uint8_t, main_board_power_fail, heater_fault, driver_error, filament_error, driver_stall, driver_warning, mcu_temperature_warning);

// Type of heater fault
enum class HeaterFaultType : uint8_t
{
	failedToReadSensor = 0, temperatureRisingTooSlowly, exceededAllowedExcursion, monitorTriggered,
	heaterFaultTypeLimit
};

// Text descriptions of the above, with an extra one to handle out-of-range parameters
static constexpr const char *_ecv_array HeaterFaultText[] =
{
	"failed to read sensor: ",						// the sensor error message will be appended
	"temperature rising too slowly: ",				// "expected ... measured ..." will be appended
	"exceeded allowed temperature excursion: ",		// "target ... actual ..." will be appended
	"",												// "monitor ... was triggered" will be appended
	"unknown error: "								// this is used if the parameter is not a valid heater fault type
};

static_assert(ARRAY_SIZE(HeaterFaultText) == (unsigned int)HeaterFaultType::heaterFaultTypeLimit + 1);

#endif /* SRC_RRF3COMMON_H_ */
